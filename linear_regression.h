#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
using namespace std;

// Funtion to read the data from csv file and store it in a vector

struct stock_data
{
    string date;
    double open;
    double high;
    double low;
    double close;
    double VWAP;
    double no_of_trades;

    stock_data(string date, double open, double high, double low, double close, double VWAP, double no_of_trades)
    {
        this->date = date;
        this->open = open;
        this->high = high;
        this->low = low;
        this->close = close;
        this->VWAP = VWAP;
        this->no_of_trades = no_of_trades;
    }
};

struct lr_trade
{
    string date;
    double price;
    string direction;
    int quantity;
};

struct independent_variables
{
    double close_t_1;
    double open_t_1;
    double VWAP_t_1;
    double low_t_1;
    double high_t_1;
    double no_of_trades_t_1;
    double open_t;

    independent_variables(double close_t_1, double open_t_1, double VWAP_t_1, double low_t_1, double high_t_1, double no_of_trades_t_1, double open_t)
    {
        this->close_t_1 = close_t_1;
        this->open_t_1 = open_t_1;
        this->VWAP_t_1 = VWAP_t_1;
        this->low_t_1 = low_t_1;
        this->high_t_1 = high_t_1;
        this->no_of_trades_t_1 = no_of_trades_t_1;
        this->open_t = open_t;
    }
};

vector<stock_data> lr_read_data(string file_name)
{
    ifstream file(file_name);

    if (!file.is_open())
    {
        cout << "Error opening file\n";
        exit(0);
    }

    string line;
    getline(file, line);
    vector<stock_data> data;
    while (getline(file, line))
    {
        string date;
        double open, high, low, close, VWAP, no_of_trades;

        stringstream ss(line);
        getline(ss, date, ',');
        ss >> close;
        ss.ignore();
        ss >> open;
        ss.ignore();
        ss >> VWAP;
        ss.ignore();
        ss >> high;
        ss.ignore();
        ss >> low;
        ss.ignore();
        ss >> no_of_trades;

        stock_data temp(date, open, high, low, close, VWAP, no_of_trades);
        data.push_back(temp);
    }

    return data;
}

// Function to find the transpose of a matrix
vector<vector<double>> get_transpose(const vector<vector<double>> &matrix)
{
    int m = matrix.size();
    int n = matrix[0].size();

    vector<vector<double>> result(n, vector<double>(m, 0.0));

    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            result[j][i] = matrix[i][j];
        }
    }

    return result;
}

// Function to find the inverse of a matrix using Gauss-Jordan elimination
vector<vector<double>> get_inverse(vector<vector<double>> X)
{
    int n = X.size();

    // Initialize the identity matrix
    vector<vector<double>> I(n, vector<double>(n, 0.0));
    for (int i = 0; i < n; ++i)
    {
        I[i][i] = 1.0;
    }

    // Gaussian elimination with partial pivoting
    for (int i = 0; i < n; ++i)
    {
        // Find pivot row and swap
        int pivotRow = i;
        for (int j = i + 1; j < n; ++j)
        {
            if (abs(X[j][i]) > abs(X[pivotRow][i]))
            {
                pivotRow = j;
            }
        }
        if (pivotRow != i)
        {
            swap(X[i], X[pivotRow]);
            swap(I[i], I[pivotRow]);
        }

        // Make the diagonal element 1
        double divisor = X[i][i];
        for (int j = 0; j < n; ++j)
        {
            X[i][j] /= divisor;
            I[i][j] /= divisor;
        }

        // Make other elements in the column 0
        for (int j = 0; j < n; ++j)
        {
            if (j != i)
            {
                double multiplier = X[j][i];
                for (int k = 0; k < n; ++k)
                {
                    X[j][k] -= X[i][k] * multiplier;
                    I[j][k] -= I[i][k] * multiplier;
                }
            }
        }
    }

    return I;
}

// Function to multiply two matrices
vector<vector<double>> multiply_matrices(const vector<vector<double>> &A, const vector<vector<double>> &B)
{
    int m = A.size();
    int n = A[0].size();
    int p = B[0].size();

    vector<vector<double>> result(m, vector<double>(p, 0.0));

    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < p; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    return result;
}

// Function to find coefficients using linear regression
vector<double> find_coefficients(vector<stock_data> data)
{
    vector<double> coefficients;
    vector<vector<double>> X;
    vector<vector<double>> Y;

    for (int i = 1; i < data.size(); i++)
    {
        vector<double> temp;
        temp.push_back(1.0);
        temp.push_back(data[i - 1].close);
        temp.push_back(data[i - 1].open);
        temp.push_back(data[i - 1].VWAP);
        temp.push_back(data[i - 1].low);
        temp.push_back(data[i - 1].high);
        temp.push_back(data[i - 1].no_of_trades);
        temp.push_back(data[i].open);
        X.push_back(temp);
        Y.push_back({data[i].close});
    }

    vector<vector<double>> X_transpose = get_transpose(X);
    vector<vector<double>> X_transpose_X = multiply_matrices(X_transpose, X);
    vector<vector<double>> X_transpose_X_inverse = get_inverse(X_transpose_X);

    vector<vector<double>> X_transpose_Y = multiply_matrices(X_transpose, Y);
    vector<vector<double>> result = multiply_matrices(X_transpose_X_inverse, X_transpose_Y);

    for (int i = 0; i < result.size(); ++i)
    {
        coefficients.push_back(result[i][0]);
    }

    return coefficients;
}

