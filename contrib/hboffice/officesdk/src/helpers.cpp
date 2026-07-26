#include "helpers.inl"
#include <cstdlib>
#include <cassert>
#include <vector>

/*---------------------------------------------------------------------------*/

const char *blib_getenv(const char *name)
{
#if defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif
    return std::getenv(name);
#if defined(_MSC_VER)
#pragma warning(default : 4996)
#endif
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

std::string utf16_to_utf8(const uint16_t *str, const uint32_t nchars)
{
    std::string result;
    result.reserve(nchars);

    for (uint32_t i = 0; i < nchars; ++i)
    {
        uint32_t codepoint = str[i];

        /* High surrogate followed by a low surrogate -> combine into one codepoint */
        if (codepoint >= 0xD800 && codepoint <= 0xDBFF && i + 1 < nchars)
        {
            uint32_t low = str[i + 1];
            if (low >= 0xDC00 && low <= 0xDFFF)
            {
                codepoint = 0x10000 + ((codepoint - 0xD800) << 10) + (low - 0xDC00);
                i += 1;
            }
        }

        if (codepoint < 0x80)
        {
            result += (char)codepoint;
        }
        else if (codepoint < 0x800)
        {
            result += (char)(0xC0 | (codepoint >> 6));
            result += (char)(0x80 | (codepoint & 0x3F));
        }
        else if (codepoint < 0x10000)
        {
            result += (char)(0xE0 | (codepoint >> 12));
            result += (char)(0x80 | ((codepoint >> 6) & 0x3F));
            result += (char)(0x80 | (codepoint & 0x3F));
        }
        else
        {
            result += (char)(0xF0 | (codepoint >> 18));
            result += (char)(0x80 | ((codepoint >> 12) & 0x3F));
            result += (char)(0x80 | ((codepoint >> 6) & 0x3F));
            result += (char)(0x80 | (codepoint & 0x3F));
        }
    }

    return result;
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

struct _osproc_t
{
    PROCESS_INFORMATION info;
};

OsProc *bproc_exec(const char *command)
{
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    /* CreateProcess requires a writable command line buffer */
    std::string cmdline = std::string("cmd /c ") + command;
    std::vector<char> buf(cmdline.begin(), cmdline.end());
    buf.push_back('\0');

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    if (!CreateProcessA(NULL, buf.data(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
        return nullptr;

    CloseHandle(pi.hThread);

    OsProc *proc = new OsProc();
    proc->info = pi;
    return proc;
}

/*---------------------------------------------------------------------------*/

void bproc_close(OsProc **proc)
{
    assert(proc != nullptr && *proc != nullptr);
    CloseHandle((*proc)->info.hProcess);
    delete *proc;
    *proc = nullptr;
}

/*---------------------------------------------------------------------------*/

#else /* Linux / macOS */

#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

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

/*---------------------------------------------------------------------------*/

struct _osproc_t
{
    pid_t pid;
};

OsProc *bproc_exec(const char *command)
{
    pid_t pid = fork();

    if (pid == -1)
        return nullptr;

    /* Child: run the command through the shell, discarding its std streams */
    if (pid == 0)
    {
        int devnull = open("/dev/null", O_RDWR);
        if (devnull >= 0)
        {
            dup2(devnull, STDIN_FILENO);
            dup2(devnull, STDOUT_FILENO);
            dup2(devnull, STDERR_FILENO);
            close(devnull);
        }

        execl("/bin/sh", "sh", "-c", command, (char *)NULL);
        _exit(127);
    }

    OsProc *proc = new OsProc();
    proc->pid = pid;
    return proc;
}

/*---------------------------------------------------------------------------*/

void bproc_close(OsProc **proc)
{
    assert(proc != nullptr && *proc != nullptr);
    /* WNOHANG: reap the child if it has already finished, without blocking */
    waitpid((*proc)->pid, NULL, WNOHANG);
    delete *proc;
    *proc = nullptr;
}

#endif
