/**
 * @file assert.h
 * @author OUSSET Gaël
 * @brief Local implementation of the assert macro
 * @version 0.1
 * @date 2024-12-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef ASSERT_H
#define ASSERT_H

#include "config.h"
#include "logging.h"

#ifndef NO_ASSERT
    /**
     * @brief Macro to check a condition and return an error code if it is false
     * @param condition Condition to check, if false, the error message is printed and the function returns the error code
     * @param errorMsg Error message to print, if the condition is false
     * @param retCode Error code to return, if the condition is false
     * 
     */
    #define assert(condition, errorMsg, retCode, ...) \
        if (!(condition)) { \
            PRINT(ERR, errorMsg, ##__VA_ARGS__); \
            PRINT(DBG, "Assertion failed at %s:%d: %s", __FILE__, __LINE__, #condition); \
            return retCode; \
        }
#else
    #warning "Assertions are disabled"
    /**
     * @brief Do nothing, assertions are disabled
     * 
     */
    #define assert(condition, errorMsg, retCode) \
        (void)(condition);

#endif

#endif // ASSERT_H