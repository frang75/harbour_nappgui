/*
 * NAppGUI Cross-platform C SDK
 * 2015-2026 Francisco Garcia Collado
 * MIT Licence
 * https://nappgui.com/en/legal/license.html
 *
 * File: ospanel.m
 *
 */

/* Operating System native panel */

#include "ospanel_osx.inl"
#include "oscontrol_osx.inl"
#include "../ospanel.h"
#include "../ospanel.inl"
#include "../oscontrol.inl"
#include "../osgui.inl"
#include "../osscrolls.inl"
#include <core/arrst.h>
#include <core/heap.h>
#include <sewer/cassert.h>

#if !defined(__MACOS__)
#error This file is only for OSX
#endif

typedef struct _area_t Area;
struct _area_t
{
    void *obj;
    NSRect rect;
    NSColor *bgcolor;
    NSColor *skcolor;
};

DeclSt(Area);

/*---------------------------------------------------------------------------*/

@interface OSXContentView : NSView
{
  @public
}
@end

@interface OSXPanel : NSView
{
  @public
    OSScrolls *scroll;
    NSView *content;
    id evmonitor;
    ArrSt(Area) *areas;
}
@end

/*---------------------------------------------------------------------------*/

static void i_draw_areas(OSXPanel *panel, NSPoint borigin)
{
    NSRect bounds;
    bounds.origin = borigin;
    bounds.size = [panel frame].size;
    cassert_no_null(panel);
    if (panel->areas != NULL)
    {
        arrst_foreach(area, panel->areas, Area)
            /* In scroll panels, avoid draw non visible areas */
            if (NSIntersectsRect(area->rect, bounds) == YES)
            {
                if (area->bgcolor != nil)
                {
                    [area->bgcolor set];
                    NSRectFill(area->rect);
                }

                if (area->skcolor != NULL)
                {
                    [area->skcolor set];
                    NSFrameRect(area->rect);
                }
            }
        arrst_end()
    }
}

/*---------------------------------------------------------------------------*/

@implementation OSXContentView

- (BOOL)isFlipped
{
    return YES;
}

/*---------------------------------------------------------------------------*/

- (void)drawRect:(NSRect)rect
{
    NSView *parent = [self superview];
    cassert([parent isKindOfClass:[OSXPanel class]]);
    [super drawRect:rect];
    i_draw_areas(cast(parent, OSXPanel), [self bounds].origin);
}

@end

/*---------------------------------------------------------------------------*/

@implementation OSXPanel

/*---------------------------------------------------------------------------*/

- (void)dealloc
{
    [super dealloc];
    heap_auditor_delete("OSXPanel");
}

/*---------------------------------------------------------------------------*/

- (void)drawRect:(NSRect)rect
{
    [super drawRect:rect];
    if (self->content == nil)
        i_draw_areas(self, NSZeroPoint);
}

/*---------------------------------------------------------------------------*/

- (BOOL)isFlipped
{
    return YES;
}

/*---------------------------------------------------------------------------*/

- (BOOL)eventIsInside:(NSEvent *)theEvent
{
    NSPoint locationInView = [self convertPoint:theEvent.locationInWindow fromView:nil];
    return NSPointInRect(locationInView, [self bounds]);
}

/*---------------------------------------------------------------------------*/

- (void)handleScroll:(NSEvent *)theEvent
{
    gui_scroll_t ev = _osscroll_wheel_event(theEvent);
    if (ev != ENUM_MAX(gui_scroll_t))
        _ospanel_scroll_event(self, ekGUI_VERTICAL, ev);
}

@end

/*---------------------------------------------------------------------------*/

