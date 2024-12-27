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

int scatter_create(scatter_t* scatter, size_t size) {
    *scatter = malloc(sizeof(**scatter));

    if (!(*scatter)) {
        PRINT(ERR, "Cannot allocate memory for scatter of size %ld", size);
        return -1;
    }

    (*scatter)->size = size;
    (*scatter)->xName = NULL;
    (*scatter)->yName = NULL;

    (*scatter)->points = calloc(size, sizeof(point_t));
    if (!(*scatter)->points) {
        PRINT(ERR, "Cannot allocate memory for %ld points", size);
        free(scatter);
        return -1;
    }

    return 0;
}

int scatter_setName(scatter_t scatter, char* xName, char* yName) {
    if (!scatter) {
        PRINT(ERR, "Scatter cannot be NULL");
        return -1;
    }

    scatter->xName = xName;
    scatter->yName = yName;

    return 0;
}

int scatter_createWName(scatter_t* scatter, size_t size, char* xName, char* yName) {
    if (scatter_create(scatter, size))
        return -1;

    scatter_setName(*scatter, xName, yName);

    return 0;
}

void scatter_destroy(scatter_t scatter) {
    free(scatter->points);
    free(scatter);
}

double scatter_getY(scatter_t scatter, size_t index) {
    return scatter->points[index].y;
}

void scatter_setY(scatter_t scatter, size_t index, double y) {
    scatter->points[index].y = y;
}

int scatter_getX(scatter_t scatter, size_t index) {
    return scatter->points[index].x;
}

void scatter_setX(scatter_t scatter, size_t index, int x) {
    scatter->points[index].x = x;
}

void scatter_print(scatter_t scatter, char separator, print_type_t print_type) {
    for (size_t i = 0; i < scatter->size; i=i+1)
        PRINT(
            print_type,
            "%lld%c%lf",
            scatter_getX(scatter, i),
            separator,
            scatter_getX(scatter, i)
        );
}