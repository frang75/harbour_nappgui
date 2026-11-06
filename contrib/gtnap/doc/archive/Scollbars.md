# Commit report on `/dev` — r7194 to r7269

This report covers the requested range, from commit **r7194** to the most recent revision on the branch at the time this report was generated (**r7269**, repository HEAD at r7270 with no changes of its own).

- **Repository**: `svn://192.168.1.2/svn/NAPPGUI`
- **Branch**: `/dev`
- **Range**: r7194 – r7269 (73 revisions with own content; r7262, r7264 and r7268 do not exist in `/dev`, they correspond to commits on another branch/project in the same repository)
- **Actual period**: 2026-10-17 11:37 → 2026-10-29 21:33 (local time, UTC+2 through r7222 on 2026-10-24, UTC+1 from r7223 on 2026-10-25 onward — DST end)
- **Authors**: `franWIN`, `franOSX`, `franLIN`, `franRED`

**Totals for the period**: **13060 lines added**, **9310 lines removed**, across 73 revisions. *r7266 and r7269 (`franRED`) include large, non-representative line counts caused by end-of-line normalization rewriting whole files, not proportional content changes — see the per-revision detail below.*

## Commit table

| Revision | Date/Time | Author | Message | Lines added | Lines removed | Files affected (#) |
|---|---|---|---|---|---|---|
| r7194 | 2026-10-17 11:37:38 | franWIN | Testing scrollbar management in macOS | 180 | 25 | 4 |
| r7195 | 2026-10-17 12:26:57 | franOSX | Use VSCode to build/debug in macOS | 78 | 7 | 4 |
| r7196 | 2026-10-17 16:49:29 | franOSX | macOS ScrollerStyle notification | 139 | 22 | 10 |
| r7197 | 2026-10-17 16:56:07 | franOSX | Updated Linux context for scrollers | 324 | 81 | 2 |
| r7198 | 2026-10-17 16:59:36 | franOSX | Changelog | 8 | 0 | 1 |
| r7199 | 2026-10-17 17:05:39 | franLIN | Added Linux CMake presets | 44 | 0 | 1 |
| r7200 | 2026-10-17 17:06:41 | franOSX | Changelog | 1 | 1 | 1 |
| r7201 | 2026-10-17 17:45:13 | franLIN | GTK scroller mode capture notifications | 185 | 138 | 4 |
| r7202 | 2026-10-17 18:10:14 | franLIN | GTK3 scrollbars change when GNOME settings | 244 | 1 | 16 |
| r7203 | 2026-10-17 18:20:09 | franLIN | GTK3 fix issue in scrollbar dimensions | 6 | 2 | 1 |
| r7204 | 2026-10-17 18:26:01 | franLIN | GTK3 scrollers updated context | 24 | 9 | 1 |
| r7205 | 2026-10-18 08:32:15 | franWIN | VS2022/Ninja CMake Presets | 30 | 0 | 1 |
| r7206 | 2026-10-18 09:19:45 | franWIN | Automatic scrollbar space reservation | 40 | 3 | 1 |
| r7207 | 2026-10-18 09:30:07 | franWIN | Removed panel_scroll_size() from public API | 48 | 73 | 10 |
| r7208 | 2026-10-18 12:07:19 | franLIN | GTK3 scrollers adapt to changes from SO Settings. | 95 | 10 | 4 |
| r7209 | 2026-10-18 12:30:42 | franOSX | macOS scrolls updates to SO settings changes. | 40 | 26 | 2 |
| r7210 | 2026-10-18 12:50:42 | franWIN | Removed temporal log commands | 27 | 65 | 5 |
| r7211 | 2026-10-18 17:15:34 | franWIN | VSCode debug configurations in Windows | 166 | 157 | 6 |
| r7212 | 2026-10-23 16:01:20 | franWIN | Panel dynamic scrollbar narrowing. | 165 | 47 | 4 |
| r7213 | 2026-10-23 16:09:56 | franWIN | Panel has its own panel_flags_t set. | 34 | 27 | 5 |
| r7214 | 2026-10-23 16:20:39 | franWIN | Panel ekPANEL_SHRINK new flag to control scrollbar mode. | 17 | 15 | 3 |
| r7215 | 2026-10-23 16:27:16 | franWIN | Panel shrink in horizontal/vertical edges. | 28 | 18 | 3 |
| r7216 | 2026-10-23 17:02:55 | franWIN | Added new ScrollView demo | 50 | 5 | 3 |
| r7217 | 2026-10-23 17:06:12 | franWIN | Fix View scrollbar calculation | 7 | 0 | 1 |
| r7218 | 2026-10-23 17:29:21 | franWIN | View scrollbar fix in border cases. | 115 | 28 | 2 |
| r7219 | 2026-10-23 17:39:54 | franWIN | Scrollbars in View perfect working in Win32 | 181 | 9 | 2 |
| r7220 | 2026-10-24 08:32:09 | franWIN | Dynamic layouts demo update | 18 | 48 | 1 |
| r7221 | 2026-10-24 08:44:23 | franWIN | Changelog | 62 | 13 | 3 |
| r7222 | 2026-10-24 11:06:06 | franOSX | View scrollbars legacy/overlay working in macOS | 261 | 19 | 8 |
| r7223 | 2026-10-25 12:01:00 | franOSX | macOS scroller fade-out doesn't use NSTimer. | 245 | 76 | 14 |
| r7224 | 2026-10-25 12:26:51 | franOSX | macOS scroller minor fix | 13 | 1 | 2 |
| r7225 | 2026-10-25 13:04:20 | franOSX | Updated View scroll context | 12 | 0 | 1 |
| r7226 | 2026-10-25 16:10:00 | franLIN | GTK3 Scrollview build errors | 54 | 10 | 3 |
| r7227 | 2026-10-25 16:29:50 | franLIN | GTK3 overlay scroll in views | 102 | 43 | 5 |
| r7228 | 2026-10-25 17:28:41 | franLIN | GTK3 legacy scrollbars working | 153 | 32 | 5 |
| r7229 | 2026-10-25 18:06:02 | franLIN | GTK3 overlay scroll thin/thick behaviour | 86 | 6 | 6 |
| r7230 | 2026-10-25 18:18:40 | franLIN | GTK3 thick version no fadeout when hover | 7 | 0 | 1 |
| r7231 | 2026-10-25 18:37:56 | franLIN | Updated scrollbars memory | 92 | 5 | 1 |
| r7232 | 2026-10-25 18:47:06 | franLIN | Updated context for final tests in Win32, macOS, Linux | 48 | 0 | 2 |
| r7233 | 2026-10-26 10:17:43 | franWIN | TableView/ListBox doesn't reserve scrollbar spaces. | 13 | 18 | 2 |
| r7234 | 2026-10-26 10:33:44 | franWIN | _osscrolls_bar_reserve_width/height for effective scrollbar sizes. | 52 | 6 | 5 |
| r7235 | 2026-10-26 10:39:57 | franWIN | ListBox scrollbar space in drawing. | 4 | 27 | 3 |
| r7236 | 2026-10-26 11:37:07 | franWIN | Win32 force redraw scrollbar when dimensions change. | 1 | 1 | 1 |
| r7237 | 2026-10-26 11:47:36 | franWIN | Minor layout fix in Splitview demo | 1 | 1 | 1 |
| r7238 | 2026-10-26 11:50:54 | franWIN | Updated changelog | 2 | 0 | 1 |
| r7239 | 2026-10-26 12:07:10 | franWIN | Updated context with Win32 final tests | 96 | 16 | 1 |
| r7240 | 2026-10-26 12:40:34 | franOSX | macOS reveal overlay scrollbar when mouse moved | 74 | 0 | 3 |
| r7241 | 2026-10-26 13:03:00 | franOSX | Updated macOS context | 93 | 8 | 1 |
| r7242 | 2026-10-26 14:32:46 | franLIN | Final scrollbars tests in Linux. Updated context. | 30 | 37 | 2 |
| r7243 | 2026-10-26 15:18:08 | franLIN | GTK3 UpDown burst click event. | 141 | 3 | 9 |
| r7244 | 2026-10-26 15:31:27 | franLIN | GTK3 fix Slider position in Dynamic Layouts | 1 | 0 | 1 |
| r7245 | 2026-10-26 19:44:12 | franLIN | macOS horizontal scroll with Shift+Wheel | 106 | 12 | 4 |
| r7246 | 2026-10-26 20:06:12 | franOSX | Fix macOS shift + wheel capture | 56 | 11 | 2 |
| r7247 | 2026-10-26 20:22:20 | franLIN | GTK3 horizontal scroll with Shift+Wheel | 39 | 11 | 3 |
| r7248 | 2026-10-26 20:26:52 | franLIN | Updated context for horizontal scroll Shift+Wheel | 62 | 10 | 1 |
| r7249 | 2026-10-26 20:34:52 | franWIN | Win32 support for Shift+Wheel horizontal scrolling. | 30 | 10 | 3 |
| r7250 | 2026-10-26 20:37:57 | franWIN | Updated context. | 25 | 0 | 1 |
| r7251 | 2026-10-26 21:11:33 | franLIN | GTK3 fix issue scrollbar legacy overlaps | 115 | 3 | 3 |
| r7252 | 2026-10-26 21:51:42 | franLIN | Updated scrollpanel demo | 16 | 1 | 1 |
| r7253 | 2026-10-27 20:46:10 | franLIN | Updated scrollpanel demo | 1 | 0 | 1 |
| r7254 | 2026-10-27 21:11:44 | franLIN | GTK3 fix PushButton vertical text centering. | 30 | 13 | 2 |
| r7255 | 2026-10-27 22:07:29 | franLIN | GTK3 fix view onMoved events when resizing. | 156 | 37 | 2 |
| r7256 | 2026-10-28 11:40:24 | franOSX | Updated context with macOS Golden Gate final tests | 44 | 7 | 1 |
| r7257 | 2026-10-28 14:49:24 | franWIN | ndoc: gui i18n and Dark modes | 143 | 34 | 5 |
| r7258 | 2026-10-28 19:47:53 | franWIN | ndoc. Panel scrollbar documentation | 111 | 27 | 6 |
| r7259 | 2026-10-28 20:15:48 | franWIN | ndoc. Updated View with scrollbars documentation | 54 | 4 | 3 |
| r7260 | 2026-10-28 20:24:45 | franWIN | Updated changelog | 4 | 4 | 1 |
| r7261 | 2026-10-28 20:29:32 | franWIN | Final context updated. All work done. | 23 | 8 | 1 |
| r7263 | 2026-10-29 09:27:08 | franLIN | Fix GCC warnings | 81 | 2 | 4 |
| r7265 | 2026-10-29 21:00:49 | franRED | Fix GTK ostabs event. | 1 | 1 | 1 |
| r7266 | 2026-10-29 21:07:41 | franRED | ListBox, TableView OnWheel hover. | 7046 | 6994 | 10 |
| r7267 | 2026-10-29 21:19:39 | franWIN | Changelog | 6 | 7 | 1 |
| r7269 | 2026-10-29 21:33:32 | franRED | Minor fix | 974 | 975 | 1 |

## File detail per revision

### r7194 — Testing scrollbar management in macOS
*2026-10-17 11:37:38 · franWIN · +180/-25 · 4 files*

- `internal/scroll_gutter_macos_handoff.md`
- `src/osapp/osx/osapp_osx.m`
- `src/osgui/gtk/ospanel.c`
- `src/osgui/osx/ospanel.m`

### r7195 — Use VSCode to build/debug in macOS
*2026-10-17 12:26:57 · franOSX · +78/-7 · 4 files*

- `.claude`
- `.claude/settings.local.json`
- `.vscode/launch.json`
- `CMakePresets.json`

### r7196 — macOS ScrollerStyle notification
*2026-10-17 16:49:29 · franOSX · +139/-22 · 10 files*

- `doc/gui/en/gui.htm`
- `doc/gui/es/gui.htm`
- `src/gui/gui.c`
- `src/gui/gui.h`
- `src/osapp/gtk/osapp_gtk.c`
- `src/osapp/osapp.c`
- `src/osapp/osapp.inl`
- `src/osapp/osx/osapp_osx.m`
- `src/osapp/win/osapp_win.c`
- `src/osgui/osx/ospanel.m`

### r7197 — Updated Linux context for scrollers
*2026-10-17 16:56:07 · franOSX · +324/-81 · 2 files*

- `internal/scroll_gutter_linux_handoff.md`
- `internal/scroll_gutter_macos_handoff.md`

### r7198 — Changelog
*2026-10-17 16:59:36 · franOSX · +8/-0 · 1 file*

- `cicd/nappgui_src/Changelog.md`

### r7199 — Added Linux CMake presets
*2026-10-17 17:05:39 · franLIN · +44/-0 · 1 file*

- `CMakePresets.json`

### r7200 — Changelog
*2026-10-17 17:06:41 · franOSX · +1/-1 · 1 file*

- `cicd/nappgui_src/Changelog.md`

### r7201 — GTK scroller mode capture notifications
*2026-10-17 17:45:13 · franLIN · +185/-138 · 4 files*

- `internal/scroll_gutter_linux_handoff.md`
- `internal/scroll_gutter_macos_handoff.md`
- `src/osapp/gtk/osapp_gtk.c`
- `src/osgui/gtk/ospanel.c`

### r7202 — GTK3 scrollbars change when GNOME settings
*2026-10-17 18:10:14 · franLIN · +244/-1 · 16 files*

- `internal/scroll_gutter_linux_handoff.md`
- `src/draw2d/guictx.c`
- `src/draw2d/guictx.h`
- `src/draw2d/guictx.hxx`
- `src/gui/component.c`
- `src/gui/component.inl`
- `src/gui/gui.c`
- `src/gui/panel.c`
- `src/gui/panel.inl`
- `src/gui/window.c`
- `src/gui/window.inl`
- `src/osgui/gtk/ospanel.c`
- `src/osgui/osguictx.c`
- `src/osgui/ospanel.h`
- `src/osgui/osx/ospanel.m`
- `src/osgui/win/ospanel.c`

### r7203 — GTK3 fix issue in scrollbar dimensions
*2026-10-17 18:20:09 · franLIN · +6/-2 · 1 file*

- `src/osgui/gtk/ospanel.c`

### r7204 — GTK3 scrollers updated context
*2026-10-17 18:26:01 · franLIN · +24/-9 · 1 file*

- `internal/scroll_gutter_linux_handoff.md`

### r7205 — VS2022/Ninja CMake Presets
*2026-10-18 08:32:15 · franWIN · +30/-0 · 1 file*

- `CMakePresets.json`

### r7206 — Automatic scrollbar space reservation
*2026-10-18 09:19:45 · franWIN · +40/-3 · 1 file*

- `src/gui/panel.c`

### r7207 — Removed panel_scroll_size() from public API
*2026-10-18 09:30:07 · franWIN · +48/-73 · 10 files*

- `cicd/nappgui_src/Changelog.md`
- `demo/guihello/scrollpanel.c`
- `demo/guihello/splits.c`
- `doc/gui/en/panel.htm`
- `doc/gui/es/panel.htm`
- `internal/scroll_gutter_linux_handoff.md`
- `internal/scroll_gutter_macos_handoff.md`
- `src/gui/panel.c`
- `src/gui/panel.h`
- `tools/designer/propedit.c`

### r7208 — GTK3 scrollers adapt to changes from SO Settings.
*2026-10-18 12:07:19 · franLIN · +95/-10 · 4 files*

- `internal/scroll_gutter_linux_handoff.md`
- `src/gui/gui.c`
- `src/osapp/gtk/osapp_gtk.c`
- `src/osgui/gtk/ospanel.c`

### r7209 — macOS scrolls updates to SO settings changes.
*2026-10-18 12:30:42 · franOSX · +40/-26 · 2 files*

- `internal/scroll_gutter_macos_handoff.md`
- `src/osgui/osx/ospanel.m`

### r7210 — Removed temporal log commands
*2026-10-18 12:50:42 · franWIN · +27/-65 · 5 files*

- `internal/scroll_gutter_linux_handoff.md`
- `internal/scroll_gutter_macos_handoff.md`
- `src/osapp/osapp.c`
- `src/osapp/osx/osapp_osx.m`
- `src/osgui/osx/ospanel.m`

### r7211 — VSCode debug configurations in Windows
*2026-10-18 17:15:34 · franWIN · +166/-157 · 6 files*

- `.vscode/launch.json`
- `.vscode/settings.json`
- `CMakePresets.json`
- `config`
- `config/vscode_settings.md`
- `src/sewer/win/bstdimp.c`

### r7212 — Panel dynamic scrollbar narrowing.
*2026-10-23 16:01:20 · franWIN · +165/-47 · 4 files*

- `CLAUDE.md`
- `demo/guihello/scrollpanel.c`
- `src/gui/panel.c`
- `src/gui/panel.h`

### r7213 — Panel has its own panel_flags_t set.
*2026-10-23 16:09:56 · franWIN · +34/-27 · 5 files*

- `src/draw2d/guictx.hxx`
- `src/gui/panel.c`
- `src/osgui/gtk/ospanel.c`
- `src/osgui/osx/ospanel.m`
- `src/osgui/win/ospanel.c`

### r7214 — Panel ekPANEL_SHRINK new flag to control scrollbar mode.
*2026-10-23 16:20:39 · franWIN · +17/-15 · 3 files*

- `src/draw2d/guictx.hxx`
- `src/gui/panel.c`
- `src/gui/panel.h`

### r7215 — Panel shrink in horizontal/vertical edges.
*2026-10-23 16:27:16 · franWIN · +28/-18 · 3 files*

- `src/draw2d/guictx.hxx`
- `src/gui/panel.c`
- `src/gui/panel.h`

### r7216 — Added new ScrollView demo
*2026-10-23 17:02:55 · franWIN · +50/-5 · 3 files*

- `demo/guihello/guihello.c`
- `demo/guihello/scrollview.c`
- `demo/guihello/scrollview.h`

### r7217 — Fix View scrollbar calculation
*2026-10-23 17:06:12 · franWIN · +7/-0 · 1 file*

- `src/osgui/osscrolls.c`

### r7218 — View scrollbar fix in border cases.
*2026-10-23 17:29:21 · franWIN · +115/-28 · 2 files*

- `demo/guihello/scrollview.c`
- `src/osgui/osscrolls.c`

### r7219 — Scrollbars in View perfect working in Win32
*2026-10-23 17:39:54 · franWIN · +181/-9 · 2 files*

- `demo/guihello/scrollview.c`
- `internal/scrollview_handoff.md`

### r7220 — Dynamic layouts demo update
*2026-10-24 08:32:09 · franWIN · +18/-48 · 1 file*

- `demo/guihello/dynlay.c`

### r7221 — Changelog
*2026-10-24 08:44:23 · franWIN · +62/-13 · 3 files*

- `CLAUDE.md`
- `cicd/nappgui_src/Changelog.md`
- `demo/guihello/dynlay.c`

### r7222 — View scrollbars legacy/overlay working in macOS
*2026-10-24 11:06:06 · franOSX · +261/-19 · 8 files*

- `internal/scrollview_handoff.md`
- `src/osgui/osscroll.inl`
- `src/osgui/osscrolls.c`
- `src/osgui/osscrolls.inl`
- `src/osgui/osx/osscroll.m`
- `src/osgui/osx/osview.m`
- `src/osgui/osx/osview_osx.inl`
- `src/osgui/win/osscroll.c`

### r7223 — macOS scroller fade-out doesn't use NSTimer.
*2026-10-25 12:01:00 · franOSX · +245/-76 · 14 files*

- `internal/scrollview_handoff.md`
- `src/draw2d/guictx.c`
- `src/draw2d/guictx.h`
- `src/draw2d/guictx.hxx`
- `src/gui/view.c`
- `src/osgui/osguictx.c`
- `src/osgui/osscroll.inl`
- `src/osgui/osscrolls.c`
- `src/osgui/osscrolls.inl`
- `src/osgui/osview.h`
- `src/osgui/osx/osscroll.m`
- `src/osgui/osx/osview.m`
- `src/osgui/win/osscroll.c`
- `src/osgui/win/osview.cpp`

### r7224 — macOS scroller minor fix
*2026-10-25 12:26:51 · franOSX · +13/-1 · 2 files*

- `internal/scrollview_handoff.md`
- `src/osgui/osx/osscroll.m`

### r7225 — Updated View scroll context
*2026-10-25 13:04:20 · franOSX · +12/-0 · 1 file*

- `internal/scrollview_handoff.md`

### r7226 — GTK3 Scrollview build errors
*2026-10-25 16:10:00 · franLIN · +54/-10 · 3 files*

- `internal/scrollview_handoff.md`
- `src/osgui/gtk/osscroll.c`
- `src/osgui/gtk/osview.c`

### r7227 — GTK3 overlay scroll in views
*2026-10-25 16:29:50 · franLIN · +102/-43 · 5 files*

- `src/osgui/gtk/osglobals.c`
- `src/osgui/gtk/osglobals_gtk.inl`
- `src/osgui/gtk/osscroll.c`
- `src/osgui/gtk/osview.c`
- `src/osgui/gtk/osview_gtk.inl`

### r7228 — GTK3 legacy scrollbars working
*2026-10-25 17:28:41 · franLIN · +153/-32 · 5 files*

- `internal/scrollview_handoff.md`
- `src/osgui/gtk/osscroll.c`
- `src/osgui/gtk/osview.c`
- `src/osgui/osscrolls.c`
- `src/osgui/osscrolls.inl`

### r7229 — GTK3 overlay scroll thin/thick behaviour
*2026-10-25 18:06:02 · franLIN · +86/-6 · 6 files*

- `src/gui/view.c`
- `src/osgui/gtk/osscroll.c`
- `src/osgui/gtk/osview.c`
- `src/osgui/gtk/osview_gtk.inl`
- `src/osgui/osscrolls.c`
- `src/osgui/osscrolls.inl`

### r7230 — GTK3 thick version no fadeout when hover
*2026-10-25 18:18:40 · franLIN · +7/-0 · 1 file*

- `src/osgui/gtk/osscroll.c`

### r7231 — Updated scrollbars memory
*2026-10-25 18:37:56 · franLIN · +92/-5 · 1 file*

- `internal/scrollview_handoff.md`

### r7232 — Updated context for final tests in Win32, macOS, Linux
*2026-10-25 18:47:06 · franLIN · +48/-0 · 2 files*

- `internal/scrollview_handoff.md`
- `src/gui/view.c`

### r7233 — TableView/ListBox doesn't reserve scrollbar spaces.
*2026-10-26 10:17:43 · franWIN · +13/-18 · 2 files*

- `src/gui/scrollview.c`
- `src/gui/tableview.c`

### r7234 — _osscrolls_bar_reserve_width/height for effective scrollbar sizes.
*2026-10-26 10:33:44 · franWIN · +52/-6 · 5 files*

- `src/osgui/gtk/osview.c`
- `src/osgui/osscrolls.c`
- `src/osgui/osscrolls.inl`
- `src/osgui/osx/osview.m`
- `src/osgui/win/osview.cpp`

### r7235 — ListBox scrollbar space in drawing.
*2026-10-26 10:39:57 · franWIN · +4/-27 · 3 files*

- `src/gui/listbox.c`
- `src/gui/scrollview.c`
- `src/gui/scrollview.inl`

### r7236 — Win32 force redraw scrollbar when dimensions change.
*2026-10-26 11:37:07 · franWIN · +1/-1 · 1 file*

- `src/osgui/win/osscroll.c`

### r7237 — Minor layout fix in Splitview demo
*2026-10-26 11:47:36 · franWIN · +1/-1 · 1 file*

- `demo/guihello/splits.c`

### r7238 — Updated changelog
*2026-10-26 11:50:54 · franWIN · +2/-0 · 1 file*

- `cicd/nappgui_src/Changelog.md`

### r7239 — Updated context with Win32 final tests
*2026-10-26 12:07:10 · franWIN · +96/-16 · 1 file*

- `internal/scrollview_handoff.md`

### r7240 — macOS reveal overlay scrollbar when mouse moved
*2026-10-26 12:40:34 · franOSX · +74/-0 · 3 files*

- `src/osgui/osx/osscroll.m`
- `src/osgui/osx/osview.m`
- `src/osgui/osx/osview_osx.inl`

### r7241 — Updated macOS context
*2026-10-26 13:03:00 · franOSX · +93/-8 · 1 file*

- `internal/scrollview_handoff.md`

### r7242 — Final scrollbars tests in Linux. Updated context.
*2026-10-26 14:32:46 · franLIN · +30/-37 · 2 files*

- `internal/scrollview_handoff.md`
- `src/gui/view.c`

### r7243 — GTK3 UpDown burst click event.
*2026-10-26 15:18:08 · franLIN · +141/-3 · 9 files*

- `src/draw2d/guictx.c`
- `src/draw2d/guictx.h`
- `src/draw2d/guictx.hxx`
- `src/gui/updown.c`
- `src/osgui/gtk/osupdown.c`
- `src/osgui/osguictx.c`
- `src/osgui/osupdown.h`
- `src/osgui/osx/osupdown.m`
- `src/osgui/win/osupdown.c`

### r7244 — GTK3 fix Slider position in Dynamic Layouts
*2026-10-26 15:31:27 · franLIN · +1/-0 · 1 file*

- `src/osgui/gtk/osslider.c`

### r7245 — macOS horizontal scroll with Shift+Wheel
*2026-10-26 19:44:12 · franLIN · +106/-12 · 4 files*

- `internal/scrollview_handoff.md`
- `src/osgui/osx/osscroll.m`
- `src/osgui/osx/osscroll_osx.inl`
- `src/osgui/osx/osview.m`

### r7246 — Fix macOS shift + wheel capture
*2026-10-26 20:06:12 · franOSX · +56/-11 · 2 files*

- `internal/scrollview_handoff.md`
- `src/osgui/osx/ospanel.m`

### r7247 — GTK3 horizontal scroll with Shift+Wheel
*2026-10-26 20:22:20 · franLIN · +39/-11 · 3 files*

- `src/osgui/gtk/osscroll.c`
- `src/osgui/gtk/osscroll_gtk.inl`
- `src/osgui/gtk/osview.c`

### r7248 — Updated context for horizontal scroll Shift+Wheel
*2026-10-26 20:26:52 · franLIN · +62/-10 · 1 file*

- `internal/scrollview_handoff.md`

### r7249 — Win32 support for Shift+Wheel horizontal scrolling.
*2026-10-26 20:34:52 · franWIN · +30/-10 · 3 files*

- `internal/scrollview_handoff.md`
- `src/osgui/win/ospanel.c`
- `src/osgui/win/osview.cpp`

### r7250 — Updated context.
*2026-10-26 20:37:57 · franWIN · +25/-0 · 1 file*

- `internal/scrollview_handoff.md`

### r7251 — GTK3 fix issue scrollbar legacy overlaps
*2026-10-26 21:11:33 · franLIN · +115/-3 · 3 files*

- `cicd/nappgui_src/Changelog.md`
- `internal/scrollview_handoff.md`
- `src/osgui/gtk/ospanel.c`

### r7252 — Updated scrollpanel demo
*2026-10-26 21:51:42 · franLIN · +16/-1 · 1 file*

- `demo/guihello/scrollpanel.c`

### r7253 — Updated scrollpanel demo
*2026-10-27 20:46:10 · franLIN · +1/-0 · 1 file*

- `demo/guihello/scrollpanel.c`

### r7254 — GTK3 fix PushButton vertical text centering.
*2026-10-27 21:11:44 · franLIN · +30/-13 · 2 files*

- `internal/scrollview_handoff.md`
- `src/osgui/gtk/osbutton.c`

### r7255 — GTK3 fix view onMoved events when resizing.
*2026-10-27 22:07:29 · franLIN · +156/-37 · 2 files*

- `internal/scrollview_handoff.md`
- `src/osgui/gtk/osview.c`

### r7256 — Updated context with macOS Golden Gate final tests
*2026-10-28 11:40:24 · franOSX · +44/-7 · 1 file*

- `internal/scrollview_handoff.md`

### r7257 — ndoc: gui i18n and Dark modes
*2026-10-28 14:49:24 · franWIN · +143/-34 · 5 files*

- `CLAUDE.md`
- `doc/gui/en/gui.htm`
- `doc/gui/es/gui.htm`
- `doc/gui/img/darkmode.gif`
- `doc/gui/img/darkmode.png`

### r7258 — ndoc. Panel scrollbar documentation
*2026-10-28 19:47:53 · franWIN · +111/-27 · 6 files*

- `doc/gui/en/panel.htm`
- `doc/gui/es/panel.htm`
- `doc/gui/img/legacy_scrollbars_narrow.gif`
- `doc/gui/img/legacy_scrollbars_narrow.png`
- `doc/gui/img/overlay_vs_legacy_scrollbars.gif`
- `doc/gui/img/overlay_vs_legacy_scrollbars.png`

### r7259 — ndoc. Updated View with scrollbars documentation
*2026-10-28 20:15:48 · franWIN · +54/-4 · 3 files*

- `doc/gui/en/view.htm`
- `doc/gui/es/view.htm`
- `internal/scrollview_handoff.md`

### r7260 — Updated changelog
*2026-10-28 20:24:45 · franWIN · +4/-4 · 1 file*

- `cicd/nappgui_src/Changelog.md`

### r7261 — Final context updated. All work done.
*2026-10-28 20:29:32 · franWIN · +23/-8 · 1 file*

- `internal/scrollview_handoff.md`

### r7263 — Fix GCC warnings
*2026-10-29 09:27:08 · franLIN · +81/-2 · 4 files*

- `CLAUDE.md`
- `CMakePresets.json`
- `src/gui/panel.c`
- `src/osgui/osscrolls.c`

### r7265 — Fix GTK ostabs event.
*2026-10-29 21:00:49 · franRED · +1/-1 · 1 file*

- `src/osgui/gtk/ostabs.c`

### r7266 — ListBox, TableView OnWheel hover.
*2026-10-29 21:07:41 · franRED · +7046/-6994 · 10 files*

- `cicd/nappgui_src/Changelog.md`
- `doc/gui/en/gui.htm`
- `doc/gui/es/gui.htm`
- `src/draw2d/guictx.hxx`
- `src/gui/listbox.c`
- `src/gui/tableview.c`
- `src/osgui/gtk/oslistener.c`
- `src/osgui/gtk/osview.c`
- `src/osgui/osx/oslistener.m`
- `src/osgui/win/oslistener.c`

### r7267 — Changelog
*2026-10-29 21:19:39 · franWIN · +6/-7 · 1 file*

- `cicd/nappgui_src/Changelog.md`

### r7269 — Minor fix
*2026-10-29 21:33:32 · franRED · +974/-975 · 1 file*

- `src/osgui/gtk/osview.c`
