import shutil
import tarfile
import tempfile
from pathlib import Path
from typing import List, Optional

from submitscript.data.assignment import Assignment
from submitscript.data.config import SkeletonVariant
from submitscript.data.data_directory import DataDirectory
from submitscript.execute_subprocess import execute_subprocess
from submitscript.util.prompt import prompt, YesNoParser, SelectionParser


def check_for_new_results(data_directory: DataDirectory):
    # Check every submission for new results
    for assignment in data_directory.get_assignments():
        for submission in assignment.get_submissions():
            if not submission.is_accepted():
                continue

            if not submission.is_evaluated():
                if submission.check_for_evaluation_results():
                    submission.print_evaluation_results()
                elif submission.submission_response.get().immediate_evaluation:
                    if prompt("Do you want to subscribe to the results of submission %s?" % submission.submission_data.get().submission_id, YesNoParser(True)):
                        submission.wait_for_evaluation_results()


def select_assignment(options: List[Assignment]) -> Optional[Assignment]:
    if len(options) == 0:
        return None

    if len(options) == 1:
        return options[0]

    return prompt("Please choose the assignment you want to submit.", SelectionParser([(a.assignment_id, a) for a in options], None))


def select_variant(root_directory: Path, data: DataDirectory) -> Optional[SkeletonVariant]:
    found_options = [v for v in data.config.get().skeleton_variants if (root_directory / v.root).is_dir()]

    if len(found_options) == 0:
        print("No skeleton variant found! Did you delete all of them?")
        return None

    if len(found_options) == 1:
        return found_options[0]

    return prompt("Please choose the variant you want to submit.", SelectionParser([(a.name, a) for a in found_options], None))


def build_solution(directory: Path, selected_variant: SkeletonVariant) -> bool:
    print("=== Building submission ===")
    print("Running '%s'." % selected_variant.build)

    build_result = execute_subprocess([selected_variant.build], cwd=directory, is_shell_command=True)

    if not build_result.is_success():
        print("Building your solution failed! Aborting submission.")
        if prompt("Do you want to view the build log?", YesNoParser(True)):
            print("=== BEGIN Build Log ===")
            print(build_result.stdout, end='')
            print("=== END Build Log ===")
        print("Aborting submission")
        return False

    print("=== Building submission finished ===")
    return True


def clean_directory(directory: Path, selected_variant: SkeletonVariant):
    def clean_helper(path: Path):
        if path.is_file():
            str(path.relative_to(directory))

            if not selected_variant.is_filename_allowed(str(path.relative_to(directory))):
                path.unlink()
        elif path.is_dir():
            for child in path.iterdir():
                clean_helper(child)
            if not any(path.iterdir()):
                path.rmdir()

    print("=== Cleaning directory for submission ===")
    clean_helper(directory)
    print("=== Cleaning directory finished ===")


def submit(root_directory: Path, data: DataDirectory) -> bool:
    print("=== Submitting ===")

    submission = None
    try:
        selected_assignment = select_assignment(data.get_assignments())
        print()
        if selected_assignment is None:
            return False

        selected_variant = select_variant(root_directory, data)
        print()

        if selected_variant is None:
            return False

        print("Selected variant '%s' for assignment '%s'." % (selected_variant.name, selected_assignment.assignment_id))

        submission = selected_assignment.create_submission()

        submission.team.set(data.team.get())
        submission.variant.set(selected_variant.variant_data)

        # Create temporary directory to build submission.
        with tempfile.TemporaryDirectory() as tmpdir:
            tmp_solution_root = Path(tmpdir) / selected_variant.root
            shutil.copytree(root_directory / selected_variant.root, tmp_solution_root)

            if not build_solution(tmp_solution_root, selected_variant):
                return False

            clean_directory(tmp_solution_root, selected_variant)

            with tempfile.TemporaryFile() as temp_file:
                with tarfile.open(fileobj=temp_file, mode='w:gz', format=tarfile.GNU_FORMAT) as tar:
                    tar.add(tmp_solution_root, recursive=True, arcname="/%s" % selected_variant.root)

                submission.set_upload_file(temp_file)

        submission.submit()
    except KeyboardInterrupt:
        print("=== Aborted Submitting ===")
        if submission is not None:
            submission.delete()
        return False
