# Control structures; selection and iteration
class Control:
    # For loop
    #                   variable    start     mode       end
    # Pushed as :for>x NUMBER *i NUMBER 0 NUMBER 0 NUMBER 10 FOR ... ^for>x
    # Describes "for i = 0 to 10 { ... }"
    def for_lp(self, args):
        step = args[0][0]  # Unified ID
        exit_step = args[0][1]
        name = args[0][2]

        # Leave loop block state
        self.leave_nest()

        # Dump the body
        self.dump_nest()

        # Go back to the start
        # TICK instruction tells the for loop to increment its variable
        self.add_instruction(
            [
                "NUMBER",
                f"*{name}",
                "TICK",
                f"^for>{step}",
                f":for>{exit_step}",
                "RESTORE",
            ]
        )

    def for_start(self, args):
        # The loop runs in a contained while loop on
        # the interpreter; checks the value of the
        # loop variable and breaks the loop once its
        # value exceeds or meets the end/exit value

        # Loops can use the step system; loops
        # do not need to be called on the fly.
        step = self.get_step()
        exit_step = self.get_step()
        name = args[0]

        # Save the step to be used later
        self.saved_steps.append(step)

        # Push the index for the end of the loop
        # onto the stack so that the loop can be
        # exited(Index marker)
        self.add_instruction(f"$for>{exit_step}")

        # Add a destination for the start of
        # the for loop
        self.add_instruction("SAVE")

        # Loop variable instructions
        self.add_instruction(["NUMBER", f"*{name}"])

        return step, exit_step, name

    # To or with?
    def for_mode(self, args):
        # To: Exclusive
        # With: Inclusive
        mode_lookup = {"to": 0, "with": 1}
        mode = args[0].value
        self.add_instruction(["NUMBER", mode_lookup[mode]])

        # Enter block
        self.enter_nest()

    # The start of the block of a for loop
    def for_block_start(self, _):
        # Add a for so that the compiler
        # knows to look for the start,
        # mode, and end values
        self.add_instruction("FOR")

        # Pop the last step to use for the *start* of the loop
        self.add_instruction(f":for>{self.saved_steps.pop()}")

    def while_lp(self, args):
        step = args[0][0]
        exit_step = args[0][1]

        self.leave_nest()

        self.dump_nest()

        # Add instruction to loop back to the beginning
        self.add_instruction([f"^while>{step}", f":while>{exit_step}", "RESTORE"])

    def while_start(self, _):
        # Get the step values
        step = self.get_step()
        exit_step = self.get_step()

        # Add these to saved_steps to be
        # used later
        # self.saved_steps.append(step) unused
        self.saved_steps.append(exit_step)

        # Save scope
        self.add_instruction("SAVE")

        # Add step.
        # step is used to restart the loop,
        # think of it as the start, if you will.
        self.add_instruction(f":while>{step}")

        return step, exit_step

    def while_block_start(self, _):
        exit_step = self.saved_steps.pop()

        # Add exit_step.
        # exit_step is used when the while loop's condition
        # is satisfied; placed after the loop.
        self.add_instruction([f"$while>{exit_step}"])

        # Add an instruction to let the
        # interpreter know when the pop
        # the condition values
        self.add_instruction("WHILE")

        self.enter_nest()

    def if_stmt(self, args):
        step = args[1]  # Get steo

        # Leave + dump the nest
        self.leave_nest()
        self.dump_nest()

        # Add restore and the end destination
        self.add_instruction(["RESTORE", f":if_{step}"])

    def if_block_start(self, _):
        # Get the if statement's unique step
        step = self.get_step()

        # Add an index marker, if the condition is false
        # then the program jumps to that index (end).
        self.add_instruction([f"$if_{step}", "IF", "SAVE"])
        self.enter_nest()

        # Return the step so that the parent
        # if_stmt node can access the if statement's
        # unique step
        return step
