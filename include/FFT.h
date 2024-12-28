/**
 * @file FFT.h
 * @author OUSSET Gaël
 * @brief Header file for FFT.c
 * @version 0.1
 * @date 2024-12-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef FFT_H
#define FFT_H

#include "scatter.h"

/**
 * @brief Return the average sampling rate of a time serie
 * 
 * @param timeSerie Time serie to get the average sampling rate from in ns
 * @return double - Average sampling rate (GSa/s)
 */
double fft_getAvgSamplingRate(scatter_t timeSerie);

/**
 * @brief Apply the Fast Fourier Transform on a cloud of points
 * 
 * @param in Cloud of points to apply the FFT on
 * @param out Cloud of points with the FFT result
 * @return int - 0 if success, -1 otherwise
 */
int fft_Iterative(scatter_t in, scatter_t out);

/**
 * @brief Transform a time serie into a frequency serie
 * 
 * @param timeSerie Time serie to transform into frequency serie
 * @param freqSerie Frequency serie.
 * @return int - 0 if success, -1 otherwise
 */
int fft_Compute(scatter_t timeSerie, scatter_t* freqSerie);

#endif // FFT_H