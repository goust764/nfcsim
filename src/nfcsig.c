/**
 * @file nfcsig.c
 * @author OUSSSET Gaël
 * @brief Library to generate NFC signals
 * @version 0.1
 * @date 2024-12-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "config.h"
#include "nfcsig.h"
#include "logging.h"
#include <stdlib.h>
#include <math.h>

int nfc_encodeData(
    char* data,
    size_t size,
    nfc_encoding_t encoding_type,
    char** encodedData,
    size_t* encodedSize
) {
    //========== Check arguments
    //----- Check input data
    if (!data || !size) {
        PRINT(ERR, "Input data cannot be NULL or empty");
        return -1;
    }
    //----- Check encoding type
    if (encoding_type != MOD_MILLER && 
        encoding_type != NRZ && 
        encoding_type != MANCHESTER) {
        PRINT(ERR, "Invalid encoding type");
        return -1;
    }

    //========== Allocate memory for the encoded data
    *encodedSize = 8 * 4 * size;
    *encodedData = (char*)malloc(*encodedSize);
    if (!(*encodedData)) {
        PRINT(ERR, "Failed to allocate memory for the encoded data");
        return -1;
    }

    //========== Encode data
    switch (encoding_type) {
        //----- Modified miller encoding
        case MOD_MILLER:
            PRINT(INFO, "Encoding data with modified miller encoding");
            // Encode the first bit
            if (data[0] & 0x01) {
                (*encodedData)[0] = 0x01;
                (*encodedData)[1] = 0x01;
                (*encodedData)[2] = 0x00;
                (*encodedData)[3] = 0x01;
            }
            // Arbitrarily encode the first bit as a 0 after 1
            else {
                (*encodedData)[0] = 0x01;
                (*encodedData)[1] = 0x01;
                (*encodedData)[2] = 0x01;
                (*encodedData)[3] = 0x01;
            }
            for (int i = 0; (size_t)i < size; i=i+1) {
                for (int j = 0; j < 8; j=j+1) {
                    if (i != 0 && j != 0) {      // Skip the first bit, already encoded above
                        // Bit at 1
                        if ((data[i] >> j) & 0x01) {
                            (*encodedData)[32*i+4*j  ] = 0x01;
                            (*encodedData)[32*i+4*j+1] = 0x01;
                            (*encodedData)[32*i+4*j+2] = 0x00;
                            (*encodedData)[32*i+4*j+3] = 0x01;
                        }
                        // Bit at 0 after a 1
                        else if (
                            // Check the last bit of the previous byte
                            (j == 0 && ((data[i] >> j) & 0x01) && !(data[i-1] >> 7            )) ||
                            // Check the previous bit of the current byte
                            (j != 0 && ((data[i] >> j) & 0x01) && !((data[i]  >> (j-1)) & 0x01))
                        ) {
                            (*encodedData)[32*i+4*j  ] = 0x01;
                            (*encodedData)[32*i+4*j+1] = 0x01;
                            (*encodedData)[32*i+4*j+2] = 0x01;
                            (*encodedData)[32*i+4*j+3] = 0x01;
                        }
                        // Bit at 0 after a 0
                        else {
                            (*encodedData)[32*i+4*j  ] = 0x01;
                            (*encodedData)[32*i+4*j+1] = 0x01;
                            (*encodedData)[32*i+4*j+2] = 0x01;
                            (*encodedData)[32*i+4*j+3] = 0x01;
                        }
                    }
                }
            }
        break;

        //----- Non-return-to-zero encoding
        case NRZ:
            PRINT(INFO, "Encoding data with NRZ encoding");
            for (int i = 0; (size_t)i < size; i=i+1) {
                for (int j = 0; j < 8; j=j+1) {
                    if ((data[i] >> j) & 0x01) {
                        (*encodedData)[32*i+4*j  ] = 0x01;
                        (*encodedData)[32*i+4*j+1] = 0x01;
                        (*encodedData)[32*i+4*j+2] = 0x01;
                        (*encodedData)[32*i+4*j+3] = 0x01;
                    }
                    else {
                        (*encodedData)[32*i+4*j  ] = 0x00;
                        (*encodedData)[32*i+4*j+1] = 0x00;
                        (*encodedData)[32*i+4*j+2] = 0x00;
                        (*encodedData)[32*i+4*j+3] = 0x00;
                    }
                }
            }
        break;

        //----- Manchester encoding
        case MANCHESTER:
            PRINT(INFO, "Encoding data with Manchester encoding");
            for (int i = 0; (size_t)i < size; i=i+1) {
                for (int j = 0; j < 8; j=j+1) {
                    if ((data[i] >> j) & 0x01) {
                        (*encodedData)[32*i+4*j  ] = 0x00;
                        (*encodedData)[32*i+4*j+1] = 0x00;
                        (*encodedData)[32*i+4*j+2] = 0x01;
                        (*encodedData)[32*i+4*j+3] = 0x01;
                    }
                    else {
                        (*encodedData)[32*i+4*j  ] = 0x01;
                        (*encodedData)[32*i+4*j+1] = 0x01;
                        (*encodedData)[32*i+4*j+2] = 0x00;
                        (*encodedData)[32*i+4*j+3] = 0x00;
                    }
                    // PRINT(
                    //     DBG,
                    //     "Encoded data: [%d]\t%d %d %d %d",
                    //     32*i+4*j,
                    //     (*encodedData)[32*i+4*j  ],
                    //     (*encodedData)[32*i+4*j+1],
                    //     (*encodedData)[32*i+4*j+2],
                    //     (*encodedData)[32*i+4*j+3]
                    // );
                }
            }
        break;

        //----- Default case
        default:
            PRINT(ERR, "Invalid encoding type");
            return -1;
        break;
    }

    return 0;
}

int nfc_modulateSubCarrier(
    char* encodeData,
    size_t encodedSize,
    nfc_subModulation_t subModulation,
    unsigned int bitRate,
    unsigned int subCarrierFreq,
    char** subModulatedData,
    size_t* subModulatedSize
) {
    //========== Variable declaration
    char isEven = 0;                             // Flag to know if the current
                                                 // value in the sub-carrier has to be 0 or 1

    //========== Check arguments
    //----- Check input data
    if (!encodeData || !encodedSize) {
        PRINT(ERR, "Encoded data cannot be NULL or empty");
        return -1;
    }
    //----- Check sub-carrier modulation type
    if (subModulation != NONE && 
        subModulation != OOK && 
        subModulation != BPSK) {
        PRINT(ERR, "Invalid sub-carrier modulation type");
        return -1;
    }
    //----- Check bit rate
    if (bitRate == 0) {
        PRINT(ERR, "Bit rate cannot be null");
        return -1;
    }
    //----- Check sub-carrier frequency
    if (subCarrierFreq == 0 && subModulation != NONE) {
        PRINT(ERR, "Sub-carrier frequency cannot be null with a sub-carrier modulation different from NONE");
        return -1;
    }
    if (subCarrierFreq % bitRate != 0)
        PRINT(WARN, "Sub-carrier frequency should be a multiple of the bit rate");

    //========== Allocate memory for the modulated data
    if (subModulation != NONE)
        // Divide by 2 because we should multiply by 2 (a 1 and a 0 by period)
        // but we already have 4 symbols per bit
        *subModulatedSize = encodedSize * (subCarrierFreq / bitRate) / 2;
    else
        *subModulatedSize = encodedSize;

    PRINT(DBG, "subModulatedSize: %ld", *subModulatedSize);

    *subModulatedData = (char*)malloc(*subModulatedSize);

    if (!(*subModulatedData)) {
        PRINT(ERR, "Failed to allocate memory for the modulated data");
        return -1;
    }

    //========== Modulate data
    switch (subModulation) {
        //----- No sub-carrier modulation
        case NONE:
            PRINT(INFO, "No sub-carrier modulation");
            for (int i = 0; (size_t)i < encodedSize; i=i+1) {
                (*subModulatedData)[i] = encodeData[i];
            }
        break;

        //----- On-off keying
        case OOK:
            PRINT(INFO, "Modulating data with on-off keying");
            for (unsigned int i = 0; (size_t)i < encodedSize; i=i+1) {
                // PRINT(
                //     DBG,
                //     "Encoded data: [%d]\t%d %d %d %d",
                //     i,
                //     encodeData[4*i],
                //     encodeData[4*i+1],
                //     encodeData[4*i+2],
                //     encodeData[4*i+3]
                // );
                if (encodeData[i]) {
                    for (unsigned int j = i*(subCarrierFreq/bitRate)/2; j < (i+1)*(subCarrierFreq/bitRate)/2; j=j+1) {
                        // PRINT(DBG, "OOK: [%d]", j);
                        (*subModulatedData)[j] = 0x01;
                    }
                }
                else {
                    isEven = 0;
                    for (unsigned int j = i*(subCarrierFreq/bitRate)/2; j < (i+1)*(subCarrierFreq/bitRate)/2; j=j+1) {
                        // PRINT(DBG, "OOK: [%d]", j);
                        (*subModulatedData)[j] = isEven;
                        isEven = !isEven;
                    }
                }
            }
        break;

        //----- Binary phase shift keying
        case BPSK:
            PRINT(INFO, "Modulating data with binary phase shift keying");
            for (unsigned int i = 0; (size_t)i < encodedSize; i=i+1) {
                if (encodeData[i]) {
                    isEven = 1;
                    for (unsigned int j = i*(subCarrierFreq/bitRate)/2; j < (i+1)*(subCarrierFreq/bitRate)/2; j=j+1) {
                        (*subModulatedData)[j] = isEven;
                        isEven = !isEven;
                    }
                }
                else {
                    isEven = 0;
                    for (unsigned int j = i*(subCarrierFreq/bitRate)/2; j < (i+1)*(subCarrierFreq/bitRate)/2; j=j+1) {
                        (*subModulatedData)[j] = isEven;
                        isEven = !isEven;
                    }
                }
            }
        break;
        
        //----- Default case
        default:
            PRINT(ERR, "Invalid sub-carrier modulation type");
            return -1;
        break;
    }

    return 0;
}

int nfc_createEnvelope(
    char* subModulatedData,
    size_t subModulatedSize,
    unsigned char modulationIndex,
    unsigned int simDuration,
    unsigned int numberOfPoints,
    scatter_t** envelope
) {
    //========== Check arguments
    if (!subModulatedData || !subModulatedSize) {
        PRINT(ERR, "Sub-modulated data cannot be NULL or empty");
        return -1;
    }
    if (modulationIndex > 100) {
        PRINT(ERR, "Modulation index cannot be greater than 100");
        return -1;
    }
    if (simDuration == 0) {
        PRINT(ERR, "Simulation duration cannot be null");
        return -1;
    }

    //========== Allocate memory for the envelope
    if (scatter_create(envelope, numberOfPoints)) {
        PRINT(ERR, "Failed to allocate memory for the envelope");
        return -1;
    }

    //========== Generate envelope
    for (unsigned int i = 0; i < numberOfPoints; i=i+1) {
        (*envelope)->points[i].x = i * simDuration / numberOfPoints;
        (*envelope)->points[i].y = modulationIndex / 100 * subModulatedData[subModulatedSize * i / numberOfPoints];
    }

    return 0;
}

int nfc_modulate(
    scatter_t enveloppe,
    unsigned int carrierFreq,
    scatter_t** modulatedSignal
) {
    //========== Check arguments
    if (!enveloppe.points || !enveloppe.size) {
        PRINT(ERR, "Enveloppe cannot be NULL or empty");
        return -1;
    }

    //========== Allocate memory for the modulated signal
    if (scatter_create(modulatedSignal, enveloppe.size)) {
        PRINT(ERR, "Failed to allocate memory for the modulated signal");
        return -1;
    }

    //========== Modulate signal
    for (int i = 0; (size_t)i < (*modulatedSignal)->size; i=i+1) {
        (*modulatedSignal)->points[i].x = enveloppe.points[i].x;
        (*modulatedSignal)->points[i].y = enveloppe.points[i].y * sin(2 * M_PI * carrierFreq * enveloppe.points[i].x / 1000);
    }

    return 0;
}

int nfc_addNoise(
    scatter_t signal,
    double noiseLevel,
    scatter_t** noisySignal
) {
    //========== Check arguments
    if (!signal.points || !signal.size) {
        PRINT(ERR, "Signal cannot be NULL or empty");
        return -1;
    }
    if (noiseLevel < 0 || noiseLevel > 1) {
        PRINT(ERR, "Noise level should be between 0 and 1");
        return -1;
    }

    //========== Allocate memory for the noisy signal
    if (scatter_create(noisySignal, signal.size)) {
        PRINT(ERR, "Failed to allocate memory for the noisy signal");
        return -1;
    }

    //========== Add noise
    for (unsigned int i = 0; i < (*noisySignal)->size; i=i+1) {
        (*noisySignal)->points[i].x = signal.points[i].x;
        (*noisySignal)->points[i].y = signal.points[i].y + noiseLevel * (double)rand() / (double)RAND_MAX;
    }

    return 0;
}

//========== General function
int nfc_createSignal(
    char* data, size_t size,
    unsigned int bitRate,
    nfc_encoding_t encoding_type,
    nfc_subModulation_t subModulation,
    unsigned int subCarrierFreq,
    unsigned int carrierFreq,
    unsigned char modulationIndex,
    double noiseLevel,
    unsigned int simDuration,
    unsigned int numberOfPoints,
    scatter_t** signal
) {
    //========== Variable declaration
    char*      encodedData      = NULL;          // Encoded data
    size_t     encodedSize      = 0;             // Size of the encoded data
    char*      subModulatedData = NULL;          // Sub-carrier modulated data
    size_t     subModulatedSize = 0;             // Size of the sub-carrier modulated data
    scatter_t* envelope         = NULL;          // Envelope of the signal
    scatter_t* modulatedSignal  = NULL;          // Modulated signal

    //========== Check arguments
    /* Inch no need, already done in sub-functions */

    PRINT(DBG, "===== INPUT DATA =====");
    for (int i = 0; (size_t)i < size; i=i+1) {
        PRINT(DBG, "Data: [%d]\t0x%02X", i, data[i]);
    }

    //========== Encode data
    PRINT(INFO, "Encoding data");
    if (nfc_encodeData(
        data, size,
        encoding_type,
        &encodedData, &encodedSize
    )) {
        PRINT(ERR, "Failed to encode data");
        return -1;
    }

    PRINT(DBG, "===== ENCODED DATA =====");
    for (int i = 0; (size_t)i < encodedSize; i=i+1) {
        PRINT(DBG, "Encoded data: [%d]\t%d", i, encodedData[i]);
    }

    //========== Sub-carrier modulation
    PRINT(INFO, "Modulating data with sub-carrier");
    if (nfc_modulateSubCarrier(
        encodedData, encodedSize,
        subModulation,
        bitRate, subCarrierFreq,
        &subModulatedData, &subModulatedSize
    )) {
        PRINT(ERR, "Failed to modulate data with sub-carrier");
        free(encodedData);
        return -1;
    }

    //========== Generate envelope
    PRINT(INFO, "Generating envelope");
    if (nfc_createEnvelope(
        subModulatedData, subModulatedSize,
        modulationIndex,
        simDuration, numberOfPoints,
        &envelope
    )) {
        PRINT(ERR, "Failed to generate envelope");
        free(encodedData);
        free(subModulatedData);
        return -1;
    }

    //========== Modulate signal
    PRINT(INFO, "Modulating signal");
    if (nfc_modulate(
        *envelope, 
        carrierFreq,
        &modulatedSignal
    )) {
        PRINT(ERR, "Failed to modulate signal");
        free(encodedData);
        free(subModulatedData);
        scatter_destroy(envelope);
        return -1;
    }

    //========== Add noise
    PRINT(INFO, "Adding noise to the signal");
    if (nfc_addNoise(
        *modulatedSignal,
        noiseLevel,
        signal
    )) {
        PRINT(ERR, "Failed to add noise to the signal");
        free(encodedData);
        free(subModulatedData);
        scatter_destroy(envelope);
        scatter_destroy(modulatedSignal);
        return -1;
    }

    //========== Free memory
    free(encodedData);
    free(subModulatedData);
    scatter_destroy(envelope);
    scatter_destroy(modulatedSignal);

    PRINT(SUCC, "Signal successfully generated");
    return 0;
}