/*
@author: Deniz Uzel
*/

#include "utils.hpp"
#include "pth.h"


// reads matrix market format, stores as a vector of triplets
int readMatrixMarket(cooMatrix* ptr_coo,
                      const string& matricesPath,
                      const string& matrixPath){
    string fullPath = matricesPath + matrixPath;

    std::ifstream infile(fullPath);
    if (!infile) {
        std::cerr << "Error opening file: " << matrixPath << std::endl;
        return -1;
    }

    while(infile.peek() == '%'){
        infile.ignore(2048, '\n');
    }

    // Variables to store matrix properties
    string line;
    int rows, cols, nonzeros;
    infile >> rows >> cols >> nonzeros;

    ptr_coo->n = rows;
    ptr_coo->nnz = nonzeros*2 - rows;
    ptr_coo->items.resize(ptr_coo->nnz);
    ptr_coo->matrixName = matrixPath.substr(matrixPath.find('/') + 1);

    // Read matrix entries
    int row, col;
    double value;
    int count = 0;
    while (infile >> row >> col >> value) {
        if (row==col){
            ptr_coo->items[count].row = row - 1;
            ptr_coo->items[count].col = col - 1;
            ptr_coo->items[count].data = value;
            count++;
        } else{
            ptr_coo->items[count].row = row - 1;
            ptr_coo->items[count].col = col - 1;
            ptr_coo->items[count].data = value;
            count++;

            ptr_coo->items[count].row = col - 1;
            ptr_coo->items[count].col = row - 1;
            ptr_coo->items[count].data = value;
            count++;

        }

    }
    assert(count == ptr_coo->nnz && "coo matrix number of non-zeros not equal to item count.");
    //std::cout << "count: " << count << "ttnz:" << ttnz << "\n";
    infile.close();
    return 0;
}

// converts coo to csr
int COOtoCSR(cooMatrix* ptr_coo, csrMatrix* ptr_csr){
    // ! CSR column indices in "indices" are not sorted

    //cout<< ptr_coo->items.size() << " " << ptr_coo->nnz <<"\n---------\n";

    assert(static_cast<int>(ptr_coo->items.size()) == ptr_coo->nnz && "items vector length and nnz mismatch.");

    int n_rows = ptr_coo->n;
    int n_items = ptr_coo->nnz;
    ptr_csr->n = n_rows;
    ptr_csr->nnz = n_items;
    ptr_csr->matrixName = ptr_coo->matrixName;

    // initialize that
    vector<unordered_set<int>> row_to_itemNums_map(n_rows);
    for (int i = 0; i < n_rows; i++)
        row_to_itemNums_map[i] = unordered_set<int>();

    // initialize inptr vector
    int indptr_size = n_rows + 1;
    ptr_csr->indptr.resize(indptr_size);

    for (int i=0; i<indptr_size; i++){
        ptr_csr->indptr[i] = 0;
    }

    // initialize indices vector
    ptr_csr->indices.resize(n_items);
    for (int i=0; i<n_items; i++){
        ptr_csr->indices[i] = 0;
    }

    // for csr usage data value is not important. Consider as 1, exist.
    // initialize data vector
    ptr_csr->data.resize(n_items);
    for (int i=0; i<n_items; i++){
        ptr_csr->data[i] = 0;
    }

    // count how many col a row has
    // store col indices of each row
    for (int i=0; i<n_items; i++){
        int row = ptr_coo->items[i].row;
        ptr_csr->indptr[row+1]++;
        row_to_itemNums_map[row].insert(i);
    }

    // debug: calculates the max num of cols in a row
    int mmax = 0;
    for (int i=0; i<n_rows+1; i++){
        int r = ptr_csr->indptr[i];
        if (r > mmax) mmax=r;
    }
    // cout <<"mmax: " << mmax << "\n";


    // prefix sum on rowCount
    for (int i=1; i<indptr_size; i++){
        ptr_csr->indptr[i] = ptr_csr->indptr[i] + ptr_csr->indptr[i-1];
    }

    int k = 0;
    for (int i=0; i<n_rows; i++){
        for (auto it = row_to_itemNums_map[i].begin(); it != row_to_itemNums_map[i].end(); ++it) {
            //cout << "i:" << i <<". k:" << k << ". it: " <<*it <<"\n";

            ptr_csr->indices[k] = ptr_coo->items[*it].col;
            // // for csr usage data value is not important. Consider as 1, exist.
            // ptr_csr->data[k] = ptr_coo->items[*it].data;
            k++;
        }
    }
    assert(k == n_items && "nnz number and creating indices, data count mismatch.");
    return 0;
}

