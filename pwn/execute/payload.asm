SECTION .data
    alloc_size    equ 1024          ; Memory allocation size = 1024 bytes

    PROT_R        equ 1             ; Page can be read
    PROT_W        equ 2             ; Page can be written
    PROT_X        equ 4             ; Page can be executed
    MAP_PRIVATE   equ 2             ; Private mapping (changes not written to file)
    MAP_ANON      equ 0x20          ; Anonymous mapping (not backed by a file)

SECTION .text
    global stage1

stage1:

    xor ebx, ebx ; EBX = 0, used as zero reference
    ; ---------------------------------------------------
    ; mmap(NULL, alloc_size, PROT_READ | PROT_WRITE | PROT_EXEC,
    ;      MAP_PRIVATE | MAP_ANON, -1, 0)
    ; Allocates executable memory
    ; ---------------------------------------------------
    mov rax, 9              ; sys_mmap system call
    xor rdi, rdi            ; addr = NULL → let kernel choose
    mov rsi, alloc_size     ; length = 1024 bytes
    mov rdx, PROT_R | PROT_W | PROT_X ; memory permissions
    mov r10, MAP_PRIVATE | MAP_ANON
    mov r8, rbx
    dec r8                  ; fd = -1 (required for MAP_ANON)
    mov r9, rbx             ; offset = 0
    syscall

    mov r12, rax            ; store returned memory pointer

    ; ---------------------------------------------------
    ; read(0, mmap_area, alloc_size)
    ; Reads user input into executable memory
    ; ---------------------------------------------------
    mov rax, rbx            ; sys_read
    mov rdi, rbx            ; stdin
    mov rsi, r12            ; buffer
    mov rdx, alloc_size
    syscall
    jmp r12                 ; jump to user input (execute it)
    nop
stage2:
    ; mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
    ; Allocate 32 bytes on heap
    mov rax, 9
    xor rdi, rdi
    mov rsi, 32
    mov rdx, 3              ; PROT_READ | PROT_WRITE
    mov r10, 0x22           ; MAP_PRIVATE | MAP_ANON
    mov r8, -1              ; fd = -1
    xor r9, r9              ; offset = 0
    syscall
    mov r13, rax            ; save heap pointer in r13

    ; open("flag.txt", O_RDONLY, 0)
    mov rax, 2              ; sys_open
    lea rdi, [rel filename]
    xor rsi, rsi            ; O_RDONLY
    xor rdx, rdx
    syscall
    mov r12, rax            ; save file descriptor

    ; read(fd, heap_buffer, 32)
    mov rax, 0              ; sys_read
    mov rdi, r12            ; fd
    mov rsi, r13            ; buffer
    mov rdx, 32             ; read 32 bytes
    syscall
    mov r14, rax            ; save number of bytes read

    ; write(1, heap_buffer, r14)
    mov rax, 1              ; sys_write
    mov rdi, 1              ; stdout
    mov rsi, r13            ; buffer
    mov rdx, r14            ; count
    syscall

    ; close(fd)
    mov rax, 3              ; sys_close
    mov rdi, r12
    syscall

    ; munmap(heap_buffer, 32)
    mov rax, 11             ; sys_munmap
    mov rdi, r13
    mov rsi, 32
    syscall

    ret
filename:
    db 'flag.txt',0          ; filename to read
