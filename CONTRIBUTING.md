# Contributing

If you have an idea or feature request, please open an [issue][1] even if you do
not have time to contribute!

## Making Changes

> [!IMPORTANT]
> This guide assumes you have a functioning C++20 development environment.
> See [Getting Started][2] for details.

To get started, [fork][3] this repository on GitHub and clone a working copy for
development:

```shell
git clone git@github.com:YOUR-USERNAME/slang-format.git
```

To build using the default CMake generator, issue:

```shell
cmake -B build && cmake --build build
```

> [!NOTE]
> Once built, binaries can be found in the `build` directory.

To run tests, issue:

```shell
ctest --output-on-failure --test-dir build
```

Finally, commit your changes and open a [pull request][4] against the default
branch for review. Ensure there are no test regressions, and add tests for any
new functionality.

## Making Releases

Making releases is automated by [GitHub Actions][5]. Releases are created from
the default branch; as such, tests should be passing at all times.

To make a release, perform the following:

1. Create a new section in the [Changelog][6] for the release, and move items
   from Unreleased to this section. Links should be updated to point to the
   correct tags for comparison.

2. To commit outstanding changes, issue:

   ```shell
   git commit -a -m "Release v<version>"
   ```

3. Push changes to the remote repository and verify the results of the [CI][7]
   workflow:

   ```shell
   git push origin <default-branch>
   ```

4. To create a release tag, issue:

   ```shell
   git tag -a -m "Release v<version>" v<version>
   ```

5. Push the release tag to the remote repository and verify the results of the
   [Release][8] workflow:

   ```shell
   git push origin --tags
   ```

## License

By contributing to this repository, you agree that your contributions will be
licensed under its MIT License.

[1]: https://github.com/sstallion/slang-format/issues
[2]: https://github.com/sstallion/slang-format/blob/main/README.md#getting-started
[3]: https://docs.github.com/en/github/getting-started-with-github/fork-a-repo
[4]: https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/creating-a-pull-request
[5]: https://docs.github.com/en/actions
[6]: https://github.com/sstallion/slang-format/blob/main/CHANGELOG.md
[7]: https://github.com/sstallion/slang-format/actions/workflows/ci.yaml
[8]: https://github.com/sstallion/slang-format/actions/workflows/release.yaml
[9]: https://llvm.org/docs/AIToolPolicy.html
