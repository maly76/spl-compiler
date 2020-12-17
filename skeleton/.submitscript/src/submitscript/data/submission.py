from pathlib import Path
from typing.io import IO

from submitscript import data
from submitscript.api.api import SubmissionRoutes
from submitscript.api.types import ApiSerializableSubmissionResponse, ApiSerializableSubmission
from submitscript.data.team import Team
from submitscript.util.prompt import prompt, YesNoParser
from submitscript.util.properties import TextFileProperty, JsonProperty


class Submission:
    def __init__(self, parent: 'data.Assignment', path: Path):
        self.parent = parent
        self.path = path

        # Paths for files used when uploading the submission
        self.team: JsonProperty[Team] = TextFileProperty(self.path / "team.json").json_serialized(Team)
        self.variant = TextFileProperty(path / "variant.json").json_serialized()
        self.upload_tgz_path = path / "upload.tgz"

        # Paths for files that relate to the submission response or evaluation result
        self.submission_data = TextFileProperty(path / "submission.json").json_serialized(ApiSerializableSubmission).cached()
        self.submission_response = TextFileProperty(path / "submission_response.json").json_serialized(ApiSerializableSubmissionResponse).cached()
        self.evaluation_log = TextFileProperty(path / "evaluation.log")
        self.submission_log = TextFileProperty(path / "submit.log")

    def delete(self):
        import shutil
        shutil.rmtree(self.path)

    def set_upload_file(self, file: IO[bytes]) -> None:
        file.seek(0)

        import shutil
        shutil.copyfileobj(file, self.upload_tgz_path.open("wb"))

    def is_accepted(self) -> bool:
        return self.submission_response.has_value() and self.submission_response.get().accepted

    def is_evaluated(self) -> bool:
        return self.is_accepted() and self.submission_data.get().evaluation_result is not None

    def rename(self, name: str):
        self.__init__(self.parent, self.path.rename(self.path.with_name(name)))

    def rename_with_prefix(self, prefix: str):
        self.rename("%s_%s" % (prefix, self.path.name))

    def submit(self) -> bool:
        print("=== Starting Upload ===")
        self.submission_response.set(self.parent.get_backend().submissions.post(self.upload_tgz_path, self.variant.base_property.get(), self.team.base_property.get()))
        self.submission_data.set(self.submission_response.get().submission)
        self.submission_log.set(self.submission_response.get().log)

        print("=== Upload finished ===")
        print("=== BEGIN Upload Log")
        print(self.submission_response.get().log)
        print("=== END Upload Log")

        if not self.submission_response.get().accepted:
            print("Submitting your solution failed. Please see the upload log above for more details.")
            self.rename_with_prefix("REJECTED")
            return False
        else:
            print("Successfully submitted your solution. Please see the upload log above for more details.")
            self.rename(self.submission_response.get().submission.submission_id)
            self.wait_for_evaluation_results()
            return True

    def prompt_evaluation_wait(self):
        if not self.submission_response.has_value() or not self.submission_response.get().immediate_evaluation:
            return

        if prompt("This submission will be evaluated immediately. Do you want to wait for results now?", YesNoParser(True)):
            self.wait_for_evaluation_results()

    def check_for_evaluation_results(self) -> bool:
        if self.get_backend().is_evaluated():
            self.submission_data.set(self.get_backend().get(True))
            self.evaluation_log.set(self.submission_data.get().evaluation_result.log)
            print("\n=== Evaluation results for submission '%s' retrieved. ===" % self.submission_data.get().submission_id)

            return True

    def print_evaluation_results(self):
        score_percentage = \
            100 * self.submission_data.get().evaluation_result.score / self.submission_data.get().evaluation_result.max_score \
                if self.submission_data.get().evaluation_result.max_score != 0 \
                else 0

        print("You achieved the score %d/%d (%d%%)." % (self.submission_data.get().evaluation_result.score,
                                                        self.submission_data.get().evaluation_result.max_score,
                                                        score_percentage))

        if self.submission_data.get().evaluation_result.comment is not None:
            print("The following comment was left by your teacher:")
            print(self.submission_data.get().evaluation_result.comment)

        if self.submission_data.get().evaluation_result.passed:
            print("You have PASSED this assignment.")
        else:
            print("You have NOT PASSED this assignment.")

        if self.submission_data.get().evaluation_result.log is not None:
            if prompt("Do you want to view the evaluation log now?", YesNoParser(True)):
                print("=== BEGIN Evaluation Log")
                print(self.submission_data.get().evaluation_result.log)
                print("=== END Evaluation Log")

        print("NOTE: This evaluation log is also available at '%s'" % self.evaluation_log.path.absolute())

    def wait_for_evaluation_results(self) -> bool:
        print("Polling for results (Press CTRL + C to abort).", end="")

        try:
            import time
            while not self.check_for_evaluation_results():
                print(".", end="", flush=True)
                time.sleep(1)

            self.print_evaluation_results()
        except KeyboardInterrupt:
            print("\nResult polling aborted.")

        return True

    def get_backend(self) -> SubmissionRoutes:
        return self.parent.get_backend().submissions[self.submission_data.get().submission_id]
