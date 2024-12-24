/**
 * @file logging.c
 * @author OUSSET Gaël
 * @brief Unified logging system accross the project
 * @version 0.1
 * @date 2024-12-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "logging.h"

#include <stdarg.h>

void printInfo(print_type_t print_type, const char* format, ...) {
    #if VERBOSITY > 0
        va_list args;                                // Variable argument list
        
        //----- Print the type of the message
        printf("\n");
        switch (print_type) {
            #if VERBOSITY >= 1
                case ERR:
                    STYLE(stderr, COLOR_RED, STYLE_REGULAR);
                    fprintf(stderr, "[ERROR]\t");
                break;
            #endif
            #if VERBOSITY >= 2
                case WARN:
                    STYLE(stderr, COLOR_YELLOW, STYLE_REGULAR);
                    fprintf(stderr, "[WARNING]\t");
                break;
            #endif
            #if VERBOSITY >= 3
                case INFO:
                    STYLE(stderr, COLOR_BLUE, STYLE_HIGH_INTENSITY_TEXT);
                    fprintf(stderr, "[INFO]\t");
                break;
            #endif
            default:
                // STYLE(stderr, COLOR_WHITE, STYLE_REGULAR);
                // fprintf(stderr, "[MSG]\t");
            break;
        }
        
        //----- Print the message
        if (VERBOSITY > print_type) {
            STYLE(stderr, COLOR_WHITE, STYLE_BOLD);
            va_start(args, format);
            vfprintf(stderr, format, args);
            va_end(args);
            printf("\n");
        }

        STYLE_RESET(stderr);

    #endif // VERBOSITY > 0
}