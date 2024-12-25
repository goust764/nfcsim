/**
 * @file scatter.c
 * @author OUSSET Gaël
 * @brief Library to manipulate clouds of points
 * @version 0.1
 * @date 2024-12-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <stdlib.h>
#include "scatter.h"
#include "config.h"
#include "logging.h"

int scatter_create(scatter_t** scatter, size_t size) {
    *scatter = malloc(sizeof(**scatter));

    if (!(*scatter)) {
        PRINT(ERR, "Cannot allocate memory for scatter of size %ld", size);
        return -1;
    }

    (*scatter)->size = size;

    (*scatter)->points = calloc(size, sizeof(point_t));
    if (!(*scatter)->points) {
        PRINT(ERR, "Cannot allocate memory for %ld points", size);
        free(scatter);
        return -1;
    }

    return 0;
}

void scatter_destroy(scatter_t* scatter) {
    free(scatter->points);
    free(scatter);
}

void scatter_print(scatter_t scatter, char separator, print_type_t print_type) {
    for (int i = 0; (size_t)i < scatter.size; i=i+1) {
        PRINT(print_type, "%u%c%lf", scatter.points[i].x, separator, scatter.points[i].y);
    }
}