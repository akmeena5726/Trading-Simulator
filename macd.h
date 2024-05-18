#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

struct macd_trade
{
    std::string date;
    std::string direction;
    int quantity;
    double price;
    double SELL_price;
    double macd;
    double sig;
};

double Emwcal(vector<macd_trade> vec, int n, double prevEmw, double price)
{
    double aplha = double(2) / double(n + 1);
    double emw = aplha * (price - prevEmw) + prevEmw;
    return emw;
}

vector<macd_trade> macd(vector<macd_trade> vec)
{
    double longemw = vec[0].price;
    double shortemw = vec[0].price;
    vector<macd_trade> sol;
    vector<macd_trade> mac;
    for (int i = 0; i < vec.size(); i++)
    {
        longemw = Emwcal(vec, 26, longemw, vec[i].price);
        shortemw = Emwcal(vec, 12, shortemw, vec[i].price);
        macd_trade temp = vec[i];
        temp.macd = shortemw - longemw;
        mac.push_back(temp);
    }
    double sig = mac[0].macd;
    for (int i = 0; i < mac.size(); i++)
    {
        double sig = Emwcal(mac, 9, sig, mac[i].macd);
        mac[i].sig = sig;
        sol.push_back(mac[i]);
        // cout<<sig<<endl;
    }
    return sol;
}

vector<macd_trade> startegy_macd(int x, vector<macd_trade> vec)
{
    vector<macd_trade> mac = macd(vec);
    vector<macd_trade> sol;
    int pos = 0;
    macd_trade first_day;
    first_day.price = 0.0;
    first_day.date = vec[0].date;
    sol.push_back(first_day);
    for (int i = 1; i < mac.size(); i++)
    {
        if (mac[i].macd > mac[i].sig)
        {
            if (pos < x)
            {
                pos++;
                mac[i].direction = "BUY";
                sol.push_back(mac[i]);
            }
            else
            {
                sol.push_back(mac[i]);
            }
        }
        else if (mac[i].macd < mac[i].sig)
        {
            if (pos > -x)
            {
                pos--;
                mac[i].direction = "SELL";
            }
            sol.push_back(mac[i]);
        }
        else
        {
            sol.push_back(mac[i]);
        }
    }
    double change = pos * (mac[mac.size() - 1].price);
    macd_trade temp;
    temp.price = change;
    sol.push_back(temp);
    return sol;
}

vector<macd_trade> macd_read_input(string filename)
{
    ifstream file(filename);
    vector<macd_trade> vec;
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        macd_trade temp;
        stringstream ss(line);
        getline(ss, temp.date, ',');
        ss >> temp.price;

        vec.push_back(temp);
    }
    return vec;
}

string macd_change_date_format(string date) // change date format from yyyy-mm-dd to dd-mm-yyyy
{
    string year = date.substr(0, 4), month = date.substr(5, 2), day = date.substr(8, 2);
    return day + "/" + month + "/" + year;
}

void macd_write_output(string cashflow_file, string stats_file, string pnl_file, vector<macd_trade> sol)
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
        file << macd_change_date_format(sol[i].date) << "," << cash << "\n";
    }
    file.close();

    ofstream stats(stats_file);
    stats << "Date,Order_dir,Quantity,Price\n";
    for (int i = 0; i < sol.size(); i++)
    {
        if (sol[i].direction == "BUY")
        {
            stats << macd_change_date_format(sol[i].date) << "," << sol[i].direction << "," << 1 << "," << sol[i].price << "\n";
        }
        else if (sol[i].direction == "SELL")
        {
            stats << macd_change_date_format(sol[i].date) << "," << sol[i].direction << "," << 1 << "," << sol[i].price << "\n";
        }
    }
    stats.close();

    ofstream pnl(pnl_file);
    pnl << cash;
    pnl.close();
}

void macd_trader(string input_file, string cashflow_file, string stats_file, string pnl_file, int x)
{
    vector<macd_trade> vec = macd_read_input(input_file);
    vector<macd_trade> sol = startegy_macd(x, vec);
    macd_write_output(cashflow_file, stats_file, pnl_file, sol);
}