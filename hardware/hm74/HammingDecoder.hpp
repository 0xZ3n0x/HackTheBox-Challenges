#ifndef HAMMING_DECODER_HPP
#define HAMMING_DECODER_HPP

#include <string>
#include <iostream>

class HammingDecoder
{
public:
    /**
     * @brief Decodes a Hamming(7,4) bitstream and corrects single-bit errors.
     * @param bits Input string of '0's and '1's.
     * @return String of concatenated 4-bit data chunks, or empty string on error.
     */
    static std::string decode(const std::string &bits)
    {
        if (bits.size() % 7 != 0)
        {
            return "";
        }

        std::string decodedData;
        int blockCount = bits.size() / 7;

        for (int i = 0; i < blockCount; ++i)
        {
            int base = i * 7;
            int block[7];

            for (int j = 0; j < 7; ++j)
                block[j] = bits[base + j] - '0';

            // Calculate Syndrome bits (s0, s1, s2)
            int s0 = block[0] ^ block[2] ^ block[4] ^ block[6];
            int s1 = block[1] ^ block[2] ^ block[5] ^ block[6];
            int s2 = block[3] ^ block[4] ^ block[5] ^ block[6];

            int errorPos = (s0 << 2) | (s1 << 1) | s2;

            // If errorPos is non-zero, a flip occurred at (errorPos - 1)
            if (errorPos > 0 && errorPos <= 7)
            {
                block[errorPos - 1] ^= 1;
            }

            // Extract the data bits at specific Hamming(7,4) positions
            // In this implementation: indices 2, 4, 5, 6 carry the 4-bit data
            decodedData.push_back(block[2] + '0');
            decodedData.push_back(block[4] + '0');
            decodedData.push_back(block[5] + '0');
            decodedData.push_back(block[6] + '0');
        }

        return decodedData;
    }
};

#endif