/* Data in this project comes from yahoo!finance */
#include"assetList.h"
#include"asset.h"
#include<mutex>
#include<thread>
#include<fstream>
#include<stdexcept>
#include<map>
#include<chrono>
using namespace std;
using namespace my_algo_trading;
// Static member initialization for print layout.
unsigned int asset::width = 10;

// Number of threads.
int const Num_Of_Threads = 5;

// Pre-set individual assets for consideration. Similar to lists of outsider subscriptions.
// Here we store the ticker name of an asset, which corresponds to the tickers in the "ticks" folder.
vector<string> StockSymbols = {"AAPL", "AMZN", "META", "NVDA", "TSLA"};
vector<string> IndexSymbols = {"^DJI", "^GSPC", "^IXIC"};
vector<string> ETFSymbols = {"IEFA", "QQQ", "VTI"};

// Initialize asset lists
assetList<my_algo_trading::stock> StockList;
assetList<my_algo_trading::index> IndexList;
assetList<my_algo_trading::etf> ETFList;

// Task queue, each task is a vector of strings as follows:
// ticker (first line)
// Date Open High Low Close Adj_Close Volume (one or more such line, total number is the period)
std::list<vector<string>> task_queue;
// Thread-safe list operations.
mutex m; 
void insert_task(vector<string> &task){
    lock_guard guard(m);
    task_queue.push_back(task);
}
bool get_task(vector<string> &task){
    lock_guard guard(m);
    if (task_queue.empty()){
        return false;
    }
    task = task_queue.front();
    task_queue.pop_front();
    return true;
}

// Producer Thread Functions: *note: We use daily ticks to simulate intra-day ticks.
// Read all symbols of a particular asset type. This function is just a demo so we store all asset types in the same folder "ticks".
// In real-world real-time data streaming, we expect that each asset type / individual asset comes from a different (customized) source.
void read_symbols(vector<string>& symbols, map<string, unique_ptr<ifstream>> &data_src){
    for (auto &symbol : symbols){
        string file_path = "./ticks/"+symbol+".csv";
        ifstream file;
        file.open(file_path);
        if (file.is_open()){
            string line;
            std::getline(file, line); // Skip header row
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
}
// Generate tasks and insert them into the global task queue.
void generate_tasks(map<string, unique_ptr<ifstream>> &data_src){
    unsigned int const period = 10; // # data pieces for one asset to receive at one time
    for (auto &[ticker,file_ptr] : data_src) {
        vector<string> task = {ticker}; // create ticker line
        string line;
        // read price data line
        for (unsigned int i=0;i<period;i++){
            std::getline(*file_ptr,line);
            //cout << line << endl;
            task.push_back(line);
        }
        insert_task(task);
    }
}
// The main producer function.
void produce(){
    // Read data source.
    map<string, unique_ptr<ifstream>> data_source;
    read_all_data(data_source);
    // Generate tasks into the task queue every fixed amount of time.
    for(;;){
        generate_tasks(data_source);
        this_thread::sleep_for(chrono::seconds(5));
    }
}

// Consumer Thread Functions:
template<typename T>
bool process_task_asset_type(vector<string> &task, string &ticker, vector<string> &type_symbols, assetList<T> &type_list){
    auto check_ticker = [=](unique_ptr<T>&ap){return ap->ticker==ticker;};
    if(std::find(type_symbols.begin(),type_symbols.end(),ticker)!=type_symbols.end()){
        
        auto ap = std::find_if(type_list.objects.begin(),type_list.objects.end(),check_ticker);
        if (ap == type_list.objects.end()) {return false;}
        else {
            cout << ticker<<"1" << endl;
            task>>dynamic_cast<asset&>(**ap);
            
            (**ap).finalize();
            cout << "success1";
            return true;
            }
    } else {
        return false;
    }
}
void process_task(vector<string> &task){
    string ticker = task.front();
    // Determin asset type and modify the conrresponding asset list
    if (!(process_task_asset_type<my_algo_trading::stock>(task,ticker,StockSymbols,StockList) ||
    process_task_asset_type<my_algo_trading::index>(task,ticker,IndexSymbols,IndexList) ||
    process_task_asset_type<my_algo_trading::etf>(task,ticker,ETFSymbols,ETFList))) {
        throw runtime_error("asset doesn't exist!");
    }
    cout << "1" << endl;
}
// The main consumer function.
void consume(){
    // Spin on the task queue and dequeue a task and process it.
    vector<string> task;
    for(;;){
        if (get_task(task)){
            process_task(task);
            cout << "0" << endl;
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

int main(){
    // Initialize asset lists
    StockSymbols>>StockList;
    IndexSymbols>>IndexList;
    ETFSymbols>>ETFList;
    // Spawn producer and consumer threads.
    jthread producer(produce);
    vector<jthread> consumers;
    for(int i=0;i<Num_Of_Threads;i++){
        consumers.push_back(jthread(consume));
    }
    // Print list every fixed amount of time
    // Optimal: Print list based on condition: 1. task_queue is empty 2. every future was waited for
    for(;;){
        cout<<StockList;
        cout<<IndexList;
        cout<<ETFList;
        this_thread::sleep_for(chrono::seconds(5));
    }
    return 0;
}