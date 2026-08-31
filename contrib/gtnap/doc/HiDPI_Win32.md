# Changes to the `/dev` branch in SVN

Repository: `svn://192.168.1.2/svn/NAPPGUI`

## Origin of the branch

The current `/dev` branch was created in **revision r6927** using `svn copy ^/trunk ^/dev`, as a clean restart after discarding a previous HiDPI approach (the `scale` parameter passing through the `gui` layer) that was never committed. The previous history of `/dev` is preserved in its entirety on the archived branch `svn://192.168.1.2/svn/NAPPGUI/win_dpi_fail` (renamed from the old `/dev` in r6926), but it is not part of this report.

Range covered in this report: **r6927 (branch creation) → r6977 (current HEAD)**, 51 commits.

## Commits en `/dev`

| Revision | Date/Time | Log message | Lines added | Lines removed | Files affected |
|---|---|---|---:|---:|---|
| r6927 | 2026-08-23 12:17 | Create new dev branch from trunk (clean restart after HiDPI scale-parameter approach failed) | 0 | 0 | *(copia de `/trunk`, sin diff de contenido)* |
| r6928 | 2026-08-23 16:43 | Win32 Font with differente DPI implementations | 132 | 48 | src/draw2d/font.c, src/draw2d/font.h, src/draw2d/font.inl, src/draw2d/gtk/osfont.c, src/draw2d/guictx.c, src/draw2d/guictx.h, src/draw2d/guictx.hxx, src/draw2d/osx/osfont.m, src/draw2d/win/draw_win.cpp, src/draw2d/win/draw_win.inl, src/draw2d/win/osfont.cpp, src/gui/gui.c, src/gui/gui.h, src/osgui/gtk/osglobals.c, src/osgui/osglobals.h, src/osgui/osguictx.c, src/osgui/osx/osglobals.m, src/osgui/win/osglobals.c, src/osgui/win/osgui_win.cpp, src/osgui/win/osgui_win.inl, src/osgui/win/oswindow.c |
| r6929 | 2026-08-23 17:27 | win32 osfont minor refactor | 7 | 39 | src/draw2d/win/osfont.cpp |
| r6930 | 2026-08-24 08:02 | Win32 font scale | 5 | 10 | src/draw2d/win/osfont.cpp |
| r6931 | 2026-08-24 08:17 | oswindow WM_DPICHANGED message (not implemented) | 6 | 0 | src/osgui/win/oswindow.c |
| r6932 | 2026-08-24 08:37 | oswindow computes its DPI | 21 | 4 | src/osgui/win/osgui_win.cpp, src/osgui/win/osgui_win.inl, src/osgui/win/oswindow.c |
| r6933 | 2026-08-24 10:12 | Removed panel_compose unused funcs | 0 | 23 | src/gui/panel.c, src/gui/panel.inl |
| r6934 | 2026-08-24 10:16 | oswindow scale value | 20 | 1 | src/osgui/win/osgui_win.ixx, src/osgui/win/oswindow.c, src/osgui/win/oswindow_win.inl |
| r6935 | 2026-08-24 11:02 | osview HiDPI scaling | 14 | 28 | demo/hello/main.c, src/osgui/win/oscontrol.cpp, src/osgui/win/osview.cpp, src/osgui/win/oswindow.c, src/osgui/win/oswindow_win.inl |
| r6936 | 2026-08-24 12:12 | GDIPlus drawing context with scaling | 12 | 13 | demo/hello/main.c, src/draw2d/dctx.c, src/draw2d/dctxh.h, src/draw2d/gtk/dctx_gtk.c, src/draw2d/osx/dctx_osx.m, src/draw2d/win/dctx_win.cpp, src/draw2d/win/draw2d_win.ixx, src/osgui/gtk/osview.c, src/osgui/osx/osview.m, src/osgui/win/osview.cpp, src/osgui/win/oswindow.c, src/osgui/win/oswindow_win.inl |
| r6937 | 2026-08-24 15:14 | Label with HiDPI working | 17 | 14 | demo/hello/main.c, src/draw2d/dctxh.h, src/draw2d/gtk/dctx_gtk.c, src/draw2d/osx/dctx_osx.m, src/draw2d/win/dctx_win.cpp, src/draw2d/win/draw2d_win.ixx, src/draw2d/win/draw_win.cpp, src/draw2d/win/osfont.cpp, src/osgui/gtk/osview.c, src/osgui/osx/osview.m, src/osgui/win/osview.cpp, src/osgui/win/oswindow.c, src/osgui/win/oswindow_win.inl |
| r6938 | 2026-08-24 15:36 | Default fonts DPI | 5 | 1 | demo/hello/main.c, src/draw2d/win/osfont.cpp |
| r6939 | 2026-08-24 16:25 | win32 osedit DPI update | 20 | 3 | demo/hello/main.c, src/osgui/win/oscontrol.cpp, src/osgui/win/osedit.c, src/osgui/win/osedit_win.inl, src/osgui/win/oswindow.c |
| r6940 | 2026-08-25 09:00 | WM_DPI_CHANGED | 10 | 42 | src/osgui/win/oswindow.c |
| r6941 | 2026-08-25 10:43 | osedit vpadding calculation DPI Aware | 15 | 45 | src/osgui/win/osedit.c |
| r6942 | 2026-08-25 10:44 | font_extents use real window DPI | 627 | 1629 | src/draw2d/font.c, src/draw2d/font.h, src/draw2d/font.inl, src/draw2d/gtk/osfont.c, src/draw2d/osx/osfont.m, src/draw2d/win/osfont.cpp, src/osgui/win/oswindow.c |
| r6943 | 2026-08-25 11:03 | Check HiDPI Slider and Progress | 0 | 0 | demo/hello/main.c |
| r6944 | 2026-08-25 12:08 | UpDown HiDPI | 35 | 17 | demo/hello/main.c, src/draw2d/guictx.c, src/draw2d/guictx.h, src/draw2d/guictx.hxx, src/gui/updown.c, src/osgui/gtk/osupdown.c, src/osgui/osguictx.c, src/osgui/osupdown.h, src/osgui/osx/osupdown.m, src/osgui/win/osupdown.c, src/osgui/win/oswindow.c |
| r6945 | 2026-08-25 15:59 | ospopup win32 HiDPI | 184 | 341 | demo/hello/main.c, src/osgui/win/oscombo.c, src/osgui/win/oscombo_win.inl, src/osgui/win/osimglist.c, src/osgui/win/osimglist.inl, src/osgui/win/ospopup.c, src/osgui/win/ospopup_win.inl, src/osgui/win/ostabs.c, src/osgui/win/oswindow.c |
| r6946 | 2026-08-25 16:48 | OSCombo HiDPI | 17 | 5 | demo/hello/main.c, src/osgui/win/oscombo.c, src/osgui/win/oscombo_win.inl, src/osgui/win/oswindow.c |
| r6947 | 2026-08-25 20:29 | font metrics to osfont | 106 | 170 | src/draw2d/font.c, src/draw2d/font.inl, src/draw2d/gtk/osfont.c, src/draw2d/osx/osfont.m, src/draw2d/win/osfont.cpp |
| r6948 | 2026-08-26 09:45 | Draw event in logical points | 24 | 105 | src/draw2d/dctxh.h, src/draw2d/gtk/dctx_gtk.c, src/draw2d/osx/dctx_osx.m, src/draw2d/win/dctx_win.cpp, src/gui/listbox.c, src/osgui/gtk/osview.c, src/osgui/osscrolls.c, src/osgui/osscrolls.inl, src/osgui/osx/osview.m, src/osgui/win/oslistener.c, src/osgui/win/oslistener.inl, src/osgui/win/ospanel.c, src/osgui/win/osview.cpp |
| r6949 | 2026-08-26 10:02 | Mouse events send positions in logical points. | 2 | 18 | src/osgui/win/oslistener.c |
| r6950 | 2026-08-26 10:39 | osdrawctrl adaptation to HiDPI | 21 | 30 | src/draw2d/dctxh.h, src/draw2d/gtk/dctx_gtk.c, src/draw2d/osx/dctx_osx.m, src/draw2d/win/dctx_win.cpp, src/osgui/win/osdrawctrl.cpp |
| r6951 | 2026-08-26 11:41 | osscrollbars dimensions HiDPI-Aware | 13 | 4 | src/osgui/win/osgui_win.cpp, src/osgui/win/osgui_win.inl, src/osgui/win/osscroll.c |
| r6952 | 2026-08-26 12:09 | oswindow AdjustWindowRect HiDPI Aware | 15 | 8 | src/osgui/win/osgui_win.cpp, src/osgui/win/osgui_win.inl, src/osgui/win/oswindow.c |
| r6953 | 2026-08-26 12:09 | Comments | 1 | 4 | src/osgui/win/oswindow.c |
| r6954 | 2026-08-26 13:28 | WM_DPICHANGED layout compose | 11 | 24 | src/gui/window.c, src/osgui/win/oswindow.c |
| r6955 | 2026-08-26 15:08 | DPI resizing with natural size | 2 | 8 | src/gui/window.c, src/osgui/win/oswindow.c |
| r6956 | 2026-08-26 16:10 | Win32 osbutton DPI-Aware | 11 | 23 | src/osgui/win/osbutton.c, src/osgui/win/osbutton_win.inl, src/osgui/win/oswindow.c |
| r6957 | 2026-08-26 16:53 | OSTabs win32 are now HiDPI-Aware | 10 | 2 | src/osgui/win/ostabs.c, src/osgui/win/ostabs_win.inl, src/osgui/win/oswindow.c |
| r6958 | 2026-08-27 10:44 | Listbox icons and checks DPI-Aware | 22 | 7 | src/draw2d/win/dctx_win.cpp, src/osgui/win/osbutton.c, src/osgui/win/osdrawctrl.cpp, src/osgui/win/osview.cpp, src/osgui/win/osview_win.inl, src/osgui/win/oswindow.c |
| r6959 | 2026-08-27 14:43 | Removed ekFPIXELS, ekFPOINTS, ekFCELL | 49 | 329 | demo/guihello/guihello.c, demo/guihello/labels.c, demo/guihello/textviews.c, doc/draw2d/en/draw2d.htm, doc/draw2d/en/font.htm, doc/draw2d/es/draw2d.htm, doc/draw2d/es/font.htm, doc/gui/en/textview.htm, doc/gui/es/textview.htm, src/draw2d/draw2d.hxx, src/draw2d/font.c, src/draw2d/font.h, src/draw2d/gtk/osfont.c, src/draw2d/guictx.hxx, src/draw2d/osx/osfont.m, src/draw2d/win/draw2d_win.ixx, src/draw2d/win/draw_win.cpp, src/draw2d/win/osfont.cpp, src/gui/textview.c, src/gui/textview.h, src/osgui/gtk/oscontrol.c, src/osgui/gtk/ostext.c, src/osgui/osx/ostext.m, src/osgui/win/osgui_win.cpp, src/osgui/win/osgui_win.inl, src/osgui/win/ostext.c |
| r6960 | 2026-08-27 14:44 | Removed fontunits demo | 0 | 163 | demo/guihello/fontunits.c, demo/guihello/fontunits.h |
| r6961 | 2026-08-27 14:55 | TextView DPI Aware | 0 | 4 | src/gui/textview.c, src/osgui/win/ostext.c |
| r6962 | 2026-08-27 20:08 | Fix osfont_extents with DPI | 1 | 2 | src/draw2d/win/osfont.cpp |
| r6963 | 2026-08-27 20:25 | TableView DPI-Aware | 1 | 0 | src/gui/tableview.c |
| r6964 | 2026-08-28 13:05 | Main windows position and sizes DPI-Aware | 46 | 44 | demo/hello/main.c, src/osgui/win/oscontrol.cpp, src/osgui/win/osglobals.c, src/osgui/win/osgui_win.cpp, src/osgui/win/osgui_win.inl, src/osgui/win/ospanel.c, src/osgui/win/ospanel_win.inl, src/osgui/win/oswindow.c |
| r6965 | 2026-08-28 15:26 | osupdown DPI-Aware | 9 | 1 | src/osgui/win/osupdown.c, src/osgui/win/osupdown_win.inl, src/osgui/win/oswindow.c |
| r6966 | 2026-08-28 16:18 | Updated THEMED_CHANGED in widgets | 45 | 0 | src/osgui/win/oscombo.c, src/osgui/win/ospopup.c, src/osgui/win/osprogress.c, src/osgui/win/osprogress_win.inl, src/osgui/win/osslider.c, src/osgui/win/osslider_win.inl, src/osgui/win/ostabs.c, src/osgui/win/oswindow.c |
| r6967 | 2026-08-28 17:01 | Panel scroll DPI Aware | 5 | 4 | src/osgui/win/oscontrol.cpp, src/osgui/win/ospanel.c, src/osgui/win/osscroll.c |
| r6968 | 2026-08-30 09:22 | PopUp menus and MenuBar DPI-Aware | 3 | 5 | src/osgui/win/osmenu.c, src/osgui/win/oswindow.c |
| r6969 | 2026-08-30 09:40 | Font width is now DPI-Aware | 2 | 1 | src/draw2d/win/osfont.cpp |
| r6970 | 2026-08-30 10:47 | SplitView is now DPi-Aware | 2 | 12 | src/osgui/win/ossplit.c |
| r6971 | 2026-08-30 10:57 | osweb is now DPI-Aware | 0 | 2 | src/osgui/win/osweb.cpp |
| r6972 | 2026-08-30 11:27 | Updated documentation | 31 | 3 | doc/draw2d/en/font.htm, doc/draw2d/es/font.htm, doc/gui/en/gui.htm, doc/gui/es/gui.htm |
| r6973 | 2026-08-30 12:07 | hidpi documentation image | 1 | 0 | doc/gui/gui.odg, doc/gui/img/hidpi.png |
| r6974 | 2026-08-30 12:46 | GUI HiDPI documentation | 69 | 1211 | doc/draw2d/en/dctx.htm, doc/draw2d/es/dctx.htm, doc/draw2d/img/noretina.png, doc/draw2d/img/retina_pixel.svg, doc/draw2d/img/retina_image.png, doc/draw2d/img/font_points.svg, doc/gui/en/gui.htm, doc/gui/en/view.htm, doc/gui/es/gui.htm, doc/gui/es/view.htm, doc/gui/img/noretina.png, doc/gui/img/retina_image.png, doc/gui/img/retina_pixel.svg |
| r6975 | 2026-08-30 12:57 | GUI points/pixels documentation issues. | 30 | 30 | doc/gui/en/combo.htm, doc/gui/en/label.htm, doc/gui/en/layout.htm, doc/gui/en/listbox.htm, doc/gui/en/panel.htm, doc/gui/en/popup.htm, doc/gui/en/tableview.htm, doc/gui/en/tabs.htm, doc/gui/es/combo.htm, doc/gui/es/label.htm, doc/gui/es/layout.htm, doc/gui/es/listbox.htm, doc/gui/es/panel.htm, doc/gui/es/popup.htm, doc/gui/es/tableview.htm, doc/gui/es/tabs.htm |
| r6976 | 2026-08-30 13:06 | OpenGL views are now DPI-Aware | 0 | 8 | demo/glhello/glhello.c, src/osgui/win/osview.cpp |
| r6977 | 2026-08-30 13:41 | Changelog | 10 | 0 | cicd/nappgui_src/Changelog.md |

