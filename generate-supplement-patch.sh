#!/bin/bash
# generate-supplement-patch.sh
# Creates downloads/patches-604/supplement.diff from the current WebKit source state.
#
# Run from the project root:
#   ./generate-supplement-patch.sh
#
# It cd's into source/webkit, runs git diff for the uncovered files,
# and writes the result to downloads/patches-604/supplement.diff.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
WK_DIR="$SCRIPT_DIR/source/webkit"
OUT="$SCRIPT_DIR/downloads/patches-604/supplement.diff"

FILES=(
    Source/JavaScriptCore/API/tests/Regress141275.mm
    Source/JavaScriptCore/PlatformMac.cmake
    Source/ThirdParty/woff2/src/woff2_dec.h
    Source/ThirdParty/woff2/src/woff2_enc.h
    Source/ThirdParty/woff2/src/woff2_out.h
    Source/WTF/wtf/Optional.h
    Source/WTF/wtf/spi/darwin/SandboxSPI.h
    Source/WebCore/Modules/indexeddb/IDBFactory.cpp
    Source/WebCore/Modules/indexeddb/IDBTransaction.h
    Source/WebCore/Modules/indexeddb/WorkerGlobalScopeIndexedDatabase.cpp
    Source/WebCore/Modules/indexeddb/WorkerGlobalScopeIndexedDatabase.h
    Source/WebCore/Modules/indexeddb/WorkerGlobalScopeIndexedDatabase.idl
    Source/WebCore/Modules/indexeddb/server/IDBServer.cpp
    Source/WebCore/Modules/indexeddb/shared/IDBResourceIdentifier.cpp
    Source/WebCore/Modules/indexeddb/shared/InProcessIDBServer.h
    Source/WebCore/PlatformMac.cmake
    Source/WebCore/css/StyleResolver.cpp
    Source/WebCore/inspector/InspectorMemoryAgent.cpp
    Source/WebCore/inspector/InspectorTimelineAgent.cpp
    Source/WebCore/page/scrolling/mac/ScrollingTreeMac.cpp
    Source/WebCore/platform/spi/cocoa/NEFilterSourceSPI.h
    Source/WebCore/platform/sql/SQLiteStatement.cpp
    Source/WebCore/rendering/RenderText.cpp
    Source/cmake/OptionsMac.cmake
    Source/WebKitLegacy/mac/Plugins/Hosted/ProxyInstance.mm
    Source/WebKitLegacy/mac/Storage/WebDatabaseManager.mm
)

cd "$WK_DIR"

echo "Generating supplement.diff for ${#FILES[@]} files from $WK_DIR ..."

rm -f "$OUT"
for f in "${FILES[@]}"; do
    git diff -- "$f" >> "$OUT" 2>/dev/null || true
done

if [ -s "$OUT" ]; then
    LINES=$(wc -l < "$OUT")
    echo "OK: $OUT ($LINES lines)"
else
    echo "WARNING: patch file is empty — are you in a clean checkout?"
    exit 1
fi
