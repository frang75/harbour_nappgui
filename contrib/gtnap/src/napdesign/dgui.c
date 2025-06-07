/* Design gui elements */

#include "dgui.h"
#include <gui/view.h>
#include <gui/view.inl>
#include <draw2d/color.h>
#include <draw2d/dctx.h>
#include <draw2d/draw.h>
#include <draw2d/font.h>
#include <core/event.h>
#include <core/heap.h>
#include <core/strings.h>
#include <sewer/cassert.h>

typedef struct _header_data_t HeaderData;

struct _header_data_t
{
    String *title;
    Font *font;
};

/*---------------------------------------------------------------------------*/

static void i_destroy_header_data(HeaderData **data)
{
    cassert_no_null(data);
    cassert_no_null(*data);
    str_destroy(&(*data)->title);
    font_destroy(&(*data)->font);
    heap_delete(data, HeaderData);
}

/*---------------------------------------------------------------------------*/

static void i_OnHeaderDraw(HeaderData *data, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    cassert_no_null(data);
    draw_clear(p->ctx, kCOLOR_BLUE);
}

/*---------------------------------------------------------------------------*/

View *dgui_panel_header(const char_t *title)
{
    View *view = _view_create(ekVIEW_CONTROL);
    HeaderData *data = heap_new0(HeaderData);
    S2Df size;
    data->title = str_c(title);
    data->font = font_system(font_regular_size(), 0);
    size.width = 100;
    size.height = font_height(data->font);
    view_size(view, size);
    view_OnDraw(view, listener(data, i_OnHeaderDraw, HeaderData));
    view_data(view, &data, i_destroy_header_data, HeaderData);
    return view;
}
