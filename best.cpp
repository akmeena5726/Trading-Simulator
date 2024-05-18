#include "basic.h"
#include "adx.h"
#include "dma_pp.h"
#include "dma.h"
#include "macd.h"
#include "rsi.h"
#include "linear_regression.h"
#include <thread>

void basic_func(void *arg)
{
    basic_trader("basic_trade_data.csv", "basic_daily_cashflow.csv", "basic_order_statistics.csv", "basic_final_pnl.txt", 5, 7);
}

void adx_func(void *arg)
{
    adx_trader("adx_trade_data.csv", "adx_daily_cashflow.csv", "adx_order_statistics.csv", "adx_final_pnl.txt", 5, 14, 25);
}

void dma_pp_func(void *arg)
{
    ama_trader("ama_trade_data.csv", "ama_daily_cashflow.csv", "ama_order_statistics.csv", "ama_final_pnl.txt", 5, 14, 2, 0.2, 5, 28);
}

void dma_func(void *arg)
{
    dma_trader("dma_trade_data.csv", "dma_daily_cashflow.csv", "dma_order_statistics.csv", "dma_final_pnl.txt", 5, 50, 2);
}

void macd_func(void *arg)
{
    macd_trader("macd_trade_data.csv", "macd_daily_cashflow.csv", "macd_order_statistics.csv", "macd_final_pnl.txt", 5);
}

void rsi_func(void *arg)
{
    rsi_trader("rsi_trade_data.csv", "rsi_daily_cashflow.csv", "rsi_order_statistics.csv", "rsi_final_pnl.txt", 5, 14, 30, 70);
}

void linear_regression_func(void *arg)
{
    lr_trader("lr_trade_data.csv", "lr_daily_cashflow.csv", "lr_order_statistics.csv", "lr_final_pnl.txt", 5, 2);
}

void clean(vector<string> files)
{
    cout << "Cleaning temporary files" << endl;
    for(string file: files)
    {
        remove((file + "_trade_data.csv").c_str());
        remove((file + "_daily_cashflow.csv").c_str());
        remove((file + "_order_statistics.csv").c_str());
        remove((file + "_final_pnl.txt").c_str());
    }
}

int main()
{
    thread basic_thread(basic_func, nullptr);
    thread adx_thread(adx_func, nullptr);
    thread dma_pp_thread(dma_pp_func, nullptr);
    thread dma_thread(dma_func, nullptr);
    thread macd_thread(macd_func, nullptr);
    thread rsi_thread(rsi_func, nullptr);
    thread linear_regression_thread(linear_regression_func, nullptr);

    basic_thread.join();
    adx_thread.join();
    dma_pp_thread.join();
    dma_thread.join();
    macd_thread.join();
    rsi_thread.join();
    linear_regression_thread.join();

    // get best final pnl
    ifstream basic_file("basic_final_pnl.txt");
    ifstream adx_file("adx_final_pnl.txt");
    ifstream dma_pp_file("dma_final_pnl.txt");
    ifstream dma_file("dma_final_pnl.txt");
    ifstream macd_file("macd_final_pnl.txt");
    ifstream rsi_file("rsi_final_pnl.txt");
    ifstream linear_regression_file("linear_regression_final_pnl.txt");

    cout << "Reading final pnl" << endl;
    double basic_pnl, adx_pnl, dma_pp_pnl, dma_pnl, macd_pnl, rsi_pnl, linear_regression_pnl;
    basic_file >> basic_pnl;
    adx_file >> adx_pnl;
    dma_pp_file >> dma_pp_pnl;
    dma_file >> dma_pnl;
    macd_file >> macd_pnl;
    rsi_file >> rsi_pnl;
    linear_regression_file >> linear_regression_pnl;

    double best_pnl = basic_pnl;
    int best_pnl_index = 0;
    vector<double> pnl = {basic_pnl, adx_pnl, dma_pp_pnl, dma_pnl, macd_pnl, rsi_pnl, linear_regression_pnl};
    for (int i = 0; i < pnl.size(); i++)
    {
        if (pnl[i] > best_pnl)
        {
            best_pnl = pnl[i];
            best_pnl_index = i;
        }
    }

    ofstream final_pnl_file("final_pnl.txt");
    final_pnl_file << best_pnl << endl;

    // copy order statistics file of best strategy to final order statistics file and also the cashflow file
    ifstream best_order_statistics_file;
    ofstream final_order_statistics_file("order_statistics.csv");
    switch (best_pnl_index)
    {
    case 0:
        best_order_statistics_file.open("basic_order_statistics.csv");
        break;
    case 1:
        best_order_statistics_file.open("adx_order_statistics.csv");
        break;
    case 2:
        best_order_statistics_file.open("dma_pp_order_statistics.csv");
        break;
    case 3:
        best_order_statistics_file.open("dma_order_statistics.csv");
        break;
    case 4:
        best_order_statistics_file.open("macd_order_statistics.csv");
        break;
    case 5:
        best_order_statistics_file.open("rsi_order_statistics.csv");
        break;
    case 6:
        best_order_statistics_file.open("linear_regression_order_statistics.csv");
        break;
    }

    final_order_statistics_file << best_order_statistics_file.rdbuf();
    best_order_statistics_file.close();

    ifstream best_cashflow_file;
    ofstream final_cashflow_file("daily_cashflow.csv");

    switch (best_pnl_index)
    {
    case 0:
        best_cashflow_file.open("basic_daily_cashflow.csv");
        break;
    case 1:
        best_cashflow_file.open("adx_daily_cashflow.csv");
        break;
    case 2:
        best_cashflow_file.open("dma_pp_daily_cashflow.csv");
        break;
    case 3:
        best_cashflow_file.open("dma_daily_cashflow.csv");
        break;
    case 4:
        best_cashflow_file.open("macd_daily_cashflow.csv");
        break;
    case 5:
        best_cashflow_file.open("rsi_daily_cashflow.csv");
        break;
    case 6:
        best_cashflow_file.open("linear_regression_daily_cashflow.csv");
        break;
    }

    final_cashflow_file << best_cashflow_file.rdbuf();
    best_cashflow_file.close();

    // delete all the temporary files
    vector<string> files = {"basic", "adx", "ama", "dma", "macd", "rsi", "lr"};
    clean(files);
    remove("lr_train_data.csv");
    return 0;
}