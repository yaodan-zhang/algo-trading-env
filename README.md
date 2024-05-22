# Real-time Algorithmic Trading Environment

Originated from an idea in [stock market program](https://www.netdevmike.com/project/c-stock-market-program), I built an algorithmic trading environment that mimics real-time exchange price data streaming input where I calculate technical indicators (alphas) on their candlestick price data over the fixed timeframe and sort them to display. The result can be further fed to conduct automatic trading. The asset types currently supported are equity, index, and ETF.

Future leverage: The calculated alphas for each period can be used to conduct automatic trading based on some thresholds. And at the same time we can report the trading activities of each such period. We expect automatic long or short based on our calculated alphas.

The projects uses a producer-consumer multithreading protocol, and all data came from yahoo!finance. The primary goal of the project serves as a final assignment of the master's course Advanced C++ taught by Professor Mike Spertus from University of Chicago, and the project implementation detail in C++ also received precious feedback from Mike.
