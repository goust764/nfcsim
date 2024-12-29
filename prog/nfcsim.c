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
    char data[] = {18, 13, 0};
    // char* data = "Hello, World!";
    scatter_t signals[8];                         // NFC signal and its FFT
    double levels[] = {-0.9, -0.6, 0.6, 0.9};     // Levels for the LCADC

    //========== Generate NFC signals
    if (!nfc_standardSignal(
        data,
        1,
        NFC_A,
        PCD,
        BIT_RATE,
        0,
        NB_POINTS,
        &signals[0]
    )) {
        fft_Compute(signals[0], &signals[1]);
        scatter_setName(signals[0], "Time (ns)", "NFC-A PCD");
        scatter_setName(signals[1], "Frequency (Hz)", "FFT NFC-A PCD");
        LCADC(signals[0], levels, 4, 2, &signals[2]);
    }

    // if (!nfc_standardSignal(
    //     data+1,
    //     1,
    //     NFC_A,
    //     PICC,
    //     BIT_RATE,
    //     0,
    //     NB_POINTS,
    //     &signals[2]
    // )) {
    //     fft_Compute(signals[2], &signals[3]);
    //     scatter_setName(signals[2], "Time (ns)", "NFC-A PICC");
    //     scatter_setName(signals[3], "Frequency (Hz)", "FFT NFC-A PICC");
    // }
    // if (!nfc_standardSignal(
    //     data,
    //     1,
    //     NFC_B,
    //     PCD,
    //     BIT_RATE,
    //     0,
    //     NB_POINTS,
    //     &signals[4]
    // )) {
    //     fft_Compute(signals[4], &signals[5]);
    //     scatter_setName(signals[4], "Time (ns)", "NFC-B PCD");
    //     scatter_setName(signals[5], "Frequency (Hz)", "FFT NFC-B PCD");
    // }
    // if (!nfc_standardSignal(
    //     data+1,
    //     1,
    //     NFC_B,
    //     PICC,
    //     BIT_RATE,
    //     0,
    //     NB_POINTS,
    //     &signals[6]
    // )) {
    //     fft_Compute(signals[6], &signals[7]);
    //     scatter_setName(signals[6], "Time (ns)", "NFC-B PICC");
    //     scatter_setName(signals[7], "Frequency (Hz)", "FFT NFC-B PICC");
    // }

    //========== Export the signals
    if (writeCSV(signals, 3, "..\\res\\signals.csv")) {
    // if (writeCSV(signals, 8, "..\\res\\signals.csv")) {
        PRINT(ERR, "Failed to export the signal");
        scatter_destroy(signals[0]);
        scatter_destroy(signals[1]);
        scatter_destroy(signals[2]);
        // scatter_destroy(signals[3]);
        // scatter_destroy(signals[4]);
        // scatter_destroy(signals[5]);
        // scatter_destroy(signals[6]);
        // scatter_destroy(signals[7]);
        return -1;
    }

    //========== Free memory
    scatter_destroy(signals[0]);
    scatter_destroy(signals[1]);
    scatter_destroy(signals[2]);
    // scatter_destroy(signals[3]);
    // scatter_destroy(signals[4]);
    // scatter_destroy(signals[5]);
    // scatter_destroy(signals[6]);
    // scatter_destroy(signals[7]);

    return 0;
}
