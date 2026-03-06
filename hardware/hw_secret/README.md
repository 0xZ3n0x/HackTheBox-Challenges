# Secret Treasures — Hardware Challenge

| | |
|---|---|
| **Platform** | [HackTheBox](https://www.hackthebox.com/) |
| **Category** | Hardware |
| **Difficulty** | Medium |

## Challenge Overview

This hardware challenge involves extracting a hidden flag from a SPI flash memory dump. The seed values were captured from a hardware signal using Saleae Logic Analyzer, then used with a custom LCG algorithm to generate memory addresses.

---

## Analysis

### Signal Capture

The seed values were extracted from a **SAL file** (Saleae Logic Analyzer capture) using the **Saleae Logic 2** software:
- Configured as Async Serial with appropriate baud rate
- The signal contained decimal digit values representing seed numbers

### Given Files

- **flash_memory_dump.bin** — 16MB SPI flash memory dump containing the flag
- **seeds.bin** — Seed values (8-digit decimal strings) extracted from SAL signal

### The Algorithm

Through reverse engineering the firmware, the following LCG was discovered:

```c
// LCG parameters (discovered from firmware)
MULTIPLIER = 0x41c64e6d
INCREMENT = 0x8042a

// Generate next random value
next_state = current_state * MULTIPLIER + INCREMENT
random = next_state % 0xffffff
```

### Address Generation

For each seed, the algorithm:
1. Initializes LCG state with the seed
2. Generates 3 consecutive random values
3. Combines them into a 24-bit address: `(rand1 & 0xff0000) | (rand2 & 0xff00) | (rand3 & 0xff)`
4. Reads the byte at that address from flash memory

This process repeats for each seed, producing 49 bytes of the flag per seed.

---

## Solution

The solver (`main.cpp`):

1. Loads the 16MB flash memory dump into memory
2. Reads seed values from `seeds.bin` (decimal strings)
3. For each seed:
   - Initializes the LCG
   - Generates 49 addresses using the algorithm
   - Extracts bytes from those addresses
   - Outputs them as ASCII

---

## Files

| File | Description |
|------|-------------|
| `main.cpp` | Solution source code |
| `flash_memory_dump.bin` | 16MB SPI flash memory dump |
| `seeds.bin` | Seed values (8-digit decimal strings from SAL) |
| `Makefile` | Build configuration |

---

## Usage

### Build & Run

```bash
cd hardware/hw_secret
make
./solver.elf
```

### Output

The solver prints the extracted flag after processing all seeds.

---

## Key Points

- **Signal Analysis**: Extract data from SAL files using Saleae Logic Analyzer
- **LCG Reverse Engineering**: Identify multiplier and increment from firmware
- **Memory Dump Analysis**: Extract data from non-sequential addresses
- **Address Construction**: Combine multiple random values into valid offsets
