/*
@author: Deniz Uzel
*/

#ifndef SPMOP_H
#define SPMOP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

typedef uint32_t size_ct;

typedef struct {
    size_ct n;              // Number of rows in the symm matrix
    size_ct nnz;            // Number of non-zero elements
    size_ct *indptr;        // Array of row indices
    size_ct *indices;       // Array of column indices
    double *data;       // Array of values
    char matrixName[256];
} CSR;

void freeCSR(CSR* csr);


typedef struct {
    size_ct row;
    size_ct col;
    double data;
} Triplet;


typedef struct{
    size_ct n;              // Number of rows in the symm matrix
    size_ct nnz;            // Number of non-zero elements
    Triplet* items;
    char matrixName[256];
} COO;

void freeCOO(COO* coo);


/**************************
 Matrix process related functions
**************************/

int cooToCsr
(
COO* coo, CSR* csr
);


int readSymmMatrixMarket
(
COO* ptr_coo,
const char* matricesPath,
const char* matrixPath
);

#endif // SPARSE_MATRIX_H