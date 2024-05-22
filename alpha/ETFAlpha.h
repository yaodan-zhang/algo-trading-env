#pragma once
#include<list>
#include<string>
#include<vector>
#include<numeric>
#include<memory>
#include<algorithm>
#include<functional>
namespace my_algo_trading{
std::list<std::string> ETFAlphaList = {
    "LM", // Largest Momentum
    "SMA" // Simple Moving Average using closing price
};
// ETF type definitions.
using EPriceT = std::unique_ptr<std::vector<double>>&;
using EVolT = std::unique_ptr<std::vector<size_t>>&;
using EDataT = unsigned int&;
using EAlphaT = std::function<double(EPriceT,EPriceT,EPriceT,EPriceT,EVolT,EDataT)>;
// ETF alpha declarations.
double LM(EPriceT,EPriceT,EPriceT,EPriceT,EVolT,EDataT);
double SMA(EPriceT,EPriceT,EPriceT,EPriceT,EVolT,EDataT);
std::vector<EAlphaT> ETFAlphaHelper = {LM, SMA};

// ETF alpha Implemantations: formula came from https://docs.anychart.com/Stock_Charts/Technical_Indicators/Mathematical_Description
// 1. Calculate Largest Price Momentum
double LM(EPriceT, EPriceT, EPriceT, EPriceT close, EVolT, EDataT period) {
    double momentum = 0;
    for(unsigned int i=1;i<period;i++){
        double momentum_i = (*close)[i]/(*close)[i-1];
        momentum = std::max(momentum_i,momentum);
    }
    return momentum;
}
// 2. Calculate Simple Moving Average
double SMA(EPriceT, EPriceT, EPriceT, EPriceT close, EVolT, EDataT period){
    double sum = std::accumulate(close->begin(), close->end(), 0.0);
    return sum/period;
}
};