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

int nfc_autoSimTime(nfc_sigParam_t* sigParam) {
    //========== Variables declaration
    unsigned int bitRate = sigParam->bitRate;
    size_t size          = sigParam->dataSize;

    //========== Check arguments
    if (bitRate == 0) {
        PRINT(ERR, "Bit rate cannot be null");
        return -1;
    }

    //========== Calculate the simulation time
    sigParam->simDuration = (unsigned int)((size * 8 * (unsigned long long)1e9) / (unsigned long long)bitRate);

    return 0;
}

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
                    if (i != 0 || j != 0) {      // Skip the first bit, already encoded above
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
                            (j == 0 && !((data[i] >> j) & 0x01) && (data[i-1] >> 7            )) ||
                            // Check the previous bit of the current byte
                            (j != 0 && !((data[i] >> j) & 0x01) && ((data[i]  >> (j-1)) & 0x01))
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
    unsigned int carrierFreq          = sigParam->carrierFreq;
    unsigned char modulationIndex     = sigParam->modulationIndex;
    unsigned int simDuration          = sigParam->simDuration;
    unsigned int numberOfPoints       = sigParam->numberOfPoints;
    double modulationDepth;                      // Depth of the modulation, 
                                                 // we suppose that the 
                                                 // amplitude of the signal is 1
    unsigned int symboleDuration;                // Duration of a symbol in ns 
                                                 // from the subModulatedData
    unsigned int transTime;                      // Time of the transition between two states in points
    unsigned int avgCounter;                     // Counter to calculate the local average of the envelope

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
    
    //----- Calculate the transition time (2 carrier periods)
    transTime = (unsigned int)(2*1e9) / carrierFreq * numberOfPoints / simDuration;
    PRINT(INFO, "Transition time: %d ns", transTime);

    //----- Calculate the modulation depth
    modulationDepth = (double)(100 - modulationIndex) / (double)(modulationIndex + 100);
    PRINT(INFO, "Modulation depth: %f", modulationDepth);

    //----- Generate the envelope
    for (unsigned int i = 0; i < numberOfPoints; i=i+1) {
        (*envelope)->points[i].x =    (int)((unsigned long long)i * 
                                            (unsigned long long)simDuration / 
                                            numberOfPoints);
        (*envelope)->points[i].y = 0;
        avgCounter = 0;
        for (int j = (int)i-(int)transTime+1; j <= (int)i; j=j+1) {
            if (j >= 0 && j < (int)numberOfPoints) {
                if (subModulatedData[(unsigned int)scatter_getX(*envelope, (size_t)j)/symboleDuration])
                    (*envelope)->points[i].y = (*envelope)->points[i].y + 1;
                else
                    (*envelope)->points[i].y = (*envelope)->points[i].y + modulationDepth;
                avgCounter = avgCounter + 1;
            }
        }
        (*envelope)->points[i].y = (*envelope)->points[i].y / (double)avgCounter;
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
    nfc_sigParam_t* sigParam,
    scatter_t* signal
) {
    //========== Variables declaration
    char*          encodedData      = NULL;      // Encoded data
    size_t         encodedSize      = 0;         // Size of the encoded data
    char*          subModulatedData = NULL;      // Sub-carrier modulated data
    size_t         subModulatedSize = 0;         // Size of the sub-carrier modulated data
    scatter_t      envelope         = NULL;      // Envelope of the signal
    scatter_t      modulatedSignal  = NULL;      // Modulated signal

    //========== Check arguments
    /* Inch no need, already done in sub-functions */

    //========== Printing parameters
    PRINT(INFO, "===== NFC SIGNAL GENERATION PARAMETERS =====");
    PRINT(INFO, "Data:                   %s",       sigParam->data);
    PRINT(INFO, "Size:                   %ld",      sigParam->dataSize);
    PRINT(INFO, "Bit rate:               %d bit/s", sigParam->bitRate);
    PRINT(INFO, "Encoding type:          %d",       sigParam->encodingType);
    PRINT(INFO, "Sub-carrier modulation: %d",       sigParam->subModulation);
    PRINT(INFO, "Sub-carrier frequency:  %d Hz",    sigParam->subCarrierFreq);
    PRINT(INFO, "Carrier frequency:      %d Hz",    sigParam->carrierFreq);
    PRINT(INFO, "Modulation index:       %d%%",     sigParam->modulationIndex);
    PRINT(INFO, "Noise level:            %f",       sigParam->noiseLevel);
    PRINT(INFO, "Simulation duration:    %d ms",    sigParam->simDuration);
    PRINT(INFO, "Number of points:       %d",       sigParam->numberOfPoints);
    PRINT(INFO, "============================================");

    // PRINT(DBG, "===== INPUT DATA =====");
    // for (int i = 0; (size_t)i < size; i=i+1)
    //     PRINT(DBG, "Data: [%d]\t0x%02X", i, data[i]);

    //========== Encode data
    PRINT(INFO, "Encoding data");
    if (nfc_encodeData(
        sigParam,
        &encodedData, &encodedSize
    )) {
        PRINT(ERR, "Failed to encode data");
        return -1;
    }

    // PRINT(DBG, "===== ENCODED DATA =====");
    // for (int i = 0; (size_t)i < encodedSize; i=i+1)
    //     PRINT(DBG, "Encoded data: [%d]\t%d", i, encodedData[i]);

    //========== Sub-carrier modulation
    PRINT(INFO, "Modulating data with sub-carrier");
    if (nfc_modulateSubCarrier(
        encodedData, encodedSize,
        sigParam,
        &subModulatedData, &subModulatedSize
    )) {
        PRINT(ERR, "Failed to modulate data with sub-carrier");
        free(encodedData);
        return -1;
    }

    // PRINT(DBG, "===== SUB-MODULATED DATA =====");
    // for (int i = 0; (size_t)i < subModulatedSize; i=i+1)
    //     PRINT(DBG, "Sub-modulated data: [%d]\t%d", i, subModulatedData[i]);

    //========== Generate envelope
    PRINT(INFO, "Generating envelope");
    if (nfc_createEnvelope(
        subModulatedData, subModulatedSize,
        sigParam,
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
        sigParam,
        sigParam->noiseLevel ? &modulatedSignal : signal
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
    if (!sigParam->noiseLevel) {
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
        sigParam,
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

int nfc_standardSignal(
    char* data,
    size_t size,
    nfc_standard_t standard,
    nfc_dataTransm_t dataTransm,
    unsigned int bitRate,
    double noiseLevel,
    unsigned int numberOfPoints,
    scatter_t* signal
) {
    //========== Variables declaration
    nfc_sigParam_t sigParam;                     // Parameters of the signal

    //========== Check arguments
    if (standard != NFC_A && standard != NFC_B) {
        PRINT(ERR, "Invalid NFC standard");
        return -1;
    }
    if (dataTransm != PCD && dataTransm != PICC) {
        PRINT(ERR, "Invalid data transmission mode");
        return -1;
    }
    if (bitRate < 106000 || bitRate > 424000)
        PRINT(
            WARN,
            "Non-standard bit rate of %d bit/s, should be between 106 kbit/s and 424 kbit/s", 
            bitRate
        );

    //========== Prepare parameters
    sigParam.data            = data;
    sigParam.dataSize        = size;
    sigParam.bitRate         = bitRate;
    sigParam.carrierFreq     = CARRIER_FREQ;
    sigParam.noiseLevel      = noiseLevel;
    sigParam.numberOfPoints  = numberOfPoints;

    switch (standard) {
        //----- NFC-A standard
        case NFC_A:
            PRINT(INFO, "Generating NFC-A signal");
            switch (dataTransm) {
                //----- Proximity Coupling Device
                case PCD:
                    PRINT(INFO, "PCD data transmitter");
                    sigParam.encodingType    = MOD_MILLER;
                    sigParam.subModulation   = NONE;
                    sigParam.subCarrierFreq  = 0;
                    sigParam.modulationIndex = 100;
                break;

                //----- Proximity Integrated Circuit Card
                case PICC:
                    PRINT(INFO, "PICC data transmitter");
                    sigParam.encodingType    = MANCHESTER;
                    sigParam.subModulation   = OOK;
                    sigParam.subCarrierFreq  = SUB_CARRIER_FREQ;
                    sigParam.modulationIndex = 10;
                break;

                //----- Default case
                default:
                    PRINT(ERR, "Invalid data transmission mode");
                    return -1;
                break;
            }
        break;

        //----- NFC-B standard
        case NFC_B:
            PRINT(INFO, "Generating NFC-B signal");
            switch (dataTransm) {
                //----- Proximity Coupling Device
                case PCD:
                    PRINT(INFO, "PCD data transmitter");
                    sigParam.encodingType    = NRZ;
                    sigParam.subModulation   = NONE;
                    sigParam.subCarrierFreq  = 0;
                    sigParam.modulationIndex = 10;
                break;

                //----- Proximity Integrated Circuit Card
                case PICC:
                    PRINT(INFO, "PICC data transmitter");
                    sigParam.encodingType    = NRZ;
                    sigParam.subModulation   = BPSK;
                    sigParam.subCarrierFreq  = SUB_CARRIER_FREQ;
                    sigParam.modulationIndex = 10;
                break;

                //----- Default case
                default:
                    PRINT(ERR, "Invalid data transmission mode");
                    return -1;
                break;
            }
        break;
    }

    nfc_autoSimTime(&sigParam);

    //========== Generate signal
    if (nfc_createSignal(
        &sigParam,
        signal
    )) {
        PRINT(ERR, "Failed to generate signal");
        return -1;
    }

    return 0;
}