#pragma once
#include<list>
#include<string>
#include<vector>
using std::vector;
// Customize Alpha - Production alphas by quantitative researchers.
// Here the alphas are randomly selected as a demo.
// References:
// https://docs.anychart.com/Stock_Charts/Technical_Indicators/Mathematical_Description

// The first alpha must be the price movement in percentage.
namespace my_algo_trading{
std::list<std::string> StockAlphaList = {
    "AMFV", // Average Money Flow Volume
    "MFI" // Money Flow Index
    };

typedef double (*SAFuncType)(std::vector<double> *, std::vector<double> *, std::vector<double> *, std::vector<double> *, std::vector<size_t> *, unsigned int);

double AMFV(vector<double> *open, vector<double> *, vector<double> *, vector<double> *, vector<size_t> *, unsigned int);
double MFI(vector<double> *open, vector<double> *, vector<double> *, vector<double> *, vector<size_t> *, unsigned int);
vector<SAFuncType> StockAlphaHelper = {&AMFV,&MFI};

// Stock Alphas Implementations: Formula came from https://docs.anychart.com/Stock_Charts/Technical_Indicators/Mathematical_Description
// Calculate average Money Flow Volume
double AMFV(vector<double> *open, vector<double> *high, vector<double> *low, vector<double> *close, vector<size_t> *volume, unsigned int period){
    double MFV = 0;
    for (unsigned int i=0; i< period; i++){
        double MFM_i = (((*close)[i]-(*low)[i])-((*high)[i]-(*close)[i]))/((*high)[i]-(*low)[i]);
        double MFV_i = MFM_i * (*volume)[i];
        MFV += MFV_i;
    }
    return MFV/double(period);
}

// Calculate Money Flow Index
double MFI(vector<double> *open, vector<double> *high, vector<double> *low, vector<double> *close, vector<size_t> *volume, unsigned int period){
    // Calculate typical price
    double typical_price_prev = ((*high)[0]+(*low)[0]+(*close)[0])/3;
    double total_positive_money_flow = 0;
    double total_negative_money_flow = 0;
    for (unsigned int i=1; i<period; i++){
        double typical_price_today = ((*high)[i]+(*low)[i]+(*close)[i])/3;
        double money_flow_today = typical_price_today*(*volume)[i];
        typical_price_today>typical_price_prev? total_positive_money_flow+=money_flow_today:total_negative_money_flow+=money_flow_today;
        typical_price_prev = typical_price_today;
    }
    return total_positive_money_flow/total_negative_money_flow;
}
};