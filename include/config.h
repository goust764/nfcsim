/**
 * @file config.h
 * @author OUSSSET Gaël
 * @brief Program global configuration file
 * @version 0.1
 * @date 2024-12-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CONFIG_H
#define CONFIG_H

//----- Program parameters
/**
 * If defined, all printf will be disabled
 * Required if compiled with the -mwindows flag
 */
// #define HEADLESS

/**
 * Verbosity level of the program
 * 0: Normal messages
 * 1: Normal and  error messages
 * 2: Normal, error and warning messages
 * 3: Normal, error, warning and success messages
 * 4: Normal, error, warning, success and info messages
 * 5: Normal, error, warning, success, info and debug messages
 *  
 */
#define VERBOSITY 5

/**
 * Remove all assert calls
 */
// #define NO_ASSERT

//----- Simulation parameters
/**
 * Use the M_PI constant from the math library
 */
#define _USE_MATH_DEFINES

/**
 * Bit rate of the NFC communication
 */
#define BIT_RATE 106000
// #define BIT_RATE 424000

/**
 * Number of points during the simulation
 */
#define NB_POINTS 16384
// #define NB_POINTS 32768

/**
 * Carrier frequency of the NFC communication
 */
#define CARRIER_FREQ 13.56e6

/**
 * Sub-carrier frequency of the NFC communication
 * Note : We do not support yet NFC-V
 */
#define SUB_CARRIER_FREQ 848e3

#endif // CONFIG_H