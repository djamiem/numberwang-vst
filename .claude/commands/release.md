# /release

Ship a release of Numberwang Pro VST. Performs all steps in order:

1. Reconfigure cmake with `NUMBERWANG_DEV_UI=OFF` (undo dev mode if active)
2. Run `./build.sh` to bump the patch version, rebuild the VST3 and Standalone with BinaryData
3. Read the new version number from `CMakeLists.txt`
4. Stage and commit all changed source files (never commit `build/` or `JUCE/`)
5. Create a git tag `v<version>`
6. Zip the built VST3 bundle using `ditto` into `/tmp/Numberwang-Pro-v<version>-macOS.vst3.zip`
7. Push the commit and tag to `origin main`
8. Create a GitHub release with `gh release create` using the tag, attach the zip, and write release notes summarising what changed (pull from `NOTES.md` since the last tag)

Print the release URL when done.

If the user passes a flag like `--minor` or `--major`, forward it to `./build.sh` so the version bump is correct.

If anything fails, stop and report the error clearly — do not force-push or delete tags to recover.
