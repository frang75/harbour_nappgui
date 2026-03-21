/* Color button */

#include "cbutton.h"
#include <gui/gui.h>
#include <gui/guicontrol.h>
#include <gui/cell.inl>
#include <gui/comwin.h>
#include <gui/view.h>
#include <gui/vctrl.inl>
#include <gui/window.h>
#include <draw2d/color.h>
#include <draw2d/draw.h>
#include <core/event.h>
#include <core/heap.h>
#include <core/strings.h>
#include <sewer/cassert.h>

typedef struct _cdata_t CData;

struct _cdata_t
{
    uint32_t color;
    bool_t hover;
    String *info;
    View *view;
};

/*---------------------------------------------------------------------------*/

static void i_OnDraw(CData *data, Event *e);
static void i_OnSize(CData *data, Event *e);
static void i_OnEnter(CData *data, Event *e);
static void i_OnExit(CData *data, Event *e);
static void i_OnMoved(CData *data, Event *e);
static void i_OnFocus(CData *data, Event *e);
static void i_OnDown(CData *data, Event *e);
static void i_OnUp(CData *data, Event *e);
static void i_OnClick(CData *data, Event *e);
static void i_OnKeyDown(CData *data, Event *e);
static void i_OnKeyUp(CData *data, Event *e);
static void i_destroy_data(CData **data);
static void i_set_empty(CButton *cbutton);
static void i_set_uint32(CButton *cbutton, const uint32_t value);

/*---------------------------------------------------------------------------*/

static VCtrlTbl i_CBUTTON_TLB = {
    "CButton",
    (FPtr_event_handler)i_OnDraw,
    NULL, /* OnOverlay */
    (FPtr_event_handler)i_OnSize,
    (FPtr_event_handler)i_OnEnter,
    (FPtr_event_handler)i_OnExit,
    (FPtr_event_handler)i_OnMoved,
    (FPtr_event_handler)i_OnDown,
    (FPtr_event_handler)i_OnUp,
    (FPtr_event_handler)i_OnClick,
    NULL, /* OnDrag */
    NULL, /* OnWheel */
    (FPtr_event_handler)i_OnKeyDown,
    (FPtr_event_handler)i_OnKeyUp,
    (FPtr_event_handler)i_OnFocus,
    NULL, /* OnResignFocus */
    NULL, /* OnAcceptFocus */
    NULL, /* OnScroll */
    (FPtr_destroy)i_destroy_data,
    NULL, /* func_locale */
    NULL, /* func_natural */
    (FPtr_gctx_call)i_set_empty,
    (FPtr_gctx_set_uint32)i_set_uint32,
    NULL /* func_image*/};

/*---------------------------------------------------------------------------*/

static CData *i_create_data(void)
{
    CData *data = heap_new0(CData);
    data->color = kCOLOR_TRANSPARENT;
    data->info = str_c("");
    return data;
}

/*---------------------------------------------------------------------------*/

static void i_destroy_data(CData **data)
{
    cassert_no_null(data);
    cassert_no_null(*data);
    str_destroy(&(*data)->info);
    heap_delete(data, CData);
}

/*---------------------------------------------------------------------------*/

static void i_update_tooltip(CData *data)
{
    String *tooltip = NULL;
    uint8_t r, g, b;
    cassert_no_null(data);
    color_get_rgb(data->color, &r, &g, &b);
    if (str_empty(data->info) == TRUE)
        tooltip = str_printf("rgb(%d,%d,%d)", r, g, b);
    else
        tooltip = str_printf("%s rgb(%d,%d,%d)", tc(data->info), r, g, b);
    view_tooltip(data->view, tc(tooltip));
    str_destroy(&tooltip);
}

/*---------------------------------------------------------------------------*/

static void i_OnDraw(CData *data, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    cassert_no_null(data);
    draw_fill_color(p->ctx, data->color);
    draw_rect(p->ctx, ekFILL, p->x, p->y, p->width, p->height);

    if (data->hover == TRUE)
    {
        draw_line_color(p->ctx, gui_link_color());
        draw_line_width(p->ctx, 2);
        draw_rect(p->ctx, ekSTROKE, p->x, p->y, p->width, p->height);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnSize(CData *data, Event *e)
{
    unref(data);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_OnMoved(CData *data, Event *e)
{
    unref(data);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_OnEnter(CData *data, Event *e)
{
    View *view = event_sender(e, View);
    cassert_no_null(data);
    data->hover = TRUE;
    view_update(view);
}

/*---------------------------------------------------------------------------*/

static void i_OnExit(CData *data, Event *e)
{
    View *view = event_sender(e, View);
    cassert_no_null(data);
    data->hover = FALSE;
    view_update(view);
}

/*---------------------------------------------------------------------------*/

static void i_OnFocus(CData *data, Event *e)
{
    unref(data);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_OnDown(CData *data, Event *e)
{
    unref(data);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_OnUp(CData *data, Event *e)
{
    unref(data);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_OnColorChange(CData *data, Event *e)
{
    const color_t *p = event_params(e, color_t);
    cassert_no_null(data);
    data->color = *p;
    _cell_update_u32(_vctrl_cell(data->view), data->color);
    view_update(data->view);
}

/*---------------------------------------------------------------------------*/

static void i_OnClick(CData *data, Event *e)
{
    Window *window = NULL;
    R2Df r2d;
    cassert_no_null(data);
    unref(e);
    window = guicontrol_get_window(guicontrol(data->view));
    r2d = window_control_frame(window, guicontrol(data->view));
    r2d.pos = window_client_to_screen(window, r2d.pos);
    comwin_color(window, tc(data->info), r2d.pos.x, r2d.pos.y, ekRIGHT, ekTOP, data->color, NULL, 0, listener(data, i_OnColorChange, CData));
}

/*---------------------------------------------------------------------------*/

static void i_OnKeyDown(CData *data, Event *e)
{
    unref(data);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_OnKeyUp(CData *data, Event *e)
{
    unref(data);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_set_empty(CButton *cbutton)
{
    CData *data = view_get_data(cast(cbutton, View), CData);
    data->color = kCOLOR_TRANSPARENT;
    i_update_tooltip(data);
}

/*---------------------------------------------------------------------------*/

static void i_set_uint32(CButton *cbutton, const uint32_t value)
{
    CData *data = view_get_data(cast(cbutton, View), CData);
    data->color = value;
    view_update(cast(cbutton, View));
    i_update_tooltip(data);
}

/*---------------------------------------------------------------------------*/

CButton *cbutton_create(const S2Df size)
{
    CData *data = i_create_data();
    data->view = _vctrl_create(ekVIEW_CONTROL| ekVIEW_NOERASE, &i_CBUTTON_TLB, data, CData);
    i_update_tooltip(data);
    view_size(data->view, size);
    return cast(data->view, CButton);
}

/*---------------------------------------------------------------------------*/

void cbutton_info(CButton *cbutton, const char_t *text)
{
    CData *data = view_get_data(cast(cbutton, View), CData);
    cassert_no_null(data);
    str_upd(&data->info, text);
    i_update_tooltip(data);
}
