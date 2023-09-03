#include <iostream>
#include <string>
#include <vector>
#include "csv_functions.h" // Include the header file

using namespace std;

int main() {
    // Read CSV file
    std::vector<std::vector<std::string>> csvData = readCSV("../example/test1.csv");

    // Check if there are at least 5 rows in the CSV data

        // Print the first 5 rows
    for (size_t i = 0; i < csvData.size(); ++i) {
        if(i==0 || i==1 ){
            continue;
        }
        for (const std::string& cell : csvData[i]) {
            std::cout << cell << "\t";
        }
        std::cout << std::endl;
    }

    // Generate a new CSV file
    // generateCSV("output.csv", csvData);

    return 0;
}
