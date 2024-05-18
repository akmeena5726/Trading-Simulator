#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <chrono>
using namespace std;

struct Trade
{
    string date;
    string direction;
    string direction1;
    string direction2;
    bool threshold_cross;
    double rolling_mean;
    double sd;
    double sold_price;
    double bought_price;
    string sold_date;
    string bought_date;
    double Z_score;
    double price1;
    double price2;
    double spread;
};
struct today_trade
{
    string today_date;
    vector<Trade> sold;
    vector<Trade> bought;
};
void spread_updater(vector<Trade> &vec, double spread)
{
    for (int i = 0; i < vec.size(); i++)
    {
        vec[i].spread = spread;
    }
}
void threshold_cross_updater(vector<Trade> &vec, double stop_loss_threshold)
{
    for (int i = 0; i < vec.size(); i++)
    {
        double newz = (vec[i].spread - vec[i].rolling_mean) / (vec[i].sd);
        if (abs(newz) > stop_loss_threshold)
        {
            vec[i].threshold_cross = true;
        }
        else
        {
            vec[i].threshold_cross = false;
        }
    }
}

double sd_cal(double mean, double sumOfSquares, double n)
{
    return sqrt((sumOfSquares / n) - (mean * mean));
}

void spreader(vector<Trade> &vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        vec[i].spread = vec[i].price1 - vec[i].price2;
    }
}
vector<today_trade> stop_loss_strategy(vector<Trade> &vec, int n, int x, double threshold, double stop_loss_threshold)
{

    double cummulative_spread = 0;
    double sumOfSquares = 0.0;
    vector<today_trade> sol;
    vector<Trade> positions;
    double average = 0;
    int pos = 0;
    spreader(vec);
    for (int i = 0; i < vec.size(); i++)
    {
        today_trade tt;
        tt.today_date = vec[i].date;
        if (i < n - 1)
        {
            sumOfSquares = sumOfSquares + (vec[i].spread) * (vec[i].spread);
            cummulative_spread = cummulative_spread + (vec[i].spread);
        }
        else
        {
            cummulative_spread = (cummulative_spread - vec[i - n].spread + vec[i].spread);
            average = cummulative_spread / double(n);

            sumOfSquares += (vec[i].spread)(vec[i].spread) - ((vec[i - n].spread)(vec[i - n].spread));

            double sd = sd_cal(average, sumOfSquares, double(n));

            Trade temp = vec[i];
            double Z_score = (temp.spread - average) / sd;
            if (Z_score > threshold and pos > -x)
            {
                pos--;
                temp.direction = "sell";
                temp.direction1 = "sell";
                temp.direction2 = "buy";
                positions.push_back(temp);
                temp.sold_price = vec[i].spread;
                temp.sold_date = vec[i].date;
                tt.sold.push_back(temp);
            }
            else if (Z_score < -threshold and pos < x)
            {
                pos++;
                temp.direction1 = "buy";
                temp.direction2 = "sell";
                positions.push_back(temp);
                temp.bought_price = vec[i].spread;
                temp.bought_date = vec[i].date;
                tt.bought.push_back(temp);
            }
            spread_updater(positions, vec[i].spread);
            threshold_cross_updater(positions, stop_loss_threshold);
            vector<Trade> newpositions;
            for (int y = 0; y < positions.size(); y++)
            {
                if (positions[y].threshold_cross)
                {
                    if (positions[y].direction == "sell")
                    {
                        positions[y].bought_price = positions[y].spread;
                        tt.bought.push_back(positions[y]);
                    }
                    else
                    {
                        positions[y].sold_price = positions[y].spread;
                        tt.sold.push_back(positions[y]);
                    }
                }
                else
                {
                    newpositions.push_back(positions[y]);
                }
            }
            positions = newpositions;
        }
        sol.push_back(tt);
    }
    double leftbuy = 0.0;
    double leftsell = 0.0;
    if (positions.size() != 0)
    {
        for (int i = 0; i < positions.size(); i++)
        {
            if (positions[i].direction == "sell")
            {
                leftbuy = leftbuy + positions[i].spread;
            }
            else
            {
                leftsell = leftsell + positions[i].spread;
            }
        }
        today_trade tt;

        Trade temp1;
        temp1.bought_price = leftbuy;
        tt.bought.push_back(temp1);
        Trade temp2;
        temp2.sold_price = leftsell;
        tt.sold.push_back(temp2);
        sol.push_back(tt);
    }

    return sol;
}



vector<Trade> pairs1_read_input(string filename_1, string filename_2)
{
    ifstream file(filename_1);
    vector<Trade> vec;
    string line;
    getline(file, line);
    while (getline(file, line))
    {
        Trade temp;
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

void pairs2_write_output(vector<today_trade> sol)
{
    ofstream file("daily_cashflow.csv");
    file << "Date,Cashflow\n";
    double cash = 0;
    for (int i = 0; i < sol.size(); i++)
    {
        vector<Trade> sold = sol[i].sold;
        vector<Trade> bought = sol[i].bought;
        for (auto s: sold)
        {
            cash += s.sold_price;
        }
        for (auto b: bought)
        {
            cash -= b.bought_price;
        }
        if (i == sol.size() -1)
        {
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
        vector<Trade> sold = sol[i].sold;
        vector<Trade> bought = sol[i].bought;
        
        double cash = 0;
        for (auto s: sold)
        {
            stats1 << pairs1_change_date_format(s.sold_date) << "," << s.direction1 << "," << 1 << "," << s.sold_price << "\n";
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

void pairs2_trader(string input_file_1, string input_file_2, int n, int x, double threshold, double stop_loss_threshold)
{
    vector<Trade> vec1 = pairs1_read_input(input_file_1, input_file_2);
    vector<today_trade> sol = stop_loss_strategy(vec1, n, x, threshold, stop_loss_threshold);
    pairs2_write_output(sol);
}

int main(int argc, char *argv[])
{
    pairs2_trader("trade_data_1.csv", "trade_data_2.csv", stoi(argv[1]), stoi(argv[2]), stod(argv[3]), stod(argv[4]));
    return 0;
}