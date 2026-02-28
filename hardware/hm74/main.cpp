#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>

#include "FrequencyQueue.hpp"
#include "HammingDecoder.hpp"

using namespace std;

// Extracts a 4-bit nibble from a bitstring and converts it to a byte value
unsigned char getNibble(const string &bitstream, size_t nibbleIndex)
{
    bitset<4> bits(bitstream.substr(nibbleIndex * 4, 4));
    return static_cast<unsigned char>(bits.to_ulong());
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <input_file_path>" << endl;
        return 1;
    }

    string filePath = argv[1];
    ifstream file(filePath);
    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << filePath << endl;
        return 1;
    }

    vector<string> allPackets;
    string line;
    while (getline(file, line))
    {
        if (line.rfind("Captured:", 0) == 0)
        {
            string bits;
            // Filter only '0' and '1' characters
            for (char c : line.substr(10))
            {
                if (c == '0' || c == '1')
                    bits.push_back(c);
            }
            if (!bits.empty())
                allPackets.push_back(bits);
        }
    }
    file.close();

    cout << "[+] Processed " << allPackets.size() << " packets from " << filePath << endl;

    // 136 nibbles = 68 bytes (standard flag length)
    const size_t numberOfNibbles = 136;
    vector<FrequencyQueue> candidates(numberOfNibbles);

    for (const auto &packet : allPackets)
    {
        string bitstream = HammingDecoder::decode(packet);
        if (bitstream.empty())
            continue;

        for (size_t i = 0; i < numberOfNibbles; ++i)
        {
            candidates[i].push(getNibble(bitstream, i));
        }
    }

    // We combine two 4-bit nibbles (High | Low) to form one 8-bit char
    cout << "Flag: ";
    for (size_t i = 0; i < numberOfNibbles; i += 2)
    {
        unsigned char high = candidates[i].pop();
        unsigned char low = candidates[i + 1].pop();

        char decodedChar = static_cast<char>((high << 4) | low);
        cout << decodedChar;
    }
    cout << endl;

    return 0;
}