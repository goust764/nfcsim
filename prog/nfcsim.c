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
#include "FFT.h"
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
    scatter_t* freqSerie = NULL;

    //========== Generate NFC signal
    if (nfc_createSignal(
        data,                                    // Data to encode
        strlen(data),                            // Size of the data
        106000,                                  // Bit rate of the input data
        MANCHESTER,                              // Type of encoding to apply to the data
        OOK,                                     // Type of sub-carrier modulation
        SUB_CARRIER_FREQ,                        // Frequency of the sub-carrier (Hz)
        CARRIER_FREQ,                            // Frequency of the carrier (Hz)
        50,                                      // Index of the modulation of the envelope (%)
        0,                                       // Signal to noise ratio
        10000,                                    // Duration of the simulation (ns)
        1024,                                    // Number of points to generate
        &signal                                  // Generated signal
    )) {
        PRINT(ERR, "Failed to generate NFC signal");
        return -1;
    }

    //========== Apply FFT
    if (fft_Compute(*signal, &freqSerie)) {
        PRINT(ERR, "Failed to apply the FFT");
        scatter_destroy(signal);
        return -1;
    }

    //========== Print the signal
    // scatter_print(*signal, ',', NORM);
    scatter_print(*freqSerie, ',', NORM);

    //========== Free memory
    scatter_destroy(signal);
    scatter_destroy(freqSerie);
    
    return 0;
}
