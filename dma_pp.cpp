#include "dma_pp.h"

int main(int argc, char *argv[])
{
    ama_trader("trade_data.csv", "daily_cashflow.csv", "order_statistics.csv", "final_pnl.txt", stoi(argv[1]), stoi(argv[2]), stod(argv[3]), stod(argv[4]), stoi(argv[5]), stoi(argv[6]));
    return 0;
}