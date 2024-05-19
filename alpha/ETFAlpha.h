#pragma once
#include<list>
#include<string>
#include<vector>
#include<numeric>
using std::vector;
namespace my_algo_trading{
std::list<std::string> ETFAlphaList = {
    "AM", // Average Momentum
    "SMA" // Simple Moving Average using closing price
};
typedef double (*EAFuncType)(std::vector<double> *, std::vector<double> *, std::vector<double> *, std::vector<double> *, std::vector<size_t> *, unsigned int);

double AM(vector<double> *, vector<double> *, vector<double> *, vector<double> *, vector<size_t> *, unsigned int);
double SMA(vector<double> *, vector<double> *, vector<double> *, vector<double> *, vector<size_t> *, unsigned int);
vector<EAFuncType> ETFAlphaHelper = {&AM, &SMA};

// Alpha Implemantations: Formula came from https://docs.anychart.com/Stock_Charts/Technical_Indicators/Mathematical_Description

// Calculate Average Price Momentum
double AM(vector<double> *, vector<double> *, vector<double> *, vector<double> *close, vector<size_t> *, unsigned int period) {
    double momentum = 0;
    for(unsigned int i=1;i<period;i++){
        momentum += (*close)[i]/(*close)[i-1];
    }
    return momentum/(period-1);
}
// Calculate Simple Moving Average
double SMA(vector<double> *, vector<double> *, vector<double> *, vector<double> *close, vector<size_t> *, unsigned int period){
    int sum = std::accumulate(close->begin(), close->end(), 0);
    return sum/period;
}
};