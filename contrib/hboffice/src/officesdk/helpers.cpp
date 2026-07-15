#include "helpers.inl"
#include <cstdlib>

/*---------------------------------------------------------------------------*/

const char *blib_getenv(const char *name)
{
    return std::getenv(name);
}

/*---------------------------------------------------------------------------*/

platform_t osbs_platform(void)
{
#if defined(_WIN32)
    return ekWINDOWS;
#elif defined(__APPLE__)
    return ekMACOS;
#elif defined(__linux__)
    return ekLINUX;
#else
#error Unknown platform
#endif
}

/*---------------------------------------------------------------------------*/

#if defined(_WIN32)

#pragma warning(push, 0)
#include <Windows.h>
#pragma warning(pop)

/*---------------------------------------------------------------------------*/

uint64_t btime_to_micro(const int16_t year, const uint8_t month, const uint8_t mday)
{
    /* 100-ns intervals between 1601-01-01 (FILETIME epoch) and 1970-01-01 (Unix epoch) */
    static const int64_t EPOCH_DIFF_100NS = 116444736000000000LL;
    SYSTEMTIME st;
    FILETIME ft;
    LARGE_INTEGER li;
    int64_t t;

    st.wYear = (WORD)year;
    st.wMonth = (WORD)month;
    st.wDay = (WORD)mday;
    st.wHour = 0;
    st.wMinute = 0;
    st.wSecond = 0;
    st.wMilliseconds = 0;
    st.wDayOfWeek = 0;
    SystemTimeToFileTime(&st, &ft);

    li.LowPart = ft.dwLowDateTime;
    li.HighPart = (LONG)ft.dwHighDateTime;
    t = li.QuadPart; /* In 100-nanosecond intervals */
    t -= EPOCH_DIFF_100NS; /* Offset to the Unix epoch */
    t /= 10; /* In microseconds */
    return (uint64_t)t;
}

/*---------------------------------------------------------------------------*/

int blib_setenv(const char *name, const char *value)
{
    return (int)_putenv_s(name, value);
}

/*---------------------------------------------------------------------------*/

#else /* Linux / macOS */

#include <ctime>

/*---------------------------------------------------------------------------*/

uint64_t btime_to_micro(const int16_t year, const uint8_t month, const uint8_t mday)
{
    struct tm tm_date;
    time_t seconds;
    tm_date.tm_year = (int)year - 1900;
    tm_date.tm_mon = (int)month - 1;
    tm_date.tm_mday = (int)mday;
    tm_date.tm_hour = 0;
    tm_date.tm_min = 0;
    tm_date.tm_sec = 0;
    tm_date.tm_isdst = 0;
    /* timegm interprets tm as UTC, unlike mktime (local time) */
    seconds = timegm(&tm_date);
    return (uint64_t)seconds * 1000000ULL;
}

/*---------------------------------------------------------------------------*/

int blib_setenv(const char *name, const char *value)
{
    return setenv(name, value, 1);
}

#endif
