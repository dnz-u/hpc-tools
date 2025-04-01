/*
@author: Deniz Uzel
*/

#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint> // for uint16_t
#include <cassert>
#include <fstream>
#include <unordered_set>
#include <iomanip>  // Required for std::setw
#include <filesystem> // For C++17 and later

using std::vector, std::string, std::unordered_set;


struct csrMatrix{
    int n;   // row size in csr matrix
    int nnz; // number of non-zeros in csr matrix
    vector<int> indptr;
    vector<int> indices;
    vector<int> data;
    string matrixName;
};


struct Triplet{
    int row;
    int col;
    double data;
};


struct cooMatrix{
    int n;   // row size in coo matrix
    int nnz; // number of non-zeros in coor matrix
    vector<Triplet> items;
    string matrixName;
};


// reads matrix market format, stores as a vector of triplets
int readMatrixMarket(cooMatrix* ptr_coo, const string& matricesPath,
                     const string& matrixPath);

// converts coo to csr
int COOtoCSR(cooMatrix* ptr_coo, csrMatrix* ptr_csr);

// calculate the row part vector
int partition_patoh(int * const rowpartvector, const cooMatrix* M, int nparts);



void write_csv(const std::vector<int>& vec, const std::string& folder,
               const std::string& filename);


void appendCSRMatrixToFile(csrMatrix* ptr_csr, const std::string& filename,
                           int nparts);

void print1dVector(vector<int>& vec);

#endif // UTILS_HPP