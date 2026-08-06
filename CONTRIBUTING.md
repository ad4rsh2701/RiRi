# Contributing to RiRi

RiRi is open to contributions and contributions are welcomed. Before contributing,
please continue reading this entire document. It lists:
- [The Acceptable Contributions](#acceptable-contributions)
- [A Note on LLM-assisted/authored Contributions](#a-note-on-llm-assistedauthored-contributions)
- [The PR/Issue Workflow](#pr-issue-workflow)
- [The Development Environment & Tests](#development-environment-and-tests)
- [The Coding Style](#coding-style)
- [The Commit Message Format](#commit-message-format)
- [The PR/issue Style/template](#prissue-styletemplate)


## Acceptable contributions

Not all contributions are accepted. For instance, the following types of contributions
are accepted:
1. Bug Fixes/Filing
2. Feature Requests
3. Documentation
4. Code Refactoring/Improvements
5. Contribution to developing features

The above list is not a strict criteria, but a guideline. However, the following
types of contributions are not accepted at all:
1. PRs/Issues that are not related to the project or are duplicate
2. Automated or Agent-generated PRs, Issues, Patches, Feature Requests, Docs, etc.
3. PRs/Issues lacking proper descriptions
4. Contributions that bring an entirely new feature.


## A note on LLM-assisted/authored contributions

While I am not against LLM-assisted contributions, LLM-authored contributions
are not accepted. LLM-authored contributions are contributions that are authored
by LLMs/Agents.

As for LLM-assisted contributions, please disclose LLM-assisted contributions
in the PR/issue description explicitly and in a clear manner. One must be able
to tell where the LLMs were used at first glance. LLM-assisted contributions
are contributions that you author, in which LLMs are used to ideate/scaffold/
reason/improve the design/code/etc.

Contributions that are entirely LLM generated/authored are not accepted.


## PR-Issue Workflow

To contribute to RiRi, please keep the following in mind:
- Every PR must be linked to an issue.
- Before starting any work, please check if there is an open issue for the same.
- Comment on the Issue to get assigned to the issue.
- Before opening a PR, one must be assigned to the relevant issue first.
- Every PR must be approved by at least one maintainer.
- Any standalone PRs will be closed immediately.

For an even more detailed idea, here are the specific cases (non-exhaustive):
1. Case: New Issue (Bug Report, Docs, Feature Request, Typo fix, etc.)
   - Open an Issue with a detailed description of the bug/feature request.
   - If you are willing to fix the bug/feature request, then let us know in
     the issue description, and we will assign you the issue (not applicable
     in case of feature requests).
2. Case: To resolve an existing unassigned issue
   - Comment on the unassigned issue and let us know that you are willing to
     work on it, and we will assign you the issue.
   - Once assigned, open a PR with a detailed description of the patch/fix while referencing
     the issue number.
   - Every PR must be linked to an issue.
3. Case: To contribute to an existing PR (for contributing to Feature development)
   - Please comment on the PR before working on it. Let the assignee know that you
     are willing to help out.
   - Only if the assignee or the maintainers approve, you can add commits to the
     PR branch.


## Development environment and Tests

To work on RiRi, ensure that you have the following:
- Clang++ Compiler, Version 20+ (recommended) or equivalent C++23 supporting GCC compiler
- CMake 3.22+
- Ninja 1.13+ (recommended) or equivalent build system

To get started:
- Clone the repository: `git clone https://github.com/ad4rsh2701/RiRi.git` or via the SSH equivalent.
- Create or switch to a new branch: `git checkout -b <branch_name>` (make sure to cd into the RiRi directory before).
- Configure RiRi with Tests: `cmake -B build -DCMAKE_CXX_COMPILER=clang++ -DRIRI_BUILD_TESTS=ON`
- Build: `cmake --build build`
- Run tests: `cd build && ctest`

Before committing changes to the remote branch, please ensure that these tests pass. If tests fail, and you are not sure
why, you may commit the changes to remote, but please mention so in the PR comments and wait for further review.

> Note: I am using JetBrains CLion IDE for the development of RiRi, you may use it if you wish.


## Coding Style

There is no fixed coding style, but please try to follow the existing style. It's a mix of Google's C++ style and
LLVM's. Here are some guidelines:

1. For Naming:
   - Private variables should be prefixed with an underscore.
   - Class/Struct/Public Variable names should be in CamelCase.
   - Private function names should be in snake_case.
   - Public function names should be in the lowerCamelCase.
   - Upper Command Layer function names/Macros should be in the SCREAMING_SNAKE_CASE.
   - Please use descriptive names (no matter how long they are!)

2. For Docstrings:
   - Leave this to us maintainers, please.
   - However, please explain things in normal comments.

3. For Comments:
   - No restrictions, just be respectful/honest (and don't spam).
   - Do not clean/touch existing comments.

4. For formatting:
   - The maximum line length for codes is 120 characters.
   - Tab space is equivalent to four spaces.
   - Please add newlines wherever necessary (minimum two between separate blocks).

These aren't fixed rules and may definitely change in the future, but these guidelines are followed
throughout the project at the moment.

> Note: I have yet to configure a clang-format style file or even a clang-tidy config file (if they do get added, please
> follow that, and I will update this section accordingly).


## Commit Message Format

For the commit message format, please follow the traditional scoping format:
```
task(scope): what to do
```

> Note: Make sure your commits are atomic. That is, a commit should only make singular changes.

Here, `task` is the type of commit:
- chore: typo-fixes, small changes, reorders, etc.
- feat: commits that add new features via class/function/etc. implementations
- fix: commits that fix bugs/errors/etc.
- docs: commits that add/update documentation.
- refactor: commits that refactor existing code (may add minor features, nothing drastic).
- test: commits touching the tests (regardless of what you did, except for chore-like commits).
- build: commits touching the build system (regardless of what you did, except for chore-like commits).

The `scope` is the file/folder/module/class/etc. that you are touching/adding/modifying/fixing/etc.

The `what to do` is the description of the commit, it should be in a single short line and must be imperative.
I.e., Instead of "Added XYZ class", it should be "Add XYZ class". Specifically, for the `fix` type of commits,
please describe the bug/error/etc. itself in the commit message (and the fix in the commit body).

For example:
- chore(README): Add the "License" section
- feat(module): Add XYZ class for validation
- fix(build): build failing on *nix systems due to ABI mismatch

> At times, you may see commits that do not follow the above format, two explanations:
> 1. The commit is a merge/squash commit, which follows its own format.
> 2. You are seeing an ancient commit made by yours truly, please ignore it.


## PR/Issue Style/Template

For PRs/Issues, please be descriptive; I enjoy a good read, so:
- keep it to the point.
- include code snippets/images wherever possible.
- and follow a logical order:
   - For Issues: The problem → Step to reproduce → Expected Behavior → Actual Behavior → Suggested Solution (if any)
   - For PRs: Context (if any) → The issue and its cause → The solution → Why this works → Improvements or known Caveats

That is the general style across both PRs and Issues. Do use headings!

For a more structured template, you can always use these: https://github.com/ad4rsh2701/RiRi/.github/ISSUE_TEMPLATE/

## 
Thank You! :light_blue_heart: