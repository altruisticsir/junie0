from lib import common


# Literals, operators and comparison.
class Math:
    # Basis of symbols; can be anything from a
    # number/string literal to the name of a variable.
    #
    # Goal: Push the token's type first, and then its value.
    def primary(self, args):
        token = args[0]

        type = token.type  # The type e.g. INT, FLOAT, SYMBOL, STRING
        value = token.value  # The value e.g. 3, 7.54, x, "Wubba lubba dub dub!"

        if type == "SYMBOL":
            # Symbols are treated differently;
            # they become number value IDs later.

            # We push as NUMBER *x. The asterisk
            # lets the compiler know that the
            # instruction needs to be turned into
            # an ID.
            if value == "true" or value == "false":
                self.add_instruction(["NUMBER", 1 if value == "true" else 0])
            else:
                self.add_instruction(["NUMBER", f"*{value}", "FETCH"])
        else:
            # Passed type is the form of type
            # that actually gets added to the
            # set.
            passed_type = type

            # INT and FLOAT are unified into a
            # basic NUMBER
            if type == "INT" or type == "FLOAT":
                passed_type = "NUMBER"

            if type == "STRING":
                # Strings need a special instruction to mark
                # the end, because the interpreter won't know
                # how long it is.
                self.add_instruction([passed_type, value, f"{passed_type}_END"])
                # (The quotation marks are kept.)
            else:
                # Normal, just add without any extra action
                self.add_instruction([passed_type, value])
        return value

    # Since the symbols required
    # are already pushed, the
    # functions for operations
    # becomes very light
    def add(self, _):
        self.add_instruction("ADD")

    def sub(self, _):
        self.add_instruction("SUB")

    def mul(self, _):
        self.add_instruction("MUL")

    def div(self, _):
        self.add_instruction("DIV")

    def mod(self, args):
        # If the right value is zero,
        # then that is illegal
        if args[1] == "0":
            common.log("Error", "Modulus cannot be zero")
        self.add_instruction("MOD")

    def parens(self, args):
        return args

    # Turns 1s into 0s and a 0s into 1s
    def negate(self, _):
        self.add_instruction("NEGATE")

    def condition(self, args):
        if len(args) > 1:
            # There is a right part,
            # this is a comparison
            comp = args[1]
        else:
            # There is no right part,
            # so this is a check for
            # existence
            comp = "EXISTS"

        # Translate operators to
        # numbers to push
        op_lookup = {
            "==": 0,
            "!=": 1,
            "<=": 2,
            ">=": 3,
            "<": 4,
            ">": 5,
            "EXISTS": 6,
        }

        self.add_instruction(["NUMBER", op_lookup[comp], "CONDITION"])

    def assigned_condition(self, args):
        self.condition(args)

    def comp(self, args):
        return args[0]
