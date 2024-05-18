#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>
#include <queue>
#include <ctime>
#include <iomanip>
using namespace std;

struct dma_pp_trade
{
    string date;
    string direction;
    int quantity;
    double price;
};

vector<pair<double, bool>> calc_ama(vector<dma_pp_trade> data, int n, double c1, double c2)
{
    double absolute_price_change = 0;
    double first_absolute_price_change = abs(data[1].price - data[0].price);
    absolute_price_change += first_absolute_price_change;

    for (int i = 2; i <= n; i++)
    {
        absolute_price_change += abs(data[i].price - data[i - 1].price);
    }
    double price_change = data[n].price - data[0].price;

    double efficiency_ratio = absolute_price_change == 0 ? 0 : price_change / absolute_price_change;
    bool flag = false;

    if (absolute_price_change == 0)
        flag = true;

    vector<pair<double, bool>> ama;
    vector<double> sf;
    double sf_o = 0.5;
    sf.push_back(sf_o);

    double ama_o = data[n].price;
    ama.push_back(make_pair(ama_o, flag));
    for (int i = n + 1; i < data.size(); i++)
    {
        absolute_price_change = (absolute_price_change - first_absolute_price_change) + abs(data[i].price - data[i - 1].price);
        price_change = data[i].price - data[i - n].price;

        first_absolute_price_change = abs(data[i-(n-1)].price - data[i-(n)].price);

        double er = absolute_price_change == 0 ? 0 : price_change / absolute_price_change;
        bool flag = false;
        if (absolute_price_change == 0)
            flag = true;

        sf_o = sf_o + c1 * (((2 * er) / (1 + c2) - 1) / ((2 * er) / (1 + c2) + 1) - sf_o);

        double ama_o = ama.back().first + (sf_o * (data[i].price - ama.back().first));
        ama.push_back(make_pair(ama_o, flag));
    }
    return ama;
}

vector<dma_pp_trade> ama_strategy(vector<dma_pp_trade> data, int x, int n, double c1, double c2, double p, int max_hold_days)
{
    vector<dma_pp_trade> result;
    int pos = 0;
    queue<pair<int, bool>> q;
    vector<pair<double, bool>> ama = calc_ama(data, n, c1, c2);

    for (int i = n; i < data.size(); i++)
    {
        int first_position = q.front().first;
        int hold_days = i - first_position;
        if (hold_days > max_hold_days)
        {
            if (q.front().second)
            {
                dma_pp_trade temp;
                temp.date = data[i].date;
                temp.direction = "SELL";
                temp.quantity = 1;
                temp.price = data[i].price;
                result.push_back(temp);
                pos--;
            }
            else
            {
                dma_pp_trade temp;
                temp.date = data[i].date;
                temp.direction = "BUY";
                temp.quantity = 1;
                temp.price = data[i].price;
                result.push_back(temp);
                pos++;
            }
            q.pop();
        }

        double price_diff = data[i].price - ama[i - n].first;
        price_diff /= ama[i - n].first;
        price_diff *= 100;

        if (price_diff >= p && pos + 1 <= x && ama[i - n].second == false)
        {
            dma_pp_trade temp;
            temp.date = data[i].date;
            temp.direction = "BUY";
            temp.quantity = 1;
            temp.price = data[i].price;
            result.push_back(temp);
            pos++;
            q.push(make_pair(i, true));
        }
        else if (price_diff <= -p && pos - 1 >= -x && ama[i - n].second == false)
        {
            dma_pp_trade temp;
            temp.date = data[i].date;
            temp.direction = "SELL";
            temp.quantity = 1;
            temp.price = data[i].price;
            result.push_back(temp);
            pos--;
            q.push(make_pair(i, false));
        }
        else
        {
            dma_pp_trade temp;
            temp.date = data[i].date;
            temp.direction = "NA";
            temp.quantity = 0;
            temp.price = data[i].price;
            result.push_back(temp);
        }
    }
    dma_pp_trade temp;
    temp.date = data[data.size() - 1].date;
    temp.direction = "NA";
    temp.price = data[data.size() - 1].price * pos;
    result.push_back(temp);

    return result;
}

vector<dma_pp_trade> ama_read_input(string filename)
{
    ifstream input_file(filename);

    if (!input_file.is_open())
    {
        cerr << "Error: Unable to open file\n";
        exit(1);
    }

    vector<dma_pp_trade> trades;
    string line;

    // Ignore header line
    getline(input_file, line);

    while (getline(input_file, line))
    {
        string date;
        double price;

        stringstream ss(line);
        getline(ss, date, ',');
        ss >> price;

        dma_pp_trade trade;
        trade.date = date;
        trade.price = price;

        trades.push_back(trade);
    }

    return trades;
}

void ama_write_output(vector<dma_pp_trade> data, string cashflow, string stats, string pnl)
{
    ofstream stats_file(stats);

    if (!stats_file.is_open())
    {
        cerr << "Error: Unable to create file\n";
        return;
    }

    stats_file << "Date,Order_dir,Quantity,Price\n";

    for (int i = 0; i < data.size(); i++)
    {
        if (data[i].direction != "NA")
            stats_file << data[i].date << "," << data[i].direction << "," << 1 << "," << data[i].price << "\n";
    }

    stats_file.close();

    ofstream cashflow_file(cashflow);

    if (!cashflow_file.is_open())
    {
        cerr << "Error: Unable to create file\n";
        return;
    }

    cashflow_file << "Date,Cashflow\n";

    double cash = 0;

    for (int i = 0; i < data.size() - 1; i++)
    {
        if (data[i].direction == "BUY")
        {
            cash -= data[i].price;
        }
        else if (data[i].direction == "SELL")
        {
            cash += data[i].price;
        }
        if (i == data.size() - 2)
        {
            cashflow_file << data[i].date << "," << cash << "\n";
            continue;
        }
        if (data[i].date != data[i + 1].date)
        {
            cashflow_file << data[i].date << "," << cash << "\n";
        }
    }

    cash += data[data.size() - 1].price;

    cashflow_file.close();

    ofstream final_pnl_file(pnl);

    if (!final_pnl_file.is_open())
    {
        cerr << "Error: Unable to create file\n";
        return;
    }

    final_pnl_file << cash << "\n";

    final_pnl_file.close();
}

void ama_trader(string input_file, string cashflow_file, string stats_file, string pnl_file, int x, int n, double c1, double c2, int p, int max_hold_days)
{
    vector<dma_pp_trade> trades = ama_read_input(input_file);
    vector<dma_pp_trade> sol = ama_strategy(trades, x, n, c1, c2, p, max_hold_days);
    ama_write_output(sol, cashflow_file, stats_file, pnl_file);
}