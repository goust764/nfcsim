/*
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include "scatter.h"

// Fonction pour extraire les valeurs y du nuage de points
void extract_signal(scatter_t* scatter, double* signal) {
    for (size_t i = 0; i < scatter->size; i++) {
        signal[i] = scatter->points[i].y;
    }
}

// Fonction pour calculer la FFT itérative
void fft_iterative(complex double *X, int n) {
    // Bit-reversal permutation
    int j = 0;
    for (int i = 1; i < n; i++) {
        int bit = n >> 1;
        while (j >= bit) {
            j -= bit;
            bit >>= 1;
        }
        j += bit;
        if (i < j) {
            complex double temp = X[i];
            X[i] = X[j];
            X[j] = temp;
        }
    }

    // FFT
    for (int len = 2; len <= n; len <<= 1) {
        double angle = -2.0 * M_PI / len;
        complex double wlen = cexp(I * angle);
        for (int i = 0; i < n; i += len) {
            complex double w = 1.0;
            for (int j = 0; j < len / 2; j++) {
                complex double u = X[i + j];
                complex double t = w * X[i + j + len / 2];
                X[i + j] = u + t;
                X[i + j + len / 2] = u - t;
                w *= wlen;
            }
        }
    }
}

// Fonction pour calculer la FFT et extraire les amplitudes
void compute_fft(scatter_t* scatter, double* amplitudes, double* frequencies, double sampling_rate) {
    size_t n = scatter->size;
    complex double* X = (complex double*)malloc(n * sizeof(complex double));

    // Extraire le signal du nuage de points
    extract_signal(scatter, (double*)X);

    // Appliquer la FFT itérative
    fft_iterative(X, n);

    // Calculer les amplitudes et les fréquences
    for (size_t i = 0; i < n / 2 + 1; i++) {
        amplitudes[i] = cabs(X[i]);
        frequencies[i] = i * sampling_rate / n;
    }

    // Libérer la mémoire
    free(X);
}

int main() {
    // Exemple de nuage de points
    scatter_t scatter;
    scatter.size = 8;
    scatter.points = (point_t*)malloc(scatter.size * sizeof(point_t));
    for (size_t i = 0; i < scatter.size; i++) {
        scatter.points[i].x = i;
        scatter.points[i].y = sin(2 * M_PI * i / scatter.size); // Exemple de signal sinusoïdal
    }

    // Fréquence d'échantillonnage (par exemple, 1 kHz)
    double sampling_rate = 1000.0;

    // Tableaux pour stocker les amplitudes et les fréquences
    double* amplitudes = (double*)malloc((scatter.size / 2 + 1) * sizeof(double));
    double* frequencies = (double*)malloc((scatter.size / 2 + 1) * sizeof(double));

    // Calculer la FFT et extraire les amplitudes et les fréquences
    compute_fft(&scatter, amplitudes, frequencies, sampling_rate);

    // Afficher les amplitudes et les fréquences
    printf("Fréquences et amplitudes:\n");
    for (size_t i = 0; i < scatter.size / 2 + 1; i++) {
        printf("Fréquence %f Hz: Amplitude %f\n", frequencies[i], amplitudes[i]);
    }

    // Libérer la mémoire
    free(scatter.points);
    free(amplitudes);
    free(frequencies);

    return 0;
}
*/