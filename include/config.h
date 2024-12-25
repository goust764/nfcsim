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

//----- Simulation parameters
/**
 * Use the M_PI constant from the math library
 */
#define _USE_MATH_DEFINES

/**
 * Duration of the simulation in ms
 */
#define SIM_DURATION 1000

/**
 * Number of points during the simulation
 */
#define NB_POINTS 1000

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