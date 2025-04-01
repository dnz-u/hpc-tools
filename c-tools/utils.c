/*
@author: Deniz Uzel
*/


#include "utils.h"

size_ct* readCsv(const char *filename, size_ct arraySize) {

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(EXIT_FAILURE);
    }

    size_ct* array = (size_ct*)malloc(arraySize * sizeof(size_ct));
    if (!array){
        fprintf(stderr, "Error creating array memory allocation.\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_ct count = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file)) {
        // Check for non-empty line
        if (line[0] != '\n') {
            int value;
            if (sscanf(line, "%d", &value) == 1) {
                if (count < arraySize) {
                    array[count] = value;
                    count++;
                } else {
                    fprintf(stderr, "Error: Array size is insufficient.\n");
                    fclose(file);
                    exit(EXIT_FAILURE);
                }
            } else {
                fprintf(stderr, "Warning: Invalid line encountered: %s", line);
                exit(EXIT_FAILURE);
            }
        }
    }

    fclose(file);

    // Check if the number of lines read matches the expected size
    if (count != arraySize) {
        fprintf(stderr, "Error: Expected %d lines but read %d lines.\n", arraySize, count);
        exit(EXIT_FAILURE);
    }
    return array;
}


void writeArrayToCsv(const char *filename, size_ct* array, size_ct arraySize) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    for (size_ct i = 0; i < arraySize; i++) {
        fprintf(file, "%u\n", array[i]);  // Write each element followed by a newline
    }

    fclose(file);
}

void writeArrayToCsvDouble(const char *filename, double* array, size_ct arraySize) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    for (size_ct i = 0; i < arraySize; i++) {
        fprintf(file, "%u\n", array[i]);  // Write each element followed by a newline
    }

    fclose(file);
}