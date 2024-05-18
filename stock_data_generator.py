import sys
from datetime import timedelta as td, datetime
from jugaad_data.nse import stock_df
import pandas as pd


def generate_data_basic(symbol, n, start_date, end_date):
    data = stock_df(symbol, start_date, end_date)
    data_len = len(data)
    data = data.iloc[::-1]
    while len(data) < data_len + n - 1:
        start_date = start_date - td(days=1)
        try:
            dt = stock_df(symbol, start_date, start_date)
        except:
            continue
        # append this data to the original data
        # first we need to remove the first row of the new data
        data = pd.concat([dt, data], ignore_index=True)

    data = data[["DATE", "CLOSE"]]
    return data

def generate_data_rsi(symbol, n, start_date, end_date):
    data = stock_df(symbol, start_date, end_date)
    data_len = len(data)
    data = data.iloc[::-1]
    while len(data) < data_len + n:
        start_date = start_date - td(days=1)
        try:
            dt = stock_df(symbol, start_date, start_date)
        except:
            continue
        # append this data to the original data
        # first we need to remove the first row of the new data
        data = pd.concat([dt, data], ignore_index=True)

    data = data[["DATE", "CLOSE"]]
    return data

def generate_data_adx(symbol, start_date, end_date):
    data = stock_df(symbol, start_date, end_date)
    # reverse the data
    data = data.iloc[::-1]
    len_data = len(data)
    while len(data) == len_data:
        start_date = start_date - td(days=1)
        try:
            dt = stock_df(symbol, start_date, start_date)
        except:
            continue
        data = pd.concat([dt, data], ignore_index=True)
    data = data[["DATE", "CLOSE", "HIGH", "LOW"]]
    return data

def generate_data(symbol, start_date, end_date):
    data = stock_df(symbol, start_date, end_date)
    data = data[["DATE", "CLOSE"]]
    data = data.iloc[::-1]
    return data


def generate_data_lr(symbol, start_date, end_date):
    data = stock_df(symbol, start_date, end_date)
    # reverse the data
    data = data.iloc[::-1]
    len_data = len(data)
    while len(data) == len_data:
        start_date = start_date - td(days=1)
        try:
            dt = stock_df(symbol, start_date, start_date)
        except:
            continue
        data = pd.concat([dt, data], ignore_index=True)
    data = data[["DATE", "CLOSE", "OPEN", "VWAP", "HIGH", "LOW", "NO OF TRADES"]]
    return data


# create a to generate date object from string (DD/MM/YYYY)
def create_date(date_str):
    date = datetime.strptime(date_str, "%d/%m/%Y").date()
    return date


if __name__ == "__main__":
    strategy = sys.argv[1]

    if strategy == "lg":
        symbol = sys.argv[2]
        train_start_date = create_date(sys.argv[3])
        train_end_date = create_date(sys.argv[4])
        start_date = create_date(sys.argv[5])
        end_date = create_date(sys.argv[6])
        train_data = generate_data_lr(symbol, train_start_date, train_end_date)
        data = generate_data_lr(symbol, start_date, end_date)
        train_data.to_csv("train_data.csv", index=False)
        data.to_csv("trade_data.csv", index=False)

    elif strategy == "macd":
        symbol = sys.argv[2]
        start_date = create_date(sys.argv[3])
        end_date = create_date(sys.argv[4])
        data = generate_data(symbol, start_date, end_date)
        data.to_csv("trade_data.csv", index=False)

    elif strategy == "pairs":
        symbol1 = sys.argv[2]
        symbol2 = sys.argv[3]
        start_date = create_date(sys.argv[4])
        end_date = create_date(sys.argv[5])
        n = int(sys.argv[6])
        data1 = generate_data_basic(symbol1, n, start_date, end_date)
        data2 = generate_data_basic(symbol2, n, start_date, end_date)
        data1.to_csv("trade_data_1.csv", index=False)
        data2.to_csv("trade_data_2.csv", index=False)
    
    elif strategy == "rsi" or strategy == "ama":
        symbol = sys.argv[2]
        start_date = create_date(sys.argv[3])
        end_date = create_date(sys.argv[4])
        n = int(sys.argv[5])
        data = generate_data_rsi(symbol, n, start_date, end_date)
        data.to_csv("trade_data.csv", index=False)

    elif strategy == "adx":
        symbol = sys.argv[2]
        start_date = create_date(sys.argv[3])
        end_date = create_date(sys.argv[4])
        data = generate_data_adx(symbol, start_date, end_date)
        data.to_csv("trade_data.csv", index=False)

    elif strategy == "best":
        symbol = sys.argv[2]
        start_date = create_date(sys.argv[3])
        end_date = create_date(sys.argv[4])
        # create data for basic strategy
        data = generate_data_basic(symbol, 7, start_date, end_date)
        data.to_csv("basic_trade_data.csv", index=False)
        # create data for adx strategy
        data = generate_data(symbol, start_date, end_date)
        data.to_csv("adx_trade_data.csv", index=False)
        # create data for ama strategy
        data = generate_data_basic(symbol, 14, start_date, end_date)
        data.to_csv("ama_trade_data.csv", index=False)
        # create data for dma strategy
        data = generate_data_basic(symbol, 50, start_date, end_date)
        data.to_csv("dma_trade_data.csv", index=False)
        # create data for macd strategy
        data = generate_data(symbol, start_date, end_date)
        data.to_csv("macd_trade_data.csv", index=False)
        # create data for rsi strategy
        data = generate_data_basic(symbol, 14, start_date, end_date)
        data.to_csv("rsi_trade_data.csv", index=False)
        # create data for linear regression strategy
        train_start_date = start_date - td(days=365)
        train_end_date = end_date - td(days=365)
        train_data = generate_data_lr(symbol, train_start_date, train_end_date)
        data = generate_data_lr(symbol, start_date, end_date)
        train_data.to_csv("lr_train_data.csv", index=False)
        data.to_csv("lr_trade_data.csv", index=False)

    else:
        symbol = sys.argv[2]
        start_date = create_date(sys.argv[3])
        end_date = create_date(sys.argv[4])
        n = int(sys.argv[5])
        data = generate_data_basic(symbol, n, start_date, end_date)
        data.to_csv("trade_data.csv", index=False)
