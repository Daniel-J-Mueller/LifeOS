# AGENT Guidelines

These guidelines apply to all contributions in this repository.

* Always consult `PLAN.md` to determine the next task. The task closest to the top without a check mark is considered **next up**.
* When a task from `PLAN.md` is completed, add one or multiple new tasks in reasonable locations to keep the plan moving forward.
* Keep `PLAN.md` as a bulleted list with GitHub style checkboxes.
* Add documentation or tests whenever applicable. If any automated tests exist, run them before committing.
* Once the work is done validating the system, and you are ready to submit a PR, delete the completed lines from the PLAN.md.
* If the user proposes an idea which is not present within the PLAN.md, add it, such that progress can continue over time.
* All systems present within the OS should be laid out in detail within the Documentation whenever changes are made. Add new dirs for crucial components or systems, and nest dirs where necessary to capture sub-components or sub-systems. At the base level, documentation should be in txt format with a descriptive filename. Each file should describe the relevant process or system. At levels above the deepest levels in the dirs within /Documentation, there should be a .txt file which describes the overall activities of all the documented components or systems below it.
* Ensure that LifeOS is safe to emulate on a typical system, such that storage is not corrupted beyond what is allocated to the OS, RAM is not exhausted such that the server remains stable, etc.