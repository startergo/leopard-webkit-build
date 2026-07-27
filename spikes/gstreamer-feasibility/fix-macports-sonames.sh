#!/bin/bash
# Loops calling gst-inspect on libgstlibav.dylib, parsing each "Library not loaded"
# error, and creating a symlink from the missing soname to the newest available
# version of the same library. Repeats until gst-inspect loads the plugin clean.
#
# Run on the macmini (10.6 host). Requires sudo for /opt/local/lib writes.

set -uo pipefail

PLUGIN=/opt/local/lib/gstreamer-1.0/libgstlibav.dylib
MAX_ITERS=20

i=0
while [ $i -lt $MAX_ITERS ]; do
    i=$((i+1))
    # Run gst-inspect on the plugin; capture stderr.
    err_output=$(GST_DEBUG=4 /opt/local/bin/gst-inspect-1.0 "$PLUGIN" 2>&1)

    # If the plugin loaded (no "module_open failed" / no "Library not loaded"),
    # we're done.
    if ! echo "$err_output" | grep -q "module_open failed"; then
        echo "[OK] Plugin loaded cleanly after $((i-1)) symlink fix(es)."
        /opt/local/bin/gst-inspect-1.0 "$PLUGIN" 2>&1 | head -5
        exit 0
    fi

    # Extract the missing library path.
    missing=$(echo "$err_output" | grep "Library not loaded" | head -1 | sed -E 's/.*Library not loaded: ([^ ]+).*/\1/')
    if [ -z "$missing" ]; then
        echo "[ERROR] Could not parse missing library from gst-inspect output:"
        echo "$err_output" | tail -5
        exit 1
    fi

    # Find an alternate (any libNAME.N.dylib in /opt/local/lib matching the stem).
    base=$(basename "$missing")
    stem=$(echo "$base" | sed -E 's/\.[0-9.]+\.dylib$//')
    alt=$(ls /opt/local/lib/${stem}.*.dylib 2>/dev/null | grep -v "^$missing\$" | sort | tail -1)

    if [ -z "$alt" ]; then
        echo "[ERROR] No alternate found for $missing (stem=$stem)."
        echo "Install the missing library via MacPorts and re-run."
        exit 1
    fi

    echo "[$i] Symlinking $missing -> $alt"
    sudo ln -sf "$alt" "$missing" || { echo "[ERROR] sudo ln failed"; exit 1; }
done

echo "[ERROR] Exceeded $MAX_ITERS iterations. Plugin still not loading."
exit 1
