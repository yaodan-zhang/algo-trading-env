/*
    This is an asset list class definition header file.
    Heavy use of templates to accommodate every asset class.
*/

#pragma once

#include"asset.h"

namespace my_algo_trading {

/* Comparitors */
template<typename T> 
bool comp_ticker (unique_ptr<T> const &a, unique_ptr<T> const &b){
    return (a->ticker)<(b->ticker);
}

template<typename T> 
bool comp_price_move (unique_ptr<T> const &a, unique_ptr<T> const &b){
    return (a->percent_move())>(b->percent_move());
}

template<typename T> class assetList{
    public:

    assetList(string val): 

    value(val){};
   
    template<typename O>friend void operator>>(vector<string>&, assetList<O>&);
    template<typename O>friend ostream& operator<<(ostream&, assetList<O> const&);

    void view_by_ticker(){
        std::cout << "[sorted by ticker]";
        sort(objects.begin(), objects.end(), comp_ticker<T>);
        std::cout << *this;
    }
    
    void view_by_price_move(){
        std::cout << "[sorted by price move]";
        sort(objects.begin(), objects.end(), comp_price_move<T>);
        std::cout << *this;
    }

    string value;
    vector<unique_ptr<T>> objects;
};

template<typename O>
void operator>>(vector<string>& symbols, assetList<O> &l){
    for(auto &ticker:symbols){
        l.objects.push_back(make_unique<O>(ticker));
    }
}

template<typename O>
ostream& operator<<(ostream& os, assetList<O> const &l){
    unsigned int const w = asset::width;
    
    // print list header, e.g., "stock list".
    os << l.value << endl;
    
    // print object header.
    for(string &header:l.objects.front()->headerList){
        os << string(format("{:^{}}",header,w));
    }
    os << endl;

    // print objects
    for(auto &o:l.objects){
        os << *o;
    }

    return os;
}
};