from pathlib import Path
from typing import List

from submitscript import data
from submitscript.api.api import AssignmentRoutes
from submitscript.data.submission import Submission


class Assignment:
    def __init__(self, parent: 'data.DataDirectory', assignment_id: str, path: Path):
        self.parent = parent
        self.assignment_id = assignment_id
        self.path = path

        self.submissions_path = path / "submissions"

    def get_submissions(self) -> List[Submission]:
        if not self.submissions_path.exists():
            return []

        return [Submission(self, path) for path in self.submissions_path.iterdir() if path.is_dir()]

    def create_submission(self) -> Submission:
        import uuid

        path = self.submissions_path / ("temp_submission%s" % uuid.uuid4())

        path.mkdir(parents=True, exist_ok=True)

        # Create a new submission with a placeholder path that will be updated as soon as the submission is accepted or rejected
        return Submission(self, path)

    def get_backend(self) -> AssignmentRoutes:
        return self.parent.get_course_backend().assignments[self.assignment_id]
