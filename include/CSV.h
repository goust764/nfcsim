/**
 * @file CSV.h
 * @author OUSSET Gaël
 * @brief Header file for CSV export
 * @version 0.1
 * @date 2024-12-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CSV_H
#define CSV_H

#include "config.h"
#include "scatter.h"

#define CSV_SEPARATOR ','
#define CSV_DOUBLE_SEPARATOR ",,"

//========== Functions
/**
 * @brief Write an array of scatters to a CSV file
 * 
 * @param scatters Array of scatters to write
 * @param nbScatters Number of scatters in the array
 * @param filename Name of the file to write
 * @return int - 0 if success, -1 otherwise
 */
int writeCSV(scatter_t** scatters, size_t nbScatters, char* filename);

#endif // CSV_H