/* The main difference between different asset types is they have different alphas
and possibly different period size to receive the tick, e.g., every 5 mins, every 10 mins. */
#pragma once
#include"./alpha/ETFAlpha.h"
#include"./alpha/IndexAlpha.h"
#include"./alpha/StockAlpha.h"
#define FMT_HEADER_ONLY
#include"./fmt-master/include/fmt/format.h"
#include<string>
#include<iostream>
#include<iomanip>
#include<regex>
#include<memory>
#include<cmath>
#include<list>
using std::regex;
using std::smatch;
using std::regex_match;
using std::unique_ptr;
using std::string;
using std::vector;
namespace my_algo_trading{
class asset{
    public:
    asset(string ticker) : ticker(ticker){};
    virtual ~asset()=0;
    // Conveniently input raw candlestick charts.
    friend void operator>>(unique_ptr<vector<string>> &, unique_ptr<asset> const &);
    // Conveniently output all asset info represented by its head and corresponding alphas.
    friend void operator<<(std::ostream &, asset const &);
    // Calculate the price movement(%), which is the default alpha to display.
    double percent_move()const;
    // Calculate alphas for subclass.
    // The goal of set_alphas is to calculate and set down all designated features of the asset type in one hop.
    // Another goal is to seperate the interface from users.
    virtual void set_alphas()=0;
    // Set header for an instance.
    virtual void set_header()=0;
    // Settle down the instance, same as the user interface.
    bool finalize() {
        set_header();
        set_alphas();
        return true;
    }
    // Base Fields
    string ticker;
    unsigned int period; // Number of records for one period, e.g., receive ticks every 5 mins with 10 ticks at a time.
    vector<double> open;
    vector<double> high;
    vector<double> low;
    vector<double> close;
    vector<size_t> volume;
    vector<double> alphas; // Alphas for subclasses
    static unsigned int width; // Print layout width
    std::list<string> headerList;
};
// Base class virtual destructor definition.
asset::~asset(){}
// Calculate the price percent move during this period, which is the first and benchmark alpha.
double asset::percent_move() const{
    return (close[period-1]-open[0])/open[0];
}
// Data source came from yahoo!finance, the format is:
// ticker (first line)
// Date Open High Low Close Adj_Close Volume (one or more such line)
void operator>>(vector<string> &records, asset &a){
    // Clear previous records.
    a.open.clear();
    a.high.clear();
    a.low.clear();
    a.close.clear();
    a.volume.clear();
    // Insert new records.
    string record;
    regex pattern(R"((\.+) (\d+\.\d+) (\d+\.\d+) (\d+\.\d+) (\d+\.\d+) (\d+\.\d+) (\d+\.*\d*))");
    smatch matches;
    a.period = records.size()-1;// Exclude ticker line
    /*for (auto &record : records){
        
        // Don't match the first line
        if (regex_match(record, matches, pattern)){
            std::cout << record << std::endl;
            // Set basic candlestick fields.
            a.open.push_back(stod(string(matches[1].str())));
            a.high.push_back(stod(string(matches[2].str())));
            a.low.push_back(stod(string(matches[3].str())));
            a.close.push_back(stod(string(matches[5].str())));
            a.volume.push_back(stoi(string(matches[6].str())));
        }
    }*/
}
// Overload output operator to print the asset.
void operator<<(std::ostream &os, asset const &a){
    unsigned int const w = asset::width;
    // Print ticker
    
    // Print alphas and round each double to 2 decimal places.
    for (auto &alpha : a.alphas){
        os << string(fmt::format("{:^{}}", std::round(alpha * 1000.0)/1000.0, w));
    }
    os << std::endl;
}

// Asset sub-classes.
// The main goal for overriding set_alphas and set_header is to leave implementation detail for an asset subclass to itself.
// Different subclasses can have different processing stage of the candlestick data and even more individual traits/member fields. 
// Here we implement it as the naive way to just let the difference be different alpha library sources.
class stock: public asset{
    public:
    stock(string ticker): asset(ticker) {};
    ~stock()override=default;
    // Set alphas for this period.
    void set_alphas() override{
        // Clear previous period's alphas
        alphas.clear();
        // Set new alphas for this period.
        // Set price movement, which is the default alpha.
        alphas.push_back(this->percent_move());
        // Set alphas
        for (auto &calculate_alpha : StockAlphaHelper) {
            alphas.push_back(calculate_alpha(&open,&high,&low,&close,&volume,period));
        }
    }
    // Set header for this period.
    void set_header() override{
        // Clean previous header.
        headerList.clear();
        // Set new header for this period.
        headerList.push_back("price_move%");
        for (auto &header : StockAlphaList){
            headerList.push_back(header);
        }
    }
};
class index: public asset{
    public:
    index(string ticker): asset(ticker) {};
    ~index()override=default;
    void set_alphas() override{
        alphas.clear();
        alphas.push_back(this->percent_move());
        for (auto &calculate_alpha : IndexAlphaHelper) {
            alphas.push_back(calculate_alpha(&open,&high,&low,&close,&volume,period));
        }
    }
    void set_header() override{
        headerList.clear();
        headerList.push_back("price_move%");
        for (auto &header : IndexAlphaList){
            headerList.push_back(header);
        }
    }
};
class etf: public asset{
    public:
    etf(string ticker): asset(ticker) {};
    ~etf()override=default;
    void set_alphas() override{
        alphas.clear();
        alphas.push_back(this->percent_move());
        for (auto &calculate_alpha : ETFAlphaHelper) {
            alphas.push_back(calculate_alpha(&open,&high,&low,&close,&volume,period));
        }
    }
    void set_header() override{
        headerList.clear();
        headerList.push_back("price_move%");
        for (auto &header : ETFAlphaList){
            headerList.push_back(header);
        }
    }
};
};