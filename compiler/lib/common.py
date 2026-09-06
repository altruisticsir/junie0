from rich import print
import struct

# This file includes a bunch of functions
# that'll probably be used quite a lot
# throughout the rest of the program.


# Logs things to the console.
# If the label "error"(case-insensitive)
# is passed, then the program will be
# terminated.
def log(label: str, content: str):
    color = "blue"
    if label.lower() == "warning":
        color = "yellow"

    if label.lower() == "error":
        # Error; log w/ red and stop the
        # program immediately
        color = "bold red"
        print(f"[{color}][Error]:[/{color}] {content}")
        exit(1)
    # Log normally
    print(f"[{color}][{label}][/{color}]: {content}")


# Returns whether or not a string is
# a number or not.
def is_number(value: str) -> bool:
    try:
        int(value)
        return True
    except ValueError:
        pass

    try:
        float(value)
        return True
    except ValueError:
        pass

    return False


# Converts a number into a little endian float
# byte array.
def endian(value: int | float) -> bytearray:
    bytes = struct.pack("<f", float(value))
    return bytes