## Summary

- **Revisions**: 51 (r6927 → r6977), from August 23, 2026, to August 30, 2026.
- **Lines added (total)**: 1,689.
- **Lines removed (total)**: 4,485 (the bulk of which corresponds to two major refactors: r6942 "font_extents use real window DPI" and r6974 "GUI HiDPI documentation," which rewrite/move entire blocks rather than adding net new code).
- **Authors**: `franWIN` (virtually all commits) and `franRED` (r6973, a single commit of a documentation screenshot from another machine).

*Data obtained directly from `svn log`/`svn diff` on `svn://192.168.1.2/svn/NAPPGUI/dev`, revisions r6927 through r6977 (HEAD at the time this report was generated).*


# History of the `/win_dpi_fail` branch in SVN

Repository: `svn://192.168.1.2/svn/NAPPGUI`

## Origin and scope of this report

`/win_dpi_fail` is not a branch created specifically for this failed attempt — it is the **former `/dev` branch**, renamed in place via `svn move ^/dev ^/win_dpi_fail` at **r6926**, once the HiDPI approach it contained was abandoned.

To keep this report focused, the range below starts at the **last "Merge from trunk" before any DPI-related work appears** (r6908), i.e. the actual HiDPI attempt begins at **r6909** ("font adds a dpi parameter") and ends at **r6926**, the rename/abandonment commit itself (a pure `svn move`, no content diff). This is the direct predecessor of the approach described in `SVN_dev.md`'s introduction ("a `scale` parameter traveling through the whole `gui` layer") — confirmed here by the later commits (r6924 `Parameter scale in _natural() funcs.`, r6925 `XXX_bounds() with scale parameter`) shortly before it was abandoned.

