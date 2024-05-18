#include "adx.h"

int main(int argc, char *argv[])
{
    adx_trader("trade_data.csv", "daily_cashflow.csv", "order_statistics.csv", "final_pnl.txt", stoi(argv[1]), stoi(argv[2]), stod(argv[3]));
}