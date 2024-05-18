#include "rsi.h"

int main(int argc, char *argv[])
{
    rsi_trader("trade_data.csv", "daily_cashflow.csv", "order_statistics.csv", "final_pnl.txt", stoi(argv[1]), stoi(argv[2]), stoi(argv[3]), stoi(argv[4]));
    return 0;
}