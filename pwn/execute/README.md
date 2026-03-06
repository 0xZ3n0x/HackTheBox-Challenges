# Execute — HTB Pwn Challenge

| | |
|---|---|
| **Platform** | [HackTheBox](https://www.hackthebox.com/) |
| **Category** | Pwn |
| **Difficulty** | Hard |

## Challenge Overview

The goal is to make the binary read and print the contents of `flag.txt` through shellcode execution. The binary reads input into a stack buffer and executes it directly, but implements a blacklist to block certain bytes.

---

## Vulnerability Analysis

### The Bug

The binary performs a classic stack-based code injection:

```c
int size = read(0, buf, 60);    // Reads up to 60 bytes
((void(*)())buf)();             // Executes buffer as code
```

The stack is **executable** (NX disabled), allowing shellcode execution.

### The Restriction

A hardcoded blacklist rejects any input containing forbidden bytes. This prevents straightforward shellcode payloads.

---

## Exploitation Strategy

### Two-Stage Shellcode

To bypass both size (60 bytes) and blacklist restrictions, we use a two-stage approach:

#### Stage 1: Tiny Loader (fits in 60 bytes)
1. Allocate an RWX memory region on the heap via `mmap()`
2. Read the second stage from stdin into that region
3. Jump to the second stage

#### Stage 2: Flag Reader
1. Open `flag.txt`
2. Read contents into a heap buffer
3. Write to stdout
4. Clean up and exit

---

## Files

| File | Description |
|------|-------------|
| `payload.asm` | Assembly source for shellcode stages |
| `Makefile` | Builds and extracts binary payload |
| `exploit.py` | Python exploit script using pwntools |
| `payload.bin` | Raw binary shellcode |

---

## Usage

### Prerequisites
- `pwntools` installed
- `nasm` for assembly

### Build Payload

```bash
make
```

This assembles `payload.asm` and extracts the `.text` section to `payload.bin`.

### Local Exploitation

```bash
# Using Makefile
make run

# Or manually with pwntools
python3 exploit.py
```

### Remote Exploitation

Modify `exploit.py` to use `remote(host, port)` instead of `process()`:

```python
io = remote('target-ip', port)
```

---

## Why This Approach?

The two-stage design:
- Keeps Stage 1 small enough to fit 60-byte limit and evade blacklist
- Allocates fresh RWX memory for Stage 2 to perform file operations
- Provides precise control over syscalls

Alternative approaches (spawning shell, direct syscalls) were blocked by the blacklist or size constraints.
