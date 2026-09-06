# The Core class contains hardware instructions
# that require extra attention so that the right
# instructions are added.

from lib.bytes import keys

# Parent child tuples convert into
# a single string instruction
lookup = {
    ("delay", "ms"): "DELAY_MS",
    ("digital", "mode"): "DIGITAL_MODE",
    ("digital", "flip"): "DIGITAL_FLIP",
    ("adc", "read"): "ADC_READ",
}

# Because the parent + child lookup
# conversions aren't inside the keys
# of bytes.py, we add them automatically
# through here
for _, value in lookup.items():
    print(value)
    keys.append(value)


class Core:
    def core_fn(self, args):
        # parent:child(...)
        parent = args[0]
        child = args[1]

        # Bundle parent and child (PC)
        pc_tuple = (parent, child)

        if pc_tuple in lookup:
            self.add_instruction(lookup[pc_tuple])
