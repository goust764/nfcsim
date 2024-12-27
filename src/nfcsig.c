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
    nfc_sigParam_t* sigParam,
    char** encodedData,
    size_t* encodedSize
) {
    //========== Variables declaration
    char* data                  = sigParam->data;
    size_t size                 = sigParam->dataSize;
    nfc_encoding_t encodingType = sigParam->encodingType;

    //========== Check arguments
    //----- Check input data
    if (!data || !size) {
        PRINT(ERR, "Input data cannot be NULL or empty");
        return -1;
    }
    //----- Check encoding type
    if (encodingType != MOD_MILLER && 
        encodingType != NRZ && 
        encodingType != MANCHESTER) {
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
    switch (encodingType) {
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
    nfc_sigParam_t* sigParam,
    char** subModulatedData,
    size_t* subModulatedSize
) {
    //========== Variables declaration
    nfc_subModulation_t subModulation = sigParam->subModulation;
    unsigned int bitRate              = sigParam->bitRate;
    unsigned int subCarrierFreq       = sigParam->subCarrierFreq;
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

    // PRINT(DBG, "subModulatedSize: %ld", *subModulatedSize);

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
    nfc_sigParam_t* sigParam,
    scatter_t* envelope
) {
    //========== Variables declaration
    nfc_subModulation_t subModulation = sigParam->subModulation;
    unsigned int bitRate              = sigParam->bitRate;
    unsigned int subCarrierFreq       = sigParam->subCarrierFreq;
    unsigned char modulationIndex     = sigParam->modulationIndex;
    unsigned int simDuration          = sigParam->simDuration;
    unsigned int numberOfPoints       = sigParam->numberOfPoints;
    double modulationDepth;                      // Depth of the modulation, 
                                                 // we suppose that the 
                                                 // amplitude of the signal is 1
    unsigned int symboleDuration;                // Duration of a symbol in ns 
                                                 // from the subModulatedData
    unsigned long long time;                     // Time of the current point in ns

    //========== Check arguments
    if (!subModulatedData || !subModulatedSize) {
        PRINT(ERR, "Sub-modulated data cannot be NULL or empty");
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
    //----- Check modulation index
    if (modulationIndex > 100) {
        PRINT(ERR, "Modulation index cannot be greater than 100");
        return -1;
    }
    //----- Check simulation duration
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
    //----- Calculate the duration of a symbol
    // If there is no sub-carrier modulation, the duration of a symbol is the bit rate
    if (subModulation == NONE)
        symboleDuration = (unsigned int)1e9 / bitRate;
    // Otherwise, the duration of a symbol is the sub-carrier frequency
    else
        symboleDuration = (unsigned int)1e9 / subCarrierFreq;
    PRINT(INFO, "Duration of a symbol: %d ns", symboleDuration);

    //----- Calculate the modulation depth
    modulationDepth = (double)(100 - modulationIndex) / (double)(modulationIndex + 100);
    PRINT(INFO, "Modulation depth: %f", modulationDepth);

    //----- Generate the envelope
    for (unsigned int i = 0; i < numberOfPoints; i=i+1) {
        time =  (unsigned long long)i * 
                (unsigned long long)simDuration / 
                numberOfPoints;
        (*envelope)->points[i].x =  (int)time;
        (*envelope)->points[i].y =  subModulatedData[time/symboleDuration] ?
                                    1 :
                                    modulationDepth;
    }

    return 0;
}

int nfc_modulate(
    scatter_t enveloppe,
    nfc_sigParam_t* sigParam,
    scatter_t* modulatedSignal
) {
    //========== Variables declaration
    unsigned int carrierFreq = sigParam->carrierFreq;

    //========== Check arguments
    if (!enveloppe->points || !enveloppe->size) {
        PRINT(ERR, "Enveloppe cannot be NULL or empty");
        return -1;
    }

    //========== Allocate memory for the modulated signal
    if (scatter_create(modulatedSignal, enveloppe->size)) {
        PRINT(ERR, "Failed to allocate memory for the modulated signal");
        return -1;
    }

    //========== Modulate signal
    for (int i = 0; (size_t)i < (*modulatedSignal)->size; i=i+1) {
        (*modulatedSignal)->points[i].x =   enveloppe->points[i].x;
        (*modulatedSignal)->points[i].y =   enveloppe->points[i].y * 
                                            sin(
                                                (double)2 *
                                                (double)M_PI *
                                                (double)carrierFreq *
                                                (double)(enveloppe->points[i].x) / (double)1e9
                                            );
    }

    return 0;
}

int nfc_addNoise(
    scatter_t signal,
    nfc_sigParam_t* sigParam,
    scatter_t* noisySignal
) {
    //========== Variables declaration
    double noiseLevel = sigParam->noiseLevel;

    //========== Check arguments
    if (!signal->points || !signal->size) {
        PRINT(ERR, "Signal cannot be NULL or empty");
        return -1;
    }
    if (noiseLevel < 0 || noiseLevel > 1) {
        PRINT(ERR, "Noise level should be between 0 and 1");
        return -1;
    }

    //========== Allocate memory for the noisy signal
    if (scatter_create(noisySignal, signal->size)) {
        PRINT(ERR, "Failed to allocate memory for the noisy signal");
        return -1;
    }

    //========== Add noise
    for (unsigned int i = 0; i < (*noisySignal)->size; i=i+1) {
        (*noisySignal)->points[i].x = signal->points[i].x;
        (*noisySignal)->points[i].y = signal->points[i].y + noiseLevel * ((double)rand() / (double)RAND_MAX - 0.5);
    }

    return 0;
}

int nfc_createSignal(
    char* data, size_t size,
    unsigned int bitRate,
    nfc_encoding_t encodingType,
    nfc_subModulation_t subModulation,
    unsigned int subCarrierFreq,
    unsigned int carrierFreq,
    unsigned char modulationIndex,
    double noiseLevel,
    unsigned int simDuration,
    unsigned int numberOfPoints,
    scatter_t* signal
) {
    //========== Variables declaration
    char*          encodedData      = NULL;      // Encoded data
    size_t         encodedSize      = 0;         // Size of the encoded data
    char*          subModulatedData = NULL;      // Sub-carrier modulated data
    size_t         subModulatedSize = 0;         // Size of the sub-carrier modulated data
    scatter_t      envelope         = NULL;      // Envelope of the signal
    scatter_t      modulatedSignal  = NULL;      // Modulated signal
    nfc_sigParam_t sigParam;                     // Parameters of the signal

    //========== Check arguments
    /* Inch no need, already done in sub-functions */

    //========== Printing parameters
    PRINT(INFO, "===== NFC SIGNAL GENERATION PARAMETERS =====");
    PRINT(INFO, "Data:                   %s",       data);
    PRINT(INFO, "Size:                   %ld",      size);
    PRINT(INFO, "Bit rate:               %d bit/s", bitRate);
    PRINT(INFO, "Encoding type:          %d",       encodingType);
    PRINT(INFO, "Sub-carrier modulation: %d",       subModulation);
    PRINT(INFO, "Sub-carrier frequency:  %d Hz",    subCarrierFreq);
    PRINT(INFO, "Carrier frequency:      %d Hz",    carrierFreq);
    PRINT(INFO, "Modulation index:       %d%%",     modulationIndex);
    PRINT(INFO, "Noise level:            %f",       noiseLevel);
    PRINT(INFO, "Simulation duration:    %d ms",    simDuration);
    PRINT(INFO, "Number of points:       %d",       numberOfPoints);
    PRINT(INFO, "============================================");

    // PRINT(DBG, "===== INPUT DATA =====");
    // for (int i = 0; (size_t)i < size; i=i+1) {
    //     PRINT(DBG, "Data: [%d]\t0x%02X", i, data[i]);
    // }

    //========== Prepare parameters
    sigParam.data            = data;
    sigParam.dataSize        = size;
    sigParam.bitRate         = bitRate;
    sigParam.encodingType    = encodingType;
    sigParam.subModulation   = subModulation;
    sigParam.subCarrierFreq  = subCarrierFreq;
    sigParam.carrierFreq     = carrierFreq;
    sigParam.modulationIndex = modulationIndex;
    sigParam.noiseLevel      = noiseLevel;
    sigParam.simDuration     = simDuration;
    sigParam.numberOfPoints  = numberOfPoints;

    //========== Encode data
    PRINT(INFO, "Encoding data");
    if (nfc_encodeData(
        &sigParam,
        &encodedData, &encodedSize
    )) {
        PRINT(ERR, "Failed to encode data");
        return -1;
    }

    // PRINT(DBG, "===== ENCODED DATA =====");
    // for (int i = 0; (size_t)i < encodedSize; i=i+1) {
    //     PRINT(DBG, "Encoded data: [%d]\t%d", i, encodedData[i]);
    // }

    //========== Sub-carrier modulation
    PRINT(INFO, "Modulating data with sub-carrier");
    if (nfc_modulateSubCarrier(
        encodedData, encodedSize,
        &sigParam,
        &subModulatedData, &subModulatedSize
    )) {
        PRINT(ERR, "Failed to modulate data with sub-carrier");
        free(encodedData);
        return -1;
    }

    // PRINT(DBG, "===== SUB-MODULATED DATA =====");
    // for (int i = 0; (size_t)i < subModulatedSize; i=i+1) {
    //     PRINT(DBG, "Sub-modulated data: [%d]\t%d", i, subModulatedData[i]);
    // }

    //========== Generate envelope
    PRINT(INFO, "Generating envelope");
    if (nfc_createEnvelope(
        subModulatedData, subModulatedSize,
        &sigParam,
        &envelope
    )) {
        PRINT(ERR, "Failed to generate envelope");
        free(encodedData);
        free(subModulatedData);
        return -1;
    }

    // PRINT(DBG, "===== ENVELOPE =====");
    // scatter_print(*envelope, '\t', DBG);

    //========== Modulate signal
    PRINT(INFO, "Modulating signal");
    if (nfc_modulate(
        envelope, 
        &sigParam,
        noiseLevel ? &modulatedSignal : signal
    )) {
        PRINT(ERR, "Failed to modulate signal");
        free(encodedData);
        free(subModulatedData);
        scatter_destroy(envelope);
        return -1;
    }

    // PRINT(DBG, "===== MODULATED SIGNAL =====");
    // scatter_print(*modulatedSignal, '\t', DBG);

    //========== Add noise
    if (!noiseLevel) {
        PRINT(INFO, "Skipping noise addition");
        free(encodedData);
        free(subModulatedData);
        scatter_destroy(envelope);
        PRINT(SUCC, "Signal successfully generated");
        return 0;
    }

    PRINT(INFO, "Adding noise to the signal");
    if (nfc_addNoise(
        modulatedSignal,
        &sigParam,
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