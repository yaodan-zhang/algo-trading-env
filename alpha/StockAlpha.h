#pragma once
#include<list>
#include<string>
#include<vector>
#include<memory>
#include<functional>
namespace my_algo_trading{
std::list<std::string> StockAlphaList = {
    "AMFV", // Average Money Flow Volume
    "MFI" // Money Flow Index
    };
// Stock type definitions.
using SPriceT = std::unique_ptr<std::vector<double>>&;
using SVolT = std::unique_ptr<std::vector<size_t>>&;
using SDataT = unsigned int&;
using SAlphaT = std::function<double(SPriceT,SPriceT,SPriceT,SPriceT,SVolT,SDataT)>;
// Stock alpha declarations.
double AMFV(SPriceT,SPriceT,SPriceT,SPriceT,SVolT,SDataT);
double MFI(SPriceT,SPriceT,SPriceT,SPriceT,SVolT,SDataT);
std::vector<SAlphaT> StockAlphaHelper = {AMFV,MFI};

// Stock Alphas Implementations: formula came from https://docs.anychart.com/Stock_Charts/Technical_Indicators/Mathematical_Description
// 1. Calculate average Money Flow Volume
double AMFV(SPriceT open, SPriceT high, SPriceT low, SPriceT close, SVolT volume, SDataT period){
    double MFV = 0;
    for (unsigned int i=0; i< period; i++){
        double MFM_i = (((*close)[i]-(*low)[i])-((*high)[i]-(*close)[i]))/((*high)[i]-(*low)[i]);
        double MFV_i = MFM_i * (*volume)[i];
        MFV += MFV_i;
    }
    return MFV/period;
}

// 2. Calculate Money Flow Index
double MFI(SPriceT open, SPriceT high, SPriceT low, SPriceT close, SVolT volume, SDataT period){
    // Calculate "typical price"
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