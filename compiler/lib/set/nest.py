from lib import common


# Includes methods that handle
# nested sets and blocks.
class Nest:
    # Increases the nested level.
    # Use when entering a new block
    def enter_nest(self):
        self.nested_level += 1

    # Decreases the nested level.
    # Use when leaving a block.
    def leave_nest(self):
        self.nested_level -= 1

    # Dumps the contents of the
    # latest nested set into the
    # previous nested set if available;
    # otherwise it goes into the final
    # instruction set.
    def dump_nest(self):
        if len(self.nested_sets) > 0:
            # There is a valid nested set
            # to dump
            set = self.nested_sets.pop()
            if len(self.nested_functions) > 0:
                fn_set = self.nested_functions.pop()
            else:
                fn_set = {}
        else:
            # There is no valid nested set;
            # usually due to an empty body
            set = []

            # instructions.py already adds
            # empty dictionaries in case
            # the nested level exceeds the
            # number of sets, so this is just
            # a precaution
            fn_set = {}

        nested_set_count = len(self.nested_sets)
        if nested_set_count != self.nested_level:
            common.log(
                "Warning",
                f"Nested set count mismatched with nested level [yellow]({nested_set_count} != {self.nested_level})",
            )

        # Level is decremented by leave_nest
        # already.
        if self.nested_level <= 0:
            # No other nested sets, extend
            # directly to the final
            # instruction set.
            self.instruction_set.extend(set)
            self.functions.update(fn_set)
        else:
            if nested_set_count > self.nested_level - 1:
                # There is a previous nested set; extend
                # this set's content into that set.
                self.nested_sets[self.nested_level - 1].extend(set)

                while len(self.nested_functions) < self.nested_level:
                    self.nested_functions.append({})
                self.nested_functions[self.nested_level - 1].update(fn_set)
            else:
                # Might be out of range, just add to final.
                self.instruction_set.extend(set)
                self.functions.update(fn_set)
