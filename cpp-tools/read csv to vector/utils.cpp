/*
@author: Deniz Uzel
*/

#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <cassert>

using std::vector;
using std::string;

// PRINT a VECTOR
void csvPrint(const string filename, const vector<int>& vec){
    std::cout << "\n" << filename << ":" << "[";
    for (size_t i=0; i<vec.size(); i++){
        if (i != 0){
            std::cout << ", ";
        }
        std::cout << vec[i];
    }
    std::cout << "]\n";
}


// READ INPUT FILES
void readInputDatas(vector<int>& row_ptr, vector<int>& col_ind, vector<int>& weight, vector<int>& taskToProc,
                   int debug){
    read_csv_to_vector("input-data/csr-row.csv", row_ptr);
    read_csv_to_vector("input-data/csr-col.csv", col_ind);
    read_csv_to_vector("input-data/csr-data.csv", weight);
    read_csv_to_vector("input-data/task_to_proc_map.csv", taskToProc);

    if (debug){
        csvPrint("Row Ptr", row_ptr);
        csvPrint("Col Ptr", col_ind);
        csvPrint("Weight Ptr", weight);
        csvPrint("Task to Proc. Mapping", taskToProc);
    }
}

// WRITE vector to CSV
void write_vector_to_csv(const string filename, const vector<int>& vec) {
    std::ofstream myfile(filename);
    for (const auto& elem : vec) {
        myfile << elem << ",";
    }
    myfile.close();
}

// READ CSV and WRITE to a vector
void read_csv_to_vector(const string filename, vector<int>& vec) {
    std::ifstream myfile(filename);
    string line;
    while (std::getline(myfile, line)) {
        std::stringstream ss(line);
        string token;
        while (std::getline(ss, token, ',')) {
            vec.push_back(std::stoi(token));
        }
    }
    myfile.close();
}