Range covered in this report: **r6909 (first HiDPI-related commit) → r6926 (rename to `win_dpi_fail`, approach abandoned)**, 18 commits.

## Commits in `/win_dpi_fail` (the abandoned HiDPI attempt)

| Revision | Date/Time | Log message | Lines added | Lines removed | Files affected |
|---|---|---|---:|---:|---|
| r6909 | 2026-08-06 21:17 | font adds a dpi parameter | 65 | 39 | src/draw2d/draw2d.h, src/draw2d/font.c, src/draw2d/font.h, src/draw2d/font.inl, src/draw2d/gtk/draw_gtk.c, src/draw2d/gtk/osfont.c, src/draw2d/osx/draw_osx.m, src/draw2d/osx/osfont.m, src/draw2d/win/draw_win.cpp, src/draw2d/win/draw_win.inl, src/draw2d/win/osfont.cpp, src/osgui/win/osgui_win.cpp, src/osgui/win/osgui_win.inl, src/osgui/win/ostext.c |
| r6910 | 2026-08-09 13:11 | osgui global win32 funcs for DPI | 27 | 1 | src/osgui/win/osgui_win.cpp, src/osgui/win/osgui_win.inl |
| r6911 | 2026-08-09 13:46 | Added gui_dpi_aware | 40 | 0 | src/draw2d/guictx.c, src/draw2d/guictx.h, src/draw2d/guictx.hxx, src/gui/gui.c, src/gui/gui.h, src/osgui/gtk/osglobals.c, src/osgui/osglobals.h, src/osgui/osguictx.c, src/osgui/osx/osglobals.m, src/osgui/win/osglobals.c, src/osgui/win/osgui_win.inl |
| r6912 | 2026-08-09 15:37 | Win32 activates DPI Aware with first windows is created. | 24 | 0 | src/osgui/win/osgui_win.cpp, src/osgui/win/osgui_win.inl, src/osgui/win/oswindow.c |
| r6913 | 2026-08-09 17:32 | Response to WM_DPICHANGED in oswindow | 14 | 0 | src/osgui/win/oswindow.c |
| r6914 | 2026-08-09 17:41 | oswindow update dpi when create. | 9 | 3 | src/osgui/win/oswindow.c |
| r6915 | 2026-08-09 18:28 | ekGUI_EVENT_WND_DPICHANGED event | 4 | 3 | src/draw2d/guictx.hxx, src/gui/window.c, src/osgui/win/oswindow.c |
| r6916 | 2026-08-09 19:17 | dpi_change. Event propagation skeleton. | 34 | 2 | src/gui/component.c, src/gui/component.inl, src/gui/gui.ixx, src/gui/imageview.c, src/gui/label.c, src/gui/listbox.c, src/gui/panel.c, src/gui/panel.inl, src/gui/tableview.c, src/gui/view.c, src/gui/view.inl, src/gui/window.c, src/gui/window.inl |
| r6917 | 2026-08-09 20:10 | dpi_change. Button, Edit, Combo update | 33 | 3 | src/gui/button.c, src/gui/button.inl, src/gui/combo.c, src/gui/combo.inl, src/gui/component.c, src/gui/edit.c, src/gui/edit.inl, src/gui/editimp.c, src/gui/editimp.inl |
| r6918 | 2026-08-09 20:12 | Label dpi_changed | 7 | 1 | src/gui/label.c |
| r6919 | 2026-08-09 20:14 | listbox dpi_change | 7 | 1 | src/gui/listbox.c |
| r6920 | 2026-08-09 20:16 | TableView dpi_change | 9 | 1 | src/gui/tableview.c |
| r6921 | 2026-08-09 20:28 | PopUp/Tabs dpi_change | 16 | 8 | src/gui/component.c, src/gui/popup.c, src/gui/popup.inl, src/gui/tabs.c, src/gui/tabs.inl |
| r6922 | 2026-08-10 20:13 | Added oswindow_scale_factor | 20 | 0 | src/draw2d/guictx.c, src/draw2d/guictx.h, src/draw2d/guictx.hxx, src/osgui/gtk/oswindow.c, src/osgui/osguictx.c, src/osgui/oswindow.h, src/osgui/osx/oswindow.m, src/osgui/win/oswindow.c |
| r6923 | 2026-08-10 20:17 | Remove _panel_compose() | 0 | 23 | src/gui/panel.c, src/gui/panel.inl |
| r6924 | 2026-08-10 20:46 | Parameter scale in _natural() funcs. | 56 | 59 | src/gui/button.c, src/gui/button.inl, src/gui/combo.c, src/gui/combo.inl, src/gui/component.c, src/gui/component.inl, src/gui/edit.c, src/gui/edit.inl, src/gui/gui.ixx, src/gui/label.c, src/gui/layout.c, src/gui/layout.inl, src/gui/line.c, src/gui/line.inl, src/gui/panel.c, src/gui/panel.inl, src/gui/popup.c, src/gui/popup.inl, src/gui/progress.c, src/gui/progress.inl, src/gui/slider.c, src/gui/slider.inl, src/gui/splitview.c, src/gui/splitview.inl, src/gui/tabs.c, src/gui/tabs.inl, src/gui/textview.c, src/gui/textview.inl, src/gui/updown.c, src/gui/updown.inl, src/gui/view.c, src/gui/view.inl, src/gui/webview.c, src/gui/webview.inl, src/gui/window.c, src/gui/window.inl |
| r6925 | 2026-08-23 11:27 | XXX_bounds() with scale parameter | 34 | 57 | src/draw2d/guictx.hxx, src/gui/button.c, src/gui/combo.c, src/gui/edit.c, src/gui/line.c, src/gui/popup.c, src/gui/slider.c, src/gui/tabs.c, src/osgui/gtk/osbutton.c, src/osgui/gtk/oscombo.c, src/osgui/gtk/osedit.c, src/osgui/gtk/osline.c, src/osgui/gtk/ospopup.c, src/osgui/gtk/osslider.c, src/osgui/gtk/ostabs.c, src/osgui/osbutton.h, src/osgui/oscombo.h, src/osgui/osedit.h, src/osgui/osline.h, src/osgui/ospopup.h, src/osgui/osslider.h, src/osgui/ostabs.h, src/osgui/osx/osbutton.m, src/osgui/osx/oscombo.m, src/osgui/osx/osedit.m, src/osgui/osx/osline.m, src/osgui/osx/ospopup.m, src/osgui/osx/osslider.m, src/osgui/osx/ostabs.m, src/osgui/win/osbutton.c, src/osgui/win/oscombo.c, src/osgui/win/osedit.c, src/osgui/win/osline.c, src/osgui/win/ospopup.c, src/osgui/win/osslider.c, src/osgui/win/ostabs.c |
| r6926 | 2026-08-23 12:16 | Rename dev to win_dpi_fail: HiDPI scale-parameter approach abandoned, restarting from trunk | 0 | 0 | *(rename `/dev` → `/win_dpi_fail`, no content diff)* |

## Summary

- **Commits**: 18 (r6909 → r6926), spanning 2026-08-06 to 2026-08-23 (with a 13-day gap between r6924 and r6925).
- **Lines added (total)**: 399.
- **Lines removed (total)**: 201.
- **Author**: `franWIN` for all 18 commits.
- **Why it was abandoned**: the approach threaded a `scale` parameter through the entire portable `gui` layer (`_natural()`, `_bounds()` functions across every widget, in all three backends — see r6924/r6925). This was judged to leak a Windows-specific, DPI-awareness concept into code that has no such notion on macOS/GTK, and was scrapped in favor of the "golden rule" adopted in `/dev`: confine all HiDPI logic to `osgui/win` and `draw2d/win`, keeping the portable layers untouched. See `SVN_dev.md` for the branch that replaced it.

*Data obtained directly from `svn log`/`svn diff` against `svn://192.168.1.2/svn/NAPPGUI/win_dpi_fail`, revisions r6909 to r6926.*
