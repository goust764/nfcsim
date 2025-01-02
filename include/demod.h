/**
 * @file demod.h
 * @author OUSSET Gaël
 * @brief Header file for demod.c
 * @version 0.1
 * @date 2024-12-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef DEMOD_H
#define DEMOD_H

#include "config.h"
#include "scatter.h"

/**
 * @brief Simulate a Level Crossing ADC
 * 
 * @param signal Input signal
 * @param levels Levels to compare the signal to
 * @param nbLevels Number of levels
 * @param skip Keep only one point every skip
 * @param output Sampled signal
 * @return int - 0 if success, -1 otherwise
 */
int LCADC(scatter_t signal, double* levels, unsigned int nbLevels, unsigned int skip, scatter_t* output);

/**
 * @brief Create a sin period lookup table
 * 
 * @param LUT Pointeur to the LUT array
 * @param LUTSize Size of the LUT
 * @return int - 0 if success, -1 otherwise
 */
int createLUT(char** LUT, size_t LUTSize);

/**
 * @brief Return the sin value of a given time, frequency and phase from the LUT
 * 
 * @param LUT Lookup table to read the sin value from
 * @param LUTSize Size of the LUT
 * @param time Time to get the sin value from
 * @param freq Frequency of the sin wave
 * @param phi Phase of the sin wave
 * @return char - Sin value
 */
char LUTSin(char* LUT, size_t LUTSize, int time, int freq);

#endif // DEMOD_H