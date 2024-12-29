/**
 * @file demod.c
 * @author OUSSET Gaël
 * @brief Test of new demodulations technics
 * @version 0.1
 * @date 2024-12-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "demod.h"
#include "logging.h"
#include "assert.h"
#include "list.h"

int LCADC(
    scatter_t signal,
    double* levels,
    unsigned int nbLevels,
    unsigned int skip,
    scatter_t* output
) {
    //========== Variable declaration
    list_t samples;                              // Temporary list of samples
    point_t* point;                              // Temporary point
    size_t i;                                    // Index for the output signal

    //========== Check arguments
    assert(signal, "Signal cannot be NULL", -1);
    assert(signal->points, "Signal cannot be NULL", -1);
    assert(signal->size, "Signal size cannot be null", -1);

    assert(levels || !nbLevels, "Levels cannot be NULL if nbLevels is not null", -1);
    if (!nbLevels) {
        PRINT(WARN, "No levels provided");
        scatter_create(output, 0);
        assert(output, "Failed to allocate memory for the output", -1);
        return 0;
    }

    //========== Initialize variables
    samples = list_new();

    //========== Sample the signal
    for (i = 1; i < signal->size; i=i+1) {
        for (size_t j = 0; j < nbLevels; j=j+1) {
            if ((scatter_getY(signal, i-1)-levels[j]) *
                (scatter_getY(signal, i)-levels[j]) < 0
            ) {
                if(point_create(&point, scatter_getX(signal, i), levels[j])) {
                    PRINT(ERR, "Failed to create a point");
                    list_delete(samples, point_destroy);
                    return -1;
                }
                samples = list_add_last(samples, point);
                break;
            }
        }
    }

    //========== Create the output
    scatter_create(output, list_length(samples));
    assert(output, "Failed to allocate memory for the output", -1);
    i = 0;
    for (; !list_empty(samples); samples = list_del_first(samples, point_destroy)) {
        point = (point_t*)list_first(samples);
        scatter_setX(*output, i, point->x);
        scatter_setY(*output, i, point->y);
        i = i + 1;
    }

    return 0;
}

