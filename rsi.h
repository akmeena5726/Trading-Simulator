#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>
using namespace std;

struct rsi_trade
{
    std::string date;
    std::string direction;
    int quantity;
    double price;
    double sell_price;
};

vector<rsi_trade> rsi_strategy(vector<rsi_trade> data, int x, int n, double oversold_threshold, double overbought_threshold)
{
    double average_gain = 0;
    double average_loss = 0;

    vector<rsi_trade> sol;
    int pos = 0;

    // initial gain and loss in sliding window
    double initial_gain = 0;
    double initial_loss = 0;

    for (int i = 1; i <= n; i++)
    {
        if (i == 1)
        {
            initial_gain = max(data[i].price - data[i - 1].price, 0.0);
            initial_loss = max(data[i - 1].price - data[i].price, 0.0);
        }
        average_gain += max(data[i].price - data[i - 1].price, 0.0);
        average_loss += max(data[i - 1].price - data[i].price, 0.0);
    }

    average_gain /= n;
    average_loss /= n;

    for (int i = n; i < data.size(); i++)
    {
        if (i > n)
        {
            average_gain = average_gain + (max(data[i].price - data[i - 1].price, 0.0) - initial_gain) / n;
            average_loss = average_loss + (max(data[i - 1].price - data[i].price, 0.0) - initial_loss) / n;

            initial_gain = max(data[i - n + 1].price - data[i - n].price, 0.0);
            initial_loss = max(data[i - n].price - data[i - n + 1].price, 0.0);
        }
        double relative_strength = average_gain / average_loss;
        double rsi = 100 - (100 / (1 + relative_strength));

        if (rsi < oversold_threshold)
        {
            int temp = pos + 1;
            if (temp <= x)
            {
                rsi_trade temp;
                temp.date = data[i].date;
                temp.direction = "BUY";
                temp.price = data[i].price;
                sol.push_back(temp);
                pos++;
            }
            else
            {
                rsi_trade temp = data[i];
                temp.price = data[i].price;
                sol.push_back(temp);
            }
        }
        else if (rsi > overbought_threshold)
        {
            int temp = pos - 1;
            if (temp >= -x)
            {
                rsi_trade temp;
                temp.date = data[i].date;
                temp.direction = "SELL";
                temp.price = data[i].price;
                sol.push_back(temp);
                pos--;
            }
            else
            {
                rsi_trade temp = data[i];
                temp.price = data[i].price;
                sol.push_back(temp);
            }
        }
        else
        {
            rsi_trade temp = data[i];
            temp.price = data[i].price;
            sol.push_back(temp);
        }
    }

    // squaring off the position at the end
    rsi_trade temp = data[data.size() - 1];
    temp.price = data[data.size() - 1].price * pos;
    sol.push_back(temp);

    return sol;
}

vector<rsi_trade> rsi_read_input(string filename)
{
    ifstream file(filename);
    vector<rsi_trade> vec;
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        rsi_trade temp;
        stringstream ss(line);
        getline(ss, temp.date, ',');
        ss >> temp.price;

        vec.push_back(temp);
    }
    return vec;
}

string rsi_change_date_format(string date) // change date format from yyyy-mm-dd to dd-mm-yyyy
{
    string year = date.substr(0, 4), month = date.substr(5, 2), day = date.substr(8, 2);
    return day + "/" + month + "/" + year;
}

void rsi_write_output(string cashflow_file, string stats_file, string pnl_file, vector<rsi_trade> sol)
{
    ofstream file(cashflow_file);
    file << "Date,Cashflow\n";
    double cash = 0;
    for (int i = 0; i < sol.size(); i++)
    {
        if (sol[i].direction == "BUY")
        {
            cash -= sol[i].price;
        }
        else if (sol[i].direction == "SELL")
        {
            cash += sol[i].price;
        }
        if (i == sol.size() - 1)
        {
            cash += sol[i].price;
            continue;
        }
        file << rsi_change_date_format(sol[i].date) << "," << cash << "\n";
    }
    file.close();

    ofstream stats(stats_file);
    stats << "Date,Order_dir,Quantity,Price\n";
    for (int i = 0; i < sol.size(); i++)
    {
        if (sol[i].direction == "BUY")
        {
            stats << rsi_change_date_format(sol[i].date) << "," << sol[i].direction << "," << 1 << "," << sol[i].price << "\n";
        }
        else if (sol[i].direction == "SELL")
        {
            stats << rsi_change_date_format(sol[i].date) << "," << sol[i].direction << "," << 1 << "," << sol[i].price << "\n";
        }
    }
    stats.close();

    ofstream pnl(pnl_file);
    pnl << cash;
    pnl.close();
}

void rsi_trader(string input_file, string cashflow_file, string stats_file, string pnl_file, int x, int n, int oversold_threshold, int overbought_threshold)
{
    vector<rsi_trade> vec = rsi_read_input(input_file);
    vector<rsi_trade> sol = rsi_strategy(vec, x, n, oversold_threshold, overbought_threshold);
    rsi_write_output(cashflow_file, stats_file, pnl_file, sol);
}