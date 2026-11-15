# Commit report on `/dev` — r7146 to r7191

This report covers the requested range, from commit **r7146** to the most recent revision on the branch at the time this report was generated (**r7191**, repository HEAD at r7192 with no changes of its own).

- **Repository**: `svn://192.168.1.2/svn/NAPPGUI`
- **Branch**: `/dev`
- **Range**: r7146 – r7191 (40 revisions with own content; r7151, r7152, r7153, r7154, r7188 and r7190 do not exist in `/dev`, they correspond to commits on another branch/`trunk`; r7145 excluded per request)
- **Actual period**: 2026-10-07 10:24 → 2026-10-15 09:55 (local time, UTC+2)
- **Authors**: `franLIN` (all revisions in this range)

**Totals for the period**: **1313 lines added**, **2722 lines removed**, across 40 revisions.

## Commit table

| Revision | Date/Time | Author | Message | Lines added | Lines removed | Files affected (#) |
|---|---|---|---|---|---|---|
| r7146 | 2026-10-07 10:24:17 | franLIN | GTk3 EditBox fix issue when width reduces < 20px. | 146 | 26 | 5 |
| r7147 | 2026-10-07 12:00:58 | franLIN | GTk3 EditBox multiline fix issue when width reduces < 45px. | 0 | 0 | 1 |
| r7148 | 2026-10-07 14:29:34 | franLIN | GTk3 GtkButton fix issue when width reduces < 45px. | 5 | 6 | 3 |
| r7149 | 2026-10-07 19:22:04 | franLIN | GTk3 GtkCheck/Radio fix issue when width reduces < 45px. | 14 | 13 | 4 |
| r7150 | 2026-10-07 19:32:57 | franLIN | GTK3 Pushbutton reduce with ellipsis. | 5 | 3 | 1 |
| r7155 | 2026-10-12 11:34:33 | franLIN | GTK3 ComboBox reduce reduce with ellipsis. | 13 | 0 | 2 |
| r7156 | 2026-10-12 13:00:55 | franLIN | GTK3 Placeholder widgets for squeeze | 77 | 34 | 6 |
| r7157 | 2026-10-12 13:44:07 | franLIN | GTK3 placeholder lacy creation | 46 | 40 | 5 |
| r7158 | 2026-10-12 15:18:16 | franLIN | GTK multiline editbox fix height bound. | 0 | 9 | 1 |
| r7159 | 2026-10-12 16:31:58 | franLIN | GTK OSEdit multiline placeholder working | 19 | 12 | 4 |
| r7160 | 2026-10-12 17:03:20 | franLIN | GTK3 pushbutton placeholder | 56 | 0 | 7 |
| r7161 | 2026-10-12 18:36:21 | franLIN | GTK3 CheckBox/RadioButton placeholders | 107 | 2 | 4 |
| r7162 | 2026-10-12 20:01:21 | franLIN | GTK3 Popup placeholder. | 70 | 42 | 8 |
| r7163 | 2026-10-12 21:42:26 | franLIN | GTK3 Slider placeholder | 133 | 8 | 12 |
| r7164 | 2026-10-12 21:53:17 | franLIN | osprogress_bounds implementation | 9 | 39 | 10 |
| r7165 | 2026-10-12 22:02:09 | franLIN | GTK3 progress fix in height measure | 1 | 8 | 1 |
| r7166 | 2026-10-12 22:33:49 | franLIN | GTK3 progress reduce | 50 | 5 | 5 |
| r7167 | 2026-10-12 22:36:53 | franLIN | GTK3 remove unused OSqueezeLevel API | 0 | 61 | 3 |
| r7168 | 2026-10-12 22:41:00 | franLIN | Remove GTK issues memory | 0 | 1620 | 1 |
| r7169 | 2026-10-13 08:27:43 | franLIN | GTK3 Improved View focused frame | 17 | 12 | 3 |
| r7170 | 2026-10-13 08:35:22 | franLIN | GTK3 TextView improved focus frame. | 9 | 45 | 3 |
| r7171 | 2026-10-13 09:32:19 | franLIN | PopUp improved bounds calculation | 17 | 32 | 9 |
| r7172 | 2026-10-13 10:12:11 | franLIN | ndoc Improved GTK3 documentation and backends section | 139 | 123 | 18 |
| r7173 | 2026-10-13 10:15:01 | franLIN | Change API menu link in NAppGUI website | 0 | 1 | 1 |
| r7174 | 2026-10-13 10:30:27 | franLIN | ndoc GTK3 Window Managers docu. | 20 | 0 | 2 |
| r7175 | 2026-10-13 10:34:16 | franLIN | ndoc backends Win32/AppKit simple descriptions. | 8 | 0 | 2 |
| r7176 | 2026-10-13 13:13:59 | franLIN | Document X11 issues  in "hot" scale factor changes. | 2 | 2 | 2 |
| r7177 | 2026-10-13 15:43:53 | franLIN | HiDPI working on Linux-Wayland. | 36 | 0 | 1 |
| r7178 | 2026-10-13 16:08:49 | franLIN | GTK3 Wayland force draw focus ring while resizing (Wayland error) | 29 | 8 | 4 |
| r7179 | 2026-10-13 20:10:45 | franLIN | HiDPI testing apps GTK3 | 177 | 0 | 2 |
| r7180 | 2026-10-14 08:58:28 | franLIN | GTK3 placeholder-->filler | 9 | 100 | 8 |
| r7181 | 2026-10-14 18:09:54 | franLIN | GTK3 Filler API review. | 3 | 72 | 8 |
| r7182 | 2026-10-14 18:32:23 | franLIN | GTK3 Button update to new OSFiller | 2 | 105 | 2 |
| r7183 | 2026-10-14 18:45:28 | franLIN | GTK OSButton/OSEdit change placeholder by filler | 15 | 33 | 3 |
| r7184 | 2026-10-14 19:56:11 | franLIN | GTK OSPopup improved filler draw function. | 28 | 116 | 4 |
| r7185 | 2026-10-14 20:24:53 | franLIN | GTK OSCombo uses filler feature | 38 | 27 | 5 |
| r7186 | 2026-10-14 20:33:14 | franLIN | GTK UpDown improved arrow drawing | 0 | 8 | 2 |
| r7187 | 2026-10-14 20:42:44 | franLIN | GTK3 progress reduce to the minimum | 0 | 78 | 3 |
| r7189 | 2026-10-15 08:01:17 | franLIN | Fix build Release warnings | 12 | 32 | 3 |
| r7191 | 2026-10-15 09:55:56 | franLIN | Fix warning | 1 | 0 | 1 |
| **Total** | | | **40 revisions** | **1313** | **2722** | |

## File detail per revision

### r7146 — GTk3 EditBox fix issue when width reduces < 20px.
*2026-10-07 10:24:17 · franLIN · +146/-26 · 5 files*

- `internal/glhello_graphics_connectors.md`
- `internal/gtk_issues.md`
- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/oscontrol_gtk.inl`
- `src/osgui/gtk/osedit.c`

### r7147 — GTk3 EditBox multiline fix issue when width reduces < 45px.
*2026-10-07 12:00:58 · franLIN · +0/-0 · 1 file*

- `src/osgui/gtk/osedit.c`

### r7148 — GTk3 GtkButton fix issue when width reduces < 45px.
*2026-10-07 14:29:34 · franLIN · +5/-6 · 3 files*

- `src/osgui/gtk/osbutton.c`
- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/osedit.c`

### r7149 — GTk3 GtkCheck/Radio fix issue when width reduces < 45px.
*2026-10-07 19:22:04 · franLIN · +14/-13 · 4 files*

- `src/osgui/gtk/osbutton.c`
- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/oscontrol_gtk.inl`
- `src/osgui/gtk/osedit.c`

### r7150 — GTK3 Pushbutton reduce with ellipsis.
*2026-10-07 19:32:57 · franLIN · +5/-3 · 1 file*

- `src/osgui/gtk/osbutton.c`

### r7155 — GTK3 ComboBox reduce reduce with ellipsis.
*2026-10-12 11:34:33 · franLIN · +13/-0 · 2 files*

- `internal/gtk_issues.md`
- `src/osgui/gtk/ospopup.c`

### r7156 — GTK3 Placeholder widgets for squeeze
*2026-10-12 13:00:55 · franLIN · +77/-34 · 6 files*

- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/oscontrol_gtk.inl`
- `src/osgui/gtk/osedit.c`
- `src/osgui/gtk/osentry.c`
- `src/osgui/gtk/osentry_gtk.inl`
- `src/osgui/gtk/osgui_gtk.ixx`

### r7157 — GTK3 placeholder lacy creation
*2026-10-12 13:44:07 · franLIN · +46/-40 · 5 files*

- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/oscontrol_gtk.inl`
- `src/osgui/gtk/osedit.c`
- `src/osgui/gtk/osedit_gtk.inl`
- `src/osgui/gtk/osgui_gtk.ixx`

### r7158 — GTK multiline editbox fix height bound.
*2026-10-12 15:18:16 · franLIN · +0/-9 · 1 file*

- `src/osgui/gtk/osedit.c`

### r7159 — GTK OSEdit multiline placeholder working
*2026-10-12 16:31:58 · franLIN · +19/-12 · 4 files*

- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/oscontrol_gtk.inl`
- `src/osgui/gtk/osedit.c`
- `src/osgui/gtk/osgui_gtk.ixx`

### r7160 — GTK3 pushbutton placeholder
*2026-10-12 17:03:20 · franLIN · +56/-0 · 7 files*

- `src/osgui/gtk/osbutton.c`
- `src/osgui/gtk/osbutton_gtk.inl`
- `src/osgui/gtk/oscontrol.c`
- `src/osgui/osbutton.inl`
- `src/osgui/ostabstop.c`
- `src/osgui/osx/osbutton.m`
- `src/osgui/win/osbutton.c`

### r7161 — GTK3 CheckBox/RadioButton placeholders
*2026-10-12 18:36:21 · franLIN · +107/-2 · 4 files*

- `internal/gtk_issues.md`
- `src/osgui/gtk/osbutton.c`
- `src/osgui/gtk/osglobals.c`
- `src/osgui/gtk/osglobals_gtk.inl`

### r7162 — GTK3 Popup placeholder.
*2026-10-12 20:01:21 · franLIN · +70/-42 · 8 files*

- `internal/gtk_issues.md`
- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/ospopup.c`
- `src/osgui/gtk/ospopup_gtk.inl`
- `src/osgui/ospopup.inl`
- `src/osgui/ostabstop.c`
- `src/osgui/osx/ospopup.m`
- `src/osgui/win/ospopup.c`

### r7163 — GTK3 Slider placeholder
*2026-10-12 21:42:26 · franLIN · +133/-8 · 12 files*

- `internal/gtk_issues.md`
- `src/osgui/gtk/osbutton.c`
- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/osglobals.c`
- `src/osgui/gtk/osglobals_gtk.inl`
- `src/osgui/gtk/ospopup.c`
- `src/osgui/gtk/osslider.c`
- `src/osgui/gtk/osslider_gtk.inl`
- `src/osgui/osslider.inl`
- `src/osgui/ostabstop.c`
- `src/osgui/osx/osslider.m`
- `src/osgui/win/osslider.c`

### r7164 — osprogress_bounds implementation
*2026-10-12 21:53:17 · franLIN · +9/-39 · 10 files*

- `internal/gtk_issues.md`
- `src/draw2d/guictx.c`
- `src/draw2d/guictx.h`
- `src/draw2d/guictx.hxx`
- `src/gui/progress.c`
- `src/osgui/gtk/osprogress.c`
- `src/osgui/osguictx.c`
- `src/osgui/osprogress.h`
- `src/osgui/osx/osprogress.m`
- `src/osgui/win/osprogress.c`

### r7165 — GTK3 progress fix in height measure
*2026-10-12 22:02:09 · franLIN · +1/-8 · 1 file*

- `src/osgui/gtk/osglobals.c`

### r7166 — GTK3 progress reduce
*2026-10-12 22:33:49 · franLIN · +50/-5 · 5 files*

- `demo/guihello/reduce.c`
- `internal/gtk_issues.md`
- `src/osgui/gtk/osglobals.c`
- `src/osgui/gtk/osglobals_gtk.inl`
- `src/osgui/gtk/osprogress.c`

### r7167 — GTK3 remove unused OSqueezeLevel API
*2026-10-12 22:36:53 · franLIN · +0/-61 · 3 files*

- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/oscontrol_gtk.inl`
- `src/osgui/gtk/osgui_gtk.ixx`

### r7168 — Remove GTK issues memory
*2026-10-12 22:41:00 · franLIN · +0/-1620 · 1 file*

- `internal/gtk_issues.md`

### r7169 — GTK3 Improved View focused frame
*2026-10-13 08:27:43 · franLIN · +17/-12 · 3 files*

- `src/osgui/gtk/osglobals.c`
- `src/osgui/gtk/osglobals_gtk.inl`
- `src/osgui/gtk/osview.c`

### r7170 — GTK3 TextView improved focus frame.
*2026-10-13 08:35:22 · franLIN · +9/-45 · 3 files*

- `src/osgui/gtk/osglobals.c`
- `src/osgui/gtk/osglobals_gtk.inl`
- `src/osgui/gtk/ostext.c`

### r7171 — PopUp improved bounds calculation
*2026-10-13 09:32:19 · franLIN · +17/-32 · 9 files*

- `CLAUDE.md`
- `src/draw2d/guictx.c`
- `src/draw2d/guictx.h`
- `src/draw2d/guictx.hxx`
- `src/gui/popup.c`
- `src/osgui/gtk/ospopup.c`
- `src/osgui/ospopup.h`
- `src/osgui/osx/ospopup.m`
- `src/osgui/win/ospopup.c`

### r7172 — ndoc Improved GTK3 documentation and backends section
*2026-10-13 10:12:11 · franLIN · +139/-123 · 18 files*

- `doc/gui/en/gui.htm`
- `doc/gui/es/gui.htm`
- `doc/gui/img/cinnamon.png`
- `doc/gui/img/gnome.png`
- `doc/gui/img/lxde.png`
- `doc/gui/img/mate.png`
- `doc/gui/img/pantheon.png`
- `doc/gui/img/sugar.png`
- `doc/gui/img/xfce.png`
- `doc/guide/en/win_mac_linux.htm`
- `doc/guide/es/win_mac_linux.htm`
- `doc/guide/img/lxde.png`
- `doc/guide/img/gnome.png`
- `doc/guide/img/mate.png`
- `doc/guide/img/sugar.png`
- `doc/guide/img/cinnamon.png`
- `doc/guide/img/pantheon.png`
- `doc/guide/img/xfce.png`

### r7173 — Change API menu link in NAppGUI website
*2026-10-13 10:15:01 · franLIN · +0/-1 · 1 file*

- `doc/config/config.json`

### r7174 — ndoc GTK3 Window Managers docu.
*2026-10-13 10:30:27 · franLIN · +20/-0 · 2 files*

- `doc/gui/en/gui.htm`
- `doc/gui/es/gui.htm`

### r7175 — ndoc backends Win32/AppKit simple descriptions.
*2026-10-13 10:34:16 · franLIN · +8/-0 · 2 files*

- `doc/gui/en/gui.htm`
- `doc/gui/es/gui.htm`

### r7176 — Document X11 issues  in "hot" scale factor changes.
*2026-10-13 13:13:59 · franLIN · +2/-2 · 2 files*

- `doc/gui/en/gui.htm`
- `doc/gui/es/gui.htm`

### r7177 — HiDPI working on Linux-Wayland.
*2026-10-13 15:43:53 · franLIN · +36/-0 · 1 file*

- `internal/wayland_handoff.md`

### r7178 — GTK3 Wayland force draw focus ring while resizing (Wayland error)
*2026-10-13 16:08:49 · franLIN · +29/-8 · 4 files*

- `src/osgui/gtk/osbutton.c`
- `src/osgui/gtk/osentry.c`
- `src/osgui/gtk/ospopup.c`
- `src/osgui/gtk/osslider.c`

### r7179 — HiDPI testing apps GTK3
*2026-10-13 20:10:45 · franLIN · +177/-0 · 2 files*

- `internal/gtk3_test.c`
- `internal/napp_test.c`

### r7180 — GTK3 placeholder-->filler
*2026-10-14 08:58:28 · franLIN · +9/-100 · 8 files*

- `src/osgui/gtk/osbutton.c`
- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/oscontrol_gtk.inl`
- `src/osgui/gtk/osedit.c`
- `src/osgui/gtk/osgui_gtk.ixx`
- `src/osgui/gtk/ospopup.c`
- `src/osgui/gtk/osprogress.c`
- `src/osgui/gtk/osslider.c`

### r7181 — GTK3 Filler API review.
*2026-10-14 18:09:54 · franLIN · +3/-72 · 8 files*

- `src/osgui/gtk/osbutton.c`
- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/oscontrol_gtk.inl`
- `src/osgui/gtk/osedit.c`
- `src/osgui/gtk/osgui_gtk.ixx`
- `src/osgui/gtk/ospopup.c`
- `src/osgui/gtk/osprogress.c`
- `src/osgui/gtk/osslider.c`

### r7182 — GTK3 Button update to new OSFiller
*2026-10-14 18:32:23 · franLIN · +2/-105 · 2 files*

- `src/osgui/gtk/osbutton.c`
- `src/osgui/gtk/oscontrol.c`

### r7183 — GTK OSButton/OSEdit change placeholder by filler
*2026-10-14 18:45:28 · franLIN · +15/-33 · 3 files*

- `demo/guihello/reduce.c`
- `src/osgui/gtk/osbutton.c`
- `src/osgui/gtk/osedit.c`

### r7184 — GTK OSPopup improved filler draw function.
*2026-10-14 19:56:11 · franLIN · +28/-116 · 4 files*

- `demo/guihello/reduce.c`
- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/oscontrol_gtk.inl`
- `src/osgui/gtk/ospopup.c`

### r7185 — GTK OSCombo uses filler feature
*2026-10-14 20:24:53 · franLIN · +38/-27 · 5 files*

- `demo/guihello/reduce.c`
- `src/osgui/gtk/oscombo.c`
- `src/osgui/gtk/oscombo_gtk.inl`
- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/osedit.c`

### r7186 — GTK UpDown improved arrow drawing
*2026-10-14 20:33:14 · franLIN · +0/-8 · 2 files*

- `demo/guihello/reduce.c`
- `src/osgui/gtk/osupdown.c`

### r7187 — GTK3 progress reduce to the minimum
*2026-10-14 20:42:44 · franLIN · +0/-78 · 3 files*

- `src/osgui/gtk/osglobals.c`
- `src/osgui/gtk/osglobals_gtk.inl`
- `src/osgui/gtk/osprogress.c`

### r7189 — Fix build Release warnings
*2026-10-15 08:01:17 · franLIN · +12/-32 · 3 files*

- `src/osgui/gtk/oscombo.c`
- `src/osgui/gtk/oscontrol.c`
- `src/osgui/gtk/oscontrol_gtk.inl`

### r7191 — Fix warning
*2026-10-15 09:55:56 · franLIN · +1/-0 · 1 file*

- `src/osgui/gtk/oscontrol.c`
