# make strategy=BASIC symbol=SBIN n=5 x=2 start_date="b" end_date="a"
# the above command should first generate the data and then run the strategy

.PHONY: all lr basic dma ama adx macd pairs

# define variables

all : $(strategy)
LINEAR_REGRESSION : lr
BASIC : basic
DMA : dma
DMA++: dma_pp
ADX : adx
MACD : macd
PAIRS : pairs
RSI: rsi
BEST_OF_ALL : best

lr:
	@echo "Running Linear Regression"
	python3 stock_data_generator.py lg $(symbol) $(train_start_date) $(train_end_date) $(start_date) $(end_date)
	g++ -o linear_regression linear_regression.cpp
	./linear_regression $(x) $(p)
	rm ./linear_regression
	rm ./trade_data.csv
	rm ./train_data.csv

basic:
	@echo "Running Basic Strategy"
	python3 stock_data_generator.py basic $(symbol) $(start_date) $(end_date) $(n)
	g++ -o basic basic.cpp
	./basic $(x) $(n)
	rm ./basic
	rm ./trade_data.csv

dma:
	@echo "Running DMA Strategy"
	python3 stock_data_generator.py dma $(symbol) $(start_date) $(end_date) $(n)
	g++ -o dma dma.cpp
	./dma $(x) $(n) $(p)
	rm ./dma
	rm ./trade_data.csv

dma_pp:
	@echo "Running AMA Strategy"
	python3 stock_data_generator.py ama $(symbol) $(start_date) $(end_date) $(n)
	g++ -o dma_pp dma_pp.cpp
	./dma_pp $(x) $(n) $(c1) $(c2)  $(p) $(max_hold_days)
	rm ./dma_pp
	rm ./trade_data.csv
	
adx:
	@echo "Running ADX Strategy"
	python3 stock_data_generator.py adx $(symbol) $(start_date) $(end_date)
	g++ -o adx adx.cpp
	./adx $(x) $(n) $(adx_threshold)
	rm ./adx
	rm ./trade_data.csv

macd:
	@echo "Running MACD Strategy"
	python3 stock_data_generator.py macd $(symbol) $(start_date) $(end_date)
	g++ -o macd macd.cpp
	./macd $(x)
	rm ./macd
	rm ./trade_data.csv

pairs:
	@if [ "$(stop_loss_threshold)" != "" ]; then \
        echo "Running pairs with stop loss threshold"; \
        python3 stock_data_generator.py pairs $(symbol1) $(symbol2) $(start_date) $(end_date) $(n); \
        g++ -o pairs2 pairs2.cpp; \
        ./pairs2 $(n) $(x) $(threshold) $(stop_loss_threshold); \
        rm -f pairs2 trade_data_1.csv trade_data_2.csv; \
	else \
        echo "Running pairs without stop loss threshold"; \
        python3 stock_data_generator.py pairs $(symbol1) $(symbol2) $(start_date) $(end_date) $(n); \
        g++ -o pairs1 pairs1.cpp; \
        ./pairs1 $(n) $(x) $(threshold); \
		rm -f pairs1 trade_data_2.csv trade_data_1.csv;\
	fi

rsi:
	@echo "Running RSI Strategy"
	python3 stock_data_generator.py rsi $(symbol) $(start_date) $(end_date) $(n)
	g++ -o rsi rsi.cpp
	./rsi $(x) $(n) $(p) $(oversold_threshold) $(overbought_threshold)
	rm -f rsi trade_data.csv

best:
	@echo "Running Best of All Strategy"
	python3 stock_data_generator.py best $(symbol) $(start_date) $(end_date)
	g++ -o best best.cpp
	./best
	rm -f best