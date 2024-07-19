/*
    This is an asset list class definition header file.
    We heavily use templates to maximally accommodate every asset classes.
*/
#pragma once
#include"asset.h"
namespace my_algo_trading {

/* Define comparitors for std::sort assets and view them in different ways. */
// 1. compare ticker name lexigraphically ascending.
template<typename T> 
bool comp_ticker (unique_ptr<T> const &a, unique_ptr<T> const &b){
    return (a->ticker)<(b->ticker);
}
// 2. compare price movement descending.
template<typename T> 
bool comp_price_move (unique_ptr<T> const &a, unique_ptr<T> const &b){
    return (a->percent_move())>(b->percent_move());
}

/* Asset List */

template<typename T> class assetList{
    public:
    assetList(string val): 
    value(val){}; // A value for the list, e.g., this is a "Stock list".

    // Overload operators to conveniently insert to and output the list.
    template<typename O>friend void operator>>(vector<string>&, assetList<O>&);
    template<typename O>friend ostream& operator<<(ostream&, assetList<O> const&);

    // View asset list by ticker ascending
    void view_by_ticker(){
        std::cout << "[sorted by ticker]";
        sort(objects.begin(), objects.end(), comp_ticker<T>);
        std::cout << *this;
    }
    // View asset list by price movement(%) descending
    void view_by_price_move(){
        std::cout << "[sorted by price move]";
        sort(objects.begin(), objects.end(), comp_price_move<T>);
        std::cout << *this;
    }
    string value;
    vector<unique_ptr<T>> objects; // objects that the list holds
};

// Operator overloading definitions:
// 1. insert objects into a list.
template<typename O>
void operator>>(vector<string>& symbols, assetList<O> &l){
    for(auto &ticker:symbols){
        l.objects.push_back(make_unique<O>(ticker));
    }
}
// 2. print list.
template<typename O>
ostream& operator<<(ostream& os, assetList<O> const &l){
    unsigned int const w = asset::width;
    // print list header, e.g., "stock list".
    os << l.value << endl;
    
    // print the object header.
    for(string &header:l.objects.front()->headerList){
        os << string(format("{:^{}}",header,w));
    }
    os << endl;
    // print objects in the list.
    for(auto &o:l.objects){
        os << *o;
    }
    return os;
}
};