OSPanel *ospanel_create(const uint32_t flags)
{
    OSXPanel *panel = nil;
    heap_auditor_add("OSXPanel");
    panel = [[OSXPanel alloc] initWithFrame:NSZeroRect];
    panel->areas = NULL;
    [panel setAutoresizesSubviews:NO];

    /* https://developer.apple.com/documentation/macos-release-notes/appkit-release-notes-for-macos-14#NSView */
#if defined(MAC_OS_VERSION_14_0) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_14
    [panel setClipsToBounds:YES];
#endif

    if (flags & ekVIEW_HSCROLL || flags & ekVIEW_VSCROLL)
    {
        panel->content = [[OSXContentView alloc] init];
        [panel addSubview:panel->content];

        panel->scroll = _osscrolls_create(cast(panel, OSControl), (bool_t)(flags & ekVIEW_HSCROLL) != 0, (bool_t)(flags & ekVIEW_VSCROLL) != 0);

        panel->evmonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskScrollWheel
                                                                 handler:^NSEvent *_Nullable(NSEvent *_Nonnull event) {
                                                                   if ([panel eventIsInside:event])
                                                                   {
                                                                       [panel handleScroll:event];
                                                                       return nil;
                                                                   }

                                                                   return event;
                                                                 }];
    }
    else
    {
        panel->content = nil;
        panel->scroll = nil;
        panel->evmonitor = nil;
    }

    if (flags & ekVIEW_BORDER)
        [panel setFocusRingType:NSFocusRingTypeExterior];
    else
        [panel setFocusRingType:NSFocusRingTypeNone];

    return cast(panel, OSPanel);
}

/*---------------------------------------------------------------------------*/

static void i_remove_area(Area *area)
{
    if (area->bgcolor != nil)
        [area->bgcolor release];
    if (area->skcolor != nil)
        [area->skcolor release];
}

/*---------------------------------------------------------------------------*/

static OSXPanel *i_get_panel(const OSPanel *panel)
{
    cassert_no_null(panel);
    cassert([cast(panel, NSView) isKindOfClass:[OSXPanel class]]);
    return cast(panel, OSXPanel);
}

/*---------------------------------------------------------------------------*/

void ospanel_destroy(OSPanel **panel)
{
    OSXPanel *lpanel = nil;
    cassert_no_null(panel);
    lpanel = i_get_panel(*panel);
    cassert_no_null(lpanel);

    if (lpanel->scroll != NULL)
        _osscrolls_destroy(&lpanel->scroll);

    if (lpanel->content != nil)
    {
        [lpanel->content removeFromSuperview];
        [lpanel->content release];
        lpanel->content = nil;
    }

    if (lpanel->evmonitor != nil)
    {
        [NSEvent removeMonitor:lpanel->evmonitor];
        lpanel->evmonitor = nil;
    }

    cassert([[lpanel subviews] count] == 0);

    if (lpanel->areas != NULL)
        arrst_destroy(&lpanel->areas, i_remove_area, Area);

    [lpanel release];
    *panel = NULL;
}

/*---------------------------------------------------------------------------*/

BOOL _ospanel_is(NSView *view)
{
    return ([view isKindOfClass:[OSXPanel class]]);
}

/*---------------------------------------------------------------------------*/

BOOL _ospanel_is_content(NSView *view)
{
    return ([view isKindOfClass:[OSXContentView class]]);
}

/*---------------------------------------------------------------------------*/

void _ospanel_destroy(OSPanel **panel)
{
    OSXPanel *lpanel = nil;
    NSArray *subviews;
    NSUInteger i, n;
    cassert_no_null(panel);
    lpanel = i_get_panel(*panel);
    cassert_no_null(lpanel);
    subviews = [lpanel subviews];
    n = [subviews count];
    for (i = 0; i < n; ++i)
    {
        NSView *child = [subviews objectAtIndex:0];
        _oscontrol_detach_and_destroy(dcast(&child, OSControl), *panel);
        cassert([subviews count] == n - i - 1);
    }

    ospanel_destroy(panel);
}

/*---------------------------------------------------------------------------*/

void ospanel_area(OSPanel *panel, void *obj, const color_t bgcolor, const color_t skcolor, const real32_t x, const real32_t y, const real32_t width, const real32_t height)
{
    OSXPanel *lpanel = i_get_panel(panel);
    cassert_no_null(lpanel);
    if (obj != NULL)
    {
        Area *area = NULL;

        if (lpanel->areas == NULL)
            lpanel->areas = arrst_create(Area);

        arrst_foreach(larea, lpanel->areas, Area)
            if (larea->obj == obj)
            {
                area = larea;
                break;
            }
        arrst_end()

        if (area == NULL)
        {
            area = arrst_new(lpanel->areas, Area);
            area->obj = obj;
            area->bgcolor = nil;
            area->skcolor = nil;
        }

        area->rect.origin.x = (CGFloat)x;
        area->rect.origin.y = (CGFloat)y;
        area->rect.size.width = (CGFloat)width;
        area->rect.size.height = (CGFloat)height;

        if (area->bgcolor != nil)
        {
            [area->bgcolor release];
            area->bgcolor = nil;
        }

        if (area->skcolor != nil)
        {
            [area->skcolor release];
            area->skcolor = nil;
        }

        if (bgcolor != 0)
            area->bgcolor = [_oscontrol_color(bgcolor) retain];

        if (skcolor != 0)
            area->skcolor = [_oscontrol_color(skcolor) retain];
    }
    else
    {
        if (lpanel->areas != NULL)
            arrst_clear(lpanel->areas, i_remove_area, Area);
    }
}

