from lib import common
from rich import print

# Key values for all accounted instructions;
# are assigned byte values later
keys = [
    "NUMBER",
    "STRING",
    "STRING_END",  # Should be okay until ~index 33 (hacky, I know!)
    "NIL",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
    "CONDITION",
    "SET",
    "FETCH",
    "FUNCTION",
    "RETURN_SAVE",
    "RETURN",
    "POP",
    "FOR",
    "WHILE",
    "IF",
    "TICK",
    "JUMP",
    "SAVE",
    "RESTORE",
    "START_FILE",
    "END_FILE",
]
conversions = {}
key_assignment_offset = 1  # Where byte assignment for keys begins
# e.g. if it's 1, then the first byte assigned is 0x01

# Number of passes the compiler has
# made so far
pass_count = 0

destination_indexes = {}

ROW_REQUIRED = 8


# Converts instructions into bytes
def get_byte_set(instruction_set: list, verbose) -> list:
    global pass_count

    # Increment; tells what
    # pass the compiler is on
    # e.g. First pass? Second pass?
    pass_count += 1

    nest_depth = 0
    if verbose == True:
        common.log(
            "Bytes",
            f"Converting instructions into bytes [yellow]({(256 - key_assignment_offset) - len(keys)} conversion bytes free)",
        )

    # Assign a byte to all of the keys
    # and place into a dictionary
    conversions.clear()
    for i, key in enumerate(keys):
        conversions[key] = i + key_assignment_offset

    byte_set = bytearray()  # Bytes are stored as INTEGERS, not bytes
    # in this list; hence "pseudo"

    symbols = {}

    # Saves scopes, or the information
    # of symbols at different points in
    # compilation
    scopes = []

    def log_conversion(original, new, number):
        nonlocal nest_depth
        used_depth = nest_depth

        if verbose == False:
            return
        # Unify spacing so that it looks nice
        longest_instruction = max(instruction_set, key=len)
        space = len(longest_instruction) - len(str(original))

        color = "blue"
        if len(new) > 1:
            # Logging a byte array
            color = "yellow"
        if original == "SAVE":
            nest_depth += 1
            color = "hot_pink2"
        elif original == "RESTORE":
            nest_depth -= 1
            used_depth = nest_depth
            color = "hot_pink2"
        elif original[0] == '"':
            # String
            color = "green"

        if number != "":
            number_info = f" [/dim][{number}]"
        else:
            number_info = ""

        if new != "":
            hex = f"=> {new.hex(' ').upper()}"
        else:
            hex = ""

        print(
            "    " * (used_depth + 1)
            + f"[{color}][dim]>[/dim] {original} [dim]"
            + (" " * space)
            + hex  # Uppercase :D
            + number_info
        )

    def add_byte(value: bytearray | int, original="", number=""):
        # Annoyingly enough, we need to
        # delocalize byte_set because it's
        # apparently out of scope
        nonlocal byte_set

        if isinstance(value, int):
            # Convert number to bytes
            byte = value.to_bytes()
        else:
            # Already bytes, proceed
            byte = value

        byte_set += byte

        # If there's no original
        # readable form of the
        # instruction, log the
        # byte's number instead
        if original == "":
            original = value

        # Number is the original number before "endian-ness"
        log_conversion(original, byte, number)

    # Loop through each instruction
    # in the set
    for pos, i in enumerate(instruction_set):
        if i in conversions:
            # Scope is handled entirely by the compiler;
            # not touched by the interpreter at all
            if i == "SAVE":  # Save instructions are not pushed
                # Save the current symbol
                # list for later
                scopes.append([symbols.copy()])
                log_conversion(i, "", "")
                continue
            if i == "RESTORE":  # Restore instructions are not pushed
                # Retrieve the last saved
                # symbol list and apply it
                scope = scopes.pop()
                symbols = scope[0]
                log_conversion(i, "", "")
                continue
            # There's a direct conversion from
            # instruction to byte; take it
            add_byte(conversions[i], i)
        elif common.is_number(i):
            # Number; convert to endian form
            add_byte(common.endian(i), i, i)
        else:
            # First-character basis tokens
            first = i[0]  # First character
            if first == '"':
                # String literal
                content = i[1:-1]
                encoded = content.encode("utf-8")
                add_byte(encoded, i)
            elif first == ":":
                # Destination marker
                i = i[1:]  # remove :

                current_index = len(byte_set)

                if i not in destination_indexes:

                    # Prevent overwriting old positions
                    destination_indexes[i] = current_index

                log_conversion(
                    f":{i}",
                    common.endian(destination_indexes[i]),
                    destination_indexes[i],
                    # Destinations are saved in the compiler's
                    # memory, but not passed in the actual
                    # bytecode
                )
            elif first == "^":
                # Jump marker; only applicable
                # on the second pass
                i = i[1:]
                if i in destination_indexes:
                    add_byte(conversions["NUMBER"], "NUMBER")
                    add_byte(
                        common.endian(destination_indexes[i]),
                        f"^{i}",
                        destination_indexes[i],
                    )
                    add_byte(conversions["JUMP"], "JUMP")
                else:
                    # Add some "fake" bytes
                    # to keep the indexes the same

                    if pass_count > 1:
                        # The program cannot jump to the destination
                        # as it does not exist.
                        #
                        # Only applies to the
                        # second pass; destinations are not expected
                        # to exist in the first pass
                        common.log("error", "Missing destination; could not jump.")

                    add_byte(b"x")  # NUMBER
                    add_byte(b"xxxx")  # Endian
                    add_byte(b"x")  # JUMP
            elif first == "$":
                # Index push marker; only applicable
                # on the second pass

                # Pushes the index of a destination
                # marker, whether it be in the future
                # or in the past.
                i = i[1:]
                if i in destination_indexes:
                    add_byte(conversions["NUMBER"], "NUMBER")
                    add_byte(
                        common.endian(destination_indexes[i]),
                        f"${i}",
                        destination_indexes[i],
                    )
                else:
                    # Add some "fake" bytes
                    # to keep the indexes the same

                    if pass_count > 1:
                        common.log(
                            "error", "Missing destination; index could not be deduced."
                        )

                    add_byte(b"x")  # NUMBER
                    add_byte(b"xxxx")  # Endian
            elif first == "*":
                # Symbol; replace with ID
                i = i[1:]
                if i not in symbols:
                    symbols[i] = len(symbols)

                    next_token = instruction_set[pos + 1]

                    # Since the first pass doesn't print out
                    # the conversions, it will only error after
                    # that so that the place where the error
                    # occured is clear
                    if next_token == "FETCH" and pass_count > 1:
                        # The symbol didn't exist before now,
                        # so it can't be fetched, because it
                        # doesn't exist at all.
                        common.log(
                            "Error", f'Attempted to access nonexistent symbol "{i}"'
                        )
                id = symbols[i]

                add_byte(common.endian(id), f"*{i}", id)
            else:
                # Unhandled instruction; warn
                if verbose == True:
                    common.log("Warning", f'Unhandled instruction [red]"{i}"')

    if nest_depth != 0:
        common.log(
            "Warning",
            f"Possible save-restore instruction mismatch [yellow](by {abs(nest_depth)})",
        )

    # Add START_FILE and END_FILE markers
    for _ in range(ROW_REQUIRED):
        byte_set.insert(0, conversions["START_FILE"])
        byte_set.append(conversions["END_FILE"])

    return byte_set


def process(instruction_set: list) -> list:
    global pass_count
    pass_count = 0
    destination_indexes.clear()

    get_byte_set(instruction_set, False)  # First pass, lock in destination indexes
    byte_set = get_byte_set(instruction_set, True)  # Second pass

    return byte_set
