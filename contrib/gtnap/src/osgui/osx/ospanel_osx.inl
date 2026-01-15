/*
 * NAppGUI Cross-platform C SDK
 * 2015-2026 Francisco Garcia Collado
 * MIT Licence
 * https://nappgui.com/en/legal/license.html
 *
 * File: ospanel_osx.inl
 *
 */

/* Operating System native panel */

#include "osgui_osx.ixx"

__EXTERN_C

BOOL _ospanel_is(NSView *view);

BOOL _ospanel_is_content(NSView *view);

void _ospanel_destroy(OSPanel **panel);

void _ospanel_attach_control(OSPanel *panel, NSView *control);

void _ospanel_detach_control(OSPanel *panel, NSView *control);

void _ospanel_scroll_event(NSView *view, const gui_orient_t orient, const gui_scroll_t event);

void _ospanel_incr_scroll(NSView *view, const int32_t incr_x, const int32_t incr_y);

__END_C