double predict_price(vector<double> coefficients, independent_variables iv)
{
    double price = coefficients[0] + coefficients[1] * iv.close_t_1 + coefficients[2] * iv.open_t_1 + coefficients[3] * iv.VWAP_t_1 + coefficients[4] * iv.low_t_1 + coefficients[5] * iv.high_t_1 + coefficients[6] * iv.no_of_trades_t_1 + coefficients[7] * iv.open_t;
    return price;
}

vector<lr_trade> lr_strategy(vector<double> coefficients, vector<stock_data> data, double x, double p)
{
    // the max position we can take is between +x and -x (both inclusive)
    vector<lr_trade> trades;
    int position = 0;
    for (int i = 1; i < data.size(); i++)
    {
        independent_variables iv(data[i - 1].close, data[i - 1].open, data[i - 1].VWAP, data[i - 1].low, data[i - 1].high, data[i - 1].no_of_trades, data[i].open);
        double predicted_price = predict_price(coefficients, iv);
        double actual_price = data[i].close;
        double percentage_change = (predicted_price - actual_price) / actual_price;
        percentage_change *= 100;
        if (percentage_change >= p)
        {
            double new_pos = position + 1;
            if (new_pos <= x && new_pos >= -x)
            {
                // BUY the stock
                position++;
                lr_trade t;
                t.date = data[i].date;
                t.price = actual_price;
                t.direction = "BUY";
                t.quantity = 1;
                trades.push_back(t);
            }
            else
            {
                // we cannot BUY the stock as it will exceed the position limit
                lr_trade t;
                t.date = data[i].date;
                t.price = actual_price;
                trades.push_back(t);
            }
        }
        else if (percentage_change <= -p)
        {
            double new_pos = position - 1;
            if (new_pos <= x && new_pos >= -x)
            {
                // SELL the stock
                position--;
                lr_trade t;
                t.date = data[i].date;
                t.price = actual_price;
                t.direction = "SELL";
                t.quantity = 1;
                trades.push_back(t);
            }
            else
            {
                lr_trade t;
                t.date = data[i].date;
                t.price = actual_price;
                trades.push_back(t);
            }
        }
        else
        {
            lr_trade t;
            t.date = data[i].date;
            t.price = actual_price;
            trades.push_back(t);
        }
    }

    lr_trade t;
    t.price = position * data[data.size() - 1].close;
    t.date = data[data.size() - 1].date;
    trades.push_back(t);

    return trades;
}

string lr_change_date_format(string date) // change date format from yyyy-mm-dd to dd-mm-yyyy
{
    string year = date.substr(0, 4), month = date.substr(5, 2), day = date.substr(8, 2);
    return day + "/" + month + "/" + year;
}

void lr_write_output(vector<lr_trade> data)
{
    // create order_statictics.csv file
    ofstream file("order_statistics.csv");

    if (!file.is_open())
    {
        cout << "Erorr in creating file" << endl;
        return;
    }

    // Write header
    file << "Date,Order_dir,Quantity,Price" << endl;

    for (const auto &trade : data)
    {
        if (trade.direction == "BUY")
        {
            file << lr_change_date_format(trade.date) << "," << trade.direction << "," << 1 << "," << trade.price << endl;
        }
        else if (trade.direction == "SELL")
        {
            file << lr_change_date_format(trade.date) << "," << trade.direction << "," << 1 << "," << trade.price << endl;
        }
    }

    file.close();

    // create daily_cashflow.csv file
    ofstream outfile("daily_cashflow.csv");

    if (!outfile.is_open())
    {
        cout << "Error in creating file" << endl;
    }

    // write header
    outfile << "Date,Cashflow" << endl;

    double cashflow = 0;

    for (int i = 0; i < data.size(); i++)
    {
        if (data[i].direction == "BUY")
        {
            cashflow -= data[i].price;
        }
        else if (data[i].direction == "SELL")
        {
            cashflow += data[i].price;
        }

        if (i == data.size() - 1)
        {
            cashflow += data[i].price;
            continue;
        }
        outfile << lr_change_date_format(data[i].date) << "," << cashflow << endl;
    }

    outfile.close();

    ofstream txtfile("final_pnl.txt");

    if (!txtfile.is_open())
    {
        cout << "Error in creating file" << endl;
    }

    txtfile << cashflow << endl;
}

void lr_trader(string input_file, string cashflow_file, string stats_file, string pnl_file, int x, double p)
{
    vector<stock_data> data = lr_read_data(input_file);
    vector<double> coefficients = find_coefficients(data);
    vector<lr_trade> trades = lr_strategy(coefficients, data, x, p);
    lr_write_output(trades);
}
