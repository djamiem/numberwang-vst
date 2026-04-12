#!/usr/bin/env bash
# build.sh — auto-bumps patch version, updates index.html, then builds.
# Usage: ./build.sh [--minor] [--major] ["Optional one-line change note"]
# Default is patch bump. Claude intervenes only for --minor or --major.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CMAKE="$SCRIPT_DIR/CMakeLists.txt"
HTML="$SCRIPT_DIR/Source/UI/index.html"
NOTES="$SCRIPT_DIR/NOTES.md"
BUILD="$SCRIPT_DIR/build"

# ── Read current version ───────────────────────────────────────────────────────
CURRENT=$(grep -m1 'project(Numberwang VERSION' "$CMAKE" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
MAJOR=$(echo "$CURRENT" | cut -d. -f1)
MINOR=$(echo "$CURRENT" | cut -d. -f2)
PATCH=$(echo "$CURRENT" | cut -d. -f3)

# ── Determine bump type ────────────────────────────────────────────────────────
BUMP="patch"
NOTE="${1:-}"
if [[ "${1:-}" == "--major" ]]; then BUMP="major"; NOTE="${2:-}"; fi
if [[ "${1:-}" == "--minor" ]]; then BUMP="minor"; NOTE="${2:-}"; fi
if [[ "${1:-}" == "--patch" ]]; then BUMP="patch"; NOTE="${2:-}"; fi

case "$BUMP" in
  major) MAJOR=$((MAJOR+1)); MINOR=0; PATCH=0 ;;
  minor) MINOR=$((MINOR+1)); PATCH=0 ;;
  patch) PATCH=$((PATCH+1)) ;;
esac

NEW="$MAJOR.$MINOR.$PATCH"
DATE=$(date +%Y-%m-%d)

echo "→ Bumping $CURRENT → $NEW ($BUMP)"

# ── Update CMakeLists.txt ──────────────────────────────────────────────────────
sed -i '' "s/project(Numberwang VERSION $CURRENT)/project(Numberwang VERSION $NEW)/" "$CMAKE"

# ── Update version in index.html ──────────────────────────────────────────────
sed -i '' "s/v$CURRENT/v$NEW/g" "$HTML"

# ── Append to NOTES.md ────────────────────────────────────────────────────────
NOTE_TEXT="${NOTE:-Auto build bump}"
printf '\n## %s — %s (v%s)\n\n%s\n' \
  "$DATE" "$NOTE_TEXT" "$NEW" "$NOTE_TEXT" >> "$NOTES"

# ── Build ─────────────────────────────────────────────────────────────────────
JOBS=$(sysctl -n hw.logicalcpu 2>/dev/null || nproc 2>/dev/null || echo 4)
cd "$BUILD"
BUILD_START=$(date +%s)
cmake --build . --config Release -- -j"$JOBS"
BUILD_END=$(date +%s)
BUILD_SECS=$((BUILD_END - BUILD_START))

echo ""
echo "✓ Built Numberwang Pro v$NEW  (${BUILD_SECS}s, ${JOBS} jobs)"
