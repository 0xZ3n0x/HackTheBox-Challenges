# PROM — EEPROM Programming Challenge

| | |
|---|---|
| **Platform** | [HackTheBox](https://www.hackthebox.com/) |
| **Category** | Hardware |
| **Difficulty** | Easy |

## Challenge Overview

This challenge simulates interacting with a physical EEPROM memory chip through a remote server. The goal is to read the flag stored in the EEPROM by manipulating address pins and reading byte values.

---

## Challenge Analysis

### EEPROM Structure

The EEPROM has 6 address pins (A10-A6 are fixed) and 5 address pins that can be controlled. This gives $2^5 = 32$ possible address combinations.

**Fixed Address Pins:** `[5.0, 12.0, 5.0, 5.0, 5.0, 5.0]` (A10-A6)

**Variable Address Pins:** 5 pins, each can be `0.0` or `5.0` volts

### Solution Approach

The solution iterates through all 32 possible combinations for the variable address pins, reading one byte at each address. The resulting bytes form the ASCII flag string.

### Key Discovery: AT28C16 Unique ID

According to the **AT28C16** datasheet, applying **12V** to a specific address pin (A9) enables the "Unique ID" mode, which allows reading a manufacturer-specific identification code. This is the technique used to access the hidden data in this challenge.

---

## Solution Implementation

The Python script (`prom.py`) performs the following:

1. **Initialize EEPROM:** Set control pins (CE, OE, WE) and initialize I/O pins
2. **Brute Force Addresses:** Generate all 32 combinations of the 5 variable address pins
3. **Read Each Byte:** For each address, send `set_address_pins()` and call `read_byte()`
4. **Parse Response:** Extract the hex value from the server response and convert to ASCII
5. **Construct Flag:** Concatenate all bytes to form the complete flag

---

## Usage

### Prerequisites
- Python 3
- `pwntools` library

### Run

```bash
python3 prom.py <IP> <PORT>
```

---

## How It Works

| Command | Description |
|---------|-------------|
| `set_ce_pin(0)` | Chip Enable - enable the EEPROM |
| `set_oe_pin(0)` | Output Enable - enable data output |
| `set_we_pin(5.0)` | Write Enable - disable writes |
| `set_io_pins([...])` | Set 8-bit data bus |
| `set_address_pins([...])` | Set address pins A0-A10 |
| `read_byte()` | Read current byte from EEPROM |

---

## Key Insight

The challenge is essentially an **address space exploration** problem. With only 5 controllable address pins, we can enumerate all 32 possible memory locations. The flag is stored contiguously across these addresses.
