#pragma once
#include<list>
#include<string>
#include<vector>
#include<numeric>
#include<memory>
#include<algorithm>
using std::vector;
namespace my_algo_trading{
std::list<std::string> ETFAlphaList = {
    "LM", // Largest Momentum
    "SMA" // Simple Moving Average using closing price
};
typedef double (*EAFuncType)(std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<size_t>>&,unsigned int&);

double LM(std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<size_t>>&,unsigned int&);
double SMA(std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<size_t>>&,unsigned int&);
vector<EAFuncType> ETFAlphaHelper = {&LM, &SMA};

// Alpha Implemantations: Formula came from https://docs.anychart.com/Stock_Charts/Technical_Indicators/Mathematical_Description

// Calculate Largest Price Momentum
double LM(std::unique_ptr<std::vector<double>>&, std::unique_ptr<std::vector<double>>&, std::unique_ptr<std::vector<double>>&, std::unique_ptr<std::vector<double>>&close, std::unique_ptr<std::vector<size_t>>&, unsigned int &period) {
    double momentum = 0;
    for(unsigned int i=1;i<period;i++){
        double momentum_i = (*close)[i]/(*close)[i-1];
        momentum = std::max(momentum_i,momentum);
    }
    return momentum;
}
// Calculate Simple Moving Average
double SMA(std::unique_ptr<std::vector<double>>&, std::unique_ptr<std::vector<double>>&, std::unique_ptr<std::vector<double>>&, std::unique_ptr<std::vector<double>>&close, std::unique_ptr<std::vector<size_t>>&, unsigned int &period){
    double sum = std::accumulate(close->begin(), close->end(), 0.0);
    return sum/period;
}
};