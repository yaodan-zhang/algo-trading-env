/* The main difference between different asset types is they have different production alphas
and possibly different additional features. */
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
    asset(string ticker) : ticker(ticker),open(make_unique<vector<double>>()),high(make_unique<vector<double>>()),
    low(make_unique<vector<double>>()),close(make_unique<vector<double>>()),volume(make_unique<vector<size_t>>()){};
    virtual ~asset()=0;
    // Conveniently input raw candlestick data.
    friend void operator>>(vector<string> &, asset &);
    // Conveniently output all asset info represented by list header and asset's alphas.
    friend void operator<<(ostream &, asset const &);
    // Calculate the price movement(%), which is the default alpha of an asset.
    double percent_move()const;
    // Calculate alphas in subclass assets.
    // The goal of set_alphas is to calculate and set down all designated features of the asset type in one hop.
    // Another goal is to seperate the interface from users.
    virtual void set_alphas()=0;
    // Set header for an instance.
    virtual void set_header()=0;
    // Settle down the asset object, same as the user interface.
    bool finalize() {
        set_header();
        set_alphas();
        return true;
    }
    string ticker;
    unsigned int num_of_data; // Number of records for one period, e.g., receive 10 price data every 5 mins.
    priceT open;
    priceT high;
    priceT low;
    priceT close;
    volT volume;
    alphaT alphas; // Alphas for subclass assets
    static unsigned int width; // Print layout width
    list<string> headerList;
};
// Base class virtual destructor definition.
asset::~asset(){}
// Calculate the price percent move during this period, which is the first and default alpha.
double asset::percent_move() const{
    return ((*close)[num_of_data-1]-(*open)[0])/(*open)[0] * 100;
}
// Record format is:
// ticker (first line)
// Date Open High Low Close Adj_Close Volume (num_of_data such line(s))
void operator>>(vector<string> &record, asset &a){
    // Clear previous record.
    a.open->clear();
    a.high->clear();
    a.low->clear();
    a.close->clear();
    a.volume->clear();
    // Insert new record.
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
    // Print asset's ticker
    os << string(format("{:^{}}", a.ticker, w));
    // Print asset's alphas and round to 2 decimal places.
    for (auto &alpha : a.alphas){
        os << string(format("{:^{}}", round(alpha*100.0)/100.0, w));
    }
    os << endl;
}

// Asset subclass.
// The main goal for overriding set_alphas and set_header is to leave implementation detail for an asset subclass to itself.
// Different subclasses can have different processing stages of the candlestick data and even more individual traits/member fields. 
// Here we implement it as the naive way to just let the difference of a subclass from its baseclasee to be different alpha library sources.

// Stock subclass
class stock: public asset{
    public:
    stock(string ticker): asset(ticker) {};
    ~stock()override=default;
    // Set alphas for this period.
    void set_alphas() override{
        // Clear previous period's alphas
        alphas.clear();
        // Set new alphas for this period.
        // Set price movement, the default alpha.
        alphas.push_back(this->percent_move());
        // Set other alphas for this subclass
        for (auto &calculate_alpha : StockAlphaHelper) {
            alphas.push_back(calculate_alpha(open,high,low,close,volume,num_of_data));
        }
    }
    // Set headers for this period.
    void set_header() override{
        // Reset base header.
        headerList = {"ticker","price_move%"};
        // Insert new headers for this period.
        for (auto &header : StockAlphaList){
            headerList.push_back(header);
        }
    }
};

// Index subclass
class index: public asset{
    public:
    index(string ticker): asset(ticker) {};
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

// ETF subclass
class etf: public asset{
    public:
    etf(string ticker): asset(ticker) {};
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