#include "macd.h"

int main(int argc, char *argv[])
{
    macd_trader("trade_data.csv", "daily_cashflow.csv", "order_statistics.csv", "final_pnl.txt", stoi(argv[1]));
    return 0;
}