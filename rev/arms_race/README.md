# Arms Race — ARM Emulator Challenge

| | |
|---|---|
| **Platform** | [HackTheBox](https://www.hackthebox.com/) |
| **Category** | Reverse Engineering |
| **Difficulty** | Medium |

## Challenge Overview

This is a remote ARM emulation challenge. The server sends a series of ARM instructions encoded as hex, and we must emulate their execution locally to compute the result in register `r0`.

---

## Challenge Analysis

### How It Works

1. Connect to the server
2. Server sends ARM instructions as hex string (e.g., `e3a00041e0811002...`)
3. Parse the hex, convert to 32-bit ARM instructions (little-endian)
4. Emulate each instruction using our own ARM emulator
5. Send back the value of `r0` as the answer

### Implemented Opcodes

The emulator supports the following ARM data-processing instructions:

| Opcode | Mnemonic | Operation |
|--------|----------|-----------|
| 0x0 | AND | `rd = rn & operand2` |
| 0x1 | EOR | `rd = rn ^ operand2` |
| 0x2 | SUB | `rd = rn - operand2` |
| 0x3 | RSB | `rd = operand2 - rn` |
| 0x4 | ADD | `rd = rn + operand2` |
| 0x5 | ADC | `rd = rn + operand2 + carry` |
| 0x6 | SBC | `rd = rn - operand2 - !carry` |
| 0xC | ORR | `rd = rn \| operand2` |
| 0xD | MOV | `rd = operand2` |

### Instruction Types

1. **Move Wide** (`MOVW`/`MOVT`): Load immediate values into registers
2. **Multiplication** (`MUL`): `rd = rm * rs`
3. **Data Processing**: Standard two-operand ALU operations

---

## Project Structure

| File | Description |
|------|-------------|
| `main.cpp` | Connects to server, parses instructions, runs emulator |
| `ArmEmulator.hpp` | Implements ARM instruction emulation |
| `Makefile` | Build configuration |

---

## Usage

### Build

```bash
make
```

### Run

```bash
./solver <IP> <PORT>
```

Example:
```bash
./solver <IP> <PORT>
```

### Makefile Targets

| Target | Description |
|--------|-------------|
| `make` or `make all` | Build the solver |
| `make run` | Build and run with default IP/PORT |
| `make clean` | Remove build artifacts |

---

## How the Solver Works

```
Server sends hex string → Parse to uint32[] → Emulate each → Get r0 value → Send to server
```

The solver:
1. Establishes TCP connection to the server
2. Receives the hex-encoded ARM instructions
3. Extracts hex digits and converts to 32-bit integers (little-endian)
4. Executes each instruction via `ARMEmulator::Execute()`
5. Sends the final value of `r0` back to the server
6. Repeats for subsequent rounds (if any)

---

## Key Insight

This challenge tests understanding of:
- ARM instruction encoding
- Little-endian byte order
- Basic emulator implementation
- Network socket programming

The solution requires building a minimal but functional ARM Thumb/ARM mode emulator that handles common data-processing operations.
