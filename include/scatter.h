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
    int    x;                                    // X coordinate
    double y;                                    // Y coordinate
} point_t;

/**
 * @brief Structure to represent a cloud of points
 * 
 */
typedef struct cloudPoint {
    size_t   size;                               // Number of points
    point_t* points;                             // Array of points
    char*    xName;                              // Name of the X axis
    char*    yName;                              // Name of the Y axis
} *scatter_t;

//========== Functions
/**
 * @brief Create a cloud of points
 * 
 * @param scatter Pointer to the created cloud of points
 * @param size Number of points
 * @return int - 0 if success, -1 otherwise
 */
int scatter_create(scatter_t* scatter, size_t size);

/**
 * @brief Set the name of the axis of a cloud of points
 * 
 * @param scatter Cloud of points
 * @param xName Name of the X axis
 * @param yName Name of the Y axis
 * @return int - 0 if success, -1 otherwise
 */
int scatter_setName(scatter_t scatter, char* xName, char* yName);

/**
 * @brief Create a cloud of points with axis names
 * 
 * @param scatter Pointer to the created cloud of points
 * @param size Number of points
 * @param xName Name of the X axis
 * @param yName Name of the Y axis
 * @return int - 0 if success, -1 otherwise
 */
int scatter_createWName(scatter_t* scatter, size_t size, char* xName, char* yName);

/**
 * @brief Destroy a cloud of points
 * 
 * @param scatter Pointer to the cloud of points to destroy
 */
void scatter_destroy(scatter_t scatter);

/**
 * @brief Return the Y value of a point in the cloud
 * 
 * @param scatter Cloud of points
 * @param index Index of the point
 * @return double - Y value of the point
 */
double scatter_getY(scatter_t scatter, size_t index);

/**
 * @brief Set the Y value of a point in the cloud
 * 
 * @param scatter Cloud of points
 * @param index Index of the point
 * @param y New Y value
 */
void scatter_setY(scatter_t scatter, size_t index, double y);

/**
 * @brief Return the X value of a point in the cloud
 * 
 * @param scatter Cloud of points
 * @param index Index of the point
 * @return int - X value of the point
 */
int scatter_getX(scatter_t scatter, size_t index);

/**
 * @brief Set the X value of a point in the cloud
 * 
 * @param scatter Cloud of points
 * @param index Index of the point
 * @param x New X value
 */
void scatter_setX(scatter_t scatter, size_t index, int x);

/**
 * @brief Print a cloud of points
 * 
 * @param scatter Cloud of points to print
 * @param separator Separator between the points
 * @param print_type Type of message to print
 */
void scatter_print(scatter_t scatter, char separator, print_type_t print_type);

#endif // SCATTER_H