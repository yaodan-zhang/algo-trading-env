/* 
    This is an asset class header file. Support stock, index, and ETF,
    and they inherit from the "asset" base class. 

    The goal is to save each asset's price data (open, high, low, close)
    over a customized receiving period. E.g., if asset data is updated every 1s
    in an exchange and the receiving period in our case is every 5s, then there will be 
    5 concatenated pieces of (open, high, low, close) data over this receiving/streaming period.
    
    Each asset class also calculates its technical indicators based on the price data
    over the period. So, we get the price feature of an asset over the period.
*/
#pragma once

#include"./alpha/ETFAlpha.h"
#include"./alpha/IndexAlpha.h"
#include"./alpha/StockAlpha.h"

#include<format>
#include<iostream>
#include<iomanip>
#include<regex>
#include<cmath>

using std::regex;
using std::smatch;
using std::regex_match;
using std::make_unique;
using std::ostream;
using std::round;
using std::endl;
using std::format;

namespace my_algo_trading{

using priceT = unique_ptr<vector<double>>;
using volT = unique_ptr<vector<size_t>>;
using alphaT = vector<double>;

class asset{
    public:
    asset(string ticker) : 
    ticker(ticker),

    open(make_unique<vector<double>>()),
    high(make_unique<vector<double>>()),
    low(make_unique<vector<double>>()),
    close(make_unique<vector<double>>()),

    volume(make_unique<vector<size_t>>())
    {};
    
    virtual ~asset()=0;
    
    friend void operator>>(vector<string> &, asset &);

    friend void operator<<(ostream &, asset const &);

    double percent_move() const;

    virtual void set_alphas() = 0;

    virtual void set_header()=0;

    bool finalize() {
        set_header();
        set_alphas();
        return true;
    }

    string ticker;
    unsigned int num_of_data;
    
    priceT open;
    priceT high;
    priceT low;
    priceT close;

    volT volume;

    alphaT alphas;
    static unsigned int width;
    list<string> headerList;
};

asset::~asset(){}

double asset::percent_move() const{
    return ((*close)[num_of_data-1]-(*open)[0])/(*open)[0] * 100;
}

void operator>>(vector<string> &record, asset &a){
    a.open->clear();
    a.high->clear();
    a.low->clear();
    a.close->clear();
    a.volume->clear();

    string r;

    regex pattern(R"(^(\d{4}-\d{2}-\d{2}),([\d.]+),([\d.]+),([\d.]+),([\d.]+),([\d.]+),(\d+)$)");
    smatch matches;

    a.num_of_data = record.size()-1;

    for (auto &r : record){
        
        if (regex_match(r, matches, pattern)){
        
            a.open->push_back(stod(string(matches[2].str())));
            a.high->push_back(stod(string(matches[3].str())));
            a.low->push_back(stod(string(matches[4].str())));
            a.close->push_back(stod(string(matches[6].str())));
            a.volume->push_back(stol(string(matches[7].str())));

        }
    }
}

void operator<<(ostream &os, asset const &a){
    unsigned int const w = asset::width;

    os << string(format("{:^{}}", a.ticker, w));

    for (auto &alpha : a.alphas){
        os << string(format("{:^{}}", round(alpha*100.0)/100.0, w));
    }

    os << endl;
}

class stock: public asset{
    public:
    stock(string ticker):

    asset(ticker) {};

    ~stock()override=default;

    void set_alphas() override{
        alphas.clear();

        alphas.push_back(this->percent_move());

        for (auto &calculate_alpha : StockAlphaHelper) {
            alphas.push_back(calculate_alpha(open, high, low, close, volume, num_of_data));
        }
    }

    void set_header() override{
        headerList = {"ticker","price_move%"};

        for (auto &header : StockAlphaList){
            headerList.push_back(header);
        }
    }
};

class index: public asset{
    public:
    index(string ticker):

    asset(ticker) {};

    ~index()override=default;

    void set_alphas() override{
        alphas.clear();
        alphas.push_back(this->percent_move());
        for (auto &calculate_alpha : IndexAlphaHelper) {
            alphas.push_back(calculate_alpha(open,high,low,close,volume,num_of_data));
        }
    }

    void set_header() override{
        headerList = {"ticker","price_move%"};
        for (auto &header : IndexAlphaList){
            headerList.push_back(header);
        }
    }
};

class etf: public asset{
    public:
    etf(string ticker):
    
    asset(ticker) {};

    ~etf()override=default;

    void set_alphas() override{
        alphas.clear();
        alphas.push_back(this->percent_move());
        for (auto &calculate_alpha : ETFAlphaHelper) {
            alphas.push_back(calculate_alpha(open,high,low,close,volume,num_of_data));
        }
    }
    
    void set_header() override{
        headerList = {"ticker","price_move%"};
        for (auto &header : ETFAlphaList){
            headerList.push_back(header);
        }
    }
};
};