/*---------------------------------------------------------------------------*/

void ospanel_scroller_size(const OSPanel *panel, real32_t *width, real32_t *height)
{
    OSXPanel *lpanel = i_get_panel(panel);
    cassert_no_null(lpanel);

    if (width != NULL)
        *width = (real32_t)_osscrolls_bar_width(lpanel->scroll, FALSE);

    if (height != NULL)
        *height = (real32_t)_osscrolls_bar_height(lpanel->scroll, FALSE);
}

//*---------------------------------------------------------------------------*/

static void i_update_scroll_bounds(OSXPanel *panel)
{
    NSPoint origin;
    OSScroll *hscroll = NULL;
    OSScroll *vscroll = NULL;
    cassert_no_null(panel);
    cassert_no_null(panel->content);
    origin = [panel->content bounds].origin;
    hscroll = _osscrolls_horizontal(panel->scroll);
    vscroll = _osscrolls_vertical(panel->scroll);

    if (hscroll != NULL)
        origin.x = _osscroll_pos(hscroll);

    if (vscroll != NULL)
        origin.y = _osscroll_pos(vscroll);

    [panel->content setBoundsOrigin:origin];
}

/*---------------------------------------------------------------------------*/

void ospanel_content_size(OSPanel *panel, const real32_t width, const real32_t height, const real32_t line_width, const real32_t line_height)
{
    OSXPanel *lpanel = i_get_panel(panel);
    cassert_no_null(lpanel);
    cassert_no_null(lpanel->content);
    _osscrolls_content_size(lpanel->scroll, (uint32_t)width, (uint32_t)height, (uint32_t)line_width, (uint32_t)line_height);
    [lpanel->content setFrameSize:NSMakeSize((CGFloat)width, (CGFloat)height)];
    i_update_scroll_bounds(lpanel);
}

/*---------------------------------------------------------------------------*/

void ospanel_display(OSPanel *panel)
{
    OSXPanel *lpanel = i_get_panel(panel);
    cassert_no_null(lpanel);
    [lpanel setNeedsDisplay:YES];
}

/*---------------------------------------------------------------------------*/

void ospanel_attach(OSPanel *panel, OSPanel *parent_panel)
{
    OSXPanel *lparent = i_get_panel(parent_panel);
    cassert_no_null(lparent);
    if (lparent->content != nil)
        _oscontrol_attach_to_parent(cast(panel, NSView), lparent->content);
    else
        _oscontrol_attach_to_parent(cast(panel, NSView), cast(parent_panel, NSView));
}

/*---------------------------------------------------------------------------*/

void ospanel_detach(OSPanel *panel, OSPanel *parent_panel)
{
    OSXPanel *lparent = i_get_panel(parent_panel);
    cassert_no_null(lparent);
    if (lparent->content != nil)
        _oscontrol_detach_from_parent(cast(panel, NSView), lparent->content);
    else
        _oscontrol_detach_from_parent(cast(panel, NSView), cast(parent_panel, NSView));
}

/*---------------------------------------------------------------------------*/

void ospanel_visible(OSPanel *panel, const bool_t visible)
{
    _oscontrol_set_visible(cast(panel, NSView), visible);
}

/*---------------------------------------------------------------------------*/

void ospanel_enabled(OSPanel *panel, const bool_t enabled)
{
    unref(panel);
    unref(enabled);
}

/*---------------------------------------------------------------------------*/

void ospanel_size(const OSPanel *panel, real32_t *width, real32_t *height)
{
    _oscontrol_get_size(cast(panel, NSView), width, height);
}

/*---------------------------------------------------------------------------*/

