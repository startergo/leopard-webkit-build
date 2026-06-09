#!/bin/bash
# fix-forwarding-headers.sh
#
# Copies WebKitLegacy forwarding headers over WebKit ones,
# converting #include to #import to prevent duplicate ObjC @interface errors.
#
# This must run AFTER cmake generate but BEFORE ninja build.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
FH_DIR="$SCRIPT_DIR/build/DerivedSources/ForwardingHeaders"

if [ ! -d "$FH_DIR/WebKitLegacy" ] || [ ! -d "$FH_DIR/WebKit" ]; then
    echo "Error: ForwardingHeaders directories not found at $FH_DIR"
    echo "Run cmake configure first."
    exit 1
fi

COPIED=0
CONVERTED=0

for HEADER in "$FH_DIR/WebKitLegacy/"*.h; do
    [ -f "$HEADER" ] || continue
    BASENAME="$(basename "$HEADER")"
    DEST="$FH_DIR/WebKit/$BASENAME"

    # Copy WebKitLegacy header over the WebKit one
    cp "$HEADER" "$DEST"
    COPIED=$((COPIED + 1))

    # Convert #include to #import for ObjC compatibility
    if grep -q '#include' "$DEST" 2>/dev/null; then
        sed -i '' 's/^#include/#import/g' "$DEST"
        CONVERTED=$((CONVERTED + 1))
    fi
done

echo "Fixed forwarding headers: $COPIED copied, $CONVERTED converted (#include → #import)"
