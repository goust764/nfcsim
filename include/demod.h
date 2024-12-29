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

#endif // DEMOD_H