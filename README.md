```
C: 
  dir: c-tools
      spmOP:
        > read a symmetric sparse Matrix Market file into COO format.
        > convert COO format to CSR format.
      utils.c
        > read a CSV file.
        > write an array of integers to a CSV file.
        > write an array of doubles to a CSV file.

C++:
  dir: read csv to vector
      utils.cpp
        > read csv file and print on to stdout.
        > read CSR format data (3 vectors) and 1-to-1 mapping vector from csv files.
        > write a vector to a CSV file.
        > Read a CSV file into a vector.

  dir: read matrix market symmetric matrix
      utils.cpp
        > read a symmetric sparse Matrix Market file into COO format.
        > convert COO format to CSR format.
        > partition using PaToH (row-part-vec).
        > write a vector to a CSV file.
        > append a CSR matrix to a file in 2D format.
        > print 1d vector to stdout.
```
