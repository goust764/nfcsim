/**
 * @file nfcsig.h
 * @author OUSSSET Gaël
 * @brief Header file for nfcsig.c
 * @version 0.1
 * @date 2024-12-24
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef NFCSIG_H
#define NFCSIG_H

#include <stdlib.h>
#include "scatter.h"

//========== Structures declarations
/**
 * @brief Encoding type
 * 
 */
typedef enum {
    MOD_MILLER,                                  // Modified miller encoding
    NRZ,                                         // Non-return-to-zero encoding
    MANCHESTER                                   // Manchester encoding
} nfc_encoding_t;

/**
 * @brief Type of sub-modulation
 * 
 */
typedef enum {
    NONE,                                        // No sub-carrier modulation
    OOK,                                         // On-off keying
    BPSK                                         // Binary phase shift keying
} nfc_subModulation_t;

/**
 * @brief Structure containing all the parameters of a signal
 * 
 */
typedef struct {
    char*               data;                    // Data to encode
    size_t              dataSize;                // Number of byte in the data
    unsigned int        bitRate;                 // Bit rate of the input data
    nfc_encoding_t      encodingType;            // Type of encoding to apply to the data
    nfc_subModulation_t subModulation;           // Type of sub-carrier modulation
    unsigned int        subCarrierFreq;          // Frequency of the sub-carrier (Hz)
    unsigned int        carrierFreq;             // Frequency of the carrier (Hz)
    unsigned char       modulationIndex;         // Index of the modulation of the envelope (%)
    double              noiseLevel;              // Signal to noise ratio
    unsigned int        simDuration;             // Duration of the simulation (ns)
    unsigned int        numberOfPoints;          // Number of points to generate
} nfc_sigParam_t;

//========== Signal coding
/**
 * @brief Convert a serie of bytes into encoded bits.
 *        The output data is encoded as following:
 *        Each input beat is slitted in four (required for the modified miller
 *        encoding). See schematic below:
 * Input  bit  : ||           in[0] & 0x01            ||           in[0] & 0x02            ||
 * Output bits : || out[0] | out[1] | out[2] | out[3] || out[4] | out[5] | out[6] | out[7] ||
 * 
 * @param sigParam Parameters of the signal
 * @param encodedData Encoded data
 * @param encodedSize Size of the encoded data (4*size)
 * @return int - 0 if success, -1 otherwise
 */
int nfc_encodeData(nfc_sigParam_t* sigParam, char** encodedData, size_t* encodedSize);

//========== Sub-carrier modulation
/**
 * @brief Convert encoded data into modulated data using a sub-carrier
 *        Note : The sub-carrier frequency should be a multiple of the bit rate
 * 
 * @param encodeData Data to modulate
 * @param encodedSize Size of the data
 * @param sigParam Parameters of the signal
 * @param subModulatedData Modulated data
 * @param subModulatedSize Size of the modulated data
 * @return int - 0 if success, -1 otherwise
 */
int nfc_modulateSubCarrier(char* encodeData, size_t encodedSize, nfc_sigParam_t* sigParam, char** subModulatedData, size_t* subModulatedSize);

//========== Signal envelope generation
/**
 * @brief Create an envelope for the modulated data.
 *        TODO : Add a low-pass on the envelope to make it more realistic ?
 * 
 * @param subModulatedData Data with the sub-carrier modulation
 * @param subModulatedSize Size of the input data
 * @param sigParam Parameters of the signal
 * @param envelope Envelope of the signal (amplitude vs time in ns)
 * @return int - 0 if success, -1 otherwise
 */
int nfc_createEnvelope(char* subModulatedData, size_t subModulatedSize, nfc_sigParam_t* sigParam, scatter_t* envelope);

//========== Modulation
/**
 * @brief Modulate the enveloppe with a carrier frequency
 * 
 * @param enveloppe Enveloppe of the signal to modulate (amplitude vs time in ns)
 * @param sigParam Parameters of the signal
 * @param modulatedSignal Modulated signal (amplitude vs time in ns)
 * @return int - 0 if success, -1 otherwise
 */
int nfc_modulate(scatter_t enveloppe, nfc_sigParam_t* sigParam, scatter_t* modulatedSignal);

//========== Noise
/**
 * @brief Add noice to a signal
 * 
 * @param signal Signal to add noise to (amplitude vs time in ns)
 * @param sigParam Parameters of the signal
 * @param noisySignal Noisy signal (amplitude vs time in ns)
 * @return int - 0 if success, -1 otherwise
 */
int nfc_addNoise(scatter_t signal, nfc_sigParam_t* sigParam, scatter_t* noisySignal);

//========== General function
/**
 * @brief Generate an NFC signal
 * 
 * @param data Data to encode
 * @param size Size of the data
 * @param bitRate Bit rate of the input data
 * @param encodingType Type of encoding to apply to the data
 * @param subModulation Type of sub-carrier modulation
 * @param subCarrierFreq Frequency of the sub-carrier (Hz)
 * @param carrierFreq Frequency of the carrier (Hz)
 * @param modulationIndex Index of the modulation of the envelope (%)
 * @param noiseLevel Signal to noise ratio
 * @param simDuration Duration of the simulation (ns)
 * @param numberOfPoints Number of points to generate
 * @param signal Generated signal (amplitude vs time in ns)
 * @return int - 0 if success, -1 otherwise
 */
int nfc_createSignal(char* data, size_t size, unsigned int bitRate, nfc_encoding_t encodingType, nfc_subModulation_t subModulation, unsigned int subCarrierFreq, unsigned int carrierFreq, unsigned char modulationIndex, double noiseLevel, unsigned int simDuration, unsigned int numberOfPoints, scatter_t* signal);

#endif // NFCSIG_H