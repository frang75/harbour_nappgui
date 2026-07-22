/*
 * Minimal, NAppGUI-independent helper functions used by officesdk.cpp.
 * Each function reimplements just enough of NAppGUI's behavior (calling the
 * pertinent OS APIs directly) to remove officesdk's dependency on the
 * vendored NAppGUI sewer/osbs/core libraries.
 */

#include "officesdk.hxx"
#include <cstdint>
#include <string>

typedef struct _osproc_t OsProc;

const char *blib_getenv(const char *name);

platform_t osbs_platform(void);

std::string utf16_to_utf8(const uint16_t *str, const uint32_t nchars);

uint64_t btime_to_micro(const int16_t year, const uint8_t month, const uint8_t mday);

int blib_setenv(const char *name, const char *value);

OsProc *bproc_exec(const char *command);

void bproc_close(OsProc **proc);

#define ptr_assign(dest, src) \
    if ((dest) != NULL) \
    (*dest) = (src)
