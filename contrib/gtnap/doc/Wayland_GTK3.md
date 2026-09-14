# Changes to the `/dev` branch in SVN

Repository: `svn://192.168.1.2/svn/NAPPGUI`

## Branches

| | URL | Last revision with real change | Date |
|---|---|---|---|
| `/dev` | `svn://192.168.1.2/svn/NAPPGUI/dev` | **r7019** | 2026-09-13 19:38:40 +0200 |
| `/trunk` | `svn://192.168.1.2/svn/NAPPGUI/trunk` | **r6980** | 2026-09-03 19:43:18 +0200 |

## Detail of commits (r6981 → r7019)

"Lines added"/"Lines removed" count lines of text (`svn diff -c <rev>`, format
unified). "Files affected" lists files with **actual content change** in that
revision (added/modified/deleted) — files and directories that are only
carry an SVN property change (e.g. `svn:mergeinfo`, which appears in almost all
directories of `r6981` because it is the revision that synchronizes `/dev` with `/trunk`, without being
real content). Files are not deduplicated between commits: the same file can be repeated
in several rows if it was touched in several revisions.

| Revision | Date/Time | Log message | Lines added | Lines removed | Files affected |
|---|---|---|---|---|---|
| 6981 | 2026-09-03 20:24:44 | Merge from trunk | 68 | 165 | `demo/hello/main.c`, `doc/gui/en/gui.htm`, `doc/gui/es/gui.htm`, `prj/NAppTarget.cmake`, `src/gui/panel.c`, `src/gui/panel.inl`, `src/osgui/win/oswindow.c` |
| 6982 | 2026-09-03 21:21:29 | Update Linux debug via CMake extension | 333 | 1 | `.vscode/settings.json`, `internal/wayland_background.md` |
| 6983 | 2026-09-03 21:24:26 | Update Debug in Linux | 2 | 1 | `.vscode/settings.json` |
| 6984 | 2026-09-03 21:32:30 | Config debug | 6 | 2 | `.vscode/settings.json` |
| 6985 | 2026-09-03 21:48:16 | Linux debug | 2 | 13 | `.vscode/settings.json` |
| 6986 | 2026-09-05 08:32:38 | Remove force X11 | 15 | 2 | `cicd/nappgui_src/Changelog.md`, `src/osapp/gtk/osapp_gtk.c` |
| 6987 | 2026-09-05 08:48:32 | fix window size | 9 | 0 | `src/osgui/gtk/oswindow.c` |
| 6988 | 2026-09-05 08:51:04 | Wayland Claude session | 159 | 0 | `internal/wayland_handoff.md` |
| 6989 | 2026-09-05 09:31:25 | Wayland fix | 172 | 69 | `internal/wayland_handoff.md`, `src/osapp/gtk/osapp_gtk.c`, `src/osgui/gtk/oswindow.c` |
| 6990 | 2026-09-05 09:58:00 | oscomcolor fix Wayland | 5 | 3 | `src/osgui/gtk/oscomwin.c` |
| 6991 | 2026-09-07 21:23:22 | window_overlay passes now the origin | 553 | 84 | `demo/drawbig/drawbig.c`, `demo/guihello/flyout.c`, `doc/gui/en/window.htm`, `doc/gui/es/window.htm`, `internal/wayland_handoff.md`, `src/draw2d/guictx.c`, `src/draw2d/guictx.h`, `src/draw2d/guictx.hxx`, `src/gui/window.c`, `src/gui/window.h`, `src/osgui/gtk/oscontrol.c`, `src/osgui/osguictx.c`, `src/osgui/oswindow.h` |
| 6992 | 2026-09-12 09:01:24 | macOS overlay launch | 60 | 19 | `src/osgui/osx/oswindow.m` |
| 6993 | 2026-09-12 09:01:36 | Win32 overlay launch | 67 | 11 | `src/osgui/win/oswindow.c` |
| 6994 | 2026-09-12 11:16:54 | Overlay windows working in Wayland | 1587 | 1252 | `demo/drawbig/drawbig.c`, `internal/wayland_handoff.md`, `src/osgui/gtk/oswindow.c` |
| 6995 | 2026-09-12 12:22:17 | Wayland window modals | 67 | 1 | `internal/wayland_handoff.md`, `src/osgui/gtk/oswindow.c` |
| 6996 | 2026-09-12 12:39:54 | osmain option -gdkbackend | 9 | 13 | `demo/guihello/guihello.c`, `src/osapp/gtk/osapp_gtk.c`, `src/osapp/osapp.c` |
| 6997 | 2026-09-12 12:48:16 | Linux log print the current backend | 15 | 4 | `src/osapp/gtk/osapp_gtk.c` |
| 6998 | 2026-09-12 13:04:44 | Remove X11 warnings from 3rd parties | 87 | 3 | `demo/guihello/guihello.c`, `internal/wayland_handoff.md`, `src/osapp/gtk/osapp_gtk.c` |
| 6999 | 2026-09-12 13:10:35 | GTK disable warnings compatible with GLIB version | 18 | 0 | `internal/wayland_handoff.md`, `src/osapp/gtk/osapp_gtk.c` |
| 7000 | 2026-09-12 13:24:53 | Wayland fix popup menu launch | 12 | 11 | `demo/guihello/guihello.c`, `src/osgui/gtk/osglobals.c`, `src/osgui/gtk/osmenu.c` |
| 7001 | 2026-09-12 15:22:59 | Wayland documentation spanish | 98 | 6 | `doc/gui/es/gui.htm`, `doc/gui/es/menu.htm`, `doc/gui/es/window.htm`, `internal/wayland_handoff.md` |
| 7002 | 2026-09-12 16:17:15 | menu_launch now uses window-local coordinates, not screen coordinates | 99 | 36 | `demo/guihello/dynmenu.c`, `demo/guihello/flyout.c`, `demo/guihello/guihello.c`, `doc/gui/es/menu.htm`, `internal/wayland_handoff.md`, `src/gui/menu.c`, `src/gui/menu.h`, `src/osgui/gtk/osmenu.c`, `src/osgui/gtk/oswindow.c`, `src/osgui/gtk/oswindow_gtk.inl`, `src/osgui/osx/osmenu.m`, `src/osgui/win/osmenu.c` |
| 7003 | 2026-09-12 16:19:59 | Minor change in doc (spanish) | 1 | 1 | `doc/gui/es/gui.htm` |
| 7004 | 2026-09-12 16:23:56 | Minor changes in doc (spanish) | 4 | 4 | `doc/gui/en/window.htm`, `doc/gui/es/gui.htm`, `doc/gui/es/window.htm` |
| 7005 | 2026-09-13 09:12:14 | Minor changes in doc  spanish | 4 | 5 | `doc/gui/es/gui.htm` |
| 7006 | 2026-09-13 09:12:49 | Wayland doc images | 63 | 18 | `doc/gui/gui.odg`, `doc/gui/img/gui_depends.svg`, `doc/gui/img/overlay_wayland.png` |
| 7007 | 2026-09-13 09:27:38 | Wayland documentation to English | 66 | 5 | `doc/gui/en/gui.htm`, `doc/gui/en/menu.htm`, `doc/gui/en/window.htm`, `internal/wayland_handoff.md` |
| 7008 | 2026-09-13 09:29:41 | Doc overlay code sample | 20 | 20 | `doc/gui/en/window.htm`, `doc/gui/es/window.htm` |
| 7009 | 2026-09-13 09:50:03 | Fix Wayland issue in osglobals_workarea | 6 | 2 | `src/osgui/gtk/osglobals.c` |
| 7010 | 2026-09-13 09:56:18 | Fix Wayland issue in osglobals_resolution | 6 | 2 | `src/osgui/gtk/osglobals.c` |
| 7011 | 2026-09-13 09:56:33 | Updated Wayland doc with new cases | 6 | 0 | `doc/gui/en/gui.htm`, `doc/gui/es/gui.htm` |
| 7012 | 2026-09-13 10:03:44 | Updated Wayland window_OnMoved documentation | 30 | 15 | `doc/gui/en/gui.htm`, `doc/gui/en/window.htm`, `doc/gui/es/gui.htm`, `doc/gui/es/window.htm`, `internal/wayland_handoff.md` |
| 7013 | 2026-09-13 10:20:39 | Avoid crash if Wayland protocol is not present. | 9 | 2 | `internal/wayland_handoff.md`, `src/osapp/osapp.c` |
| 7014 | 2026-09-13 10:24:31 | Avoid compiler warnings (halign switch). | 6 | 0 | `src/osgui/gtk/oswindow.c`, `src/osgui/osx/oswindow.m`, `src/osgui/win/oswindow.c` |
| 7015 | 2026-09-13 11:44:03 | Wayland support for OpenGL contexts. | 308 | 15 | `demo/glhello/glhello.c`, `demo/guihello/guihello.c`, `internal/wayland_handoff.md`, `prj/NAppTarget.cmake`, `src/ogl3d/CMakeLists.txt`, `src/ogl3d/gtk/ogl3dimp.c` |
| 7016 | 2026-09-13 19:01:53 | Restored failed implementation of OpenGL in Wayland | 359 | 114 | `demo/glhello/glhello.c`, `internal/wayland_handoff.md`, `prj/NAppTarget.cmake`, `src/ogl3d/CMakeLists.txt`, `src/ogl3d/gtk/ogl3dimp.c` |
| 7017 | 2026-09-13 19:14:33 | Fix warnings in Windows | 8 | 8 | `demo/drawbig/drawbig.c`, `demo/guihello/flyout.c`, `src/osgui/win/oswindow.c` |
| 7018 | 2026-09-13 19:33:58 | Wayland changelog | 14 | 6 | `cicd/nappgui_src/Changelog.md`, `internal/wayland_handoff.md` |
| 7019 | 2026-09-13 19:38:40 | Updated session memory | 5 | 1 | `internal/wayland_handoff.md` |
| **Total** | | **39 commits** | **4358** | **1914** | **113 files** |
