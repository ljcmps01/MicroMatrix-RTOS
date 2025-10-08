# Branch Management & Good Practices

## Updating Your Branch with `master`

\* Note that all this can also be done through the VS Code git extension

1. **Fetch Latest Changes**
    ```sh
    git fetch origin
    ```
2. **Switch to Your Branch**
    ```sh
    git checkout your-feature-branch
    ```
3. **Merge `master` into Your Branch**
    ```sh
    git merge origin/master
    ```
    - Resolve any conflicts if prompted.
    - Commit the merge if necessary.

4. **Push Updated Branch**
    ```sh
    git push origin your-feature-branch
    ```

## Before pushing changes
- After making your changes and compiling them, check multiple times that it works on the target as intended
- Even after that, it's recommended to run the static analysis to see if there're suggestion to be applied and make sure it will pass the github action test

## Good Practices

### Creating Branches
- Use descriptive names: `feature/login-page`, `bugfix/crash-on-startup`.
- Branch from `master` (or main development branch).
- Keep branches focused on a single task or feature.

### Making Commits
- Write clear, concise commit messages.
  - Example: `Fix typo in README`
- Keep commits small (split the changes into precise commits, and avoid bulk changes in a single commit)
- Commit only related changes.
- Test your code before committing.

### General Tips
- Pull latest changes from `master` regularly. VS Code will ask you if you want to fetch changes regularly, enabling that option may be a good idea
- Rebase if your branch diverges significantly.
- Delete branches after merging to keep the repo clean.

---

**References:**
- [Git Branching Documentation](https://git-scm.com/book/en/v2/Git-Branching-Branches-in-a-Nutshell)
- [Commit Message Guidelines](https://chris.beams.io/posts/git-commit/)