/*
    Stock Alpha Library
*/

#pragma once

#include<list>
#include<string>
#include<vector>
#include<memory>
#include<functional>

using std::list;
using std::string;
using std::unique_ptr;
using std::vector;
using std::function;

namespace my_algo_trading{

list<string> StockAlphaList = {
    "AMFV",
    "MFI"
};

using SPriceT = unique_ptr<vector<double>>&;
using SVolT = unique_ptr<vector<size_t>>&;
using SDataT = unsigned int&;
using SAlphaT = function<double(SPriceT,SPriceT,SPriceT,SPriceT,SVolT,SDataT)>;

double AMFV(SPriceT,SPriceT,SPriceT,SPriceT,SVolT,SDataT);
double MFI(SPriceT,SPriceT,SPriceT,SPriceT,SVolT,SDataT);

vector<SAlphaT> StockAlphaHelper = {AMFV,MFI};

double AMFV(SPriceT open, SPriceT high, SPriceT low, SPriceT close, SVolT volume, SDataT period){
    double MFV = 0;
    for (unsigned int i=0; i< period; i++){
        double MFM_i = (((*close)[i]-(*low)[i])-((*high)[i]-(*close)[i]))/((*high)[i]-(*low)[i]);
        double MFV_i = MFM_i * (*volume)[i];
        MFV += MFV_i;
    }
    return MFV/period;
}

double MFI(SPriceT open, SPriceT high, SPriceT low, SPriceT close, SVolT volume, SDataT period){
    
    // calculate "typical price" in MFI
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