/**
 * @file scatter.h
 * @author OUSSET Gaël
 * @brief Library to manipulate clouds of points
 * @version 0.1
 * @date 2024-12-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef SCATTER_H
#define SCATTER_H

#include <stdlib.h>
#include "logging.h"

//========== Structures
/**
 * @brief Structure to represent a point in a 2D space
 * 
 */
typedef struct {
    unsigned int x;                              // X coordinate
    double       y;                              // Y coordinate
} point_t;

/**
 * @brief Structure to represent a cloud of points
 * 
 */
typedef struct cloudPoint {
    size_t  size;                               // Number of points
    point_t *points;                            // Array of points
} scatter_t;

//========== Functions
/**
 * @brief Create a cloud of points
 * 
 * @param scatter Pointer to the created cloud of points
 * @param size Number of points
 * @return int - 0 if success, -1 otherwise
 */
int scatter_create(scatter_t** scatter, size_t size);

/**
 * @brief Destroy a cloud of points
 * 
 * @param scatter Pointer to the cloud of points to destroy
 */
void scatter_destroy(scatter_t* scatter);

/**
 * @brief Print a cloud of points
 * 
 * @param scatter Cloud of points to print
 * @param separator Separator between the points
 * @param print_type Type of message to print
 */
void scatter_print(scatter_t scatter, char separator, print_type_t print_type);

#endif // SCATTER_H