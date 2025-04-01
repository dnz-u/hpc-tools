/*
@author: Deniz Uzel
*/

#include "spmOP.h"

void freeCOO(COO* coo){
    free(coo->items);
    coo->items = NULL;
}

void freeCSR(CSR* csr){
    free(csr->indptr); free(csr->indices); free(csr->data);
    csr->indptr  = NULL;
    csr->indices = NULL;
    csr->data    = NULL;
}

int cooToCsr(COO* coo, CSR* csr){
    csr->n = coo->n;
    csr->nnz = coo->nnz;

    csr->indptr  = (size_ct*) malloc ( (csr->n+1) * sizeof(size_ct)    );
    csr->indices = (size_ct*) malloc (  csr->nnz  * sizeof(size_ct)    );
    csr->data    = (double*)  malloc (  csr->nnz  * sizeof(double) );

    if (!csr->indptr || !csr->indices || !csr->data){
        fprintf(stderr, "Error creating csr memory allocation.\n");
        exit(EXIT_FAILURE);
    }

    csr->indptr[0] = 0;

    for (size_ct i = 0; i < coo->nnz; ++i){
        csr->indptr[coo->items[i].row+1]++;
    }

    // prefix sum
    for (size_ct i = 1; i <= csr->n+1; ++i){
        csr->indptr[i] = csr->indptr[i] + csr->indptr[i-1];
    }

    for (size_ct i = 0; i < coo->nnz; ++i){
        size_ct row = coo->items[i].row;
        size_ct idx = csr->indptr[row]++;

        csr->indices[idx] = coo->items[i].col;
        csr->data[idx] = coo->items[i].data;
    }

    // revert indptr values
    for (size_ct i = csr->n; 0 < i; --i){
        csr->indptr[i] = csr->indptr[i-1];
    }
    csr->indptr[0] = 0;

    return 0;
};



// reads matrix market format, stores as a vector of triplets
int readSymmMatrixMarket(
            COO* ptr_coo,
            const char* matricesPath,
            const char* matrixPath)
{
    size_t insurance = 16;
    size_t size_fullPath = strlen(matrixPath) + strlen(matrixPath) + insurance;
    char fullPath[size_fullPath];

    snprintf(fullPath, sizeof(fullPath), "%s/%s", matricesPath, matrixPath);
    printf("\nFull Path: %s\n", fullPath);

    FILE* infile = fopen(fullPath, "r");

    if (!infile) {
        fprintf(stderr, "Error opening file: %s\n", matrixPath);
        exit(EXIT_FAILURE);
    }

    // Skip comments
    char line[2048];
    while(fgets(line, sizeof(line), infile) != NULL){
        if (line[0] != '%'){
            break;
        }
    }

    // Variables to store matrix properties
    int rows, cols, nonzeros;
    // sscanf: string scanf
    sscanf(line, "%d %d %d", &rows, &cols, &nonzeros);

    ptr_coo->n     = rows;
    ptr_coo->nnz   = nonzeros*2 - rows;
    ptr_coo->items = (Triplet*)malloc(ptr_coo->nnz * sizeof(Triplet));
    if (!ptr_coo->items) {
        fclose(infile);
        exit(EXIT_FAILURE); // Memory allocation failed
    }

    // Extract matrix name
    const char* lastSlash = strrchr(matrixPath, '/');
    if (lastSlash) {
        strncpy(ptr_coo->matrixName, lastSlash + 1, sizeof(ptr_coo->matrixName) - 1);
        ptr_coo->matrixName[sizeof(ptr_coo->matrixName) - 1] = '\0'; // Null-terminate
    } else {
        strncpy(ptr_coo->matrixName, matrixPath, sizeof(ptr_coo->matrixName) - 1);
        ptr_coo->matrixName[sizeof(ptr_coo->matrixName) - 1] = '\0'; // Null-terminate
    }

    // Read matrix entries
    size_ct row, col;
    double value;

    size_ct count = 0;
    while (fscanf(infile, "%d %d %lf", &row, &col, &value) == 3) {
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

    fclose(infile);
    return 0;
}