/*
 * Minimal, NAppGUI-independent helper functions used by officesdk.cpp.
 * Each function reimplements just enough of NAppGUI's behavior (calling the
 * pertinent OS APIs directly) to remove officesdk's dependency on the
 * vendored NAppGUI sewer/osbs/core libraries.
 */

#include <cstdint>

uint64_t btime_to_micro(const int16_t year, const uint8_t month, const uint8_t mday);

const char *blib_getenv(const char *name);

int blib_setenv(const char *name, const char *value);
