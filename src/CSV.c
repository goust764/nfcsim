/**
 * @file CSV.c
 * @author OUSSET Gaël
 * @brief Library to export clouds of points to CSV files
 * @version 0.1
 * @date 2024-12-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "CSV.h"
#include "logging.h"
#include <stdio.h>

int writeCSV(scatter_t* scatters, size_t nbScatters, char* filename) {
    PRINT(INFO, "Writing %ld cloud(s) of points to file \"%s\"", nbScatters, filename);

    //========== Variables declaration
    FILE* file;                                  // File to write
    size_t nbLines;                              // Number of lines in the file

    //========== Check arguments
    if (!(*scatters)) {
        PRINT(ERR, "*scatters cannot be NULL");
        return -1;
    }
    if (filename == NULL || filename[0] == '\0') {
        PRINT(ERR, "Filename cannot be NULL or empty");
        return -1;
    }

    //========== Initializing variables
    //----- Opening the file
    file = fopen(filename, "w");
    if (!file) {
        PRINT(ERR, "Cannot open file %s", filename);
        return -1;
    }

    //----- Initializing the number of lines
    nbLines = 0;
    for (size_t i = 0; i < nbScatters; i=i+1)
        if (scatters[i]->size > nbLines)
            nbLines = scatters[i]->size;

    //========== Writing the *scatters
    //----- Writing the header
    for (size_t i = 0; i < nbScatters; i=i+1) {
        if (scatters[i]->xName)
            fprintf(file, "%s%c", scatters[i]->xName, CSV_SEPARATOR);
        else
            fprintf(file, "X%lld%c", i, CSV_SEPARATOR);
        if (scatters[i]->yName)
            fprintf(file, "%s%s", scatters[i]->yName, CSV_DOUBLE_SEPARATOR);
        else
            fprintf(file, "Y%lld%s", i, CSV_DOUBLE_SEPARATOR);
    }
    fprintf(file, "\n");

    //----- Writing the content
    for (size_t i = 0; i < nbLines; i=i+1) {
        for (size_t j = 0; j < nbScatters; j=j+1) {
            if (i < scatters[j]->size)
                fprintf(file, "%d%c%f%s", scatters[j]->points[i].x, CSV_SEPARATOR, scatters[j]->points[i].y, CSV_DOUBLE_SEPARATOR);
            else
                fprintf(file, "%c%s", CSV_SEPARATOR, CSV_DOUBLE_SEPARATOR);
        }
        fprintf(file, "\n");
    }

    //========== Closing the file
    fclose(file);
    PRINT(SUCC, "Cloud(s) of points written to file \"%s\"", filename);
    
    return 0;
}