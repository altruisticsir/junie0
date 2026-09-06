# File management module

from lib import common
from pathlib import Path
from lib.export import ROW_REQUIRED


# Reads from a file via path,
# if it exists.
# If it does not exist, then the
# program stops.
def read_file(path: str, mode="r") -> str:
    # Get path instance
    file_path: Path = Path(path)

    # Check if the file exists
    # at that path
    if file_path.is_file():
        # It does, so read from the
        # file and return its contents
        with open(path, mode) as file:
            content = file.read()

        common.log("File", f'Successfully read from "{path}"')
        return content
    else:
        # It does not, so pass an error
        common.log("Error", f'Could not read from file "{path}"')


# Writes to a specified file via path.
# If offset is provided, the total byte count
# that is announced will be subtracted by whatever
# amount is provided.
def write_file(path: str, content: str, mode="w", offset=0):
    try:
        with open(path, mode) as file:
            file.write(content)
        if mode == "wb":
            common.log(
                "File",
                f'Successfully wrote [green][{len(content) - offset} bytes][/green] to "{path}"',
            )
        else:
            common.log(
                "File",
                f'Successfully wrote to "{path}"',
            )
    except:
        common.log(
            "Error",
            f'Could not write to file "{path}"',
        )
