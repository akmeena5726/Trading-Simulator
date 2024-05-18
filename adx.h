#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
using namespace std;

struct adx_trade
{
    std::string date;
    std::string direction;
    double price;
    double sell_price;
    double macd;
    double high;
    double low;
    double ADX;
};

double calc_ewm(double prev_ewm, double param, int n)
{
    double alpha = double(2) / double(n + 1);
    double ewm = (alpha * (param - prev_ewm)) + prev_ewm;
    return ewm;
}

double get_max(vector<double> arr)
{
    double max = arr[0];
    for (int i = 1; i < arr.size(); i++)
    {
        if (arr[i] > max)
        {
            max = arr[i];
        }
    }
    return max;
}

vector<pair<double, bool>> calc_adx_list(vector<adx_trade> data, int n)
{
    vector<double> tr_list;
    vector<double> dmh_list;
    vector<double> dml_list;
    vector<double> dih_list;
    vector<double> dil_list;
    vector<double> dx_list;
    vector<pair<double, bool>> adx_list;
    vector<double> atr_list;

    for (int i = 1; i < data.size(); i++)
    {
        double tr = get_max({(data[i].high - data[i - 1].price), (data[i].high - data[i].low), (data[i].low - data[i - 1].price)});
        tr_list.push_back(tr);
        double dmh = max(data[i].high - data[i - 1].high, 0.0);
        dmh_list.push_back(dmh);
        double dml = max(data[i].low - data[i - 1].low, 0.0);
        dml_list.push_back(dml);
        double atr, dih, dil;
        if (i == 1)
        {
            atr = tr;
            atr_list.push_back(tr);
            dih = atr == 0 ? 0 : dmh / atr;
            dih_list.push_back(dih);
            dil = atr == 0 ? 0 : dml / atr;
            dil_list.push_back(dil);
        }
        else
        {
            atr = calc_ewm(atr_list.back(), tr, n);
            atr_list.push_back(atr);
            dih = atr == 0 ? calc_ewm(dih_list.back(), 0, n) : calc_ewm(dih_list.back(), dmh / atr, n);
            dih_list.push_back(dih);
            dil = atr == 0 ? calc_ewm(dil_list.back(), 0, n) : calc_ewm(dil_list.back(), dml / atr, n);
            dil_list.push_back(dil);
        }

        double dx = (dih + dil == 0) ? 0 : ((dih - dil) / (dih + dil)) * 100.0;
        dx_list.push_back(dx);
        bool flag;
        if (dih + dil == 0)
            flag = true;
        else
            flag = false;

        double adx;
        if (i == 1)
        {
            adx = dx;
            adx_list.push_back({adx, flag});
        }
        else
        {
            adx = calc_ewm(adx_list.back().first, dx, n);
            adx_list.push_back({adx, flag});
        }
        // cout << "ADX: " << adx << endl;
    }
    return adx_list;
}

vector<adx_trade> adx_strategy(vector<adx_trade> data, int x, int n, double adx_threshold)
{
    vector<adx_trade> sol;
    int pos = 0;
    vector<pair<double, bool>> adx_list = calc_adx_list(data, n);

    for (int i = 1; i < data.size(); i++)
    {
        if ((adx_list[i - 1].first >= adx_threshold) && (!adx_list[i - 1].second) && (pos + 1 <= x))
        {
            adx_trade temp;
            temp = data[i];
            temp.direction = "BUY";
            sol.push_back(temp);
            pos++;
        }
        else if ((adx_list[i - 1].first <= adx_threshold) && (!adx_list[i - 1].second) && (pos - 1 >= -x))
        {
            adx_trade temp;
            temp = data[i];
            temp.direction = "SELL";
            sol.push_back(temp);
            pos--;
        }
        else
        {
            adx_trade temp;
            temp = data[i];
            temp.direction = "HOLD";
            sol.push_back(temp);
        }
    }
    double change = (data[data.size() - 1].price) * pos;
    adx_trade temp;
    temp = data[data.size() - 1];
    temp.direction = "SQUARE_OFF";
    temp.price = change;
    sol.push_back(temp);
    return sol;
}

string adx_change_date_format(string date) // change date format from yyyy-mm-dd to dd-mm-yyyy
{
    string year = date.substr(0, 4), month = date.substr(5, 2), day = date.substr(8, 2);
    return day + "/" + month + "/" + year;
}

vector<adx_trade> adx_read_input(string filename)
{
    ifstream file(filename);

    vector<adx_trade> data;
    string line;

    getline(file, line);

    while (getline(file, line))
    {
        stringstream ss(line);
        string date;
        string direction;
        double price, high, low;

        // date,price,high,low
        getline(ss, date, ',');
        ss >> price;
        ss.ignore();
        ss >> high;
        ss.ignore();
        ss >> low;

        adx_trade temp;
        temp.date = date;
        temp.price = price;
        temp.high = high;
        temp.low = low;

        data.push_back(temp);
    }

    return data;
}

void adx_write_output(vector<adx_trade> data, string cash_file, string stats, string pnl)
{
    ofstream cashflow_file(cash_file);

    if (!cashflow_file.is_open())
    {
        cout << "Error opening file" << endl;
        return;
    }

    cashflow_file << "Date,Cashflow" << endl;

    double cashflow = 0;

    for (int i = 0; i < data.size(); i++)
    {
        if (data[i].direction == "BUY")
        {
            cashflow -= data[i].price;
        }
        else if (data[i].direction == "SELL")
        {
            cashflow += data[i].price;
        }
        if (i == data.size() - 1)
        {
            cashflow += data[i].price;
            continue;
        }
        cashflow_file << adx_change_date_format(data[i].date) << "," << cashflow << endl;
    }

    cashflow_file.close();

    ofstream pnl_file(pnl);

    if (!pnl_file.is_open())
    {
        cout << "Error in creating final_pnl.txt" << endl;
        return;
    }

    pnl_file << cashflow << endl;

    pnl_file.close();

    ofstream stats_file(stats);

    if (!stats_file.is_open())
    {
        cout << "Error in creating order_statistics.csv" << endl;
        return;
    }

    stats_file << "Date,Order_dir,Quantity,Price" << endl;

    for (int i = 0; i < data.size(); i++)
    {
        if (data[i].direction == "BUY" || data[i].direction == "SELL")
        {
            stats_file << adx_change_date_format(data[i].date) << "," << data[i].direction << "," << 1 << "," << data[i].price << endl;
        }
    }

    stats_file.close();
}

void adx_trader(string input_file, string cashflow_file, string stats_file, string pnl_file, int x, int n, double adx_threshold)
{
    vector<adx_trade> data = adx_read_input(input_file);
    vector<adx_trade> sol = adx_strategy(data, x, n, adx_threshold);
    adx_write_output(sol, cashflow_file, stats_file, pnl_file);
}
