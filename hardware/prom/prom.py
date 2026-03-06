import sys
from pwn import remote
from itertools import product

# -------------------------
# Check command line args
# -------------------------
if len(sys.argv) != 3:
    print(f"Usage: python {sys.argv[0]} <IP> <PORT>")
    sys.exit(1)

HOST = sys.argv[1]
PORT = int(sys.argv[2])

# -------------------------
# Base EEPROM pins
# -------------------------
base_pins = [5.0, 12.0, 5.0, 5.0, 5.0, 5.0]  # A10-A6
last_5_options = [0.0, 5.0]

# Generate all 32 combinations for last 5 pins
combinations = list(product(last_5_options, repeat=5))

# -------------------------
# Connect to EEPROM server
# -------------------------
conn = remote(HOST, PORT)

# -------------------------
# Helper: send command & capture output until '> '
# -------------------------
def send_and_capture(cmd):
    conn.sendline(cmd.encode())
    data = conn.recvuntil(b"> ").decode(errors="ignore")
    output = data.rstrip("> ").rstrip()
    # Print EEPROM response only
    print(output)
    return output

# -------------------------
# Initialize EEPROM pins
# -------------------------
send_and_capture("set_ce_pin(0)")
send_and_capture("set_oe_pin(0)")
send_and_capture("set_we_pin(5.0)")
send_and_capture("set_io_pins([0, 0, 0, 0, 0, 0, 0, 0])")

# -------------------------
# Read all 32 bytes and build ASCII string
# -------------------------
ascii_string = ""

for combo in combinations:
    pins = base_pins + list(combo)
    send_and_capture(f"set_address_pins({pins})")
    output = send_and_capture("read_byte()")

    # Parse first hex value and convert to ASCII
    for line in output.splitlines():
        line = line.strip()
        if line.startswith("Read"):
            hex_val = line.split()[1]  # e.g., "0x41"
            ascii_char = chr(int(hex_val, 16))
            ascii_string += ascii_char
            break

conn.close()

# -------------------------
# Print final ASCII string
# -------------------------
print("\nFinal ASCII string:")
print(ascii_string + '}')
