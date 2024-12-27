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
#include "CSV.h"
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
    scatter_t signals[2] = {NULL, NULL};         // NFC signal and its FFT

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
        0.0,                                     // Signal to noise ratio
        981132,                                  // Duration of the simulation (ns)
        262144,                                  // Number of points to generate
        &signals[0]                              // Generated signal
    )) {
        PRINT(ERR, "Failed to generate NFC signal");
        return -1;
    }
    
    //========== Apply FFT
    if (fft_Compute(signals[0], &signals[1])) {
        PRINT(ERR, "Failed to apply the FFT");
        scatter_destroy(signals[0]);
        return -1;
    }

    //========== Print the signal
    // scatter_print(*signals[0], ',', NORM);
    // scatter_print(*signals[1], ',', NORM);

    //========== Export the signals
    scatter_setName(signals[0], "Time (ns)", "Amplitude");
    scatter_setName(signals[1], "Frequency (Hz)", "Amplitude");
    if (writeCSV(signals, 2, "..\\res\\signals.csv")) {
        PRINT(ERR, "Failed to export the signal");
        scatter_destroy(signals[0]);
        scatter_destroy(signals[1]);
        return -1;
    }

    //========== Free memory
    scatter_destroy(signals[0]);
    scatter_destroy(signals[1]);
    
    return 0;
}
