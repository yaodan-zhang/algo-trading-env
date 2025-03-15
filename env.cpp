/*
    The multi-stages producer-consumer environment.
*/

#include"assetList.h"
#include"asset.h"

#include<mutex>
#include<thread>
#include<fstream>
#include<stdexcept>
#include<map>
#include<chrono>
#include<condition_variable>
#include<atomic>

using namespace std;
using namespace std::chrono;
using namespace my_algo_trading;

// Static member initialization for printer layout.
unsigned int asset::width = 20;

// Number of consumer threads.
unsigned int const Num_Of_Threads = 8;

// Number of price data of an individual asset to arrive at a time/over a period.
unsigned int const Num_Of_Price_Data_Per_Period = 6;

// Time over which the data is received, e.g. 6 pieces of data is received every 5s for an asset.
auto Timeframe = 5s;

// Modes of viewing the asset list, can select one or more than one modes.
constexpr int view_list_by_ticker = 1;
constexpr int view_list_by_price_move = 0;

// Individual assets under considerations. Similar to lists of outsider subscription. More can be added.
// Here we store the ticker of an asset, which corresponds to the asset symbols in the "price-data" folder.
vector<string> StockSymbols = {"AAPL", "AMZN", "META", "NVDA", "TSLA"};
vector<string> IndexSymbols = {"^DJI", "^GSPC", "^IXIC"};
vector<string> ETFSymbols = {"IEFA", "QQQ", "VTI"};

// Initialize asset lists with headers.
assetList<my_algo_trading::stock> StockList("Stock list");
assetList<my_algo_trading::index> IndexList("Index list");
assetList<my_algo_trading::etf> ETFList("ETF list");

/*
    Below is a multi-stages producer-consumer-viewer environment:
    stage 1 : producer generate tasks -> consumer process tasks
    stage 2: consumer process tasks -> viewer print list(s)
*/

/*
    Below is a thread-safe task queue
 */

std::list<vector<string>> task_queue;
mutex m;
condition_variable stage1_cond;
condition_variable stage2_cond;
atomic_bool print_this_period = false;
atomic_int task_counter;

/* 
 Producer Thread Functions: *note: We use daily price data to simulate intra-day price data.
 Read all symbols of a particular asset. A demo so we store all asset types in the folder "price-data".
 In real world, we expect that each asset type / individual asset comes from a customized source.
*/
void read_symbols(vector<string>& symbols, map<string, unique_ptr<ifstream>> &data_src){
    for (auto &symbol : symbols){
        string file_path = "./price-data/" + symbol + ".csv";
        ifstream file;
        file.open(file_path);
        if (file.is_open()){
            string line;
            getline(file, line);
            data_src[symbol] = make_unique<ifstream>(move(file));
        } else{
            throw runtime_error("Can't open " + file_path + "!");
        }
    }
}

// Read the data source into the variable "data_src".
void read_all_data(map<string, unique_ptr<ifstream>> &data_src){
    // Read stock, index, and etf data.
    read_symbols(StockSymbols, data_src);
    read_symbols(IndexSymbols, data_src);
    read_symbols(ETFSymbols, data_src);

    // ...more can be added and the program scales...

}

// Generate tasks and insert them into the global task queue.
void generate_tasks(map<string, unique_ptr<ifstream>> &data_src){
    for (auto &[ticker,file_ptr] : data_src) {
        vector<string> task = {ticker}; // create the ticker line or the 1st line
        string line;

        // read data lines
        for (unsigned int i=0; i<Num_Of_Price_Data_Per_Period; i++){
            std::getline(*file_ptr,line);
            task.push_back(line);
        }

        lock_guard lk(m);
        task_queue.push_back(task);
        task_counter+=1;
        stage1_cond.notify_one();
    }
}

/* The Producer Thread Function. */ 
void produce(){
    // Read data source.
    map<string, unique_ptr<ifstream>> data_source;
    read_all_data(data_source);

    // Generate tasks into the task queue every fixed amount of time.
    for(;;){
        generate_tasks(data_source);
        print_this_period = true;
        this_thread::sleep_for(Timeframe);
    }}

/* Consumer Thread Functions: */ 
template<typename T>
bool process_task_asset_type(vector<string> &task, string &ticker, vector<string> &type_symbols, assetList<T> &type_list){
    auto check_ticker = [=](unique_ptr<T>&ap){return ap->ticker==ticker;};
    if(find(type_symbols.begin(),type_symbols.end(),ticker)!=type_symbols.end())
    {
        auto ap = find_if(type_list.objects.begin(),type_list.objects.end(),check_ticker);
        if (ap == type_list.objects.end()) 
        {
            return false;
        } else 
        {
            task>>dynamic_cast<asset&>(**ap);
            (**ap).finalize();
            return true;
        }
    } else
    {
        return false;
    }
}

void process_task(vector<string> &task){
    string ticker = task.front();
    
    if (!(process_task_asset_type<my_algo_trading::stock>(task,ticker,StockSymbols,StockList) ||
    process_task_asset_type<my_algo_trading::index>(task,ticker,IndexSymbols,IndexList) ||
    process_task_asset_type<my_algo_trading::etf>(task,ticker,ETFSymbols,ETFList))) {
        throw runtime_error("asset doesn't exist!");
    }

}

/* The Consumer Function */
void consume(){
    vector<string> task;
    for(;;){
        { 
            unique_lock<mutex> lk(m);
            stage1_cond.wait(lk,[]{return !task_queue.empty();});
            task = move(task_queue.front());
            task_queue.pop_front();
        }

        process_task(task);
        task_counter -= 1;
        stage2_cond.notify_one();

    }
}

/* Printing Thread Functions: */
void view() {
    for(;;){
        { 
            unique_lock<mutex> lk(m);
            stage2_cond.wait(lk,[]{return task_counter==0 && print_this_period;});
        }

        auto readable_time_point = format("{0:%F}T{0:%T%z}\n", system_clock::now());
        
        cout << readable_time_point;
        if constexpr(view_list_by_ticker) {
            
            StockList.view_by_ticker();
            IndexList.view_by_ticker();
            ETFList.view_by_ticker();

        }

        if constexpr(view_list_by_price_move) {

            StockList.view_by_price_move();
            IndexList.view_by_price_move();
            ETFList.view_by_price_move();

        }

        cout << endl;
        print_this_period = false;
    }
}


int main() {
    // Initialize asset lists
    StockSymbols>>StockList;
    IndexSymbols>>IndexList;
    ETFSymbols>>ETFList;
    
    // We use jthread as an RAII improvement from thread.
    // Spawn producer thread.
    jthread producer(produce);

    // Spawn consumer threads.
    vector<jthread> consumers;
    for(unsigned int i = 0 ; i < Num_Of_Threads ; i++){

        consumers.push_back(jthread(consume));

    }

    // Spawn printing thread.
    jthread viewer(view);
        
    return 0;
}