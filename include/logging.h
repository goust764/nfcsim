/**
 * @file logging.h
 * @author OUSSET Gaël
 * @brief Unified logging system accross the project
 * @version 0.1
 * @date 2024-12-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

//========== Rescourses related to colors and styles
#define ANSI_ESC                        "\x1b["
#define STYLE_REGULAR                   ANSI_ESC"0;3"
#define STYLE_BOLD                      ANSI_ESC"1;3"
#define STYLE_UNDERLINE                 ANSI_ESC"4;3"
#define STYLE_BACKGROUND                ANSI_ESC"4"
#define STYLE_BLINK                     ANSI_ESC"5;3"
#define STYLE_HIGH_INTENSITY_BACKGROUND ANSI_ESC"0;10"
#define STYLE_HIGH_INTENSITY_TEXT       ANSI_ESC"0;9"
#define STYLE_HIGH_INTENSITY_BOLD_TEXT  ANSI_ESC"1;9"

#define COLOR_BLK      0
#define COLOR_RED      1
#define COLOR_GREEN    2
#define COLOR_YELLOW   3
#define COLOR_BLUE     4
#define COLOR_MAGENTA  5
#define COLOR_CYAN     6
#define COLOR_WHITE    7

#define STYLE( file, color, style ) do {		           \
        char fmt[ 128 ];					               \
        snprintf( fmt, 127, "%s%dm", style, color );	   \
        fprintf( (file), "%s", fmt );			           \
    } while ( 0 )

#define STYLE_RESET( file ) fprintf( file, ANSI_ESC"0m" )

/**
 * Verbosity level of the program
 * 0: No message
 * 1: Error messages
 * 2: Error and warning messages
 * 3: Error, warning and info messages
 *  
 */
#ifndef VERBOSITY
    #define VERBOSITY 3
#endif

#ifdef HEADLESS
    #define PRINT(level, format, ...) ((void)0)
#else
    #define PRINT(type, format, ...) printInfo(type, format, ##__VA_ARGS__)
#endif

//========== Rescourses related to the print type
typedef enum {
    ERR,
    WARN,
    INFO
} print_type_t;

/**
 * @brief Print a message with a specific type
 * 
 * @param print_type Specify the type of the message, can be PRINT_ERR, PRINT_WARN, PRINT_INFO or PRINT_NEUTRAL
 * @param format The format of the message
 * @param ... The arguments of the message
 */
void printInfo(print_type_t print_type, const char* format, ...);

#endif // LOGGING_H
