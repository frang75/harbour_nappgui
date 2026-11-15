# WIP notes

## CMake Presets / VS Code setup (Windows, Sprint-117)

Root `CMakePresets.json` currently has a single configure/build preset:
`windows-ninja-vs2026-debug` (Ninja generator, MSVC v145/VS2026 toolset,
`architecture`/`toolset` with `strategy: "external"`).

- Requires `"cmake.useVsDeveloperEnvironment": "always"` in
  `.vscode/settings.json`. With the default `"auto"`, CMake Tools skips
  injecting the VS2026 dev environment whenever *any* compiler is already
  reachable on PATH (e.g. MSYS2's gcc), so it silently configures with GNU
  instead of MSVC.
- `.vscode/settings.json`'s `cmake.sourceDirectory` must be
  `"${workspaceFolder}"` — found it pointing at a stale WSL path
  (`/home/fran/harbour_nappgui`) left over from a previous machine; that's
  why the preset selector didn't show up in `[F1]` at first.

A `windows-mingw-gcc-debug` preset was tried and **dropped**: the combined
"Harbour Exemplo FULL Debug solution" (root `CMakeLists.txt`) always
`add_subdirectory()`s `contrib/hboffice/officesdk`, which hard-requires MSVC
on Windows (LibreOffice SDK's Windows import libs are MSVC-only). Confirmed
other gaps too if a MinGW-only (no officesdk) build is ever wanted later:
`nap-dev/HarbourTools.cmake`'s `harbour_to_c_exe` only wires the MSVC
`hbmk2.exe` on `WIN32` (the mingw64 one is vendored at
`bin/win/mingw64/hbmk2.exe`, just never routed to), and
`contrib/hbaws/src/CMakeLists.txt`'s GNU branch would resolve to
`awssdk/bin/linux/gcc` even on Windows (the correct vendored folder is
`awssdk/bin/win/gcc`, which does exist). Not pursued — not needed for now.

**CMake gotcha found along the way**: `if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")`
is broken when the left side is unquoted — CMake defines a predefined
variable literally named `MSVC` (boolean, true when the compiler is MSVC),
and the unquoted substitution result gets re-dereferenced as that variable
instead of staying a literal string. Fixed in `officesdk/CMakeLists.txt` and
`contrib/hbaws/src/CMakeLists.txt` by quoting **both** sides:
`"${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC"`. The pre-existing pattern
elsewhere in the repo (`NAppCompilers.cmake`, `NAppTarget.cmake`,
`NAppUtils.cmake`, `nap-dev/HarbourTools.cmake`) — `STREQUAL MSVC`, both
sides bare/unquoted — happens to work, but only by a lucky double-dereference
coincidence (both sides end up comparing the `MSVC` variable's value against
itself). Don't copy that pattern into new code; always quote both sides.

## `.vscode/launch.json`

Three configs (Exemplo, Designer, Debugger), all `"type": "cppvsdbg"`,
`program` under `build/windows-ninja-vs2026-debug/Debug/bin/*.exe`, `cwd`
set to `contrib/gtnap/tests/cuademo/gtnap_cualib` (where the CUALIB demo's
data files live). Exemplo's `environment` appends
`${env:AWS_SDK_ROOT}\msvc64\Release\bin` to `PATH` (needed for the vendored
AWS-SDK DLLs; `AWS_SDK_ROOT`/`LIBREOFFICE_HOME` themselves are already set as
Windows user env vars, no need to hardcode them here).

These three were previously pointing at a flat `build/Debug/bin/...` (no
`.exe`, no preset subfolder) and had Linux-only `environment` entries
(`LIBREOFFICE_HOME=/usr/lib/libreoffice`, `AWS_SDK_ROOT=/home/fran/aws-sdk`,
`LD_LIBRARY_PATH=...`) left over from a prior WSL/Linux session — those
silently overrode the correct Windows env vars and were the actual cause of
Exemplo crashing at launch before this cleanup.

We also tried routing `[F5]` (CMake Tools' "launch the debugger for the
selected target", which ignores `launch.json` and is driven instead by the
`cmake.debugConfig` setting in `.vscode/settings.json`) — reverted per user
request in favor of sticking with the Run & Debug (`launch.json`) workflow
for running Exemplo across different machines/platforms.

## TODO when we move to Linux/macOS

Don't create separate named launch configs per OS (`Exemplo (Linux)` etc.).
Linux/macOS/Windows are never active at the same time on one machine (unlike
MSVC vs MinGW, which can coexist on the same Windows box — that's why
Windows got its own dedicated entry instead of an OS-override block). Extend
the *same* "Exemplo" entry using `launch.json`'s native per-OS override keys
(`"windows"`, `"linux"`, `"osx"` inside the configuration object), so one
entry adapts automatically to whichever OS VS Code is running on. Needs, per
OS:
- `program` without `.exe`, correct build dir for that OS's preset.
- `"type": "cppdbg"` with `"MIMode": "gdb"` (Linux) or `"lldb"` (macOS)
  instead of `cppvsdbg`.
- `LD_LIBRARY_PATH` (Linux) / `DYLD_LIBRARY_PATH` (macOS) instead of `PATH`,
  pointing at that OS's vendored AWS-SDK bin folder
  (`awssdk/bin/linux/gcc`, `awssdk/bin/darwin/clang`) and LibreOffice's
  `program` folder.
