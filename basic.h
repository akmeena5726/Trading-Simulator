#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
using namespace std;

struct basic_trade
{
    string date;
    double sell_price;
    string direction;
    int quantity;
    double price;
};

vector<basic_trade> basic_strategy(vector<basic_trade> vec, int x, int n)
{
    vector<basic_trade> sol;
    int mi = 0;
    int md = 0;
    int pos = 0;
    int not_started = 0;

    for (int i = 1; i < vec.size(); i++)
    {
        if (not_started < n - 2)

        {

            not_started++;

            if (vec[i].price > vec[i - 1].price)

            {

                mi++;

                md = 0;
            }

            else if (vec[i].price < vec[i - 1].price)

            {

                mi = 0;

                md++;
            }
        }

        else

        {

            if (vec[i].price > vec[i - 1].price)

            {

                mi++;

                md = 0;

                if (mi >= n)

                {

                    pos++;

                    if (pos <= x)

                    {

                        basic_trade temp;

                        temp = vec[i];

                        temp.direction = "BUY";

                        sol.push_back(temp);
                    }

                    else

                    {
                        sol.push_back(vec[i]);

                        pos--;

                        // write condtiion for what to do if you you wont BUY or SELL
                    }
                }

                else

                {
                    sol.push_back(vec[i]);

                    // write condtiion for what to do if you you wont BUY or SELL
                }
            }

            else if (vec[i].price < vec[i - 1].price)

            {

                md++;

                mi = 0;

                if (md >= n)

                {

                    pos--;

                    if (pos >= -x)

                    {

                        basic_trade temp;

                        temp = vec[i];

                        temp.sell_price = vec[i].price;

                        temp.direction = "SELL";

                        sol.push_back(temp);
                    }

                    else

                    {

                        pos++;

                        sol.push_back(vec[i]);

                        // write condtiion for what to do if you you wont BUY or SELL
                    }
                }

                else

                {

                    // write condtiion for what to do if you you wont BUY or SELL

                    sol.push_back(vec[i]);
                }
            }

            else
            {

                mi = 0;

                md = 0;

                sol.push_back(vec[i]);
            }
        }
    }

    double change = (pos)*vec[vec.size() - 1].price;

    basic_trade temp;

    temp = vec[vec.size() - 1];

    temp.direction = "none";

    temp.price = change;

    sol.push_back(temp);

    return sol;
}

string basic_change_date_format(string date) // change date format from yyyy-mm-dd to dd-mm-yyyy
{
    string year = date.substr(0, 4), month = date.substr(5, 2), day = date.substr(8, 2);
    return day + "/" + month + "/" + year;
}

vector<basic_trade> basic_read_input(string filename)
{
    ifstream file(filename);
    vector<basic_trade> vec;
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        string date;
        double price;
        basic_trade temp;
        stringstream ss(line);
        getline(ss, date, ',');
        ss >> price;
        temp.date = date;
        temp.price = price;
        vec.push_back(temp);
    }
    return vec;
}

void basic_write_output(string cashflow_file, string stats_file, string pnl_file, vector<basic_trade> sol)
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
        file << basic_change_date_format(sol[i].date) << "," << cash << "\n";
    }
    file.close();

    ofstream stats(stats_file);
    stats << "Date,Order_dir,Quantity,Price\n";
    for (int i = 0; i < sol.size(); i++)
    {
        if (sol[i].direction == "BUY")
        {
            stats << basic_change_date_format(sol[i].date) << "," << sol[i].direction << "," << 1 << "," << sol[i].price << "\n";
        }
        else if (sol[i].direction == "SELL")
        {
            stats << basic_change_date_format(sol[i].date) << "," << sol[i].direction << "," << 1 << "," << sol[i].price << "\n";
        }
    }
    stats.close();

    ofstream pnl(pnl_file);
    pnl << cash;
    pnl.close();
}

void basic_trader(string input_file, string cashflow_file, string stats_file, string pnl_file, int x, int n)
{
    vector<basic_trade> data = basic_read_input(input_file);
    vector<basic_trade> sol = basic_strategy(data, x, n);
    basic_write_output(cashflow_file, stats_file, pnl_file, sol);
}
