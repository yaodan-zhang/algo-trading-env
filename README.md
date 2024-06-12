# Real-time Algorithmic Trading Environment

Originated from a piece of idea in [stock market program](https://www.netdevmike.com/project/c-stock-market-program), I built a real-time algorithmic trading environment using C++20 that mimics live data-streaming input and calculates technical indicators (alphas) using each asset's price data over that period, the calculated asset lists are then displayed to the user. The results can be further fed to conduct automatic trading. Asset types currently supported are equity, index, and ETF, out of their data availability.

After compiling through `env.cpp`, the user is expected to see dynamically updated output every predefined amount of time (5s in my case), which is the timeframe over which the data is streamed, and the output contains all asset alphas related to that period. The output list can be sorted by ticker lexigraphcally ascending or by price movement descending by setting the mode variable in the `env.cpp`. Output format: (updated every 5s)

![output](./terminal_output.png)

Further leverages: The calculated alphas for each period can be used to conduct automatic trading based on well-researched thresholds. At the same time we can report the trading activities of each such period to the output. We expect automatic long or short for an asset(s).

The projects uses a multiple-stage producer-consumer protocol. The primary goal of the project serves as the final assignment of the Advanced C++ master's course taught by Professor Mike Spertus from University of Chicago, but the project is highly related to my personal interest and future career goal, which I enjoy greatly doing. The project implementation details in C++20 also received precious feedback from Mike over his office hours.

All price data in this project came from yahoo!finance
