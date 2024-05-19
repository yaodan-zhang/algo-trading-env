#pragma once
#include<list>
#include<string>
#include<vector>
#include<algorithm>
#include<memory>
namespace my_algo_trading{
std::list<std::string> IndexAlphaList = {
    "RSI", // Relative Strength Index
    "WILLIAMS %R" // Williams % R
};
typedef double (*IAFuncType)(std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<size_t>>&,unsigned int&);
double RSI(std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<size_t>>&,unsigned int&);
double WilliamsR(std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<size_t>>&,unsigned int&);
std::vector<IAFuncType> IndexAlphaHelper = {&RSI, &WilliamsR};

// Alpha Implementations: Formula came from https://school.stockcharts.com/doku.php?id=technical_indicators:relative_strength_index_rsi
// and https://docs.anychart.com/Stock_Charts/Technical_Indicators/Mathematical_Description
// Calculate RSI
double RSI(std::unique_ptr<std::vector<double>>&open,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&close,std::unique_ptr<std::vector<size_t>>&,unsigned int &period){
    // Calculate sum of gains and sum of absolute losses using same day's open and close
    double sum_of_gain = 0;
    double sum_of_abs_loss = 0;
    for (unsigned int i=0;i<period;i++){
        double delta_return = ((*close)[i]-(*open)[i])/(*close)[i];
        delta_return>0? sum_of_gain+=delta_return : sum_of_abs_loss-=delta_return;
    }
    double RS = sum_of_gain/sum_of_abs_loss;
    return 100-100/(1+RS);
}
// Calculate Williams % R.
double WilliamsR(std::unique_ptr<std::vector<double>>&,std::unique_ptr<std::vector<double>>&high,std::unique_ptr<std::vector<double>>&low,std::unique_ptr<std::vector<double>>&close,std::unique_ptr<std::vector<size_t>>&,unsigned int &period){
    auto max_high = std::max_element(high->begin(), high->end());
    auto min_low = std::min_element(low->begin(), low->end());
    double R = ((*close)[period-1]-*max_high)/(*max_high-*min_low)*100;
    return R;
}
};