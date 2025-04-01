/*
@author: Deniz Uzel
*/

#ifndef UTILS_H
#define UTILS_H

#include "spmOP.h"

#define MAX_LINE_LENGTH 64


size_ct* readCsv
(
const char *filename, size_ct arraySize
);


void writeArrayToCsv
(
const char *filename, size_ct* array, size_ct arraySize
);


void writeArrayToCsvDouble
(
const char *filename, double* array, size_ct arraySize
);

#endif