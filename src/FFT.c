/**
 * @file FFT.c
 * @author OUSSET Gaël
 * @brief Implementation of the FFT functions
 * @version 0.1
 * @date 2024-12-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "FFT.h"
#include "logging.h"
#include "assert.h"
#include <stdlib.h>
#include <math.h>
#include <complex.h>

double fft_getAvgSamplingRate(scatter_t timeSerie) {
    //========== Variables declaration
    double avgSamplingRate = 0;

    //========== Check arguments
    assert(timeSerie, "Time serie cannot be NULL", -1);
    assert(timeSerie->points, "Time serie cannot be NULL", -1);
    assert(timeSerie->size, "Time serie size cannot be null", -1);

    //========== Calculate the average sampling rate
    for (size_t i = 1; i < timeSerie->size; i=i+1) {
        avgSamplingRate =   avgSamplingRate              + 
                            scatter_getX(timeSerie, i  ) - 
                            scatter_getX(timeSerie, i-1) ;
    }
    avgSamplingRate = (double)(timeSerie->size-1) / avgSamplingRate * 1e9;

    return avgSamplingRate;
}

int fft_Iterative(scatter_t in, scatter_t out) {
    //========== Variables declaration
    int j = 0;
    int bit;
    double temp;
    double angle;
    complex double wlen;
    complex double w;
    complex double u;
    complex double t;
    complex double* X;

    //========== Check variables
    assert(in, "Input cloud of points cannot be NULL", -1);
    assert(in->points, "Input cloud of points cannot be NULL", -1);
    assert(in->size, "Input cloud of points size cannot be null", -1);
    assert(out->points, "Output cloud of points cannot be NULL", -1);
    assert(
        out->points && out->size,
        "Output cloud of points should be initialized",
        -1
    );

    // Check if the size of the output cloud is a power of 2
    assert(
        !(out->size & (out->size - 1)),
        "Output cloud of points size should be a power of 2",
        -1
    );

    X = malloc(in->size * sizeof(complex double));
    assert(X, "Failed to allocate memory for the complex cloud of points", -1);

    for (size_t i = 0; i < in->size; i=i+1)
        X[i] = scatter_getY(in, i);

    //========== Bit-reversal permutation
    for (int i = 1; (size_t)i < in->size; i=i+1) {
        bit = (int)in->size >> 1;
        while (j >= bit) {
            j = j - bit;
            bit = bit >> 1;
        }
        j = j + bit;
        if (i < j) {
            temp = scatter_getY(in, (size_t)i);
            X[i] = scatter_getY(in, (size_t)j);
            X[j] = temp;
        }
    }
    
    //========== FFT
    for (int len = 2; (size_t)len <= in->size; len=len<<1) {
        angle = -2.0 * M_PI / len;
        wlen = cexp(I * angle);
        for (int i = 0; (size_t)i < in->size; i=i+len) {
            w = 1.0;
            for (j = 0; j < len / 2; j=j+1) {
                u = X[i + j];
                t = w * X[i+j+len/2];
                X[i+j]       = u + t;
                X[i+j+len/2] = u - t;
                w = w * wlen;
            }
        }
    }

    //========== Set the output cloud of points
    for (size_t i = 0; i < in->size; i=i+1)
        scatter_setY(out, i, cabs(X[i]));

    free(X);
    return 0;
}

int fft_Compute(scatter_t timeSerie, scatter_t* freqSerie) {
    //========== Variable declaration
    double AvgSamplingRate;

    //========== Check arguments
    assert(timeSerie, "Time serie cannot be NULL", -1);
    assert(timeSerie->points, "Time serie cannot be NULL", -1);
    assert(timeSerie->size, "Time serie size cannot be null", -1);

    //========== Allocate memory for the freqSerie
    assert(
        !scatter_create(freqSerie, timeSerie->size),
        "Failed to allocate memory for the frequency serie",
        -1
    )

    //========== Apply the FFT
    PRINT(INFO, "Processing the FFT");
    if (fft_Iterative(timeSerie, *freqSerie)) {
        PRINT(ERR, "Failed to apply the iterative FFT");
        scatter_destroy(*freqSerie);
        return -1;
    }

    //========== Apply the frequencies value on the X axis and convert the
    //           amplitudes in real
    PRINT(INFO, "Converting the amplitudes in real");
    AvgSamplingRate = fft_getAvgSamplingRate(timeSerie);
    for (size_t i = 0; i < (*freqSerie)->size; i=i+1) {
        scatter_setX(
            *freqSerie,
            i, 
            i < (*freqSerie)->size/2 ?
                (int)((double)i * AvgSamplingRate / (double)(timeSerie->size)) :
                (int)((double)((int)i - (int)(*freqSerie)->size) * AvgSamplingRate / (double)(timeSerie->size))
        );
        scatter_setY(*freqSerie, i, cabs((complex double)(scatter_getY(*freqSerie, i))));
    }

    PRINT(SUCC, "FFT successfully applied");
    return 0;
}