void ospanel_origin(const OSPanel *panel, real32_t *x, real32_t *y)
{
    _oscontrol_get_origin(cast(panel, NSView), x, y);
}

/*---------------------------------------------------------------------------*/

void ospanel_frame(OSPanel *panel, const real32_t x, const real32_t y, const real32_t width, const real32_t height)
{
    OSXPanel *lpanel = i_get_panel(panel);
    _oscontrol_set_frame(cast(panel, NSView), x, y, width, height);
    cassert_no_null(lpanel);
    if (lpanel->scroll != nil)
    {
        _osscrolls_control_size(lpanel->scroll, (uint32_t)width, (uint32_t)height);
        i_update_scroll_bounds(lpanel);
    }
}

/*---------------------------------------------------------------------------*/

void _ospanel_attach_control(OSPanel *panel, NSView *control)
{
    OSXPanel *lpanel = i_get_panel(panel);
    if (lpanel->content != nil)
        _oscontrol_attach_to_parent(control, lpanel->content);
    else
        _oscontrol_attach_to_parent(control, cast(lpanel, NSView));
}

/*---------------------------------------------------------------------------*/

void _ospanel_detach_control(OSPanel *panel, NSView *control)
{
    OSXPanel *lpanel = i_get_panel(panel);
    if (lpanel->content != nil)
        _oscontrol_detach_from_parent(control, lpanel->content);
    else
        _oscontrol_detach_from_parent(control, cast(lpanel, NSView));
}

/*---------------------------------------------------------------------------*/

void _ospanel_scroll_event(NSView *view, const gui_orient_t orient, const gui_scroll_t event)
{
    OSXPanel *lpanel = cast(view, OSXPanel);
    cassert_no_null(lpanel);
    cassert_no_null(lpanel->scroll);
    cassert([lpanel isKindOfClass:[OSXPanel class]]);
    if (_osscrolls_event(lpanel->scroll, orient, event, TRUE) == TRUE)
        [lpanel setNeedsDisplay:YES];
}

/*---------------------------------------------------------------------------*/

void _ospanel_incr_scroll(NSView *view, const int32_t incr_x, const int32_t incr_y)
{
    if (incr_x != 0 || incr_y != 0)
    {
        OSXPanel *lpanel = cast(view, OSXPanel);
        cassert_no_null(lpanel);
        cassert_no_null(lpanel->scroll);
        cassert([lpanel isKindOfClass:[OSXPanel class]]);
        i_update_scroll_bounds(lpanel);
    }
}

/*---------------------------------------------------------------------------*/

bool_t _ospanel_with_scroll(const OSPanel *panel)
{
    OSXPanel *lpanel = i_get_panel(panel);
    cassert_no_null(lpanel);
    return (bool_t)(lpanel->scroll != nil);
}

/*---------------------------------------------------------------------------*/

void _ospanel_scroll(OSPanel *panel, const int32_t x, const int32_t y)
{
    OSXPanel *lpanel = i_get_panel(panel);
    cassert_no_null(lpanel);
    cassert_no_null(lpanel->scroll);

    if (x != INT32_MAX)
    {
        OSScroll *scroll = _osscrolls_horizontal(lpanel->scroll);
        _osscroll_set_pos(scroll, (uint32_t)(x < 0 ? 0 : x));
    }

    if (y != INT32_MAX)
    {
        OSScroll *scroll = _osscrolls_vertical(lpanel->scroll);
        _osscroll_set_pos(scroll, (uint32_t)(y < 0 ? 0 : y));
    }

    if (x != INT32_MAX || y != INT32_MAX)
        i_update_scroll_bounds(lpanel);
}

/*---------------------------------------------------------------------------*/

void _ospanel_scroll_frame(const OSPanel *panel, OSFrame *rect)
{
    NSPoint origin;
    NSSize size;
    OSXPanel *lpanel = i_get_panel(panel);
    cassert_no_null(rect);
    cassert_no_null(lpanel);
    cassert_no_null(lpanel->content);
    origin = [lpanel->content bounds].origin;
    size = [lpanel frame].size;
    rect->left = (int32_t)origin.x;
    rect->top = (int32_t)origin.y;
    rect->right = rect->left + (int32_t)size.width;
    rect->bottom = rect->top + (int32_t)size.height;
}
