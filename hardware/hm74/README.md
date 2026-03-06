# HM74 — Hamming(7,4) Statistical Signal Decoder

| | |
|---|---|
| **Platform** | [HackTheBox](https://www.hackthebox.com/) |
| **Category** | Hardware |
| **Difficulty** | Medium |

## Challenge Overview

This challenge involves recovering a hidden flag from a noisy bitstream transmitted by a SystemVerilog-based encoder using **Hamming(7,4)** error-correction coding.

---

## Challenge Analysis

### Signal Encoding Logic

The transmitter uses **Hamming(7,4)** to encode 4-bit data nibbles into 7-bit codewords with 3 parity bits.

**Data Bits ($d$):** A 4-bit input nibble ($d_3, d_2, d_1, d_0$)

**Parity Bits ($p$):**
- $p_0 = d_3 \oplus d_2 \oplus d_0$
- $p_1 = d_3 \oplus d_1 \oplus d_0$
- $p_2 = d_2 \oplus d_1 \oplus d_0$

**Serialized Output:** `{p0, p1, d3, p2, d2, d1, d0}`

### The Problem

The captured bitstream contains noise that causes bit flips in individual packets. While standard Hamming(7,4) can correct single-bit errors, this challenge has enough noise that multi-bit errors occur frequently. The solution must handle this through statistical analysis.

---

## Solution Approach

### 1. Hamming Decoding

The `HammingDecoder` processes each 7-bit block:
1. Calculate the **syndrome** ($s_0, s_1, s_2$) using the encoder's XOR logic
2. Identify and flip the erroneous bit (if any)
3. Extract the 4 original data bits

### 2. Statistical Denoising

Since noise exceeds single-packet correction capability, we use a **frequency-based approach**:

- Track every nibble position across all captured packets using a **Max-Heap** (`FrequencyQueue`)
- Select the most frequent nibble at each position as the ground truth
- Combine pairs of 4-bit nibbles to form 8-bit ASCII characters

This "majority vote" technique ensures 100% flag recovery even with high noise levels.

---

## Project Structure

| File | Description |
|------|-------------|
| `main.cpp` | Entry point, handles file I/O and CLI arguments |
| `HammingDecoder.hpp` | Syndrome calculation and bit correction |
| `FrequencyQueue.hpp` | Statistical tracking using max-heap |
| `CMakeLists.txt` | Build configuration |
| `capture.txt` | Input data (noisy bitstream) |

---

## Usage

### Build

```bash
mkdir build && cd build
cmake ..
make
```

### Run

```bash
./hm /path/to/capture.txt
```

---

## Key Insight

The flag is reconstructed by combining the two most-frequent 4-bit nibbles at each position:

```
char = (HighNibble << 4) | LowNibble
```

This robust approach succeeds even in highly unstable transmission environments where single-packet decoding would fail.
