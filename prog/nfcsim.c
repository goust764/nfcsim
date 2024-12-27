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
    char data[] = {77, 22, 0};
    // char* data = "Hello, World!";
    scatter_t signals[8];                        // NFC signal and its FFT

    //========== Generate NFC signals
    if (!nfc_standardSignal(
        data,
        strlen(data),
        NFC_A,
        PCD,
        BIT_RATE,
        0,
        NB_POINTS,
        &signals[0]
    )) {
        fft_Compute(signals[0], &signals[1]);
    }
    if (!nfc_standardSignal(
        data,
        strlen(data),
        NFC_A,
        PICC,
        BIT_RATE,
        0,
        NB_POINTS,
        &signals[2]
    )) {
        fft_Compute(signals[2], &signals[3]);
    }
    if (!nfc_standardSignal(
        data,
        strlen(data),
        NFC_B,
        PCD,
        BIT_RATE,
        0,
        NB_POINTS,
        &signals[4]
    )) {
        fft_Compute(signals[4], &signals[5]);
    }
    if (!nfc_standardSignal(
        data,
        strlen(data),
        NFC_B,
        PICC,
        BIT_RATE,
        0,
        NB_POINTS,
        &signals[6]
    )) {
        fft_Compute(signals[6], &signals[7]);
    }

    //========== Export the signals
    scatter_setName(signals[0], "Time (ns)", "Amplitude");
    scatter_setName(signals[1], "Frequency (Hz)", "Amplitude");
    scatter_setName(signals[2], "Time (ns)", "Amplitude");
    scatter_setName(signals[3], "Frequency (Hz)", "Amplitude");
    scatter_setName(signals[4], "Time (ns)", "Amplitude");
    scatter_setName(signals[5], "Frequency (Hz)", "Amplitude");
    scatter_setName(signals[6], "Time (ns)", "Amplitude");
    scatter_setName(signals[7], "Frequency (Hz)", "Amplitude");
    if (writeCSV(signals, 8, "..\\res\\signals.csv")) {
        PRINT(ERR, "Failed to export the signal");
        scatter_destroy(signals[0]);
        scatter_destroy(signals[1]);
        scatter_destroy(signals[2]);
        scatter_destroy(signals[3]);
        scatter_destroy(signals[4]);
        scatter_destroy(signals[5]);
        scatter_destroy(signals[6]);
        scatter_destroy(signals[7]);
        return -1;
    }

    //========== Free memory
    scatter_destroy(signals[0]);
    scatter_destroy(signals[1]);
    scatter_destroy(signals[2]);
    scatter_destroy(signals[3]);
    scatter_destroy(signals[4]);
    scatter_destroy(signals[5]);
    scatter_destroy(signals[6]);
    scatter_destroy(signals[7]);

    return 0;
}
