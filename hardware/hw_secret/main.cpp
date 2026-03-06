#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <charconv>

// Linear Congruential Generator (LCG) state
uint32_t lcg_state = 0;

// LCG parameters (extracted from firmware reverse engineering)
const uint32_t LCG_MULTIPLIER = 0x41c64e6d;
const uint32_t LCG_INCREMENT = 0x8042a;

// Generate next random value using LCG algorithm
// Formula: next = (current * multiplier) + increment
uint32_t lcg_next(void)
{
    lcg_state = lcg_state * LCG_MULTIPLIER + LCG_INCREMENT;
    return lcg_state % 0xffffff;
}

int main()
{
    // Device unique ID extracted from firmware
    const std::vector<uint8_t> unique_id = {0xd2, 0x66, 0xb4, 0x21, 0x83, 0x51, 0x30, 0x2c};
    (void)unique_id; // Suppress unused variable warning

    // Load SPI flash memory dump
    std::vector<uint8_t> flash_data;
    std::ifstream flash_file("flash_memory_dump.bin", std::ios::binary);
    
    if (!flash_file)
    {
        std::cerr << "Failed to open flash_memory_dump.bin\n";
        return 1;
    }

    // Read entire file into memory
    flash_file.seekg(0, std::ios::end);
    size_t flash_size = flash_file.tellg();
    flash_file.seekg(0, std::ios::beg);

    flash_data.resize(flash_size);
    flash_file.read(reinterpret_cast<char*>(flash_data.data()), flash_size);

    if (!flash_file)
    {
        std::cerr << "Error reading flash_memory_dump.bin\n";
        return 1;
    }

    std::cout << "Loaded SPI flash: " << flash_data.size() << " bytes\n";

    // Load seed values from input channel capture
    std::ifstream seeds_file("seeds.bin", std::ios::binary);
    if (!seeds_file)
    {
        std::cerr << "Failed to open seeds.bin\n";
        return 1;
    }

    std::vector<uint32_t> seeds;
    char seed_buffer[8];

    // Each seed is stored as 8 decimal digits
    while (seeds_file.read(seed_buffer, 8))
    {
        uint32_t seed_value;
        auto [ptr, ec] = std::from_chars(seed_buffer, seed_buffer + 8, seed_value);
        if (ec != std::errc())
        {
            std::cerr << "Conversion error for seed\n";
            continue;
        }
        seeds.push_back(seed_value);
    }

    std::cout << "Loaded " << seeds.size() << " seeds\n";

    // Process each seed to extract flag bytes
    for (const uint32_t& seed : seeds)
    {
        lcg_state = seed;

        // Generate 49 addresses per seed
        for (int i = 0; i < 49; i++)
        {
            // Generate 3 random values to form 24-bit address
            uint32_t rand1 = lcg_next();
            uint32_t rand2 = lcg_next();
            uint32_t rand3 = lcg_next();

            // Construct address: 0xRRGGBB format
            uint32_t address = (rand1 & 0xff0000) | (rand2 & 0xff00) | (rand3 & 0xff);

            // Read byte from flash memory at generated address
            if (address < flash_data.size())
            {
                putchar(flash_data[address]);
            }
            else
            {
                std::cerr << "Address out of range: " << address << "\n";
            }
        }

        putchar('\n');
    }

    return 0;
}
