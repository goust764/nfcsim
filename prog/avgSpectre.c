/**
 * @file nfcsim.c
 * @author OUSSET Gaël
 * @brief Simulate for each standard NFC signal with all bytes, FFT them and
 *        average them
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
    scatter_t signal;
    scatter_t spectres[4][256];
    scatter_t avgSpectres[4];

    for (size_t i = 0; i < 4; i=i+1) {
        scatter_create(&avgSpectres[i], NB_POINTS);
        for (size_t j = 0; j < NB_POINTS; j=j+1) {
            scatter_setX(avgSpectres[i], j, 0);
            scatter_setY(avgSpectres[i], j, 0);
        }
    }

    for (size_t i = 0; i < 256; i=i+1) {
        PRINT(NORM, "Processing byte %d\n", i);
        char byte[] = {(char)i};
        nfc_standardSignal(
            byte,
            1,
            NFC_A,
            PCD,
            BIT_RATE,
            0,
            NB_POINTS,
            &signal
        );
        fft_Compute(signal, &spectres[0][i]);
        scatter_destroy(signal);

        nfc_standardSignal(
            byte,
            1,
            NFC_A,
            PICC,
            BIT_RATE,
            0,
            NB_POINTS,
            &signal
        );
        fft_Compute(signal, &spectres[1][i]);
        scatter_destroy(signal);

        nfc_standardSignal(
            byte,
            1,
            NFC_B,
            PCD,
            BIT_RATE,
            0,
            NB_POINTS,
            &signal
        );
        fft_Compute(signal, &spectres[2][i]);
        scatter_destroy(signal);

        nfc_standardSignal(
            byte,
            1,
            NFC_B,
            PICC,
            BIT_RATE,
            0,
            NB_POINTS,
            &signal
        );
        fft_Compute(signal, &spectres[3][i]);
        scatter_destroy(signal);

        for (size_t j = 0; j < 4; j=j+1) {
            for (size_t k = 0; k < NB_POINTS; k=k+1) {
                scatter_setY(
                    avgSpectres[j],
                    k,
                    scatter_getY(avgSpectres[j], k) + scatter_getY(spectres[j][i], k)
                );
            }
        }
    }

    scatter_setName(avgSpectres[0], "Frequency (Hz)", "NFC-A PCD");
    scatter_setName(avgSpectres[1], "Frequency (Hz)", "NFC-A PICC");
    scatter_setName(avgSpectres[2], "Frequency (Hz)", "NFC-B PCD");
    scatter_setName(avgSpectres[3], "Frequency (Hz)", "NFC-B PICC");

    for (size_t j = 0; j < 4; j=j+1) {
        for (size_t k = 0; k < NB_POINTS; k=k+1) {
            scatter_setX(
                avgSpectres[j],
                k,
                scatter_getX(spectres[j][0], k)
            );
            scatter_setY(
                avgSpectres[j],
                k,
                scatter_getY(avgSpectres[j], k) / 256
            );
        }
    }

    writeCSV(avgSpectres, 4, "..\\res\\average spectre.csv");

    for (size_t i = 0; i < 4; i=i+1)
        scatter_destroy(avgSpectres[i]);

    return 0;
}