// creates partition vector, returns cut size
int partition_patoh(int * const rpv,
                     const cooMatrix* M,
                     int nparts){
    /*
    This function copied and modified from vptStats, "pth.h", "pth.c", "utils.cpp".

    Author of the copied section is Nabil Aboubaker, PhD.
    /github.com/Nabil
    */

    int _c          = M->n;    // number of cell of the hp
    int _n          = M->n;    // number of nets of the hp
    int _nnz        = M->nnz;  // number of non-zero elements
    int _nconst     = 1;                          // number of constraints
    int useFixCells = 0;                          // if non-zero, partitioning is done pre-assigned cells
    int cut;

    double imbalance = 0.1;

    vector<int> cwghts(_c, 0);           // array of size c× nconst that stores the weights of each cell.
    vector<int> nwghts(_n, 1);           // array of size n that stores the cost of each net.
    vector<int> xpins(_n+2, 0);          // stores the beginning index of pins (cells) connected to nets.
    vector<int> pins(_nnz);              // pin-lists of nets. Cells connected to net nj are stored in pins[xpins[j]] through pins[xpins[j+1]-1].
    vector<int> partweights(nparts, 0);  // array of size pargs-> k× nconst that returns the total part weight of each part.

    const vector<Triplet> item = M->items;

    for (int i = 0; i < _nnz; ++i) {
        xpins[item[i].col + 2]++;
        cwghts[item[i].row]++;
    }

    for (int i = 2; i < _n+2; ++i) {
        xpins[i] += xpins[i-1];
    }

    assert(xpins[_n+1] == _nnz);

    for (int i = 0; i < _nnz; ++i) {
        pins[xpins[item[i].col +1]++] = item[i].row;
    }

    assert(xpins[_n] == xpins[_n+1]);

    pthdata pth;

    pth.c      = _c;
    pth.n      = _n;
    pth.p      = _nnz;
    pth.nconst = _nconst;

    pth.cwghts = cwghts.data();
    pth.nwghts = nwghts.data();
    pth.pins   = pins.data();
    pth.xpins  = xpins.data();

    cut = pth_partition(&pth, nparts, imbalance, rpv, useFixCells);

    return cut;
}


void write_csv(const std::vector<int>& vec, const std::string& folder,
               const std::string& filename) {
    namespace fs = std::filesystem;

    // Construct the full path to the file
    fs::path dir(folder);
    fs::path file_path = dir / filename;

    // Check if the directory exists, create it if it does not
    if (!fs::exists(dir)) {
        try {
            fs::create_directories(dir);
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Failed to create directory: " << e.what() << std::endl;
            return;
        }
    }

    std::ofstream out(file_path);
    if (out) {
        for (auto& v : vec) {
            out << v << std::endl;
        }
    } else {
        std::cerr << "Failed to open file " << filename << std::endl;
    }
}

void appendCSRMatrixToFile(csrMatrix* ptr_csr, const std::string& filename, int nparts) {
    std::ofstream file(filename, std::ios_base::app); // Open file in append mode
    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return;
    }

    for (int row = 0; row < nparts; ++row) {
        int row_start = ptr_csr->indptr[row];
        int row_end = ptr_csr->indptr[row + 1];

        for (int col = 0; col < nparts; ++col) {
            bool found = false;
            for (int i = row_start; i < row_end; ++i) {
                if (ptr_csr->indices[i] == col) {
                    file << std::setw(5) << ptr_csr->data[i];
                    found = true;
                    break;
                }
            }
            if (!found) {
                file << std::setw(5) << 0;
            }
        }
        file << std::endl;
    }
    file << "--------------------" << std::endl;

    file.close();
}


void print1dVector(vector<int>& vec){
    std::cout << "< ";
    for (size_t i=0; i<vec.size(); i++){
        std::cout << vec[i] <<", ";
    }
    std::cout << ">\n";
}
