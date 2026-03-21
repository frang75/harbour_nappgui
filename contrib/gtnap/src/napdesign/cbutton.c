/* Color button */

#include "cbutton.h"
#include <gui/cell.inl>
#include <gui/view.h>
#include <gui/vctrl.inl>
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
    String *tooltip;
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
    return data;
}

/*---------------------------------------------------------------------------*/

static void i_destroy_data(CData **data)
{
    cassert_no_null(data);
    cassert_no_null(*data);
    str_destopt(&(*data)->tooltip);
    heap_delete(data, CData);
}

/*---------------------------------------------------------------------------*/

static void i_update_tooltip(CData *data, CButton *cbutton)
{
    String *tooltip = NULL;
    uint8_t r, g, b;
    cassert_no_null(data);
    color_get_rgb(data->color, &r, &g, &b);
    if (str_empty(data->tooltip) == TRUE)
        tooltip = str_printf("rgb(%d,%d,%d)", r, g, b);
    else
        tooltip = str_printf("%s rgb(%d,%d,%d)", tc(data->tooltip), r, g, b);
    view_tooltip(cast(cbutton, View), tc(tooltip));
    str_destroy(&tooltip);
}

/*---------------------------------------------------------------------------*/

static void i_OnDraw(CData *data, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    cassert_no_null(data);
    draw_fill_color(p->ctx, data->color);
    draw_rect(p->ctx, ekFILL, p->x, p->y, p->width, p->height);
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
    unref(data);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_OnExit(CData *data, Event *e)
{
    unref(data);
    unref(e);
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

static void i_OnClick(CData *data, Event *e)
{
    unref(data);
    unref(e);
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
    i_update_tooltip(data, cbutton);
}

/*---------------------------------------------------------------------------*/

static void i_set_uint32(CButton *cbutton, const uint32_t value)
{
    CData *data = view_get_data(cast(cbutton, View), CData);
    data->color = value;
    view_update(cast(cbutton, View));
    i_update_tooltip(data, cbutton);
}

/*---------------------------------------------------------------------------*/

CButton *cbutton_create(const S2Df size)
{
    CData *data = i_create_data();
    View *view = _vctrl_create(ekVIEW_CONTROL | ekVIEW_BORDER | ekVIEW_NOERASE, &i_CBUTTON_TLB, data, CData);
    i_update_tooltip(data, cast(view, CButton));
    view_size(view, size);
    return cast(view, CButton);
}

/*---------------------------------------------------------------------------*/

void cbutton_tooltip(CButton *cbutton, const char_t *text)
{
    CData *data = view_get_data(cast(cbutton, View), CData);
    cassert_no_null(data);
    str_upd(&data->tooltip, text);
    i_update_tooltip(data, cbutton);
}
