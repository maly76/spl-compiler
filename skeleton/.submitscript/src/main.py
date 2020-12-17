from pathlib import Path

import requests

from submitscript.data.data_directory import DataDirectory
from submitscript.submit import check_for_new_results, submit
from submitscript.team import ensure_team_existence
from submitscript.update import update
from submitscript.util.prompt import YesNoParser, prompt


def main():
    root_directory = Path.cwd().parent.parent
    data_directory = DataDirectory(Path.cwd().parent / "data")

    update(data_directory)

    check_for_new_results(data_directory)
    ensure_team_existence(data_directory)

    while True:
        if prompt("Do you want to submit a solution now?", YesNoParser(True)):
            print()
            submit(root_directory, data_directory)
            print()
        else:
            break


if __name__ == '__main__':
    try:
        print("=== Welcome ===")
        main()
    except requests.exceptions.ConnectionError as e:
        print("The server can not be reached. You may need to be logged in to the VPN.")
        print("Aborting.")
        exit(1)
    except KeyboardInterrupt:
        print()  # Print a single linefeed to avoid having the new command prompt dangle in the same line as the last output line
        exit(0)
