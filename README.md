# Real-time Algorithmic Trading Environment

Originated from an idea in [stock market program](https://www.netdevmike.com/project/c-stock-market-program), I built an algorithmic trading environment that mimics real-time ticks input where I calculate technical indicators (alphas) on their candlestick data and sort them to display. The result can be further feeded to conduct automatic trading. The asset types I supported are equity/stock, index, ETF, and option.

Future improvement/leverage: The calculated alphas for each period can be used to conduct automatic trading based on some alpha thresholds. And at the same time we can report the trading activities of each such period. We expect automatic long or short based on our calculated alphas.

The program structure uses a producer-consumer protocol suggested by Professor Mike Spertus from University of Chicago. The primary goal of the project serves as a final homework for the Advanced C++ course by Mr. Spertus from U of C.
