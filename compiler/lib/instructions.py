from lark import Transformer

from lib.set.math import Math
from lib.set.symbols import Symbols
from lib.set.control import Control
from lib.set.nest import Nest
from lib.set.core import Core


# Set handler; creates instruction sets
# that will be translated into bytecode
class SetHandler(Transformer, Nest, Math, Symbols, Control, Core):
    def __init__(self):
        # The FINAL instruction set;
        # collectors extend to this
        # list
        self.instruction_set = []

        # Lists the IDs of functions,
        # allows for nested functions
        self.functions = {}
        self.nested_functions = []
        # Acts the same as nested
        # instruction sets, but with
        # functions

        self.nested_level = 0
        self.nested_sets = []  # Nested instruction sets

        self.unique_step = 0  # Every time this number is accessed,
        # it goes up, to prevent reuse

        self.saved_steps = []

    # Adds an instruction to a collector
    # or an instruction set, depending on
    # the nested state of the program
    #
    # Collector: A nested instruction set
    # that collects instructions, before
    # dumping its contents to the previous
    # set, all the way to the final, used
    # instruction set.
    def add_instruction(self, instruction: list | str):
        # The list to add to. It could be a
        # collector, or it could just stay
        # as is.
        target_list = self.instruction_set
        if self.nested_level > 0:
            while len(self.nested_sets) < self.nested_level:
                self.nested_sets.append([])
            target_list = self.nested_sets[self.nested_level - 1]

        if isinstance(instruction, list):
            for i in instruction:
                # Instruction could be a
                # number, so stringify
                i = str(i)
                target_list.append(i)
        else:
            # Same here, could be a number,
            # so stringify
            instruction = str(instruction)
            target_list.append(instruction)

    # Assigns an ID to a function via name,
    # if it's not assigned already. (Scope
    # sensitive)
    def function_id(self, name):
        target_list = self.functions

        # Check nested level
        if self.nested_level > 0:
            # The program is nested; this function is a local function
            while len(self.nested_functions) < self.nested_level:
                self.nested_functions.append({})

            # This essentially isolated the function from those in
            # other scopes. If there's another function with the
            # same name, they'll be treated as different entities
            # altogether and have different IDs.
            target_list = self.nested_functions[self.nested_level - 1]
            # In other words: isolates scopes

        if name not in target_list:
            # Assign ID; no two functions have the same
            # IDs.
            target_list[name] = self.get_step()
        return target_list[name]

    # Retrieve a unique step (never used twice!)
    def get_step(self):
        step = self.unique_step
        self.unique_step += 1
        return step
