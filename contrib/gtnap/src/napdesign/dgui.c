/* Design gui elements */

#include "dgui.h"
#include <gui/gui.h>
#include <gui/drawctrl.inl>
#include <gui/layout.h>
#include <gui/panel.h>
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
    View *view;
    Panel *panel;
    real32_t width;
    real32_t height;
    bool_t over;
    bool_t on_down;
    bool_t open;
    Listener *listener;
};

/*---------------------------------------------------------------------------*/

static color_t i_HEADER_GRADIENT[2] = {0, 0};
static color_t i_DRAWER_GRADIENT[2] = {0, 0};
static const real32_t i_HEADER_TEXT_MARGIN = 4;
static const real32_t i_DRAWER_TRIANGLE_WIDTH = 10;
static const char_t *i_UTF8_CLOSE_BUTTON = "✖";
static const char_t *i_UTF8_RIGHT_ARROW = "▶";
static const char_t *i_UTF8_DOWN_ARROW = "▼";

/*---------------------------------------------------------------------------*/

static void i_destroy_header_data(HeaderData **data)
{
    cassert_no_null(data);
    cassert_no_null(*data);
    str_destroy(&(*data)->title);
    font_destroy(&(*data)->font);
    listener_destroy(&(*data)->listener);
    heap_delete(data, HeaderData);
}

/*---------------------------------------------------------------------------*/

