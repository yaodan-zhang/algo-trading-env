/* 
    This is an asset class header file. I supported stock, index, and ETF,
    and they inherit from the "asset" base class. 

    The main goal is to save each asset's price data (open, high, low, close)
    over a customized receiving period, for example, if asset data is updated every 1s
    in an exchange and the receiving period in our case is every 5s, then there will be 
    5 pieces of (open, high, low, close) data for this particular asset over this period.
    
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

// We use RAII to manipulate the fields.
using priceT = unique_ptr<vector<double>>; // unique pointer of open/high/low/close data over the period
using volT = unique_ptr<vector<size_t>>; // unique pointer of volumes traded over the period
using alphaT = vector<double>; // unique pointer of asset alphas

class asset{
    public:
    // Initialize an asset with its ticker symbol and empty fields
    asset(string ticker) : 
    ticker(ticker),
    open(make_unique<vector<double>>()),
    high(make_unique<vector<double>>()),
    low(make_unique<vector<double>>()),
    close(make_unique<vector<double>>()),
    volume(make_unique<vector<size_t>>())
    {};
    
    virtual ~asset()=0;
    
    // Overload to conveniently insert price data.
    friend void operator>>(vector<string> &, asset &);

    // Overload to conveniently output asset info.
    friend void operator<<(ostream &, asset const &);

    // Price movement(%), the default alpha.
    double percent_move() const;

    // Virtual alpha calculation that will be overrided in subclass,
    // for different asset class have different alphas of interest.
    virtual void set_alphas() = 0;

    // Set header, e.g., ticker alpha1_name alpha2_name ..., as the first line, for better comprehension.
    virtual void set_header()=0;

    // Zip the data processing functions, same as user interface.
    bool finalize() {
        set_header();
        set_alphas();
        return true;
    }

    string ticker;
    unsigned int num_of_data; // Number of (open,high,low,close) per receiving period
    priceT open;
    priceT high;
    priceT low;
    priceT close;
    volT volume;
    alphaT alphas;
    static unsigned int width; // Printing layout width
    list<string> headerList; // Contains the header set in set_header
};

// Base class virtual destructor definition.
asset::~asset(){}

// Price percent movement definition, the default alpha.
double asset::percent_move() const{
    return ((*close)[num_of_data-1]-(*open)[0])/(*open)[0] * 100;
}

// Insertion operator overloading definition.
// Data insertion format is:
// ticker (first line)
// date open high low close adj_close volume (num_of_data such line(s))
void operator>>(vector<string> &record, asset &a){
    // Clear previous record.
    a.open->clear();
    a.high->clear();
    a.low->clear();
    a.close->clear();
    a.volume->clear();

    // Use REGEX to parse and insert new record.
    string r;
    regex pattern(R"(^(\d{4}-\d{2}-\d{2}),([\d.]+),([\d.]+),([\d.]+),([\d.]+),([\d.]+),(\d+)$)");
    smatch matches;
    a.num_of_data = record.size()-1;// Exclude ticker line
    for (auto &r : record){
        // Don't match the first line
        if (regex_match(r, matches, pattern)){
            // Set basic candlestick fields.
            a.open->push_back(stod(string(matches[2].str())));
            a.high->push_back(stod(string(matches[3].str())));
            a.low->push_back(stod(string(matches[4].str())));
            a.close->push_back(stod(string(matches[6].str())));
            a.volume->push_back(stol(string(matches[7].str())));
        }
    }
}

// Overload output operator to print the asset.
void operator<<(ostream &os, asset const &a){
    unsigned int const w = asset::width;

    // Using std::format to print the ticker
    os << string(format("{:^{}}", a.ticker, w));

    // Using std::format to print alphas rounded up to 2 decimal places.
    for (auto &alpha : a.alphas){
        os << string(format("{:^{}}", round(alpha*100.0)/100.0, w));
    }

    os << endl;
}

/* Asset Subclass */

class stock: public asset{
    public:
    stock(string ticker):
    asset(ticker) {}; // Call base clasee contructor.

    ~stock()override=default;

    void set_alphas() override{
        // Clear previous period's alphas
        alphas.clear();

        // Set new alphas for this period:

        // 1. set price movement, the default alpha.
        alphas.push_back(this->percent_move());

        // 2. set other alphas for stock class
        for (auto &calculate_alpha : StockAlphaHelper) {
            alphas.push_back(calculate_alpha(open, high, low, close, volume, num_of_data));
        }
    }

    void set_header() override{
        // Set basic headers
        headerList = {"ticker","price_move%"};

        // Set alpha headers
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