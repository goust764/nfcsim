/**
 * @file nfcsim.c
 * @author OUSSET Gaël
 * @brief Simulation of an NFC communication
 * @version 0.1
 * @date 2024-12-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "nfcsim.h"
#include "config.h"
#include "logging.h"
#include "nfcsig.h"
#include <string.h>

/**
 * @brief Main function
 * 
 * @param argc Number of arguments
 * @param argv Arguments
 * @return int - 0 if success
 */
int main(/*int argc, char *argv[]*/) {
    //========== Variable declaration
    char* data = "Hello, World!";
    scatter_t* signal = NULL;

    //========== Generate NFC signal
    if (nfc_createSignal(
        data,
        strlen(data),
        106000,
        MANCHESTER,
        OOK,
        SUB_CARRIER_FREQ,
        CARRIER_FREQ,
        10,
        0.1,
        1000,
        1000,
        &signal
    )) {
        PRINT(ERR, "Failed to generate NFC signal");
        return -1;
    }

    //========== Print the signal
    scatter_print(*signal, ',', NORM);

    //========== Free memory
    scatter_destroy(signal);

    return 0;
}
