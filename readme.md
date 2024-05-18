# Algorithmic Trading Strategies Report

## Introduction

Algorithmic trading is a rapidly evolving field that leverages computational tools to make trading decisions. In this report, we'll discuss and analyze four algorithmic trading strategies: **Pair Trading**, **RSI Trading**, **Linear Regression Trading**, and **DMA (Dual Moving Average) Trading**. Each strategy has its unique approach to capturing market opportunities.

### Strategy for STOP_loss_Trading
 In this strategy  we are storing the data of postions until the end of the code and in this strategy we are basically keeping track of  Z-score of each position we hold  so that we can close the postions when the Z-score of the position we are holding has crossestop loss threshold so we prevent our loss from the indesired change of Z-score 


## Pair Trading

### Description

Pair Trading involves identifying pairs of assets whose prices historically move together. The strategy takes advantage of the temporary dislocation in the relationship between the two assets, aiming to profit when the historical correlation is restored.

### Code Overview

The Pair Trading strategy is implemented in C++. The key components include:

- **Data Structure**: The `pt1_trade` struct represents individual trades with relevant information.
- **Spread Calculation**: The `spreader` function calculates the spread between two assets.
- **Pair Strategy Logic**: The `pair_strategy` function implements the core logic, considering a rolling window and Z-score to determine trading signals.
- **Input/Output Handling**: Functions like `pairs1_read_input`, `pairs1_change_date_format`, and `pairs1_write_output` handle file I/O and formatting.

### Time Complexity

The time complexity of the Pair Trading strategy mainly depends on the size of the input dataset, denoted as *N*. The `pair_strategy` function iterates through the dataset once, resulting in a time complexity of *O(N)*.

## RSI Trading

### Description

The Relative Strength Index (RSI) trading strategy aims to identify overbought or oversold conditions in an asset. It calculates the RSI indicator and triggers buy/sell signals based on specified thresholds.

### Code Overview

The RSI Trading strategy is also implemented in C++. The key components include:

- **Data Structure**: The `rsi_trade` struct represents individual trades with relevant information.
- **RSI Calculation**: The `rsi_strategy` function calculates the RSI indicator and generates trading signals accordingly.
- **Input/Output Handling**: Functions like `rsi_read_input`, `rsi_change_date_format`, and `rsi_write_output` handle file I/O and formatting.

### Time Complexity

The time complexity of the RSI Trading strategy depends on the size of the input dataset, denoted as *N*. The `rsi_strategy` function iterates through the dataset once, resulting in a time complexity of *O(N)*.

## Linear Regression Trading

### Description

Linear Regression Trading involves fitting a linear regression model to historical price data. The strategy generates buy/sell signals based on the model's predictions, aiming to profit from anticipated price movements.

### Code Overview

The Linear Regression Trading strategy is implemented in C++. The key components include:

- **Data Structure**: The `lr_trade` struct represents individual trades with relevant information.
- **Linear Regression Model**: The `linear_regression_strategy` function fits a linear regression model to historical price data and generates trading signals based on the model's predictions.
- **Input/Output Handling**: Functions like `lr_read_input`, `lr_change_date_format`, and `lr_write_output` handle file I/O and formatting.

### Time Complexity

The time complexity of the Linear Regression Trading strategy depends on the size of the input dataset, denoted as *N^3*. The `linear_regression_strategy` function involves linear regression computations, resulting in a time complexity influenced by the algorithm used for regression  and due to usage of Gaussian elimination the time complexity has been reduced.

## DMA Trading

### Description

The DMA Trading strategy, also known as Adaptive Moving Average (AMA), is based on a dynamic moving average that adjusts to changing market conditions. It involves buying/selling based on price deviations from the adaptive moving average.

### Code Overview

The DMA Trading strategy is implemented in C++. The key components include:

- **Data Structure**: The `dma_pp_trade` struct represents individual trades with relevant information.
- **Adaptive Moving Average Calculation**: The `ama_strategy` function calculates the AMA and generates buying/selling signals accordingly.
- **Input/Output Handling**: Functions like `ama_read_input`, `ama_write_output` handle file I/O and formatting.

### Time Complexity

The time complexity of the DMA Trading strategy depends on the size of the input dataset, denoted as *N*. The `ama_strategy` function iterates through the dataset once, resulting in a time complexity of *O(N)*.

 ### Multi-Strategy Execution
Concurrent Execution:

    The provided main function executes multiple trading strategies concurrently using separate threads.
    Strategies include basic, ADX, DMA-PP, DMA, MACD, and Linear Regression.

Clean-up:

    The clean function removes generated files associated with each strategy.

Conclusion:

    Evaluate the performance of each strategy to determine their effectiveness.
    Consider adjusting parameters to optimize strategies based on market conditions.



## Conclusion

Each algorithmic trading strategy discussed in this report has its strengths and weaknesses. The effectiveness of a strategy depends on various factors, including market conditions, asset characteristics, and parameter tuning. Additionally, continuous monitoring and adaptation are crucial for these strategies to perform well in dynamic markets. It's essential to backtest these strategies on historical data and consider transaction costs for a comprehensive evaluation.