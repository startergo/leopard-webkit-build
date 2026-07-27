// Automatically generated from /Users/macbookpro/leopard-webkit-build/sources_610/Source/JavaScriptCore/runtime/IntlDateTimeFormatPrototype.cpp using /Users/macbookpro/leopard-webkit-build/sources_610/Source/JavaScriptCore/create_hash_table. DO NOT EDIT!

#include "Lookup.h"

namespace JSC {

static const struct CompactHashIndex dateTimeFormatPrototypeTableIndex[9] = {
    { -1, -1 },
    { -1, -1 },
    { -1, -1 },
    { 2, -1 },
    { -1, -1 },
    { 0, 8 },
    { -1, -1 },
    { -1, -1 },
    { 1, -1 },
};

static const struct HashTableValue dateTimeFormatPrototypeTableValues[3] = {
   { "format", static_cast<unsigned>(PropertyAttribute::DontEnum|PropertyAttribute::Accessor), NoIntrinsic, { (intptr_t)static_cast<RawNativeFunction>(IntlDateTimeFormatPrototypeGetterFormat), (intptr_t)static_cast<RawNativeFunction>(nullptr) } },
   { "formatToParts", static_cast<unsigned>(PropertyAttribute::DontEnum|PropertyAttribute::Function), NoIntrinsic, { (intptr_t)static_cast<RawNativeFunction>(IntlDateTimeFormatPrototypeFuncFormatToParts), (intptr_t)(1) } },
   { "resolvedOptions", static_cast<unsigned>(PropertyAttribute::DontEnum|PropertyAttribute::Function), NoIntrinsic, { (intptr_t)static_cast<RawNativeFunction>(IntlDateTimeFormatPrototypeFuncResolvedOptions), (intptr_t)(0) } },
};

static const struct HashTable dateTimeFormatPrototypeTable =
    { 3, 7, true, nullptr, dateTimeFormatPrototypeTableValues, dateTimeFormatPrototypeTableIndex };

} // namespace JSC
