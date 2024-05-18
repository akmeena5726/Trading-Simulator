#include "linear_regression.h"

int main(int argc, char *argv[])
{
    lr_trader("trade_data.csv", "daily_cashflow.csv", "order_statistics.csv", "final_pnl.txt", stoi(argv[1]), stod(argv[2]));
    return 0;
}