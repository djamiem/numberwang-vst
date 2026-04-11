# Stereo Widener — Claude Instructions

## Version bumping
Before every build, bump the version in `CMakeLists.txt` (`project(StereoWidener VERSION x.y.z)`):
- **Patch** (`z`): default for any build
- **Minor** (`y`, reset `z` to 0): new feature or significant behaviour change
- **Major** (`x`, reset `y` and `z` to 0): breaking/architectural change

After bumping, also append the new version and a one-line description of the change to `NOTES.md`.
