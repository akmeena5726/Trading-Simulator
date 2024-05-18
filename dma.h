#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>
using namespace std;

struct dma_trade
{
    string date;
    string direction;
    int quantity;
    double price;
};


double calc_standard_dev(double mean, const std::vector<dma_trade> &t, int i, int n)
{
    double sum_diff_square = 0.0;

    for (int j = i - n + 1; j <= i; j++)
    {
        double diff = mean - t[j].price;
        sum_diff_square += diff * diff;
    }

    double variance = sum_diff_square / double(n); // Bessel's correction
    double sd = std::sqrt(variance);
    return sd;
}

vector<dma_trade> dma_strategy(vector<dma_trade> data, int x, int n, double p)
{
    vector<dma_trade> result;
    int position = 0; // no. of stocks bought/sold

    double dma = 0;                // daily moving average
    double standard_deviation = 0; // standard deviation
    double first_day_price = data[0].price;
    for (int i = 0; i < n; i++)
    {
        dma += data[i].price;
    }
    dma /= n; // dma for first n days

    for (int i = 0; i < n; i++)
    {
        standard_deviation += pow(data[i].price - dma, 2);
    }
    standard_deviation = sqrt(standard_deviation / n); // standard deviation for first n days

    for (int i = n - 1; i < data.size(); i++) // start from nth day
    {
        // update dma and standard deviation
        if (i >= n)
        {
            dma = dma + (data[i].price - data[i - n].price) / n;
            standard_deviation = calc_standard_dev(dma, data, i, n);
        }

        double current_price = data[i].price; // current day price
        double diff = current_price - dma;    // difference between current price and dma
        if (diff > p * standard_deviation)    // BUY
        {
            int temp = position + 1; // check if position is less than x
            if (temp <= x)
            {
                dma_trade t = data[i];
                t.direction = "BUY";
                result.push_back(t);
                position++;
            }
            else
            {
                dma_trade t = data[i];
                result.push_back(t);
            }
        }
        else if (diff <= -p * standard_deviation) // SELL
        {
            int temp = position - 1;
            if (temp >= -x)
            {
                dma_trade t = data[i];
                t.direction = "SELL";
                result.push_back(t);
                position--;
            }
            else
            {
                dma_trade t = data[i];
                result.push_back(t);
            }
        }
        else
        {
            dma_trade t = data[i];
            result.push_back(t);
        }
    }

    dma_trade t = data[data.size() - 1];
    double change = position * t.price;
    t.price = change;
    result.push_back(t);

    return result;
}

string dma_change_date_format(string date) // change date format from yyyy-mm-dd to dd-mm-yyyy
{
    string year = date.substr(0, 4), month = date.substr(5, 2), day = date.substr(8, 2);
    return day + "/" + month + "/" + year;
}

vector<dma_trade> dma_read_input(string filename)
{
    ifstream file(filename);
    vector<dma_trade> vec;
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        dma_trade temp;
        stringstream ss(line);
        getline(ss, temp.date, ',');
        ss >> temp.price;

        vec.push_back(temp);
    }
    return vec;
}


void dma_write_output(string cashflow_file, string stats_file, string pnl_file, vector<dma_trade> sol)
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
        file << dma_change_date_format(sol[i].date) << "," << cash << "\n";
    }
    file.close();

    ofstream stats(stats_file);
    stats << "Date,Order_dir,Quantity,Price\n";
    for (int i = 0; i < sol.size(); i++)
    {
        if (sol[i].direction == "BUY")
        {
            stats << dma_change_date_format(sol[i].date) << "," << sol[i].direction << "," << 1 << "," << sol[i].price << "\n";
        }
        else if (sol[i].direction == "SELL")
        {
            stats << dma_change_date_format(sol[i].date) << "," << sol[i].direction << "," << 1 << "," << sol[i].price << "\n";
        }
    }
    stats.close();

    ofstream pnl(pnl_file);
    pnl << cash;
    pnl.close();
}

void dma_trader(string input_file, string cashflow_file, string stats_file, string pnl_file, int x, int n, double p)
{
    vector<dma_trade> data = dma_read_input(input_file);
    vector<dma_trade> sol = dma_strategy(data, x, n, p);
    dma_write_output(cashflow_file, stats_file, pnl_file, sol);
}
