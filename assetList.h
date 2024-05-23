#pragma once
#include"asset.h"
namespace my_algo_trading{
// Comparitors for std::sort, used to view asset list in different ways.
// Compare ticker name lexigraphically ascending.
template<typename T> 
bool comp_ticker (unique_ptr<T> const &a, unique_ptr<T> const &b){
    return (a->ticker)<(b->ticker);
}
// Compare percent price movement descending.
template<typename T> 
bool comp_price_move (unique_ptr<T> const &a, unique_ptr<T> const &b){
    return (a->percent_move())>(b->percent_move());
}

// Asset List.
template<typename T> class assetList{
    public:
    assetList(string val): value(val){}; // List value constructor, e.g., "Stock list"
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
    vector<unique_ptr<T>> objects;
};

// Insert objects into a list using ticker symbols.
template<typename O>
void operator>>(vector<string>& symbols, assetList<O> &l){
    for(auto &ticker:symbols){
        l.objects.push_back(make_unique<O>(ticker));
    }
}
// Overload print list operator.
template<typename O>
ostream& operator<<(ostream& os, assetList<O> const &l){
    unsigned int const w = asset::width;
    // Print list value, typically serving as a list header.
    os << l.value << endl;
    // Print object header in the list.
    for(string &header:l.objects.front()->headerList){
        os << string(format("{:^{}}",header,w));
    }
    os << endl;
    // Print objects that the list holds.
    for(auto &o:l.objects){
        os << *o;
    }
    return os;
}
};