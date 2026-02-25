# Execute — HTB Pwn Challenge (Analysis)

This writeup documents a solution to the **Execute** challenge from Hack The Box (Pwn category). It explains the exploitation strategy used to make the target binary read and print the contents of `flag.txt`. The challenge's executable files can be downloaded from the Hack The Box (HTB) website. No binary/source code is included here — this document contains only my analysis and solutions.

## Goal

The program should read the contents of `flag.txt` (located next to the binary) and print them to stdout. The objective is to supply an input payload that, when executed by the binary, performs the required syscalls to `open`, `read` and `write` the flag.

## Exploitation method

The binary reads up to 60 bytes into a stack buffer:

- `int size = read(0, buf, 60);`

and then executes that buffer:

- `((void(*)())buf)();`

The stack is executable, which allows code injection. However, the program uses a hard‑coded blacklist and exits if any forbidden byte is present in the input. The chosen approach is to craft a compact, position‑independent payload that performs `open("flag.txt")`, `read`, and `write`, or alternatively to use a tiny decoder plus an encoded payload so the transmitted bytes avoid the blacklist.

## How I exploit it ?

The plan is a two‑stage shellcode:

- Stage 1 is a tiny loader that allocates an RWX memory region on the heap, reads a second‑stage payload from stdin into that region, then jumps to it.
- Stage 2 (the larger payload) opens `flag.txt`, reads its contents into a heap buffer, writes them to stdout, and cleans up.

This two‑stage design keeps the first stage compact (to satisfy the input‑size and blacklist constraints) and moves the more complex work into an executable area allocated at runtime.

### Stage 1 - Loader

This tiny loader's job is to create a safe, executable region in memory and fetch a larger payload into it. Concretely, it:

1. Requests a new memory mapping with read, write and execute permissions,
2. Reads the second‑stage payload from stdin into that mapping,
3. Jumps to the newly filled memory to transfer control.

Keeping the loader minimal allows it to bypass size and blacklist restrictions in the original program; the heavy lifting is then done by the second stage running from the fresh RWX memory.

### Stage 2 - Flag Reader

The second stage performs the actual file operations:

1. Allocates a small writable buffer on the heap,
2. Opens the file named `flag.txt`,
3. Reads the file into the heap buffer,
4. Writes the bytes read to standard output,
5. Closes the file and frees the heap mapping before returning.

In short: Stage 2 executes `open → read → write` (and cleanup), while Stage 1 simply makes room and transfers control.

## Why I did use this Method

The two‑stage, heap‑based approach provides precise control while keeping the initial payload small enough to avoid the binary’s size and blacklist restrictions. By allocating an RWX region and loading a larger second stage there, the loader remains tiny and position‑independent, and the second stage can perform file operations reliably. An alternative would have been to spawn a shell and print the flag, but the heap‑based method gives better control and fits the constraints of this challenge.

## How to exploit

Place the build and runner files in the challenge directory (Makefile, assembler source(s) such as payload.asm, payload.bin, and exploit.py). The general workflow:

1. Preparation
   - Put Makefile, assembler source and exploit.py in the same folder as the `execute` binary.

2. Build the payload
   - Run the Makefile to assemble and extract the .text section into payload.bin (e.g. `make`).

3. Local exploitation
   - Use the Makefile `run` target or run the exploit script directly (`python3 exploit.py`). A local exploit typically uses pwntools `process()` to start the target, sends payload.bin, and switches to interactive mode to view output.

4. Remote exploitation
   - Adapt the exploit script to connect to a remote service using pwntools `remote(host, port)`