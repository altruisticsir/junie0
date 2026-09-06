from lark import Lark
from rich import print
from lib import file, common, instructions, bytes, export, communication
import threading

# Junie source code ===> Bytecode


def process(input_path, output_path):
    # Parser via ruleset
    parser = Lark.open("assets/ruleset", parser="lalr", lexer="contextual")

    # Read from the input_path to get
    # source
    source = file.read_file(input_path)

    # Obtain the tree
    tree = parser.parse(source)
    print(tree)

    # Prepare the instruction set
    set_handler = instructions.SetHandler()
    set_handler.transform(tree)

    # Retrieve the instruction set and log
    instruction_set = set_handler.instruction_set
    joined = " ".join(instruction_set)
    common.log("Instruction Set", f"[green]{joined}")

    bytecode = bytes.process(instruction_set)

    # serial_thread = threading.Thread(target=communication.write, args=(bytecode,))
    # serial_thread.start()

    # The row bytes (start and end) are not saved to the program when read,
    # so assign the offset to the number of those omitted bytes
    file.write_file(output_path, bytecode, "wb", offset=export.ROW_REQUIRED * 2)

    export.export()

    common.log("Junie", "All done.")
    communication.write(bytecode)
    # serial_thread.join()


def main():
    common.log("Junie", "Welcome to the Junie compiler!")
    process("main.jn", "bin/jn.o")


main()
