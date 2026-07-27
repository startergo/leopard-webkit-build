#!/bin/bash
# Rewrites absolute /opt/local paths in MacPorts pkg-config (.pc) files to
# point at the local mirror under dist/macports-mirror. Run once after rsync.
#
# Why: MacPorts .pc files are emitted with prefix=/opt/local, which doesn't
# exist on the macbookpro build host. pkg-config then resolves include/lib
# paths to /opt/local/... and CMake's find_package(GStreamer) fails. We
# could use --define-prefix, but it's cleaner to bake the new prefix into
# the mirror so any consumer of these .pc files Just Works.
#
# Idempotent: re-running on already-patched files is a no-op (the /opt/local
# pattern is no longer present).

set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
MIRROR="$PROJECT_ROOT/dist/macports-mirror"
PC_DIR="$MIRROR/lib/pkgconfig"
NEW_PREFIX="$MIRROR"

if [ ! -d "$PC_DIR" ]; then
    echo "[ERROR] $PC_DIR not found. Run the macmini rsync first." >&2
    exit 1
fi

echo "[INFO] Rewriting /opt/local -> $NEW_PREFIX in .pc files under $PC_DIR"

# Count first
count=$(find "$PC_DIR" -name '*.pc' -type f | wc -l | tr -d ' ')
echo "[INFO] Found $count .pc files"

# Bulk rewrite. We use in-place sed; macOS sed needs -i ''.
find "$PC_DIR" -name '*.pc' -type f -print0 | while IFS= read -r -d '' pc; do
    # Only rewrite if /opt/local appears (skips already-patched files).
    if grep -q '/opt/local' "$pc"; then
        sed -i '' "s|/opt/local|${NEW_PREFIX}|g" "$pc"
    fi
done

echo "[OK] Done. Sample (gstreamer-1.0.pc):"
head -5 "$PC_DIR/gstreamer-1.0.pc" 2>/dev/null || true
