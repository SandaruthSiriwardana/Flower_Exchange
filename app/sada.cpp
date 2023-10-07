#include <iostream>
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <map>

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

string Exchange::GetTime() {
    // Get the current time
    auto now = std::chrono::system_clock::now(); // Use std::chrono::

    // Convert the current time to a time_t object
    std::time_t t = std::chrono::system_clock::to_time_t(now); // Use std::

    // Get milliseconds
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000; // Use std::

    // Convert the time to a tm structure in the local time zone
    std::tm tm_local;

    // Use localtime_s function (Windows) or localtime_r function (POSIX)
    #ifdef _WIN32
        localtime_s(&tm_local, &t); // Windows
    #else
        localtime_r(&t, &tm_local); // POSIX
    #endif

    std::stringstream timestamp;
    timestamp << std::put_time(&tm_local, "%Y%m%d-%H%M%S") << "." << std::setw(3) << std::setfill('0') << ms.count();
    return timestamp.str();
}

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
    int orderCount;
    string GetTime() {
        auto now = chrono::system_clock::now();
        time_t t = chrono::system_clock::to_time_t(now);
        auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
        tm tm_local;
        localtime(&t, &tm_local);
        stringstream timestamp;
        timestamp << put_time(&tm_local, "%Y%m%d-%H%M%S") << "." << setw(3) << setfill('0') << ms.count();
        return timestamp.str();
    }

public:
    Exchange() : orderCount(0) {}

    void submitOrder(string clientId, string instrument, int side, double price, int quantity, ofstream& executionReport) {
        orderCount++;
        string orderId = "ord" + to_string(orderCount);
        string time_str = GetTime();
        Order order(orderId, clientId, instrument, side, price, quantity);

        if (orderBooks.find(instrument) == orderBooks.end()) {
            orderBooks[instrument] = side == 1 ? priority_queue<Order, vector<Order>, OrderCompare>() : priority_queue<Order, vector<Order>, OrderCompare, OrderCompare>();
        }

        if (side == 2) {  // Sell order
            while (!orderBooks[instrument].empty() && orderBooks[instrument].top().price <= order.price && order.quantity > 0) {
                Order topOrder = orderBooks[instrument].top();
                if (topOrder.quantity >= order.quantity) {
                    topOrder.execute(order.quantity, time_str);
                    executionReport << topOrder.orderId << "," << topOrder.clientId << "," << topOrder.instrument << ","
                                    << topOrder.side << ",Fill," << order.quantity << "," << topOrder.price << ",, " << topOrder.timestamp << "\n";
                    orderBooks[instrument].pop();
                } else {
                    order.execute(topOrder.quantity, time_str);
                    executionReport << topOrder.orderId << "," << topOrder.clientId << "," << topOrder.instrument << ","
                                    << topOrder.side << ",Fill," << topOrder.quantity << "," << topOrder.price << ",, " << topOrder.timestamp << "\n";
                    orderBooks[instrument].pop();
                }
            }
        } else {  // Buy order
            while (!orderBooks[instrument].empty() && orderBooks[instrument].top().price >= order.price && order.quantity > 0) {
                Order topOrder = orderBooks[instrument].top();
                if (topOrder.quantity >= order.quantity) {
                    topOrder.execute(order.quantity, time_str);
                    executionReport << topOrder.orderId << "," << topOrder.clientId << "," << topOrder.instrument << ","
                                    << topOrder.side << ",Fill," << order.quantity << "," << topOrder.price << ",, " << topOrder.timestamp << "\n";
                    orderBooks[instrument].pop();
                } else {
                    order.execute(topOrder.quantity, time_str);
                    executionReport << topOrder.orderId << "," << topOrder.clientId << "," << topOrder.instrument << ","
                                    << topOrder.side << ",Fill," << topOrder.quantity << "," << topOrder.price << ",, " << topOrder.timestamp << "\n";
                    orderBooks[instrument].pop();
                }
            }
        }

        if (order.quantity > 0) {
            order.status = "Rejected";
            order.rejectReason = "No matching orders";
            executionReport << order.orderId << "," << order.clientId << "," << order.instrument << ","
                            << order.side << "," << order.status << ",," << order.price << "," << order.rejectReason << ", " << time_str << "\n";
        }
    }
};

int main() {
    Exchange exchange;
    ofstream executionReport("execution_report.csv");

    if (!executionReport.is_open()) {
        cout << "Failed to open execution_report.csv" << endl;
        return 1;
    }

    // Header of the execution report
    executionReport << "OrderID,Client_order_id,Instrument,Side,Execution_Status,Quantity,Price,Reason_If_Rejected,Transaction_time\n";

    string clientId, instrument, sideStr, priceStr, quantityStr;
    int side;
    double price;
    int quantity;

    cout << "Enter orders (Client Order ID, Instrument, Side (1 for Buy, 2 for Sell), Price, Quantity):" << endl;

    while (cin >> clientId >> instrument >> sideStr >> priceStr >> quantityStr) {
        side = stoi(sideStr);
        price = stod(priceStr);
        quantity = stoi(quantityStr);

        exchange.submitOrder(clientId, instrument, side, price, quantity, executionReport);
    }

    executionReport.close();
    return 0;
}
