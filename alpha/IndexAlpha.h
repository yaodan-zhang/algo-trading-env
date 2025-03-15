/*
    Index Alpha Library
*/

#pragma once

#include<list>
#include<string>
#include<vector>
#include<algorithm>
#include<memory>
#include <functional>

using std::list;
using std::string;
using std::unique_ptr;
using std::vector;
using std::function;
using std::max_element;
using std::min_element;

namespace my_algo_trading {

list<string> IndexAlphaList = {
    "RSI", // Relative Strength Index
    "WILLIAMS %R" // Williams % R

};

using IPriceT = unique_ptr<vector<double>>&;
using IVolT = unique_ptr<vector<size_t>>&;
using IDataT = unsigned int&;
using IAlphaT = function<double(IPriceT,IPriceT,IPriceT,IPriceT,IVolT,IDataT)>;

double RSI(IPriceT, IPriceT, IPriceT, IPriceT, IVolT, IDataT);
double WilliamsR(IPriceT, IPriceT, IPriceT, IPriceT, IVolT, IDataT);

vector<IAlphaT> IndexAlphaHelper = {RSI, WilliamsR};


double RSI(IPriceT open, IPriceT, IPriceT, IPriceT close, IVolT, IDataT period) {

    // Calculate sum of gains and sum of absolute losses using same day's open and close
    double sum_of_gain = 0;
    double sum_of_abs_loss = 0;

    for (unsigned int i=0; i<period; i++){

        double delta_return = ((*close)[i]-(*open)[i])/(*close)[i];
        delta_return>0? sum_of_gain+=delta_return : sum_of_abs_loss-=delta_return;

    }

    double RS = sum_of_gain/sum_of_abs_loss;
    return 100-100/(1+RS);

}

double WilliamsR(IPriceT, IPriceT high, IPriceT low, IPriceT close, IVolT, IDataT period) {
    
    auto max_high = max_element(high->begin(), high->end());
    auto min_low = min_element(low->begin(), low->end());

    double R = ((*close)[period-1]-(*max_high))/((*max_high)-(*min_low))*100;

    return R;
}

};