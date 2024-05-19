#pragma once
#include"asset.h"
#include<vector>
#include<memory>
#include<string>
#include<iostream>
#include<list>
#define FMT_HEADER_ONLY
#include"./fmt-master/include/fmt/format.h"
using std::list;
using std::vector;
using std::unique_ptr;
using std::string;
namespace my_algo_trading{
// Comparitors
// Compare ticker name lexigraphically ascending.
template<typename T> 
bool comp_ticker (unique_ptr<T> const &a, unique_ptr<T> const &b){
    return (a->ticker) < (b->ticker);
}
// Compare price percent movement descending.
template<typename T> 
bool comp_price_move (unique_ptr<T> const &a, unique_ptr<T> const &b){
    return (a->percent_move()) > (b->percent_move());
}
// Asset List
template<typename T> class assetList{
    public:
    template<typename O>
    friend void operator>>(vector<string>&, assetList<O>&);
    template<typename O>
    friend std::ostream& operator<<(std::ostream&, assetList<O> const&);
    // View asset list by ticker ascending
    void view_by_ticker(){
        sort(objects.begin(), objects.end(), comp_ticker<T>);
        std::cout << *this;
    }
    // View asset list by price movement(%) descending
    void view_by_gain(){
        sort(objects.begin(), objects.end(), comp_price_move<T>);
        std::cout << *this;
    }
    // Fields
    vector<unique_ptr<T>> objects;
};
// Create list from ticker symbols
template<typename O>
void operator>>(vector<string>& symbols, assetList<O> &l){
    for(auto &ticker:symbols){
        l.objects.push_back(make_unique<O>(ticker));
    }
}
// Print list
template<typename O>
std::ostream& operator<<(std::ostream& os, assetList<O> const &l){
    unsigned int const w = asset::width;
    // Print header
    for(std::string &header:l.objects.front()->headerList){
        os << string(fmt::format("{:^{}}",header,w));
    }
    os << std::endl;
    // Print objects
    for(auto &o:l.objects){
        //asset &o_a = dynamic_cast<asset&>(*o);
        for (auto & alpha: o->alphas){
            os <<  alpha << std::endl;
        }
    }
    return os;
}
};