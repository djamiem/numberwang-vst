#!/usr/bin/env bash
# dev.sh — build with NUMBERWANG_DEV_UI=ON so the standalone reads HTML/CSS/JS
# from Source/UI/ on disk.  After this one-time build you can:
#   1. Edit any file in Source/UI/
#   2. Quit and relaunch the standalone (takes ~2s, no rebuild needed)
#
# When you're ready to ship, run ./build.sh as normal — that build uses the
# baked-in BinaryData and does NOT read from disk.

set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD="$SCRIPT_DIR/build"
STANDALONE="$BUILD/Numberwang_artefacts/Release/Standalone/Numberwang Pro.app"

JOBS=$(sysctl -n hw.logicalcpu 2>/dev/null || nproc 2>/dev/null || echo 4)

echo "→ Configuring with NUMBERWANG_DEV_UI=ON …"
cmake -B "$BUILD" -S "$SCRIPT_DIR" -DNUMBERWANG_DEV_UI=ON -DCMAKE_BUILD_TYPE=Release

echo "→ Building Standalone …"
BUILD_START=$(date +%s)
cmake --build "$BUILD" --config Release --target Numberwang_Standalone -- -j"$JOBS"
BUILD_END=$(date +%s)
echo "✓ Dev build done  ($((BUILD_END - BUILD_START))s)"

echo ""
echo "Workflow:"
echo "  • Edit Source/UI/*.html / *.css / *.js freely"
echo "  • Quit and relaunch the standalone to see changes (~2s)"
echo "  • Run ./build.sh when ready to bake a release VST3"
echo ""

open "$STANDALONE"
