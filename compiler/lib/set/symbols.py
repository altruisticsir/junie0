class Symbols:
    # Sets a variable to any number/string
    # value.
    def set_var(self, args):
        name = args[0].value

        # The value to set the variable as is already
        # pushed; just add some instructions about the
        # variable's ID
        self.add_instruction(["NUMBER", f"*{name}", "SET"])

    # Same functionality as set_var;
    # forward "args" to that method
    def reassign_var(self, args):
        self.set_var(args)

    # Sets a function.
    def set_fn(self, args):
        # ^x>skip :x NUMBER *a NUMBER *b NUMBER 2 FUNCTION ... NUMBER 0 RETURN :x>skip

        name = args[0].value
        params = args[1] if args[1] is not None else ""
        id = self.function_id(name)

        # End the current nest/block for the function
        # (the body is already processed)
        self.leave_nest()

        # Add a skip marker; the function shouldn't run
        # until called so skip all instructions linked
        # to the method
        self.add_instruction(["RETURN_SAVE", f"^{name}_{id}>skip"])

        # Add a destination marker; if the function
        # is called the program jumps here
        self.add_instruction([f":{name}_{id}", "SAVE"])

        # Add instructions for parameter IDs(as symbols)
        for p in params:
            self.add_instruction(["NUMBER", f"*{p}"])

        # Add the number of parameters and a marker to
        # extract the arguments and start the function
        self.add_instruction(["NUMBER", len(params), "FUNCTION"])

        # Dump the body
        self.dump_nest()

        # Destination for the skip
        # Return brings the program back to where the
        # function was called.

        # Fall back nil return value, if the function has no return value
        self.add_instruction(["NIL", "RETURN", "RESTORE", f":{name}_{id}>skip"])

    def call_fn(self, args):
        name = args[0].value
        arguments = args[1] if args[1] is not None else ""
        id = self.function_id(name)

        # Provide the number of arguments there are
        # self.add_instruction(["NUMBER", len(arguments)])

        # Skip to the function being called
        self.add_instruction(["RETURN_SAVE", f"^{name}_{id}", "POP"])
        # POP is for the return value; since this
        # is a standalone call that isn't being
        # set to any variable it gets tossed

    def return_fn(self, args):
        # Same as call_fn, but the return value
        # is not popped as it is to be used
        name = args[0].value
        arguments = args[1] if args[1] is not None else ""
        id = self.function_id(name)

        # Provide the number of arguments there are
        # self.add_instruction(["NUMBER", len(arguments)])

        # Skip to the function being called
        self.add_instruction(["RETURN_SAVE", f"^{name}_{id}"])

    def parameters(self, args):
        # [a, b, c, ...]
        return args

    def arguments(self, args):
        # [a, b, c, ...]
        return args

    def return_stmt(self, _):
        self.add_instruction("RETURN")

    def block_start(self, _):
        self.enter_nest()