void dgui_init(void)
{
    if (gui_dark_mode() == TRUE)
    {
        cassert(FALSE);
    }
    else
    {
        i_HEADER_GRADIENT[0] = color_rgb(0xDE, 0xDE, 0xDE);
        i_HEADER_GRADIENT[1] = color_rgb(0xDA, 0xDA, 0xDA);
        i_DRAWER_GRADIENT[0] = color_rgb(0xF2, 0xF2, 0xF2);
        i_DRAWER_GRADIENT[1] = color_rgb(0xE4, 0xE4, 0xE4);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnHeaderDraw(HeaderData *data, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    real32_t stop[2] = {0, 1};
    real32_t back_width = 0;
    int32_t text_ypos = 0;
    cassert_no_null(data);

    draw_font(p->ctx, data->font);
    back_width = p->width;
    text_ypos = (int32_t)((p->height - font_height(data->font)) / 2);

    if (data->over == TRUE)
        back_width -= p->height;

    /* Background */
    draw_fill_linear(p->ctx, i_HEADER_GRADIENT, stop, 2, 0, 0, 0, p->height);
    draw_rect(p->ctx, ekFILL, 0.f, 0.f, back_width, p->height);

    /* Close button */
    {
        int32_t xpos = (int32_t)(p->width - p->height);

        if (data->over == TRUE)
            drawctrl_fill(p->ctx, xpos, 0, (int32_t)p->height, (int32_t)p->height, data->on_down ? ekCTRL_STATE_PRESSED : ekCTRL_STATE_HOT);

        draw_text_width(p->ctx, p->height);
        draw_text_halign(p->ctx, ekCENTER);
        drawctrl_text(p->ctx, i_UTF8_CLOSE_BUTTON, xpos, text_ypos, ekCTRL_STATE_NORMAL);
    }

    /* Title */
    {
        real32_t twidth = p->width - p->height - 2 * i_HEADER_TEXT_MARGIN;
        draw_text_width(p->ctx, twidth);
        draw_text_halign(p->ctx, ekLEFT);
        draw_text_trim(p->ctx, ekELLIPEND);
        drawctrl_text(p->ctx, tc(data->title), (int32_t)i_HEADER_TEXT_MARGIN, text_ypos, ekCTRL_STATE_NORMAL);
    }

    /* Frame */
    draw_line_color(p->ctx, gui_line_color());
    draw_line_width(p->ctx, 1);
    draw_rect(p->ctx, ekSTROKE, 0.f, 0.f, p->width - 1, p->height - 1);
}

/*---------------------------------------------------------------------------*/

static void i_OnHeaderSize(HeaderData *data, Event *e)
{
    const EvSize *p = event_params(e, EvSize);
    cassert_no_null(data);
    data->width = p->width;
    data->height = p->height;
}

/*---------------------------------------------------------------------------*/

static void i_OnHeaderMove(HeaderData *data, Event *e)
{
    bool_t over_close = FALSE;
    const EvMouse *p = event_params(e, EvMouse);
    cassert_no_null(data);

    if (p->lx >= data->width - data->height)
        over_close = TRUE;

    if (data->over != over_close)
    {
        data->over = over_close;

        if (data->over == FALSE)
            data->on_down = FALSE;

        view_update(data->view);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnHeaderDown(HeaderData *data, Event *e)
{
    cassert_no_null(data);
    unref(e);
    data->on_down = TRUE;
    if (data->over == TRUE)
        view_update(data->view);
}

/*---------------------------------------------------------------------------*/

static void i_OnHeaderUp(HeaderData *data, Event *e)
{
    cassert_no_null(data);
    unref(e);
    data->on_down = FALSE;
    if (data->over == TRUE)
    {
        view_update(data->view);
        if (data->listener != NULL)
            listener_event(data->listener, ekDEVENT_HEADER_CLOSE, data->view, NULL, NULL, View, void, void);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnHeaderExit(HeaderData *data, Event *e)
{
    cassert_no_null(data);
    unref(e);
    if (data->over == TRUE)
    {
        data->over = FALSE;
        view_update(data->view);
    }
}

/*---------------------------------------------------------------------------*/

View *dgui_panel_header(const char_t *title, const Font *font, Listener *OnClose)
{
    View *view = _view_create(ekVIEW_CONTROL);
    HeaderData *data = heap_new0(HeaderData);
    S2Df size;
    data->title = str_c(title);
    data->font = font_copy(font);
    data->view = view;
    size.width = 100;
    size.height = font_height(data->font) + 4;
    listener_update(&data->listener, OnClose);
    view_size(view, size);
    view_OnDraw(view, listener(data, i_OnHeaderDraw, HeaderData));
    view_OnSize(view, listener(data, i_OnHeaderSize, HeaderData));
    view_OnMove(view, listener(data, i_OnHeaderMove, HeaderData));
    view_OnDown(view, listener(data, i_OnHeaderDown, HeaderData));
    view_OnUp(view, listener(data, i_OnHeaderUp, HeaderData));
    view_OnExit(view, listener(data, i_OnHeaderExit, HeaderData));
    view_data(view, &data, i_destroy_header_data, HeaderData);
    return view;
}

/*---------------------------------------------------------------------------*/

static void i_OnDrawerDraw(HeaderData *data, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    real32_t stop[2] = {0, 1};
    int32_t text_ypos = 0;
    cassert_no_null(data);
    draw_font(p->ctx, data->font);
    text_ypos = (int32_t)((p->height - font_height(data->font)) / 2);

    /* Background */
    draw_fill_linear(p->ctx, i_DRAWER_GRADIENT, stop, 2, 0, 0, 0, p->height);
    draw_rect(p->ctx, ekFILL, 0.f, 0.f, p->width, p->height);

    /* Title */
    {
        real32_t twidth = p->width - i_DRAWER_TRIANGLE_WIDTH - 2 * i_HEADER_TEXT_MARGIN;
        draw_text_width(p->ctx, twidth);
        draw_text_halign(p->ctx, ekCENTER);
        draw_text_trim(p->ctx, ekELLIPEND);
        drawctrl_text(p->ctx, tc(data->title), (int32_t)(i_HEADER_TEXT_MARGIN + i_DRAWER_TRIANGLE_WIDTH), text_ypos, ekCTRL_STATE_NORMAL);
        draw_text_halign(p->ctx, ekLEFT);
        draw_text_color(p->ctx, data->over ? gui_link_color() : kCOLOR_DEFAULT);
        drawctrl_text(p->ctx, data->open ? i_UTF8_DOWN_ARROW : i_UTF8_RIGHT_ARROW, (int32_t)i_HEADER_TEXT_MARGIN, text_ypos, ekCTRL_STATE_HOT);
    }

    /* Frame */
    draw_line_color(p->ctx, gui_line_color());
    draw_line_width(p->ctx, 1);
    draw_rect(p->ctx, ekSTROKE, 0.f, 0.f, p->width - 1, p->height - 1);
}

/*---------------------------------------------------------------------------*/

static void i_OnDrawerSize(HeaderData *data, Event *e)
{
    const EvSize *p = event_params(e, EvSize);
    cassert_no_null(data);
    data->width = p->width;
    data->height = p->height;
}

/*---------------------------------------------------------------------------*/

static void i_OnDrawerMove(HeaderData *data, Event *e)
{
    cassert_no_null(data);
    unref(e);
    if (data->over == FALSE)
    {
        data->over = TRUE;
        view_update(data->view);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnDrawerDown(HeaderData *data, Event *e)
{
    cassert_no_null(data);
    unref(e);
    data->open = !data->open;
    panel_visible_layout(data->panel, data->open ? 1 : 0);
    panel_update(data->panel);
    if (data->listener != NULL)
        listener_event(data->listener, ekDEVENT_DRAWER_CHANGE, data->panel, &data->open, NULL, Panel, bool_t, void);
}

/*---------------------------------------------------------------------------*/

static void i_OnDrawerUp(HeaderData *data, Event *e)
{
    unref(data);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_OnDrawerExit(HeaderData *data, Event *e)
{
    cassert_no_null(data);
    unref(e);
    if (data->over == TRUE)
    {
        data->over = FALSE;
        view_update(data->view);
    }
}

/*---------------------------------------------------------------------------*/

static View *i_drawer_header(const char_t *title, const Font *font)
{
    View *view = _view_create(ekVIEW_CONTROL);
    HeaderData *data = heap_new0(HeaderData);
    S2Df size;
    data->title = str_c(title);
    data->font = font_copy(font);
    data->view = view;
    size.width = 100;
    size.height = font_height(data->font) + 4;
    view_size(view, size);
    view_OnDraw(view, listener(data, i_OnDrawerDraw, HeaderData));
    view_OnSize(view, listener(data, i_OnDrawerSize, HeaderData));
    view_OnMove(view, listener(data, i_OnDrawerMove, HeaderData));
    view_OnDown(view, listener(data, i_OnDrawerDown, HeaderData));
    view_OnUp(view, listener(data, i_OnDrawerUp, HeaderData));
    view_OnExit(view, listener(data, i_OnDrawerExit, HeaderData));
    view_data(view, &data, i_destroy_header_data, HeaderData);
    return view;
}

/*---------------------------------------------------------------------------*/

Panel *dgui_drawer(const char_t *title, const Font *font, Panel *child, const bool_t open, Listener *OnChange)
{
    View *view = i_drawer_header(title, font);
    HeaderData *data = view_get_data(view, HeaderData);
    Layout *layout1 = layout_create(1, 1);
    Layout *layout2 = layout_create(1, 2);
    Panel *panel = panel_create();
    layout_view(layout1, view, 0, 0);
    layout_view(layout2, view, 0, 0);
    layout_panel(layout2, child, 0, 1);
    panel_layout(panel, layout1);
    panel_layout(panel, layout2);
    panel_visible_layout(panel, open ? 1 : 0);
    listener_update(&data->listener, OnChange);
    data->open = open;
    data->panel = panel;
    return panel;
}
