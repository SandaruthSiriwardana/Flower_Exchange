// csv_functions.h

#ifndef CSV_FUNCTIONS_H
#define CSV_FUNCTIONS_H

#include <string>
#include <vector>

std::vector<std::vector<std::string>> readCSV(const std::string& filename);
void generateCSV(const std::string& filename, const std::vector<std::vector<std::string>>& data);

#endif
