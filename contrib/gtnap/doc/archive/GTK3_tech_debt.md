# Commit report on `/dev` r7026 to r7076


- **Repository**: `svn://192.168.1.2/svn/NAPPGUI`
- **Branch**: `/dev`
- **Range**: r7026-r7076 (49 revisions with own content; r7040 and r7044 do not exist in `/dev`, they correspond to commits on another branch/`trunk`)
- **Actual period**: 2026-09-20 14:46 - 2026-09-29 18:31 (local time, UTC+2)
- **Authors**: `franWIN`, `franOSX`, `franRED`, `franLIN` (variants of Frang depending on the machine/platform used to work: Windows, macOS, "Red"/web, Linux)

**Totals for the period**: **1580 lines added**, **1529 lines removed**, across 49 revisions.

## Commit table

| Revision | Date/Time | Author | Message | Lines added | Lines removed | Files affected (#) |
|---|---|---|---|---|---|---|
| r7026 | 2026-09-20 14:46:55 | franWIN | macOS runner update | 2 | 0 | 1 |
| r7027 | 2026-09-20 14:49:03 | franWIN | Update from trunk | 53 | 59 | 58 |
| r7028 | 2026-09-20 14:50:07 | franWIN | Line endings | 2 | 2 | 2 |
| r7029 | 2026-09-20 15:00:35 | franRED | Update web landing image | 0 | 0 | 2 |
| r7030 | 2026-09-20 17:14:35 | franWIN | Updated xcode 26.6 doc | 1 | 1 | 1 |
| r7031 | 2026-09-20 17:55:44 | franWIN | ndoc macos arquitectures | 33 | 73 | 4 |
| r7032 | 2026-09-20 22:38:02 | franWIN | Memory about font scaling macOS bug | 122 | 0 | 1 |
| r7033 | 2026-09-21 12:10:55 | franOSX | Fix super-big font size when text transform Tahoe/Golden Gate | 208 | 431 | 1 |
| r7034 | 2026-09-21 14:44:42 | franOSX | Updated changelog | 35 | 104 | 2 |
| r7035 | 2026-09-21 14:49:51 | franWIN | Changelog | 8 | 8 | 1 |
| r7036 | 2026-09-21 14:54:39 | franWIN | Golden Gate runner IP assign | 1 | 0 | 1 |
| r7037 | 2026-09-21 19:43:19 | franOSX | Added Golden Gate runner and jobs | 2 | 28 | 2 |
| r7038 | 2026-09-21 19:52:12 | franWIN | Changelog | 2 | 2 | 2 |
| r7039 | 2026-09-21 20:01:05 | franRED | Updated CI/CD dashboard | 0 | 0 | 1 |
| r7041 | 2026-09-22 19:18:26 | franWIN | Merge from trunk | 0 | 0 | 44 |
| r7042 | 2026-09-22 21:08:57 | franLIN | ogl3d support for GdkGLContext in Wayland | 60 | 57 | 1 |
| r7043 | 2026-09-22 21:56:16 | franLIN | OpenGL working on Wayland | 51 | 28 | 2 |
| r7045 | 2026-09-26 08:58:27 | franLIN | Die GTK3 resizing issues fixed | 140 | 72 | 3 |
| r7046 | 2026-09-26 09:45:31 | franLIN | Fix GTK resize when show/hide menubars (Wayland/X11) | 2 | 5 | 3 |
| r7047 | 2026-09-26 10:14:38 | franLIN | Testing GTK3 resizing issues | 71 | 6 | 6 |
| r7048 | 2026-09-26 10:35:35 | franLIN | Testing GTK resize issues | 2 | 2 | 2 |
| r7049 | 2026-09-26 12:34:28 | franLIN | OpenGL build error in UB16 | 29 | 0 | 2 |
| r7050 | 2026-09-26 13:41:52 | franLIN | GTK Tech debt report | 33 | 13 | 3 |
| r7051 | 2026-09-26 15:47:23 | franLIN | Don't force Wayland in apps | 9 | 9 | 9 |
| r7052 | 2026-09-26 16:32:17 | franLIN | GTK sweep from UB12/UB26 and issue reporting | 18 | 9 | 2 |
| r7053 | 2026-09-26 17:44:54 | franLIN | Updated context of OpenGL Wayland issue in context change | 90 | 8 | 2 |
| r7054 | 2026-09-26 17:54:05 | franLIN | Fix GTK Border color for Views | 9 | 1 | 1 |
| r7055 | 2026-09-26 18:05:27 | franLIN | Fix issue in osglobals (frame_color). | 32 | 17 | 2 |
| r7056 | 2026-09-26 18:30:49 | franLIN | Fix GTK3 scroll panel focus widget visible always (automatic scroll) | 13 | 9 | 2 |
| r7057 | 2026-09-26 19:45:29 | franLIN | Fix GTK UB24+ application icon | 69 | 20 | 4 |
| r7058 | 2026-09-26 20:08:08 | franLIN | Updated GTK issues sesion | 57 | 85 | 1 |
| r7059 | 2026-09-27 08:36:53 | franLIN | GTK3 improved 'hover' color for tables and listview | 10 | 12 | 3 |
| r7060 | 2026-09-27 09:30:34 | franLIN | GTK3 fix hover/focused TableView/Listbox color effect | 22 | 11 | 4 |
| r7061 | 2026-09-27 10:29:44 | franLIN | TableView hover color when no-focus | 3 | 14 | 2 |
| r7062 | 2026-09-27 11:24:19 | franLIN | Fix Wayland extra window width with wide titles. | 61 | 31 | 2 |
| r7063 | 2026-09-27 11:45:25 | franLIN | Steps to reproduce OpenGL issues in a GPU host. | 44 | 3 | 2 |
| r7064 | 2026-09-27 12:21:10 | franLIN | Revert "cut-title" in Wayland static Windows | 4 | 137 | 2 |
| r7065 | 2026-09-27 12:29:46 | franLIN | Updated GTK context | 0 | 4 | 1 |
| r7066 | 2026-09-27 13:00:40 | franLIN | Updated OpenGL context from a real GPU machine | 1 | 60 | 2 |
| r7067 | 2026-09-27 13:18:41 | franLIN | OpenGL test for real GPU | 47 | 4 | 3 |
| r7068 | 2026-09-27 14:40:30 | franLIN | Fixed OpenGL context change in Wayland | 8 | 119 | 4 |
| r7069 | 2026-09-27 14:47:29 | franLIN | Fix Gtk3 free glib references when assign GtkGlArea | 2 | 13 | 2 |
| r7070 | 2026-09-27 15:09:18 | franLIN | Fix OpenGL scale factor in real GPU (Wayland) | 7 | 0 | 3 |
| r7071 | 2026-09-27 15:20:50 | franLIN | Fix OpenGL crash in X11 when real GPU | 22 | 1 | 2 |
| r7072 | 2026-09-27 15:37:54 | franLIN | Fix crash when desktop app in Linux without display. | 7 | 11 | 3 |
| r7073 | 2026-09-27 15:52:31 | franLIN | Added OpenGL 4.1 demo in GLHello | 109 | 2 | 3 |
| r7074 | 2026-09-27 16:04:34 | franLIN | GTK fix UpDown clipping. | 6 | 7 | 2 |
| r7075 | 2026-09-27 20:28:11 | franLIN | Kali Linux Dark mode test. Added new issues detected. | 10 | 20 | 1 |
| r7076 | 2026-09-29 18:31:30 | franLIN | Updated documentation | 63 | 31 | 4 |
| **Total** | | | **49 revisions** | **1580** | **1529** | |

## File detail per revision

### r7026 macOS runner update
*2026-09-20 14:46:55 · franWIN · +2/-0 · 1 file*

- `cicd/doc/macOS new runner.md`

### r7027  Update from trunk
*2026-09-20 14:49:03 · franWIN · +53/-59 · 58 files*

- `cicd/nappgui_src/Changelog.md`
- `demo/drawbig`
- `demo/guihello/guihello.c`
- `demo/guihello/labels.c`
- `demo/guihello`
- `demo`
- `doc/draw2d/es/draw2d.htm`
- `doc/draw2d`
- `doc/gui/en/gui.htm`
- `doc/gui/es/gui.htm`
- `doc/gui/es/textview.htm`
- `doc/guide/en/win_mac_linux.htm`
- `doc/guide/es/win_mac_linux.htm`
- `doc/guide/img/golden_gate.png`
- `doc/guide`
- `doc/home/en/home.html`
- `doc/home/es/home.html`
- `doc/home`
- `doc/osbs/en/osbs.htm`
- `doc/osbs/es/osbs.htm`
- `doc`
- `prj/NAppMacOS.cmake`
- `prj`
- `src/draw2d/draw2d.hxx`
- `src/draw2d/font.c`
- `src/draw2d/font.h`
- `src/draw2d/font.inl`
- `src/draw2d/osx/dctx_osx.m`
- `src/draw2d/osx/osfont.m`
- `src/draw2d/win/dctx_win.cpp`
- `src/draw2d/win/draw2d_win.ixx`
- `src/draw2d/win/osfont.cpp`
- `src/draw2d`
- `src/gui/gui.c`
- `src/gui/tableview.c`
- `src/gui/textview.c`
- `src/gui/textview.h`
- `src/osapp/gtk/osapp_gtk.c`
- `src/osapp/osapp.c`
- `src/osbs/osbs.hxx`
- `src/osbs/osx/sinfo.m`
- `src/osgui/gtk/osglobals.c`
- `src/osgui/gtk/oswindow.c`
- `src/osgui/osguictx.c`
- `src/osgui/osscrolls.c`
- `src/osgui/osscrolls.inl`
- `src/osgui/osx/ostext.m`
- `src/osgui/osx/osview.m`
- `src/osgui/win/osdrawctrl.cpp`
- `src/osgui/win/ospanel.c`
- `src/osgui/win/ospanel_win.inl`
- `src/osgui/win/ostext.c`
- `src/osgui/win/osview.cpp`
- `src`
- `tools/nbuild/host.c`
- `tools/nbuild/report.c`
- `tools/nbuild`
- `.`

### r7028 Line endings
*2026-09-20 14:50:07 · franWIN · +2/-2 · 2 files*

- `src/osapp/gtk/osapp_gtk.c`
- `src/osgui/gtk/osglobals.c`

### r7029 Update web landing image
*2026-09-20 15:00:35 · franRED · +0/-0 · 2 files*

- `doc/home/gen/home.odg`
- `doc/home/res/all_c_compilers.png`

### r7030 Updated xcode 26.6 doc
*2026-09-20 17:14:35 · franWIN · +1/-1 · 1 file*

- `doc/guide/es/win_mac_linux.htm`

### r7031 ndoc macos arquitectures
*2026-09-20 17:55:44 · franWIN · +33/-73 · 4 files*

- `doc/guide/en/build.htm`
- `doc/guide/en/win_mac_linux.htm`
- `doc/guide/es/build.htm`
- `doc/guide/es/win_mac_linux.htm`

### r7032 Memory about font scaling macOS bug
*2026-09-20 22:38:02 · franWIN · +122/-0 · 1 file*

- `internal/macos_golden_gate_font_handoff.md`

### r7033 Fix super-big font size when text transform Tahoe/Golden Gate
*2026-09-21 12:10:55 · franOSX · +208/-431 · 1 file*

- `src/draw2d/osx/osfont.m`

### r7034 Updated changelog
*2026-09-21 14:44:42 · franOSX · +35/-104 · 2 files*

- `cicd/nappgui_src/Changelog.md`
- `internal/macos_golden_gate_font_handoff.md`

### r7035 Changelog
*2026-09-21 14:49:51 · franWIN · +8/-8 · 1 file*

- `cicd/nappgui_src/Changelog.md`

### r7036 Golden Gate runner IP assign
*2026-09-21 14:54:39 · franWIN · +1/-0 · 1 file*

- `cicd/doc/NewBuildNetwork.md`

### r7037 Added Golden Gate runner and jobs
*2026-09-21 19:43:19 · franOSX · +2/-28 · 2 files*

- `cicd/nappgui_src/workflow.json`
- `cicd/network.json`

### r7038 Changelog
*2026-09-21 19:52:12 · franWIN · +2/-2 · 2 files*

- `CLAUDE.md`
- `cicd/nappgui_src/Changelog.md`

### r7039 Updated CI/CD dashboard
*2026-09-21 20:01:05 · franRED · +0/-0 · 1 file*

- `cicd/doc/dashboard.odg`

### r7041 Merge from trunk
*2026-09-22 19:18:26 · franWIN · +0/-0 · 44 files*

- `cicd/nappgui_src/Changelog.md`
- `cicd/network.json`
- `demo/drawbig`
- `demo/guihello/guihello.c`
- `demo/guihello/labels.c`
- `demo/guihello`
- `demo`
- `doc/draw2d/es/draw2d.htm`
- `doc/draw2d`
- `doc/gui/es/textview.htm`
- `doc/guide/en/win_mac_linux.htm`
- `doc/guide/es/win_mac_linux.htm`
- `doc/guide`
- `doc/home`
- `doc`
- `prj`
- `src/draw2d/draw2d.hxx`
- `src/draw2d/font.c`
- `src/draw2d/font.h`
- `src/draw2d/font.inl`
- `src/draw2d/osx/dctx_osx.m`
- `src/draw2d/osx/osfont.m`
- `src/draw2d/win/dctx_win.cpp`
- `src/draw2d/win/draw2d_win.ixx`
- `src/draw2d/win/osfont.cpp`
- `src/draw2d`
- `src/gui/gui.c`
- `src/gui/tableview.c`
- `src/gui/textview.c`
- `src/gui/textview.h`
- `src/osapp/osapp.c`
- `src/osgui/osguictx.c`
- `src/osgui/osscrolls.c`
- `src/osgui/osscrolls.inl`
- `src/osgui/osx/ostext.m`
- `src/osgui/osx/osview.m`
- `src/osgui/win/osdrawctrl.cpp`
- `src/osgui/win/ospanel.c`
- `src/osgui/win/ospanel_win.inl`
- `src/osgui/win/ostext.c`
- `src/osgui/win/osview.cpp`
- `src`
- `tools/nbuild`
- `.`

### r7042 ogl3d support for GdkGLContext in Wayland
*2026-09-22 21:08:57 · franLIN · +60/-57 · 1 file*

- `src/ogl3d/gtk/ogl3dimp.c`

### r7043 OpenGL working on Wayland
*2026-09-22 21:56:16 · franLIN · +51/-28 · 2 files*

- `src/ogl3d/gtk/ogl3dimp.c`
- `src/osgui/gtk/osview.c`

### r7045 Die GTK3 resizing issues fixed
*2026-09-26 08:58:27 · franLIN · +140/-72 · 3 files*

- `demo/die/main.c`
- `internal/wayland_handoff.md`
- `src/osgui/gtk/oswindow.c`

### r7046 Fix GTK resize when show/hide menubars (Wayland/X11)
*2026-09-26 09:45:31 · franLIN · +2/-5 · 3 files*

- `demo/die/main.c`
- `demo/drawbig/drawbig.c`
- `src/osgui/gtk/oswindow.c`

### r7047 Testing GTK3 resizing issues
*2026-09-26 10:14:38 · franLIN · +71/-6 · 6 files*

- `demo/bode/bode.c`
- `demo/bricks/bricks.c`
- `demo/col2dhello/col2dhello.c`
- `demo/colorview/colorview.c`
- `demo/glhello/glhello.c`
- `internal/wayland_handoff.md`

### r7048 Testing GTK resize issues
*2026-09-26 10:35:35 · franLIN · +2/-2 · 2 files*

- `demo/splits/main.c`
- `demo/webhello/webhello.c`

### r7049 OpenGL build error in UB16
*2026-09-26 12:34:28 · franLIN · +29/-0 · 2 files*

- `internal/gtk_issues.md`
- `src/ogl3d/gtk/ogl3dimp.c`

### r7050 GTK Tech debt report
*2026-09-26 13:41:52 · franLIN · +33/-13 · 3 files*

- `demo/splits/main.c`
- `internal/gtk_issues.md`
- `internal/wayland_handoff.md`

### r7051 Don't force Wayland in apps
*2026-09-26 15:47:23 · franLIN · +9/-9 · 9 files*

- `demo/bode/bode.c`
- `demo/bricks/bricks.c`
- `demo/col2dhello/col2dhello.c`
- `demo/colorview/colorview.c`
- `demo/die/main.c`
- `demo/drawbig/drawbig.c`
- `demo/glhello/glhello.c`
- `demo/splits/main.c`
- `demo/webhello/webhello.c`

### r7052 GTK sweep from UB12/UB26 and issue reporting
*2026-09-26 16:32:17 · franLIN · +18/-9 · 2 files*

- `internal/gtk_issues.md`
- `internal/wayland_handoff.md`

### r7053 Updated context of OpenGL Wayland issue in context change
*2026-09-26 17:44:54 · franLIN · +90/-8 · 2 files*

- `internal/gtk_issues.md`
- `internal/wayland_handoff.md`

### r7054 Fix GTK Border color for Views
*2026-09-26 17:54:05 · franLIN · +9/-1 · 1 file*

- `src/osgui/gtk/osglobals.c`

### r7055 Fix issue in osglobals (frame_color).
*2026-09-26 18:05:27 · franLIN · +32/-17 · 2 files*

- `internal/gtk_issues.md`
- `src/osgui/gtk/osglobals.c`

### r7056 Fix GTK3 scroll panel focus widget visible always (automatic scroll)
*2026-09-26 18:30:49 · franLIN · +13/-9 · 2 files*

- `internal/gtk_issues.md`
- `src/osgui/gtk/oscontrol.c`

### r7057 Fix GTK UB24+ application icon
*2026-09-26 19:45:29 · franLIN · +69/-20 · 4 files*

- `internal/gtk_issues.md`
- `prj/NAppDesktopFile.cmake`
- `prj/NAppTarget.cmake`
- `src/osapp/gtk/osapp_gtk.c`

### r7058 Updated GTK issues sesion
*2026-09-26 20:08:08 · franLIN · +57/-85 · 1 file*

- `internal/gtk_issues.md`

### r7059 GTK3 improved 'hover' color for tables and listview
*2026-09-27 08:36:53 · franLIN · +10/-12 · 3 files*

- `src/osgui/gtk/osdrawctrl.c`
- `src/osgui/gtk/osglobals.c`
- `src/osgui/gtk/osglobals_gtk.inl`

### r7060 GTK3 fix hover/focused TableView/Listbox color effect
*2026-09-27 09:30:34 · franLIN · +22/-11 · 4 files*

- `internal/gtk_issues.md`
- `src/osgui/gtk/osdrawctrl.c`
- `src/osgui/gtk/osglobals.c`
- `src/osgui/gtk/osglobals_gtk.inl`

### r7061 TableView hover color when no-focus
*2026-09-27 10:29:44 · franLIN · +3/-14 · 2 files*

- `internal/gtk_issues.md`
- `src/gui/tableview.c`

### r7062 Fix Wayland extra window width with wide titles.
*2026-09-27 11:24:19 · franLIN · +61/-31 · 2 files*

- `internal/gtk_issues.md`
- `src/osgui/gtk/oswindow.c`

### r7063 Steps to reproduce OpenGL issues in a GPU host.
*2026-09-27 11:45:25 · franLIN · +44/-3 · 2 files*

- `internal/glarea_freeze_checklist.md`
- `internal/gtk_issues.md`

### r7064 Revert "cut-title" in Wayland static Windows
*2026-09-27 12:21:10 · franLIN · +4/-137 · 2 files*

- `internal/gtk_issues.md`
- `src/osgui/gtk/oswindow.c`

### r7065 Updated GTK context
*2026-09-27 12:29:46 · franLIN · +0/-4 · 1 file*

- `internal/gtk_issues.md`

### r7066 Updated OpenGL context from a real GPU machine
*2026-09-27 13:00:40 · franLIN · +1/-60 · 2 files*

- `internal/glarea_freeze_checklist.md`
- `internal/gtk_issues.md`

### r7067 OpenGL test for real GPU
*2026-09-27 13:18:41 · franLIN · +47/-4 · 3 files*

- `demo/glhello/CMakeLists.txt`
- `demo/glhello/glhello.c`
- `internal/wayland_handoff.md`

### r7068 Fixed OpenGL context change in Wayland
*2026-09-27 14:40:30 · franLIN · +8/-119 · 4 files*

- `demo/glhello/CMakeLists.txt`
- `demo/glhello/glhello.c`
- `internal/gtk_issues.md`
- `src/ogl3d/gtk/ogl3dimp.c`

### r7069 Fix Gtk3 free glib references when assign GtkGlArea
*2026-09-27 14:47:29 · franLIN · +2/-13 · 2 files*

- `internal/gtk_issues.md`
- `src/osgui/gtk/osview.c`

### r7070 Fix OpenGL scale factor in real GPU (Wayland)
*2026-09-27 15:09:18 · franLIN · +7/-0 · 3 files*

- `internal/gtk_issues.md`
- `internal/wayland_handoff.md`
- `src/osgui/gtk/osview.c`

### r7071 Fix OpenGL crash in X11 when real GPU
*2026-09-27 15:20:50 · franLIN · +22/-1 · 2 files*

- `internal/gtk_issues.md`
- `src/ogl3d/gtk/ogl3dimp.c`

### r7072 Fix crash when desktop app in Linux without display.
*2026-09-27 15:37:54 · franLIN · +7/-11 · 3 files*

- `internal/gtk_issues.md`
- `src/osbs/osbs.c`
- `src/sewer/sewer.c`

### r7073 Added OpenGL 4.1 demo in GLHello
*2026-09-27 15:52:31 · franLIN · +109/-2 · 3 files*

- `demo/glhello/glhello.c`
- `demo/glhello/ogl4.c`
- `demo/glhello/ogl4.h`

### r7074 GTK fix UpDown clipping.
*2026-09-27 16:04:34 · franLIN · +6/-7 · 2 files*

- `internal/gtk_issues.md`
- `src/osgui/gtk/osupdown.c`

### r7075 Kali Linux Dark mode test. Added new issues detected.
*2026-09-27 20:28:11 · franLIN · +10/-20 · 1 file*

- `internal/gtk_issues.md`

### r7076 Updated documentation
*2026-09-29 18:31:30 · franLIN · +63/-31 · 4 files*

- `cicd/nappgui_src/Changelog.md`
- `doc/gui/en/gui.htm`
- `doc/gui/es/gui.htm`
- `internal/gtk_issues.md`
