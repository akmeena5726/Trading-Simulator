#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>
using namespace std;

struct pt1_trade
{
    std::string date;
    std::string direction1;
    string direction2;
    double price1;
    double price2;
    double spread;
};

double sd_cal(double mean, double sumOfSquares, double n)
{
    return sqrt((sumOfSquares / n) - (mean * mean));
}

void spreader(vector<pt1_trade> &vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        vec[i].spread = vec[i].price1 - vec[i].price2;
    }
}

vector<pt1_trade> pair_strategy(vector<pt1_trade> vec, int n, int x, double threshold)
{

    double cummulative_spread = 0;
    vector<pt1_trade> sol;
    double average = 0;
    int pos = 0;
    double sumOfSquares = 0;
    spreader(vec);
    for (int i = 0; i < vec.size(); i++)
    {
        if (i < n - 1)

        {
            sumOfSquares = sumOfSquares + (vec[i].spread) * (vec[i].spread);
            cummulative_spread = cummulative_spread + (vec[i].spread);
        }
        else
        {
            cummulative_spread = (cummulative_spread - vec[i - n].spread + vec[i].spread);
            average = cummulative_spread / double(n);

            sumOfSquares += (vec[i].spread) * (vec[i].spread) - ((vec[i - n].spread) * (vec[i - n].spread));

            double sd = sd_cal(average, sumOfSquares, double(n));

            pt1_trade temp = vec[i];
            double Z_score = (temp.spread - average) / sd;
            if ((Z_score > threshold) and (pos > -x))
            {
                pos--;
                temp.direction1 = "SELL";
                temp.direction2 = "BUY";
                sol.push_back(temp);
            }
            else if ((Z_score < -threshold) and (pos < x))
            {
                pos++;
                temp.direction1 = "BUY";
                temp.direction2 = "SELL";
                sol.push_back(temp);
            }
            else
            {
                temp.direction1 = "NONE";
                temp.direction2 = "NONE";
                sol.push_back(temp);
            }
        }
    }

    double change = vec[vec.size() - 1].spread * pos;
    pt1_trade temp;
    temp.date = vec[vec.size() - 1].date;
    temp.spread = change;
    sol.push_back(temp);

    return sol;
}

vector<pt1_trade> pairs1_read_input(string filename_1, string filename_2)
{
    ifstream file(filename_1);
    vector<pt1_trade> vec;
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        pt1_trade temp;
        stringstream ss(line);
        getline(ss, temp.date, ',');
        ss >> temp.price1;

        vec.push_back(temp);
    }

    ifstream file2(filename_2);
    string line2;
    getline(file2, line2);
    int i = 0;
    while (getline(file2, line2))
    {
        stringstream ss(line2);
        string date;
        double price;
        getline(ss, date, ',');
        ss >> price;
        vec[i].price2 = price;
        i++;
    }
    return vec;
}

string pairs1_change_date_format(string date) // change date format from yyyy-mm-dd to dd-mm-yyyy
{
    string year = date.substr(0, 4), month = date.substr(5, 2), day = date.substr(8, 2);
    return day + "/" + month + "/" + year;
}

void pairs1_write_output(vector<pt1_trade> sol)
{
    ofstream file("daily_cashflow.csv");
    file << "Date,Cashflow\n";
    double cash = 0;
    for (int i = 0; i < sol.size(); i++)
    {
        if (sol[i].direction1 == "BUY")
        {
            cash -= sol[i].price1;
        }
        else if (sol[i].direction1 == "SELL")
        {
            cash += sol[i].price1;
        }
        if (sol[i].direction2 == "BUY")
        {
            cash -= sol[i].price2;
        }
        else if (sol[i].direction2 == "SELL")
        {
            cash += sol[i].price2;
        }
        if (i == sol.size() - 1)
        {
            cash += sol[i].spread;
            continue;
        }
        file << pairs1_change_date_format(sol[i].date) << "," << to_string(cash) << "\n";
    }
    file.close();

    ofstream pnl("final_pnl.txt");
    pnl << cash;
    pnl.close();

    ofstream stats1("order_statistics_1.csv");
    stats1 << "Date,Order_dir,Quantity,Price\n";
    for (int i = 0; i < sol.size(); i++)
    {
        if (sol[i].direction1 == "BUY")
        {
            stats1 << pairs1_change_date_format(sol[i].date) << "," << sol[i].direction1 << "," << 1 << "," << sol[i].price1 << "\n";
        }
        else if (sol[i].direction1 == "SELL")
        {
            stats1 << pairs1_change_date_format(sol[i].date) << "," << sol[i].direction1 << "," << 1 << "," << sol[i].price1 << "\n";
        }
    }
    stats1.close();

    ofstream stats2("order_statistics_2.csv");
    stats2 << "Date,Order_dir,Quantity,Price\n";
    for (int i = 0; i < sol.size(); i++)
    {
        if (sol[i].direction2 == "BUY")
        {
            stats2 << pairs1_change_date_format(sol[i].date) << "," << sol[i].direction2 << "," << 1 << "," << sol[i].price2 << "\n";
        }
        else if (sol[i].direction2 == "SELL")
        {
            stats2 << pairs1_change_date_format(sol[i].date) << "," << sol[i].direction2 << "," << 1 << "," << sol[i].price2 << "\n";
        }
    }
    stats2.close();
}

void pairs1_trader(string input_file_1, string input_file_2, int n, int x, double threshold)
{
    vector<pt1_trade> vec1 = pairs1_read_input(input_file_1, input_file_2);
    vector<pt1_trade> sol = pair_strategy(vec1, n, x, threshold);
    pairs1_write_output(sol);
}

int main(int argc, char *argv[])
{
    pairs1_trader("trade_data_1.csv", "trade_data_2.csv", stoi(argv[1]), stoi(argv[2]), stod(argv[3]));
    return 0;
}