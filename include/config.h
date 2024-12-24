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
 * 0: No message
 * 1: Error messages
 * 2: Error and warning messages
 * 3: Error, warning and info messages
 *  
 */
#define VERBOSITY 3

#endif // CONFIG_H