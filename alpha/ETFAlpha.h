/*
    This is the Exchange-Traded Fund (ETF) alpha library 
*/
#pragma once
#include<list>
#include<string>
#include<vector>
#include<numeric>
#include<memory>
#include<algorithm>
#include<functional>
using std::list;
using std::string;
using std::unique_ptr;
using std::vector;
using std::function;
using std::max;
using std::accumulate;

namespace my_algo_trading{
// The pre-selected alphas for ETF
list<string> ETFAlphaList = {
    "LM", // Largest Momentum
    "SMA" // Simple Moving Average
};

using EPriceT = unique_ptr<vector<double>>&;
using EVolT = unique_ptr<vector<size_t>>&;
using EDataT = unsigned int&;
using EAlphaT = function<double(EPriceT,EPriceT,EPriceT,EPriceT,EVolT,EDataT)>; // using std::function to represent an alpha

// alpha declarations
double LM(EPriceT,EPriceT,EPriceT,EPriceT,EVolT,EDataT);
double SMA(EPriceT,EPriceT,EPriceT,EPriceT,EVolT,EDataT);
vector<EAlphaT> ETFAlphaHelper = {LM, SMA};

// alpha Implemantation:
// 1. Largest Momentum
double LM(EPriceT, EPriceT, EPriceT, EPriceT close, EVolT, EDataT period){
    double momentum = 0;
    for(unsigned int i=1;i<period;i++){
        double momentum_i = (*close)[i]/(*close)[i-1];
        momentum = max(momentum_i,momentum);
    }
    return momentum;
}

// 2. Simple Moving Average
double SMA(EPriceT, EPriceT, EPriceT, EPriceT close, EVolT, EDataT period){
    double sum = accumulate(close->begin(), close->end(), 0.0);
    return sum/period;
}
};