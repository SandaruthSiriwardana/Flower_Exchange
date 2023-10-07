#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <map>
#include <ctime>
#include <chrono>
#include <iomanip>

using namespace std;

class Order {
public:
    string orderId;
    string clientId;
    string instrument;
    int side; // 1 for buy, 2 for sell
    double price;
    int quantity;
    string status;
    string rejectReason;
    string timestamp;

    Order(string id, string client, string instr, int s, double p, int qty)
        : orderId(id), clientId(client), instrument(instr), side(s), price(p), quantity(qty), status("New"), rejectReason(""), timestamp("") {}

    void execute(int qty, const string& time) {
        if (quantity >= qty) {
            quantity -= qty;
            if (quantity == 0)
                status = "Filled";
            else
                status = "PartiallyFilled";
            timestamp = time;
        }
    }
};

struct OrderCompare {
    bool operator()(const Order& a, const Order& b) {
        if (a.side == 1) {  // Buy orders should be in descending order of price
            return a.price < b.price;
        } else {  // Sell orders should be in ascending order of price
            return a.price > b.price;
        }
    }
};

class Exchange {
private:
    map<string, priority_queue<Order, vector<Order>, OrderCompare>> orderBooks;

public:
    string GetTime() {
        // Get the current time
        auto now = chrono::system_clock::now();

        // Convert the current time to a time_t object
        time_t t = chrono::system_clock::to_time_t(now);

        // Get milliseconds
        auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;

        // Convert the time to a tm structure in the local time zone
        tm tm_local;

        // Use localtime function with one argument
        localtime(&t);

        stringstream timestamp;
        timestamp << put_time(&tm_local, "%Y%m%d-%H%M%S") << "." << setw(3) << setfill('0') << ms.count();
        return timestamp.str();
    }

    void submitOrder(string clientId, string instrument, int side, double price, int quantity, ofstream& executionReport) {
        if (side == 1) {
            orderBooks[instrument] = priority_queue<Order, vector<Order>, OrderCompare>();
        } else {
            orderBooks[instrument] = priority_queue<Order, vector<Order>, OrderCompare>();
        }

        // Rest of your code for the submitOrder function...
    }

    // Other member functions and variables...
};

int main() {
    // Your main function code...
    return 0;
}
