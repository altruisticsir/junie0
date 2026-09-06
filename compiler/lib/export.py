from lib.bytes import conversions, ROW_REQUIRED
from lib import file

lookup_output_path = "interpreter/lib/lookup.h"

# A list of all of the pins
# and their letter + number
# assignments
pin_map = [
    ["A", 0, 0],  # => 2 : 0
    ["A", 1, 1],  # => 3 : 1
    ["B", 0, 2],  # => 4 : 2
    ["B", 1, 3],  # => 5 : 3
    ["B", 2, 4],  # => 6 : 4
    ["B", 3, 5],  # => 7 : 5
    ["A", 2],  # => 9 : 6
    ["A", 3],  # => 10 : 7
    ["B", 4],  # => 11 : 8
    ["A", 4],  # => 12 : 9
    ["B", 5],  # => 14 : 10
    ["B", 7],  # => 16 : 11
    ["B", 8],  # => 17 : 12
    ["B", 9],  # => 18 : 13
    ["B", 10],  # => 21 : 14
    ["B", 11],  # => 22 : 15
    ["B", 13, 11],  # => 24 : 16
    ["B", 14, 10],  # => 25 : 17
    ["B", 15, 9],  # => 26 : 18
]


def export():
    output = "#ifndef LOOKUP\n#define LOOKUP\n\n/* auto-generated */\n"
    for key in conversions:
        byte = conversions[key]
        output += f"#define {key} 0x{byte:02X}U\n"

    # Auto export pin map information

    # Check that the program isn't emulated (checking
    # os directly because the error checks are crazy)
    output += f"#if !defined(_WIN32) && !defined(__APPLE__)\n"

    # Pin map register struct
    output += """
typedef struct {
    volatile uint32_t *const lat_set;
    volatile uint32_t *const lat_clr;
    volatile uint32_t *const lat_inv;
    volatile uint32_t *const tris_set;
    volatile uint32_t *const tris_clr;
    volatile uint32_t *const port;
    volatile uint32_t *const ansel_set;
    volatile uint32_t *const ansel_clr;
    uint32_t mask;
    int analog_channel;
} pin_registers;

"""

    # Pin map array initialization
    output += "static const pin_registers pin_map[] = {\n"

    # Define each pin map element with the information
    # provided in the pin_map list above
    for map in pin_map:
        letter = map[0]
        number = map[1]
        if len(map) > 2:
            analog = map[2]
        else:
            analog = -1
        output += "\t{\n"
        output += f"\t\t.lat_set = &LAT{letter}SET,\n"
        output += f"\t\t.lat_clr = &LAT{letter}CLR,\n"
        output += f"\t\t.lat_inv = &LAT{letter}INV,\n"
        output += f"\t\t.tris_set = &TRIS{letter}SET,\n"
        output += f"\t\t.tris_clr = &TRIS{letter}CLR,\n"
        output += f"\t\t.port = &PORT{letter},\n"
        output += f"\t\t.ansel_set = &ANSEL{letter}SET,\n"
        output += f"\t\t.ansel_clr = &ANSEL{letter}CLR,\n"
        output += f"\t\t.mask = (1 << {number}),\n"
        output += f"\t\t.analog_channel = {analog},\n"
        output += "\t},\n"

    output += "};\n#endif\n"

    output += f"#define ROW_REQUIRED {ROW_REQUIRED - 1}\n"
    output += "\n#endif"

    file.write_file(lookup_output_path, output)
