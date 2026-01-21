/* Property editor */

#include "propedit.h"
#include "designer.h"
#include "dlayout.h"
#include "dform.h"
#include "dialogs.h"
#include "res_designer.h"
#include <nflib/nflib.h>
#include <nflib/flayout.h>
#include <gui/gui.h>
#include <gui/cell.h>
#include <gui/comwin.h>
#include <gui/button.h>
#include <gui/edit.h>
#include <gui/imageview.h>
#include <gui/label.h>
#include <gui/layout.h>
#include <gui/listbox.h>
#include <gui/panel.h>
#include <gui/popup.h>
#include <gui/view.h>
#include <gui/updown.h>
#include <draw2d/image.h>
#include <geom2d/s2d.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <core/event.h>
#include <core/heap.h>
#include <core/strings.h>
#include <sewer/bstd.h>
#include <sewer/blib.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>

typedef struct _propdata_t PropData;

struct _propdata_t
{
    DSelect sel;
    Designer *app;
    DForm *form;
    Panel *cell_panel;
    Layout *layout_layout;
    Layout *column_layout;
    Layout *row_layout;
    Layout *cell_layout;
    Layout *label_layout;
    Layout *button_layout;
    Layout *check_layout;
    Layout *radio_layout;
    Layout *tool_layout;
    Layout *popup_layout;
    Layout *edit_layout;
    Layout *combo_layout;
    Layout *listbox_layout;
    Layout *slider_layout;
    Layout *vslider_layout;
    Layout *progress_layout;
    Layout *view_layout;
    Layout *text_layout;
    Layout *image_layout;
    Layout *table_layout;
    Layout *header_layout;
    ListBox *popup_list;
    ListBox *listbox_list;
    ListBox *table_list;
    Cell *column_margin_cell;
    Cell *row_margin_cell;
    Label *layout_type_label;
    Label *cell_type_label;
    PopUp *column_popup;
    PopUp *row_popup;

    Label *label_tool_icon;
    ImageView *view_tool_icon;
    Button *button_tool_icon;
    Label *label_image_icon;
    ImageView *view_image_icon;
    Button *button_image_icon;
};

/*---------------------------------------------------------------------------*/

static const real32_t i_LABEL_COLUMN_MARGIN = 5;

/*---------------------------------------------------------------------------*/

static void i_remove_elem(FElem *elem)
{
    dbind_remove(elem, FElem);
}

/*---------------------------------------------------------------------------*/

static void i_remove_header(FHeader *header)
{
    dbind_remove(header, FHeader);
}

/*---------------------------------------------------------------------------*/

static Layout *i_no_sel_layout(void)
{
    return layout_create(1, 1);
}

/*---------------------------------------------------------------------------*/

static Layout *i_value_updown_layout(const char_t *tooltip)
{
    Layout *layout = layout_create(2, 1);
    Edit *edit = edit_create();
    UpDown *updown = updown_create();

    if (str_empty_c(tooltip) == FALSE)
        edit_tooltip(edit, tooltip);

    edit_align(edit, ekRIGHT);
    layout_edit(layout, edit, 0, 0);
    layout_updown(layout, updown, 1, 0);
    layout_hexpand(layout, 0);
    return layout;
}

/*---------------------------------------------------------------------------*/

static void i_OnPromoteLeft(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(data->sel.elem != ekLAYELEM_CELL);
    unref(e);
    designer_promote_left(data->app, &data->sel);
}

/*---------------------------------------------------------------------------*/

static void i_OnPromoteRight(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(data->sel.elem != ekLAYELEM_CELL);
    unref(e);
    designer_promote_right(data->app, &data->sel);
}

/*---------------------------------------------------------------------------*/

static void i_OnPromoteTop(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(data->sel.elem != ekLAYELEM_CELL);
    unref(e);
    designer_promote_top(data->app, &data->sel);
}

/*---------------------------------------------------------------------------*/

static void i_OnPromoteBottom(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(data->sel.elem != ekLAYELEM_CELL);
    unref(e);
    designer_promote_bottom(data->app, &data->sel);
}

/*---------------------------------------------------------------------------*/

static Layout *i_promote_buttons(PropData *data)
{
    Layout *layout = layout_create(4, 1);
    Button *button1 = button_flat();
    Button *button2 = button_flat();
    Button *button3 = button_flat();
    Button *button4 = button_flat();
    button_image(button1, gui_image(PROMOTE_LEFT16_PNG));
    button_image(button2, gui_image(PROMOTE_RIGHT16_PNG));
    button_image(button3, gui_image(PROMOTE_TOP16_PNG));
    button_image(button4, gui_image(PROMOTE_BOTTOM16_PNG));
    button_tooltip(button1, gui_text(TIP_PROMOTE_LEFT));
    button_tooltip(button2, gui_text(TIP_PROMOTE_RIGHT));
    button_tooltip(button3, gui_text(TIP_PROMOTE_TOP));
    button_tooltip(button4, gui_text(TIP_PROMOTE_BOTTOM));
    button_OnClick(button1, listener(data, i_OnPromoteLeft, PropData));
    button_OnClick(button2, listener(data, i_OnPromoteRight, PropData));
    button_OnClick(button3, listener(data, i_OnPromoteTop, PropData));
    button_OnClick(button4, listener(data, i_OnPromoteBottom, PropData));
    layout_button(layout, button1, 0, 0);
    layout_button(layout, button2, 1, 0);
    layout_button(layout, button3, 2, 0);
    layout_button(layout, button4, 3, 0);
    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_taborder_layout(void)
{
    Layout *layout = layout_create(2, 1);
    Button *radio1 = button_radio();
    Button *radio2 = button_radio();
    button_text(radio1, gui_text(TEXT_COLUMNS));
    button_text(radio2, gui_text(TEXT_ROWS));
    button_tooltip(radio1, gui_text(TIP_TABORDER_COLS));
    button_tooltip(radio2, gui_text(TIP_TABORDER_ROWS));
    layout_button(layout, radio1, 0, 0);
    layout_button(layout, radio2, 1, 0);
    layout_hmargin(layout, 0, 5);
    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_margin_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 8);
    Layout *layout2 = i_promote_buttons(data);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    Label *label5 = label_create();
    Label *label6 = label_create();
    Label *label7 = label_create();
    Label *label8 = label_create();
    Label *label9 = label_create();
    Edit *edit = edit_create();
    Layout *val1 = i_value_updown_layout(gui_text(TIP_TOP_MARGIN));
    Layout *val2 = i_value_updown_layout(gui_text(TIP_LEFT_MARGIN));
    Layout *val3 = i_value_updown_layout(gui_text(TIP_BOTTOM_MARGIN));
    Layout *val4 = i_value_updown_layout(gui_text(TIP_RIGHT_MARGIN));
    Layout *val5 = i_taborder_layout();
    cassert_no_null(data);
    edit_tooltip(edit, gui_text(TIP_LAYOUT_NAME));
    label_text(label1, gui_text(TEXT_TYPE));
    label_text(label2, gui_text(TEXT_NAME));
    label_text(label3, gui_text(TEXT_TOP));
    label_text(label4, gui_text(TEXT_LEFT));
    label_text(label5, gui_text(TEXT_BOTTOM));
    label_text(label6, gui_text(TEXT_RIGHT));
    label_text(label7, gui_text(TEXT_TABORDER));
    label_text(label8, gui_text(TEXT_PROMOTE));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_label(layout1, label3, 0, 2);
    layout_label(layout1, label4, 0, 3);
    layout_label(layout1, label5, 0, 4);
    layout_label(layout1, label6, 0, 5);
    layout_label(layout1, label7, 0, 6);
    layout_label(layout1, label8, 0, 7);
    layout_label(layout1, label9, 1, 0);
    layout_edit(layout1, edit, 1, 1);
    layout_layout(layout1, val1, 1, 2);
    layout_layout(layout1, val2, 1, 3);
    layout_layout(layout1, val3, 1, 4);
    layout_layout(layout1, val4, 1, 5);
    layout_layout(layout1, val5, 1, 6);
    layout_layout(layout1, layout2, 1, 7);
    layout_margin4(layout1, 1, 0, 0, 0);
    layout_vmargin(layout1, 0, 1);
    layout_vmargin(layout1, 5, 2);
    layout_vmargin(layout1, 6, 2);
    layout_halign(layout1, 1, 0, ekJUSTIFY);
    layout_halign(layout1, 1, 6, ekLEFT);
    layout_halign(layout1, 1, 7, ekLEFT);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    data->layout_type_label = label9;
    cell_dbind(layout_cell(layout1, 1, 1), FLayout, String *, name);
    cell_dbind(layout_cell(layout1, 1, 2), FLayout, real32_t, margin_top);
    cell_dbind(layout_cell(layout1, 1, 3), FLayout, real32_t, margin_left);
    cell_dbind(layout_cell(layout1, 1, 4), FLayout, real32_t, margin_bottom);
    cell_dbind(layout_cell(layout1, 1, 5), FLayout, real32_t, margin_right);
    cell_dbind(layout_cell(layout1, 1, 6), FLayout, bool_t, row_tabstop);
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_set_column_obj(PropData *data, const uint32_t col)
{
    FColumn *fcol = NULL;
    uint32_t ncols = 0;
    cassert_no_null(data);
    fcol = flayout_column(data->sel.flayout, col);
    ncols = flayout_ncols(data->sel.flayout);
    layout_dbind_obj(data->column_layout, fcol, FColumn);
    cell_enabled(data->column_margin_cell, col < ncols - 1);
}

/*---------------------------------------------------------------------------*/

static void i_set_row_obj(PropData *data, const uint32_t row)
{
    FRow *frow = NULL;
    uint32_t nrows = 0;
    cassert_no_null(data);
    frow = flayout_row(data->sel.flayout, row);
    nrows = flayout_nrows(data->sel.flayout);
    layout_dbind_obj(data->row_layout, frow, FRow);
    cell_enabled(data->row_margin_cell, row < nrows - 1);
}

/*---------------------------------------------------------------------------*/

static void i_OnColumnSelect(PropData *data, Event *e)
{
    const EvButton *p = event_params(e, EvButton);
    cassert_no_null(data);
    i_set_column_obj(data, p->index);
    data->sel.col = p->index;
    dform_update_sel(data->form, &data->sel);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static void i_OnRowSelect(PropData *data, Event *e)
{
    const EvButton *p = event_params(e, EvButton);
    cassert_no_null(data);
    i_set_row_obj(data, p->index);
    data->sel.row = p->index;
    dform_update_sel(data->form, &data->sel);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static void i_column_selector(PropData *data)
{
    uint32_t i, ncols, col = 0;
    char_t text[64];
    cassert_no_null(data);
    ncols = flayout_ncols(data->sel.flayout);
    popup_clear(data->column_popup);

    for (i = 0; i < ncols; ++i)
    {
        bstd_sprintf(text, sizeof(text), "%d", i);
        popup_add_elem(data->column_popup, text, NULL);
    }

    if (data->sel.elem == ekLAYELEM_MARGIN_COLUMN || data->sel.elem == ekLAYELEM_LAYOUT)
        col = data->sel.col;

    popup_selected(data->column_popup, col);
    i_set_column_obj(data, col);
}

/*---------------------------------------------------------------------------*/

static void i_row_selector(PropData *data)
{
    uint32_t i, nrows, row = 0;
    char_t text[64];
    cassert_no_null(data);
    nrows = flayout_nrows(data->sel.flayout);
    popup_clear(data->row_popup);

    for (i = 0; i < nrows; ++i)
    {
        bstd_sprintf(text, sizeof(text), "%d", i);
        popup_add_elem(data->row_popup, text, NULL);
    }

    if (data->sel.elem == ekLAYELEM_MARGIN_ROW || data->sel.elem == ekLAYELEM_LAYOUT)
        row = data->sel.row;

    popup_selected(data->row_popup, row);
    i_set_row_obj(data, row);
}

/*---------------------------------------------------------------------------*/

static void i_add_column(PropData *data, const uint32_t col_id)
{
    cassert_no_null(data);
    cassert_no_null(data->sel.dlayout);
    dform_insert_col(data->form, &data->sel, col_id);
    data->sel = dform_get_sel(data->form);
    i_column_selector(data);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static void i_OnColumnLeft(PropData *data, Event *e)
{
    uint32_t col_id = 0;
    unref(e);
    cassert_no_null(data);
    col_id = popup_get_selected(data->column_popup);
    i_add_column(data, col_id);
}

/*---------------------------------------------------------------------------*/

static void i_OnColumnRight(PropData *data, Event *e)
{
    uint32_t col_id = 0;
    unref(e);
    cassert_no_null(data);
    col_id = popup_get_selected(data->column_popup);
    i_add_column(data, col_id + 1);
}

/*---------------------------------------------------------------------------*/

static void i_remove_column(PropData *data, const uint32_t col_id)
{
    Window *parent = NULL;
    const Font *font = NULL;    
    cassert_no_null(data);
    cassert_no_null(data->sel.dlayout);
    parent = designer_main_window(data->app);
    font = designer_default_font(data->app);
    if (dlayout_ncols(data->sel.dlayout) > 1)
    {
        if (dialog_remove_col(parent, font, tc(data->sel.flayout->name), col_id) == TRUE)
        {
            dform_remove_col(data->form, &data->sel, col_id);
            data->sel = dform_get_sel(data->form);
            i_column_selector(data);
            designer_canvas_update(data->app);
        }
    }
    else
    {
        dialog_no_remove_col(parent, font, tc(data->sel.flayout->name));
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnColumnDelete(PropData *data, Event *e)
{
    uint32_t col_id = 0;
    unref(e);
    cassert_no_null(data);
    col_id = popup_get_selected(data->column_popup);
    i_remove_column(data, col_id);
}

/*---------------------------------------------------------------------------*/

static Layout *i_column_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 4);
    Layout *layout2 = layout_create(4, 1);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    PopUp *popup = popup_create();
    Button *button1 = button_flat();
    Button *button2 = button_flat();
    Button *button3 = button_flat();
    Button *button4 = button_flatgle();
    Layout *val1 = i_value_updown_layout(gui_text(TIP_COLUMN_MARGIN));
    Layout *val2 = i_value_updown_layout(gui_text(TIP_COLUMN_WIDTH));
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_COLUMN));
    label_text(label2, gui_text(TEXT_RIGHT));
    label_text(label3, gui_text(TEXT_WIDTH));
    popup_tooltip(popup, gui_text(TIP_COLUMN));
    popup_OnSelect(popup, listener(data, i_OnColumnSelect, PropData));
    button_image(button1, gui_image(LCOL16_PNG));
    button_image(button2, gui_image(RCOL16_PNG));
    button_image(button3, gui_image(DCOL16_PNG));
    button_image(button4, gui_image(COLEXP16_PNG));
    button_tooltip(button1, gui_text(TIP_COLUMN_LEFT));
    button_tooltip(button2, gui_text(TIP_COLUMN_RIGHT));
    button_tooltip(button3, gui_text(TIP_COLUMN_DELETE));
    button_tooltip(button4, gui_text(TIP_COLUMN_EXPAND));
    button_OnClick(button1, listener(data, i_OnColumnLeft, PropData));
    button_OnClick(button2, listener(data, i_OnColumnRight, PropData));
    button_OnClick(button3, listener(data, i_OnColumnDelete, PropData));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_label(layout1, label3, 0, 2);
    layout_popup(layout1, popup, 1, 0);
    layout_button(layout2, button1, 0, 0);
    layout_button(layout2, button2, 1, 0);
    layout_button(layout2, button3, 2, 0);
    layout_button(layout2, button4, 3, 0);
    layout_layout(layout1, val1, 1, 1);
    layout_layout(layout1, val2, 1, 2);
    layout_layout(layout1, layout2, 1, 3);
    layout_halign(layout1, 1, 3, ekLEFT);
    layout_vmargin(layout1, 0, 1);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    layout_hmargin(layout2, 2, 5);
    data->column_popup = popup;
    data->column_margin_cell = layout_cell(layout1, 1, 1);
    cell_dbind(layout_cell(layout1, 1, 1), FColumn, real32_t, margin_right);
    cell_dbind(layout_cell(layout1, 1, 2), FColumn, real32_t, forced_width);
    cell_dbind(layout_cell(layout2, 3, 0), FColumn, bool_t, expand);
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_add_row(PropData *data, const uint32_t row_id)
{
    cassert_no_null(data);
    cassert_no_null(data->sel.dlayout);
    dform_insert_row(data->form, &data->sel, row_id);
    data->sel = dform_get_sel(data->form);
    i_row_selector(data);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static void i_OnRowTop(PropData *data, Event *e)
{
    uint32_t row_id = 0;
    unref(e);
    cassert_no_null(data);
    row_id = popup_get_selected(data->row_popup);
    i_add_row(data, row_id);
}

/*---------------------------------------------------------------------------*/

static void i_OnRowBottom(PropData *data, Event *e)
{
    uint32_t row_id = 0;
    unref(e);
    cassert_no_null(data);
    row_id = popup_get_selected(data->row_popup);
    i_add_row(data, row_id + 1);
}

/*---------------------------------------------------------------------------*/

static void i_remove_row(PropData *data, const uint32_t row_id)
{
    Window *parent = NULL;
    const Font *font = NULL;    
    cassert_no_null(data);
    cassert_no_null(data->sel.dlayout);
    parent = designer_main_window(data->app);
    font = designer_default_font(data->app);
    if (dlayout_nrows(data->sel.dlayout) > 1)
    {
        if (dialog_remove_row(parent, font, tc(data->sel.flayout->name), row_id) == TRUE)
        {
            dform_remove_row(data->form, &data->sel, row_id);
            data->sel = dform_get_sel(data->form);
            i_row_selector(data);
            designer_canvas_update(data->app);
        }
    }
    else
    {
        dialog_no_remove_row(parent, font, tc(data->sel.flayout->name));
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnRowDelete(PropData *data, Event *e)
{
    uint32_t row_id = 0;
    unref(e);
    cassert_no_null(data);
    row_id = popup_get_selected(data->row_popup);
    i_remove_row(data, row_id);
}

/*---------------------------------------------------------------------------*/

static Layout *i_row_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 4);
    Layout *layout2 = layout_create(4, 1);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    PopUp *popup = popup_create();
    Button *button1 = button_flat();
    Button *button2 = button_flat();
    Button *button3 = button_flat();
    Button *button4 = button_flatgle();
    Layout *val1 = i_value_updown_layout(gui_text(TIP_ROW_MARGIN));
    Layout *val2 = i_value_updown_layout(gui_text(TIP_ROW_HEIGHT));
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_ROW));
    label_text(label2, gui_text(TEXT_BOTTOM));
    label_text(label3, gui_text(TEXT_HEIGHT));
    popup_tooltip(popup, gui_text(TIP_ROW));
    popup_OnSelect(popup, listener(data, i_OnRowSelect, PropData));
    button_image(button1, gui_image(TROW16_PNG));
    button_image(button2, gui_image(BROW16_PNG));
    button_image(button3, gui_image(DROW16_PNG));
    button_image(button4, gui_image(ROWEXP16_PNG));
    button_tooltip(button1, gui_text(TIP_ROW_TOP));
    button_tooltip(button2, gui_text(TIP_ROW_BOTTOM));
    button_tooltip(button3, gui_text(TIP_ROW_DELETE));
    button_tooltip(button4, gui_text(TIP_ROW_EXPAND));
    button_OnClick(button1, listener(data, i_OnRowTop, PropData));
    button_OnClick(button2, listener(data, i_OnRowBottom, PropData));
    button_OnClick(button3, listener(data, i_OnRowDelete, PropData));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_label(layout1, label3, 0, 2);
    layout_popup(layout1, popup, 1, 0);
    layout_button(layout2, button1, 0, 0);
    layout_button(layout2, button2, 1, 0);
    layout_button(layout2, button3, 2, 0);
    layout_button(layout2, button4, 3, 0);
    layout_layout(layout1, val1, 1, 1);
    layout_layout(layout1, val2, 1, 2);
    layout_layout(layout1, layout2, 1, 3);
    layout_halign(layout1, 1, 3, ekLEFT);
    layout_vmargin(layout1, 0, 1);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    data->row_popup = popup;
    data->row_margin_cell = layout_cell(layout1, 1, 1);
    cell_dbind(layout_cell(layout1, 1, 1), FRow, real32_t, margin_bottom);
    cell_dbind(layout_cell(layout1, 1, 2), FRow, real32_t, forced_height);
    cell_dbind(layout_cell(layout2, 3, 0), FRow, bool_t, expand);
    return layout1;
}

/*---------------------------------------------------------------------------*/

static void i_OnLayoutNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FLayout, String *, name) == TRUE)
    {
        designer_inspect_update(data->app);
    }
    else if (evbind_modify(e, FLayout, real32_t, margin_left) == TRUE 
        || evbind_modify(e, FLayout, real32_t, margin_top) == TRUE 
        || evbind_modify(e, FLayout, real32_t, margin_right) == TRUE 
        || evbind_modify(e, FLayout, real32_t, margin_bottom) == TRUE
        || evbind_modify(e, FLayout, bool_t, row_tabstop) == TRUE)
    {
        cassert(evbind_object(e, FLayout) == data->sel.flayout);
        dform_synchro_layout(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnColumnNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FColumn, real32_t, margin_right) == TRUE
        || evbind_modify(e, FColumn, real32_t, forced_width) == TRUE)
    {
        FColumn *fcol = evbind_object(e, FColumn);
        uint32_t col = popup_get_selected(data->column_popup);
        dform_synchro_col(data->form, &data->sel, fcol, col);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
    else if (evbind_modify(e, FColumn, bool_t, expand) == TRUE)
    {
        dform_synchro_cols_expand(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnRowNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FRow, real32_t, margin_bottom) == TRUE
        || evbind_modify(e, FRow, real32_t, forced_height) == TRUE)

    {
        FRow *frow = evbind_object(e, FRow);
        uint32_t row = popup_get_selected(data->row_popup);
        dform_synchro_row(data->form, &data->sel, frow, row);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
    else if (evbind_modify(e, FRow, bool_t, expand) == TRUE)
    {
        dform_synchro_rows_expand(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_layout_layout(PropData *data, const real32_t mright)
{
    Layout *layout1 = layout_create(1, 4);
    Layout *layout2 = i_margin_layout(data);
    Layout *layout3 = i_column_layout(data);
    Layout *layout4 = i_row_layout(data);
    Panel *panel1 = panel_create();
    Panel *panel2 = panel_create();
    Panel *panel3 = panel_create();
    Panel *panel4 = NULL;
    Panel *panel5 = NULL;
    Panel *panel6 = NULL;
    cassert_no_null(data);
    panel_layout(panel1, layout2);
    panel_layout(panel2, layout3);
    panel_layout(panel3, layout4);
    panel4 = designer_drawer(data->app, panel1, ekDRAWER_LAYOUT_PROPS);
    panel5 = designer_drawer(data->app, panel2, ekDRAWER_COLUMN_PROPS);
    panel6 = designer_drawer(data->app, panel3, ekDRAWER_ROW_PROPS);
    layout_panel(layout1, panel4, 0, 0);
    layout_panel(layout1, panel5, 0, 1);
    layout_panel(layout1, panel6, 0, 2);
    layout_margin4(layout1, 0, mright, 0, 0);
    layout_vexpand(layout1, 3);
    layout_dbind(layout2, listener(data, i_OnLayoutNotify, PropData), FLayout);
    layout_dbind(layout3, listener(data, i_OnColumnNotify, PropData), FColumn);
    layout_dbind(layout4, listener(data, i_OnRowNotify, PropData), FRow);
    data->layout_layout = layout2;
    data->column_layout = layout3;
    data->row_layout = layout4;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static Layout *i_empty_cell_layout(void)
{
    return layout_create(1, 1);
}

/*---------------------------------------------------------------------------*/

static Layout *i_layout_cell_layout(void)
{
    return layout_create(1, 1);
}

/*---------------------------------------------------------------------------*/

static Layout *i_drawer_layout(Designer *app, Layout *content_layout, const drawer_t drawer)
{
    Panel *panel = panel_create();
    Layout *layout = layout_create(1, 1);
    Panel *dpanel = NULL;
    panel_layout(panel, content_layout);
    dpanel = designer_drawer(app, panel, drawer);
    layout_panel(layout, dpanel, 0, 0);
    return layout;
}

/*---------------------------------------------------------------------------*/

static void i_OnLabelNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_label(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_label_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 4);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_LABEL_MWIDTH));
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    Edit *edit = edit_create();
    Button *check = button_check();
    PopUp *popup = popup_create();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_TEXT));
    label_text(label2, gui_text(TEXT_MULTILINE));
    label_text(label3, gui_text(TEXT_WIDTH));
    label_text(label4, gui_text(TEXT_ALIGN));
    edit_tooltip(edit, gui_text(TIP_LABEL_TEXT));
    button_tooltip(check, gui_text(TIP_LABEL_MLINE));
    popup_tooltip(popup, gui_text(TIP_LABEL_ALIGN));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_label(layout1, label3, 0, 2);
    layout_label(layout1, label4, 0, 3);
    layout_edit(layout1, edit, 1, 0);
    layout_button(layout1, check, 1, 1);
    layout_layout(layout1, layout2, 1, 2);
    layout_popup(layout1, popup, 1, 3);
    layout_margin4(layout1, 0, 0, 1, 0);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    cell_dbind(layout_cell(layout1, 1, 0), FLabel, String *, text);
    cell_dbind(layout_cell(layout1, 1, 1), FLabel, bool_t, multiline);
    cell_dbind(layout_cell(layout1, 1, 2), FLabel, real32_t, min_width);
    cell_dbind(layout_cell(layout1, 1, 3), FLabel, halign_t, align);
    layout_dbind(layout1, listener(data, i_OnLabelNotify, PropData), FLabel);
    data->label_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_LABEL_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnButtonNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_button(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_button_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 5);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_BUTTON_WIDTH));
    Layout *layout3 = i_value_updown_layout(gui_text(TIP_BUTTON_HPAD));
    Layout *layout4 = i_value_updown_layout(gui_text(TIP_BUTTON_VPAD));
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    Label *label5 = label_create();
    Edit *edit1 = edit_create();
    Edit *edit2 = edit_create();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_TEXT));
    label_text(label2, gui_text(TEXT_TOOLTIP));
    label_text(label3, gui_text(TEXT_WIDTH));
    label_text(label4, gui_text(TEXT_HPADDING));
    label_text(label5, gui_text(TEXT_VPADDING));
    edit_tooltip(edit1, gui_text(TIP_BUTTON_TEXT));
    edit_tooltip(edit2, gui_text(TIP_BUTTON_TOOLTIP));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_label(layout1, label3, 0, 2);
    layout_label(layout1, label4, 0, 3);
    layout_label(layout1, label5, 0, 4);
    layout_edit(layout1, edit1, 1, 0);
    layout_edit(layout1, edit2, 1, 1);
    layout_layout(layout1, layout2, 1, 2);
    layout_layout(layout1, layout3, 1, 3);
    layout_layout(layout1, layout4, 1, 4);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    cell_dbind(layout_cell(layout1, 1, 0), FButton, String *, text);
    cell_dbind(layout_cell(layout1, 1, 1), FButton, String *, tooltip);
    cell_dbind(layout_cell(layout1, 1, 2), FButton, real32_t, min_width);
    cell_dbind(layout_cell(layout1, 1, 3), FButton, real32_t, hpadding);
    cell_dbind(layout_cell(layout1, 1, 4), FButton, real32_t, vpadding);
    layout_dbind(layout1, listener(data, i_OnButtonNotify, PropData), FButton);
    data->button_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_BUTTON_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnCheckNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_check(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_check_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 1);
    Label *label = label_create();
    Edit *edit = edit_create();
    cassert_no_null(data);
    label_text(label, gui_text(TEXT_TEXT));
    edit_tooltip(edit, gui_text(TIP_CHECK_TEXT));
    layout_label(layout1, label, 0, 0);
    layout_edit(layout1, edit, 1, 0);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    cell_dbind(layout_cell(layout1, 1, 0), FCheck, String *, text);
    layout_dbind(layout1, listener(data, i_OnCheckNotify, PropData), FCheck);
    data->check_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_CHECKBOX_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnRadioNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_radio(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_radio_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 1);
    Label *label = label_create();
    Edit *edit = edit_create();
    cassert_no_null(data);
    label_text(label, gui_text(TEXT_TEXT));
    edit_tooltip(edit, gui_text(TIP_RADIO_TEXT));
    layout_label(layout1, label, 0, 0);
    layout_edit(layout1, edit, 1, 0);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    cell_dbind(layout_cell(layout1, 1, 0), FRadio, String *, text);
    layout_dbind(layout1, listener(data, i_OnRadioNotify, PropData), FRadio);
    data->radio_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_RADIO_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_update_icon(ImageView *view, Label *label, Button *button, const char_t *folder_path, const char_t *icon_path)
{
    String *path = str_cpath("%s/%s", folder_path, icon_path);
    Image *image = image_from_file(tc(path), NULL);

    if (image != NULL)
    {
        String *filename = NULL;
        str_split_pathname(icon_path, NULL, &filename);
        imageview_image(view, image);
        label_text(label, tc(filename));
        button_tooltip(button, icon_path);
        str_destroy(&filename);
        image_destroy(&image);
    }
    else
    {
        imageview_image(view, nflib_default_icon());
        label_text(label, gui_text(TEXT_DEFAULT));
        button_tooltip(button, gui_text(TEXT_LOAD_ICON));
    }

    str_destroy(&path);
}

/*---------------------------------------------------------------------------*/

static void i_OnLoadIcon(PropData *data, Event *e)
{
    const char_t *folder_path = NULL;
    const char_t *imgpath = NULL;
    Window *window = NULL;
    cassert_no_null(data);
    unref(e);
    folder_path = designer_folder_path(data->app);
    imgpath = comwin_open_file(window, NULL, NULL, 0, folder_path, NULL);
    window = designer_main_window(data->app);
    if (imgpath != NULL)
    {
        Image *image = image_from_file(imgpath, NULL);
        if (image != NULL)
        {
            String *relpath = str_relpath(ekLINUX, folder_path, imgpath);
            FCell *cell = dform_sel_fcell(&data->sel);
            const DColors *colors = designer_colors(data->app);
            cassert_no_null(cell);
            if (cell->type == ekCELL_TYPE_TOOL)
            {
                Button *button = layout_get_button(data->sel.glayout, data->sel.col, data->sel.row);
                button_image(button, image);
                str_upd(&cell->widget.tool->path, tc(relpath));
                layout_dbind_obj(data->tool_layout, cell->widget.tool, FTool);
                i_update_icon(data->view_tool_icon, data->label_tool_icon, data->button_tool_icon, folder_path, tc(cell->widget.tool->path));
            }
            else if (cell->type == ekCELL_TYPE_IMAGE)
            {
                ImageView *view = layout_get_imageview(data->sel.glayout, data->sel.col, data->sel.row);
                imageview_image(view, image);
                str_upd(&cell->widget.image->path, tc(relpath));
                layout_dbind_obj(data->image_layout, cell->widget.image, FImage);
                i_update_icon(data->view_image_icon, data->label_image_icon, data->button_image_icon, folder_path, tc(cell->widget.image->path));
            }
            else
            {
                cassert(FALSE);
            }

            dlayout_set_image(data->sel.dlayout, image, data->sel.col, data->sel.row, colors);
            dform_compose(data->form);
            designer_canvas_update(data->app);
            str_destroy(&relpath);
            image_destroy(&image);
            dform_set_need_save(data->form);
        }
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_icon_layout(PropData *data, Label **icon_label, ImageView **icon_view, Button **icon_button)
{
    Layout *layout = layout_create(3, 1);
    ImageView *view = imageview_create();
    Label *label = label_create();
    Button *button = button_push();
    cassert_no_null(data);
    cassert_no_null(icon_label);
    cassert_no_null(icon_view);
    cassert_no_null(icon_button);
    imageview_size(view, s2df(16, 16));
    imageview_scale(view, ekGUI_SCALE_ASPECT);
    /* label_ellipsis(label, ekELLIPBEGIN);    When NAppGUI supports */
    button_text(button, "...");
    button_tooltip(button, gui_text(TEXT_LOAD_ICON));
    button_OnClick(button, listener(data, i_OnLoadIcon, PropData));
    button_hpadding(button, 20);
    layout_imageview(layout, view, 0, 0);
    layout_label(layout, label, 1, 0);
    layout_button(layout, button, 2, 0);
    layout_hmargin(layout, 0, 3);
    layout_hexpand(layout, 1);
    layout_halign(layout, 1, 0, ekJUSTIFY);
    *icon_label = label;
    *icon_view = view;
    *icon_button = button;
    return layout;
}

/*---------------------------------------------------------------------------*/

static void i_OnToolNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_tool(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_tool_layout(PropData *data)
{
    Label *label_icon = NULL;
    ImageView *view_icon = NULL;
    Button *button_icon = NULL;
    Layout *layout1 = layout_create(2, 4);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_BUTTON_HPAD));
    Layout *layout3 = i_value_updown_layout(gui_text(TIP_BUTTON_VPAD));
    Layout *layout4 = i_icon_layout(data, &label_icon, &view_icon, &button_icon);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    Edit *edit = edit_create();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_TOOLTIP));
    label_text(label2, gui_text(TEXT_HPADDING));
    label_text(label3, gui_text(TEXT_VPADDING));
    label_text(label4, gui_text(TEXT_ICON));
    edit_tooltip(edit, gui_text(TIP_BUTTON_TOOLTIP));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_label(layout1, label3, 0, 2);
    layout_label(layout1, label4, 0, 3);
    layout_edit(layout1, edit, 1, 0);
    layout_layout(layout1, layout2, 1, 1);
    layout_layout(layout1, layout3, 1, 2);
    layout_layout(layout1, layout4, 1, 3);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    cell_dbind(layout_cell(layout1, 1, 0), FTool, String*, tooltip);
    cell_dbind(layout_cell(layout1, 1, 1), FTool, real32_t, hpadding);
    cell_dbind(layout_cell(layout1, 1, 2), FTool, real32_t, vpadding);
    layout_dbind(layout1, listener(data, i_OnToolNotify, PropData), FTool);
    data->label_tool_icon = label_icon;
    data->view_tool_icon = view_icon;
    data->button_tool_icon = button_icon;
    data->tool_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_TOOL_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_update_elem_list(const ArrSt(FElem) *elems, ListBox *list, const char_t *folder_path)
{
    listbox_clear(list);
    arrst_foreach_const(elem, elems, FElem)
        Image *image = NULL;
        if (str_empty(elem->iconpath) == FALSE)
        {
            String *path = str_cpath("%s/%s", folder_path, tc(elem->iconpath));
            image = image_from_file(tc(path), NULL);
            str_destroy(&path);
        }
        listbox_add_elem(list, tc(elem->text), image);
        ptr_destopt(image_destroy, &image, Image);
    arrst_end()
}

/*---------------------------------------------------------------------------*/

static void i_OnElementAdd(PropData *data, Event *e)
{
    Window *window = NULL;
    FCell *cell = NULL;
    const char_t *folder_path = NULL;
    const Font *font = NULL;
    ResId iconId = NULL;
    ResId headerId = NULL;
    String *caption = NULL;
    FElem *elem = NULL;
    cassert_no_null(data);
    unref(e);
    window = designer_main_window(data->app);
    cell = dform_sel_fcell(&data->sel);
    folder_path = designer_folder_path(data->app);
    font = designer_default_font(data->app);
    cassert_no_null(cell);

    if (cell->type == ekCELL_TYPE_POPUP)
    {
        iconId = POPUP_PNG;
        headerId = TEXT_ELEM_POPUP;
        caption = str_printf(gui_text(TEXT_NEW_ELEM_POPUP), tc(cell->name));
    }
    else if (cell->type == ekCELL_TYPE_LISTBOX)
    {
        iconId = LISTVIEW_PNG;
        headerId = TEXT_ELEM_LIST;
        caption = str_printf(gui_text(TEXT_NEW_ELEM_LIST), tc(cell->name));
    }
    else
    {
        cassert(FALSE);
    }

    elem = dialog_new_elem(window, font, tc(caption), iconId, headerId, folder_path);

    if (elem != NULL)
    {
        const DColors *colors = designer_colors(data->app);
        if (cell->type == ekCELL_TYPE_POPUP)
        {
            FPopUp *fpopup = layout_dbind_get_obj(data->popup_layout, FPopUp);
            FElem *nelem = arrst_new(fpopup->elems, FElem);
            nelem->text = str_copy(elem->text);
            nelem->iconpath = str_copy(elem->iconpath);
            i_update_elem_list(fpopup->elems, data->popup_list, folder_path);
            dform_synchro_popup(data->form, &data->sel, folder_path);
            dlayout_synchro_elems(data->sel.dlayout, data->sel.col, data->sel.row, fpopup->elems, folder_path, colors);
        }
        else if (cell->type == ekCELL_TYPE_LISTBOX)
        {
            FListBox *flistbox = layout_dbind_get_obj(data->listbox_layout, FListBox);
            FElem *nelem = arrst_new(flistbox->elems, FElem);
            nelem->text = str_copy(elem->text);
            nelem->iconpath = str_copy(elem->iconpath);
            i_update_elem_list(flistbox->elems, data->listbox_list, folder_path);
            dform_synchro_listbox(data->form, &data->sel, folder_path);
            dlayout_synchro_elems(data->sel.dlayout, data->sel.col, data->sel.row, flistbox->elems, folder_path, colors);
        }
        else
        {
            cassert(FALSE);
        }

        dform_compose(data->form);
        designer_canvas_update(data->app);
        dbind_destroy(&elem, FElem);
    }

    str_destopt(&caption);
}

/*---------------------------------------------------------------------------*/

static void i_OnElementRemove(PropData *data, Event *e)
{
    FCell *cell = NULL;
    const char_t *folder_path = NULL;
    const DColors *colors = NULL;
    cassert_no_null(data);
    unref(e);
    cell = dform_sel_fcell(&data->sel);
    folder_path = designer_folder_path(data->app);
    colors = designer_colors(data->app);
    cassert_no_null(cell);

    if (cell->type == ekCELL_TYPE_POPUP)
    {
        FPopUp *fpopup = layout_dbind_get_obj(data->popup_layout, FPopUp);
        uint32_t index = listbox_get_selected(data->popup_list);
        arrst_delete(fpopup->elems, index, i_remove_elem, FElem);
        i_update_elem_list(fpopup->elems, data->popup_list, folder_path);
        dform_synchro_popup(data->form, &data->sel, folder_path);
        dlayout_synchro_elems(data->sel.dlayout, data->sel.col, data->sel.row, fpopup->elems, folder_path, colors);
    }
    else if (cell->type == ekCELL_TYPE_LISTBOX)
    {
        FListBox *flistbox = layout_dbind_get_obj(data->listbox_layout, FListBox);
        uint32_t index = listbox_get_selected(data->listbox_list);
        arrst_delete(flistbox->elems, index, i_remove_elem, FElem);
        i_update_elem_list(flistbox->elems, data->listbox_list, folder_path);
        dform_synchro_listbox(data->form, &data->sel, folder_path);
        dlayout_synchro_elems(data->sel.dlayout, data->sel.col, data->sel.row, flistbox->elems, folder_path, colors);
    }
    else
    {
        cassert(FALSE);
    }

    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static void i_OnElementClear(PropData *data, Event *e)
{
    FCell *cell = NULL;
    const char_t *folder_path = NULL;
    const DColors *colors = NULL;
    cassert_no_null(data);
    unref(e);
    cell = dform_sel_fcell(&data->sel);
    folder_path = designer_folder_path(data->app);
    colors = designer_colors(data->app);
    cassert_no_null(cell);

    if (cell->type == ekCELL_TYPE_POPUP)
    {
        FPopUp *fpopup = layout_dbind_get_obj(data->popup_layout, FPopUp);
        arrst_clear(fpopup->elems, i_remove_elem, FElem);
        i_update_elem_list(fpopup->elems, data->popup_list, folder_path);
        dform_synchro_popup(data->form, &data->sel, folder_path);
        dlayout_synchro_elems(data->sel.dlayout, data->sel.col, data->sel.row, fpopup->elems, folder_path, colors);
    }
    else if (cell->type == ekCELL_TYPE_LISTBOX)
    {
        FListBox *flistbox = layout_dbind_get_obj(data->listbox_layout, FListBox);
        arrst_clear(flistbox->elems, i_remove_elem, FElem);
        i_update_elem_list(flistbox->elems, data->listbox_list, folder_path);
        dform_synchro_listbox(data->form, &data->sel, folder_path);
        dlayout_synchro_elems(data->sel.dlayout, data->sel.col, data->sel.row, flistbox->elems, folder_path, colors);
    }
    else
    {
        cassert(FALSE);
    }

    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_elems_layout(PropData *data, ListBox **elist)
{
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(3, 1);
    Label *label = label_create();
    ListBox *list = listbox_create();
    Button *button1 = button_flat();
    Button *button2 = button_flat();
    Button *button3 = button_flat();
    cassert_no_null(data);
    cassert_no_null(elist);
    label_text(label, gui_text(TEXT_ELEMENTS));
    button_image(button1, cast_const(PLUS16_PNG, Image));
    button_image(button2, cast_const(ERROR16_PNG, Image));
    button_image(button3, cast_const(RETRY16_PNG, Image));
    button_tooltip(button1, gui_text(TIP_ELEMENT_ADD));
    button_tooltip(button2, gui_text(TIP_ELEMENT_REMOVE));
    button_tooltip(button3, gui_text(TIP_ELEMENT_CLEAR));
    button_OnClick(button1, listener(data, i_OnElementAdd, PropData));
    button_OnClick(button2, listener(data, i_OnElementRemove, PropData));
    button_OnClick(button3, listener(data, i_OnElementClear, PropData));
    layout_label(layout1, label, 0, 0);
    layout_listbox(layout1, list, 0, 1);
    layout_button(layout2, button1, 0, 0);
    layout_button(layout2, button2, 1, 0);
    layout_button(layout2, button3, 2, 0);
    layout_layout(layout1, layout2, 0, 2);
    layout_halign(layout1, 0, 2, ekLEFT);
    *elist = list;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static Layout *i_popup_layout(PropData *data)
{
    ListBox *list = NULL;
    Layout *layout1 = layout_create(1, 2);
    Layout *layout2 = i_elems_layout(data, &list);
    cassert_no_null(data);
    layout_layout(layout1, layout2, 0, 0);
    layout_vexpand(layout1, 1);
    layout_dbind(layout1, NULL, FPopUp);
    data->popup_layout = layout1;
    data->popup_list = list;
    return i_drawer_layout(data->app, layout1, ekDRAWER_POPUP_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnEditNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FEdit, bool_t, passmode) == TRUE || evbind_modify(e, FEdit, bool_t, autosel) == TRUE || evbind_modify(e, FEdit, halign_t, text_align) == TRUE || evbind_modify(e, FEdit, real32_t, min_width) == TRUE)
    {
        dform_synchro_edit(data->form, &data->sel);

        if (evbind_modify(e, FEdit, real32_t, min_width) == TRUE)
        {
            dform_compose(data->form);
            designer_canvas_update(data->app);
        }
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_edit_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 4);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_EDIT_MWIDTH));
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    PopUp *popup = popup_create();
    Button *check1 = button_check();
    Button *check2 = button_check();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_TEXT_ALIGN));
    label_text(label2, gui_text(TEXT_MIN_WIDTH));
    label_text(label3, gui_text(TEXT_PASSMODE));
    label_text(label4, gui_text(TEXT_AUTOSELECT));
    popup_tooltip(popup, gui_text(TIP_EDIT_ALIGN));
    button_tooltip(check1, gui_text(TIP_EDIT_PASSMODE));
    button_tooltip(check2, gui_text(TIP_EDIT_AUTOSEL));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_label(layout1, label3, 0, 2);
    layout_label(layout1, label4, 0, 3);
    layout_popup(layout1, popup, 1, 0);
    layout_layout(layout1, layout2, 1, 1);
    layout_button(layout1, check1, 1, 2);
    layout_button(layout1, check2, 1, 3);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    cell_dbind(layout_cell(layout1, 1, 0), FEdit, halign_t, text_align);
    cell_dbind(layout_cell(layout1, 1, 1), FEdit, real32_t, min_width);
    cell_dbind(layout_cell(layout1, 1, 2), FEdit, bool_t, passmode);
    cell_dbind(layout_cell(layout1, 1, 3), FEdit, bool_t, autosel);
    layout_dbind(layout1, listener(data, i_OnEditNotify, PropData), FEdit);
    data->edit_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_EDIT_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnComboNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_combo(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_combo_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 4);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_COMBO_MWIDTH));
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    PopUp *popup = popup_create();
    Button *check1 = button_check();
    Button *check2 = button_check();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_TEXT_ALIGN));
    label_text(label2, gui_text(TEXT_MIN_WIDTH));
    label_text(label3, gui_text(TEXT_PASSMODE));
    label_text(label4, gui_text(TEXT_AUTOSELECT));
    popup_tooltip(popup, gui_text(TIP_COMBO_ALIGN));
    button_tooltip(check1, gui_text(TIP_COMBO_PASSMODE));
    button_tooltip(check2, gui_text(TIP_COMBO_AUTOSEL));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_label(layout1, label3, 0, 2);
    layout_label(layout1, label4, 0, 3);
    layout_popup(layout1, popup, 1, 0);
    layout_layout(layout1, layout2, 1, 1);
    layout_button(layout1, check1, 1, 2);
    layout_button(layout1, check2, 1, 3);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    cell_dbind(layout_cell(layout1, 1, 0), FCombo, halign_t, text_align);
    cell_dbind(layout_cell(layout1, 1, 1), FCombo, real32_t, min_width);
    cell_dbind(layout_cell(layout1, 1, 2), FCombo, bool_t, passmode);
    cell_dbind(layout_cell(layout1, 1, 3), FCombo, bool_t, autosel);
    layout_dbind(layout1, listener(data, i_OnComboNotify, PropData), FCombo);
    data->combo_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_COMBO_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnListBoxNotify(PropData *data, Event *e)
{
    const char_t *folder_path = NULL;
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    folder_path = designer_folder_path(data->app);
    dform_synchro_listbox(data->form, &data->sel, folder_path);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_listbox_layout(PropData *data)
{
    ListBox *list = NULL;
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 2);
    Layout *layout3 = i_elems_layout(data, &list);
    Layout *layout4 = i_value_updown_layout(gui_text(TIP_LIST_MWIDTH));
    Layout *layout5 = i_value_updown_layout(gui_text(TIP_LIST_MHEIGHT));
    Label *label1 = label_create();
    Label *label2 = label_create();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_MIN_WIDTH));
    label_text(label2, gui_text(TEXT_MIN_HEIGHT));
    layout_label(layout2, label1, 0, 0);
    layout_label(layout2, label2, 0, 1);
    layout_layout(layout2, layout4, 1, 0);
    layout_layout(layout2, layout5, 1, 1);
    layout_layout(layout1, layout2, 0, 0);
    layout_layout(layout1, layout3, 0, 1);
    layout_hmargin(layout2, 0, i_LABEL_COLUMN_MARGIN);
    layout_vexpand(layout1, 2);
    cell_dbind(layout_cell(layout2, 1, 0), FListBox, real32_t, min_width);
    cell_dbind(layout_cell(layout2, 1, 1), FListBox, real32_t, min_height);
    layout_dbind(layout1, listener(data, i_OnListBoxNotify, PropData), FListBox);
    data->listbox_layout = layout1;
    data->listbox_list = list;
    return i_drawer_layout(data->app, layout1, ekDRAWER_LIST_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnSliderNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_slider(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_slider_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 2);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_SLIDER_MWIDTH));
    Label *label1 = label_create();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_MIN_WIDTH));
    layout_label(layout1, label1, 0, 0);
    layout_layout(layout1, layout2, 1, 0);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    cell_dbind(layout_cell(layout1, 1, 0), FSlider, real32_t, min_width);
    layout_dbind(layout1, listener(data, i_OnSliderNotify, PropData), FSlider);
    data->slider_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_HSLIDER_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnVSliderNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_vslider(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_vslider_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 2);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_SLIDER_MHEIGHT));
    Label *label1 = label_create();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_MIN_HEIGHT));
    layout_label(layout1, label1, 0, 0);
    layout_layout(layout1, layout2, 1, 0);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    cell_dbind(layout_cell(layout1, 1, 0), FVSlider, real32_t, min_height);
    layout_dbind(layout1, listener(data, i_OnVSliderNotify, PropData), FVSlider);
    data->vslider_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_VSLIDER_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnProgressNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_progress(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_progress_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 2);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_PROGRESS_MWIDTH));
    Label *label1 = label_create();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_MIN_WIDTH));
    layout_label(layout1, label1, 0, 0);
    layout_layout(layout1, layout2, 1, 0);
    layout_hexpand(layout1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    cell_dbind(layout_cell(layout1, 1, 0), FProgress, real32_t, min_width);
    layout_dbind(layout1, listener(data, i_OnProgressNotify, PropData), FProgress);
    data->progress_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_PROGRESS_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnViewNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_view(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_view_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 3);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_VIEW_MWIDTH));
    Layout *layout3 = i_value_updown_layout(gui_text(TIP_VIEW_MHEIGHT));
    Label *label1 = label_create();
    Label *label2 = label_create();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_MIN_WIDTH));
    label_text(label2, gui_text(TEXT_MIN_HEIGHT));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_layout(layout1, layout2, 1, 0);
    layout_layout(layout1, layout3, 1, 1);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    layout_vexpand(layout1, 2);
    cell_dbind(layout_cell(layout1, 1, 0), FView, real32_t, min_width);
    cell_dbind(layout_cell(layout1, 1, 1), FView, real32_t, min_height);
    layout_dbind(layout1, listener(data, i_OnViewNotify, PropData), FView);
    data->view_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_VIEW_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnTextNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_textview(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_text_layout(PropData *data)
{
    Layout *layout1 = layout_create(2, 4);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_TEXT_MWIDTH));
    Layout *layout3 = i_value_updown_layout(gui_text(TIP_TEXT_MHEIGHT));
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Button *check = button_check();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_MIN_WIDTH));
    label_text(label2, gui_text(TEXT_MIN_HEIGHT));
    label_text(label3, gui_text(TEXT_READ_ONLY));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_label(layout1, label3, 0, 2);
    layout_layout(layout1, layout2, 1, 0);
    layout_layout(layout1, layout3, 1, 1);
    layout_button(layout1, check, 1, 2);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    layout_vexpand(layout1, 3);
    cell_dbind(layout_cell(layout1, 1, 0), FText, real32_t, min_width);
    cell_dbind(layout_cell(layout1, 1, 1), FText, real32_t, min_height);
    cell_dbind(layout_cell(layout1, 1, 2), FText, bool_t, read_only);
    layout_dbind(layout1, listener(data, i_OnTextNotify, PropData), FText);
    data->text_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_TEXT_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_OnImageNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_imageview(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static Layout *i_image_layout(PropData *data)
{
    Label *label_icon = NULL;
    ImageView *view_icon = NULL;
    Button *button_icon = NULL;
    Layout *layout1 = layout_create(2, 5);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_IMAGE_MWIDTH));
    Layout *layout3 = i_value_updown_layout(gui_text(TIP_IMAGE_MHEIGHT));
    Layout *layout4 = i_icon_layout(data, &label_icon, &view_icon, &button_icon);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    PopUp *popup = popup_create();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_MIN_WIDTH));
    label_text(label2, gui_text(TEXT_MIN_HEIGHT));
    label_text(label3, gui_text(TEXT_SCALE));
    label_text(label4, gui_text(TEXT_IMAGE));
    popup_tooltip(popup, gui_text(TIP_IMAGE_SCALE));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_label(layout1, label3, 0, 2);
    layout_label(layout1, label4, 0, 3);
    layout_layout(layout1, layout2, 1, 0);
    layout_layout(layout1, layout3, 1, 1);
    layout_popup(layout1, popup, 1, 2);
    layout_layout(layout1, layout4, 1, 3);
    layout_vexpand(layout1, 4);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    layout_vmargin(layout1, 2, 2);
    cell_dbind(layout_cell(layout1, 1, 0), FImage, real32_t, min_width);
    cell_dbind(layout_cell(layout1, 1, 1), FImage, real32_t, min_height);
    cell_dbind(layout_cell(layout1, 1, 2), FImage, scale_t, scale);
    layout_dbind(layout1, listener(data, i_OnImageNotify, PropData), FImage);
    data->label_image_icon = label_icon;
    data->view_image_icon = view_icon;
    data->button_image_icon = button_icon;
    data->image_layout = layout1;
    return i_drawer_layout(data->app, layout1, ekDRAWER_IMAGE_PROPS);
}

/*---------------------------------------------------------------------------*/

static void i_init_column(const uint32_t id, FHeader *header)
{
    char_t title[64];
    cassert_no_null(header);
    dbind_init(header, FHeader);
    bstd_sprintf(title, sizeof(title), "Column-%d", id);
    str_upd(&header->title, title);
}

/*---------------------------------------------------------------------------*/

static void i_OnTableNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert_unref(event_type(e) == ekGUI_EVENT_OBJCHANGE, e);
    dform_synchro_table(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static void i_OnTableColumnAdd(PropData *data, Event *e)
{
    uint32_t id = 0;
    FTable *ftable = NULL;
    FHeader *fheader = NULL;
    cassert_no_null(data);
    ftable = layout_dbind_get_obj(data->table_layout, FTable);
    id = arrst_size(ftable->headers, FHeader);
    fheader = arrst_new0(ftable->headers, FHeader);
    i_init_column(id, fheader);
    listbox_add_elem(data->table_list, tc(fheader->title), NULL);
    listbox_select(data->table_list, id, TRUE);
    layout_dbind_obj(data->header_layout, fheader, FHeader);
    dform_synchro_table(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_OnTableColumnRemove(PropData *data, Event *e)
{
    uint32_t id = 0;
    cassert_no_null(data);
    unref(e);
    id = listbox_get_selected(data->table_list);

    if (id != UINT32_MAX)
    {
        FTable *ftable = layout_dbind_get_obj(data->table_layout, FTable);
        FHeader *fheader = NULL;
        uint32_t n = 0;

        listbox_del_elem(data->table_list, id);
        arrst_delete(ftable->headers, id, i_remove_header, FHeader);
        n = arrst_size(ftable->headers, FHeader);

        if (n > 0)
        {
            cassert(id <= n);
            if (id == n)
                id -= 1;
            fheader = arrst_get(ftable->headers, id, FHeader);
            listbox_select(data->table_list, id, TRUE);
        }

        layout_dbind_obj(data->header_layout, fheader, FHeader);
        dform_synchro_table(data->form, &data->sel);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnTableColumnClear(PropData *data, Event *e)
{
    FTable *ftable = NULL;
    cassert_no_null(data);
    unref(e);
    ftable = layout_dbind_get_obj(data->table_layout, FTable);
    listbox_clear(data->table_list);
    arrst_clear(ftable->headers, i_remove_header, FHeader);
    layout_dbind_obj(data->header_layout, NULL, FHeader);
    dform_synchro_table(data->form, &data->sel);
    dform_compose(data->form);
    designer_canvas_update(data->app);
}

/*---------------------------------------------------------------------------*/

static void i_OnTableHeaderSelect(PropData *data, Event *e)
{
    const EvButton *p = event_params(e, EvButton);
    FTable *ftable = layout_dbind_get_obj(data->table_layout, FTable);
    FHeader *fheader = NULL;
    if (p->index != UINT32_MAX)
        fheader = arrst_get(ftable->headers, p->index, FHeader);
    layout_dbind_obj(data->header_layout, fheader, FHeader);
}

/*---------------------------------------------------------------------------*/

static void i_OnTableColumnNotify(PropData *data, Event *e)
{
    uint32_t id = UINT32_MAX;
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    id = listbox_get_selected(data->table_list);
    dform_synchro_table(data->form, &data->sel);
    designer_canvas_update(data->app);

    if (evbind_modify(e, FHeader, String *, title) == TRUE)
    {
        FHeader *fheader = evbind_object(e, FHeader);
        listbox_set_elem(data->table_list, id, tc(fheader->title), NULL);
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_columns_layout(PropData *data, ListBox **hlist)
{
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(3, 1);
    Label *label = label_create();
    ListBox *list = listbox_create();
    Button *button1 = button_flat();
    Button *button2 = button_flat();
    Button *button3 = button_flat();
    cassert_no_null(data);
    cassert_no_null(hlist);
    label_text(label, gui_text(TEXT_COLUMNS));
    listbox_OnSelect(list, listener(data, i_OnTableHeaderSelect, PropData));
    button_image(button1, cast_const(PLUS16_PNG, Image));
    button_image(button2, cast_const(ERROR16_PNG, Image));
    button_image(button3, cast_const(RETRY16_PNG, Image));
    button_tooltip(button1, gui_text(TIP_COLUMN_ADD));
    button_tooltip(button2, gui_text(TIP_COLUMN_REMOVE));
    button_tooltip(button3, gui_text(TIP_COLUMN_CLEAR));
    button_OnClick(button1, listener(data, i_OnTableColumnAdd, PropData));
    button_OnClick(button2, listener(data, i_OnTableColumnRemove, PropData));
    button_OnClick(button3, listener(data, i_OnTableColumnClear, PropData));
    layout_label(layout1, label, 0, 0);
    layout_listbox(layout1, list, 0, 1);
    layout_button(layout2, button1, 0, 0);
    layout_button(layout2, button2, 1, 0);
    layout_button(layout2, button3, 2, 0);
    layout_layout(layout1, layout2, 0, 2);
    layout_halign(layout1, 0, 2, ekLEFT);
    *hlist = list;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static Layout *i_table_frame_layout(PropData *data)
{
    ListBox *list = NULL;
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 2);
    Layout *layout3 = i_columns_layout(data, &list);
    Layout *layout4 = i_value_updown_layout(gui_text(TIP_TABLE_MWIDTH));
    Layout *layout5 = i_value_updown_layout(gui_text(TIP_TABLE_MHEIGHT));
    Label *label1 = label_create();
    Label *label2 = label_create();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_MIN_WIDTH));
    label_text(label2, gui_text(TEXT_MIN_HEIGHT));
    layout_label(layout2, label1, 0, 0);
    layout_label(layout2, label2, 0, 1);
    layout_layout(layout2, layout4, 1, 0);
    layout_layout(layout2, layout5, 1, 1);
    layout_layout(layout1, layout2, 0, 0);
    layout_layout(layout1, layout3, 0, 1);
    layout_hmargin(layout2, 0, i_LABEL_COLUMN_MARGIN);
    layout_vexpand(layout1, 2);
    cell_dbind(layout_cell(layout2, 1, 0), FTable, real32_t, min_width);
    cell_dbind(layout_cell(layout2, 1, 1), FTable, real32_t, min_height);
    data->table_list = list;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static Layout *i_table_column_layout(void)
{
    Layout *layout1 = layout_create(2, 7);
    Layout *layout2 = i_value_updown_layout(gui_text(TIP_COLUMN_TWIDTH));
    Layout *layout3 = i_value_updown_layout(gui_text(TIP_COLUMN_HALIGN));
    Layout *layout4 = i_value_updown_layout(gui_text(TIP_COLUMN_DALIGN));
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    Label *label5 = label_create();
    Label *label6 = label_create();
    Label *label7 = label_create();
    Edit *edit = edit_create();
    PopUp *popup1 = popup_create();
    PopUp *popup2 = popup_create();
    Button *check = button_check();
    label_text(label1, gui_text(TEXT_TITLE));
    label_text(label2, gui_text(TEXT_ALIGN));
    label_text(label3, gui_text(TEXT_DALIGN));
    label_text(label4, gui_text(TEXT_WIDTH));
    label_text(label5, gui_text(TEXT_MIN_WIDTH));
    label_text(label6, gui_text(TEXT_MAX_WIDTH));
    label_text(label7, gui_text(TEXT_RESIZABLE));
    edit_tooltip(edit, gui_text(TIP_COLUMN_TITLE));
    popup_tooltip(popup1, gui_text(TIP_COLUMN_HALIGN));
    popup_tooltip(popup2, gui_text(TIP_COLUMN_DALIGN));
    button_tooltip(check, gui_text(TIP_COLUMN_RESIZE));
    layout_label(layout1, label1, 0, 0);
    layout_label(layout1, label2, 0, 1);
    layout_label(layout1, label3, 0, 2);
    layout_label(layout1, label4, 0, 3);
    layout_label(layout1, label5, 0, 4);
    layout_label(layout1, label6, 0, 5);
    layout_label(layout1, label7, 0, 6);
    layout_edit(layout1, edit, 1, 0);
    layout_popup(layout1, popup1, 1, 1);
    layout_popup(layout1, popup2, 1, 2);
    layout_layout(layout1, layout2, 1, 3);
    layout_layout(layout1, layout3, 1, 4);
    layout_layout(layout1, layout4, 1, 5);
    layout_button(layout1, check, 1, 6);
    layout_hmargin(layout1, 0, i_LABEL_COLUMN_MARGIN);
    cell_dbind(layout_cell(layout1, 1, 0), FHeader, String *, title);
    cell_dbind(layout_cell(layout1, 1, 1), FHeader, halign_t, align);
    cell_dbind(layout_cell(layout1, 1, 2), FHeader, halign_t, dalign);
    cell_dbind(layout_cell(layout1, 1, 3), FHeader, real32_t, width);
    cell_dbind(layout_cell(layout1, 1, 4), FHeader, real32_t, min_width);
    cell_dbind(layout_cell(layout1, 1, 5), FHeader, real32_t, max_width);
    cell_dbind(layout_cell(layout1, 1, 6), FHeader, bool_t, resizable);
    return layout1;
}

/*---------------------------------------------------------------------------*/

static Layout *i_table_layout(PropData *data)
{
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = i_table_frame_layout(data);
    Layout *layout3 = i_table_column_layout();
    Panel *panel1 = panel_create();
    Panel *panel2 = panel_create();
    Panel *panel3 = NULL;
    Panel *panel4 = NULL;
    cassert_no_null(data);
    panel_layout(panel1, layout2);
    panel_layout(panel2, layout3);
    panel3 = designer_drawer(data->app, panel1, ekDRAWER_TABLE_FRAME_PROPS);
    panel4 = designer_drawer(data->app, panel2, ekDRAWER_TABLE_COLS_PROPS);
    layout_panel(layout1, panel3, 0, 0);
    layout_panel(layout1, panel4, 0, 1);
    layout_vexpand(layout1, 2);
    layout_dbind(layout2, listener(data, i_OnTableNotify, PropData), FTable);
    layout_dbind(layout3, listener(data, i_OnTableColumnNotify, PropData), FHeader);
    data->table_layout = layout2;
    data->header_layout = layout3;
    return layout1;
}

/*---------------------------------------------------------------------------*/

static Panel *i_cell_content_panel(PropData *data)
{
    Layout *layout1 = i_empty_cell_layout();
    Layout *layout2 = i_layout_cell_layout();
    Layout *layout3 = i_label_layout(data);
    Layout *layout4 = i_button_layout(data);
    Layout *layout5 = i_check_layout(data);
    Layout *layout6 = i_radio_layout(data);
    Layout *layout7 = i_tool_layout(data);
    Layout *layout8 = i_popup_layout(data);
    Layout *layout9 = i_edit_layout(data);
    Layout *layout10 = i_combo_layout(data);
    Layout *layout11 = i_listbox_layout(data);
    Layout *layout12 = i_slider_layout(data);
    Layout *layout13 = i_vslider_layout(data);
    Layout *layout14 = i_progress_layout(data);
    Layout *layout15 = i_view_layout(data);
    Layout *layout16 = i_text_layout(data);
    Layout *layout17 = i_image_layout(data);
    Layout *layout18 = i_table_layout(data);
    Panel *panel = panel_create();
    cassert_no_null(data);
    panel_layout(panel, layout1);
    panel_layout(panel, layout2);
    panel_layout(panel, layout3);
    panel_layout(panel, layout4);
    panel_layout(panel, layout5);
    panel_layout(panel, layout6);
    panel_layout(panel, layout7);
    panel_layout(panel, layout8);
    panel_layout(panel, layout9);
    panel_layout(panel, layout10);
    panel_layout(panel, layout11);
    panel_layout(panel, layout12);
    panel_layout(panel, layout13);
    panel_layout(panel, layout14);
    panel_layout(panel, layout15);
    panel_layout(panel, layout16);
    panel_layout(panel, layout17);
    panel_layout(panel, layout18);
    panel_visible_layout(panel, 0);
    data->cell_panel = panel;
    return panel;
}

/*---------------------------------------------------------------------------*/

static void i_OnCellNotify(PropData *data, Event *e)
{
    cassert_no_null(data);
    cassert(event_type(e) == ekGUI_EVENT_OBJCHANGE);
    if (evbind_modify(e, FCell, String *, name) == TRUE)
    {
        designer_inspect_update(data->app);
        dform_set_need_save(data->form);
    }
    else if (evbind_modify(e, FCell, halign_t, halign) == TRUE
        || evbind_modify(e, FCell, valign_t, valign) == TRUE
        || evbind_modify(e, FCell, bool_t, tabstop) == TRUE)
    {
        FCell *fcell = evbind_object(e, FCell);
        dform_synchro_cell(data->form, &data->sel, fcell, data->sel.col, data->sel.row);
        dform_compose(data->form);
        designer_canvas_update(data->app);
    }
}

/*---------------------------------------------------------------------------*/

static Panel *i_cell_props_panel(PropData *data)
{
    Panel *panel = panel_create();
    Layout *layout = layout_create(2, 5);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Label *label4 = label_create();
    Label *label5 = label_create();
    Label *label6 = label_create();
    Edit *edit = edit_create();
    PopUp *popup1 = popup_create();
    PopUp *popup2 = popup_create();
    Button *check = button_check();
    cassert_no_null(data);
    label_text(label1, gui_text(TEXT_TYPE));
    label_text(label2, gui_text(TEXT_NAME));
    label_text(label3, gui_text(TEXT_HALIGN));
    label_text(label4, gui_text(TEXT_VALIGN));
    label_text(label5, gui_text(TEXT_TABSTOP));
    edit_tooltip(edit, gui_text(TIP_CELL_NAME));
    popup_tooltip(popup1, gui_text(TIP_HALIGN));
    popup_tooltip(popup2, gui_text(TIP_VALIGN));
    button_tooltip(check, gui_text(TIP_CELL_TABSTOP));
    layout_label(layout, label1, 0, 0);
    layout_label(layout, label2, 0, 1);
    layout_label(layout, label3, 0, 2);
    layout_label(layout, label4, 0, 3);
    layout_label(layout, label5, 0, 4);
    layout_label(layout, label6, 1, 0);
    layout_edit(layout, edit, 1, 1);
    layout_popup(layout, popup1, 1, 2);
    layout_popup(layout, popup2, 1, 3);
    layout_button(layout, check, 1, 4);
    layout_margin4(layout, 1, 0, 1, 0);
    layout_vmargin(layout, 0, 1);
    layout_halign(layout, 1, 0, ekJUSTIFY);
    layout_hmargin(layout, 0, i_LABEL_COLUMN_MARGIN);
    layout_hexpand(layout, 1);
    data->cell_type_label = label6;
    cell_dbind(layout_cell(layout, 1, 1), FCell, String *, name);
    cell_dbind(layout_cell(layout, 1, 2), FCell, halign_t, halign);
    cell_dbind(layout_cell(layout, 1, 3), FCell, valign_t, valign);
    cell_dbind(layout_cell(layout, 1, 4), FCell, bool_t, tabstop);
    layout_dbind(layout, listener(data, i_OnCellNotify, PropData), FCell);
    panel_layout(panel, layout);
    data->cell_layout = layout;
    return panel;
}

/*---------------------------------------------------------------------------*/

static Layout *i_cell_layout(PropData *data, const real32_t mright)
{
    Layout *layout = layout_create(1, 3);
    Panel *panel1 = i_cell_props_panel(data);
    Panel *panel2 = i_cell_content_panel(data);
    Panel *panel3 = NULL;
    cassert_no_null(data);
    panel3 = designer_drawer(data->app, panel1, ekDRAWER_CELL_PROPS);
    layout_panel(layout, panel3, 0, 0);
    layout_panel(layout, panel2, 0, 1);
    layout_margin4(layout, 0, mright, 0, 0);
    layout_vexpand(layout, 2);
    return layout;
}

/*---------------------------------------------------------------------------*/

static PropData *i_data(Designer *app)
{
    PropData *data = heap_new0(PropData);
    data->app = app;
    return data;
}

/*---------------------------------------------------------------------------*/

static void i_destroy_data(PropData **data)
{
    heap_delete(data, PropData);
}

/*---------------------------------------------------------------------------*/

Panel *propedit_create(Designer *app)
{
    PropData *data = i_data(app);
    Panel *panel = panel_custom(FALSE, TRUE, FALSE);
    real32_t mright = panel_scroll_width(panel);
    Layout *layout0 = i_no_sel_layout();
    Layout *layout1 = i_layout_layout(data, mright);
    Layout *layout2 = i_cell_layout(data, mright);
    panel_layout(panel, layout0);
    panel_layout(panel, layout1);
    panel_layout(panel, layout2);
    panel_data(panel, &data, i_destroy_data, PropData);
    panel_visible_layout(panel, 0);
    return panel;
}

/*---------------------------------------------------------------------------*/

static void i_update_header_list(ArrSt(FHeader) *headers, ListBox *list, Layout *header_layout)
{
    FHeader *fheader = NULL;
    listbox_clear(list);
    arrst_foreach_const(header, headers, FHeader)
        listbox_add_elem(list, tc(header->title), NULL);
    arrst_end()

    if (arrst_size(headers, FHeader) > 0)
    {
        fheader = arrst_first(headers, FHeader);
        listbox_select(list, 0, TRUE);
    }

    layout_dbind_obj(header_layout, fheader, FHeader);
}

/*---------------------------------------------------------------------------*/

void propedit_set(Panel *panel, DForm *form, const DSelect *sel)
{
    PropData *data = panel_get_data(panel, PropData);
    cassert_no_null(sel);
    data->form = form;
    data->sel = *sel;

    /* Unbind all objects in edition */
    layout_dbind_obj(data->layout_layout, NULL, FLayout);
    layout_dbind_obj(data->cell_layout, NULL, FCell);
    layout_dbind_obj(data->label_layout, NULL, FLabel);
    layout_dbind_obj(data->button_layout, NULL, FButton);
    layout_dbind_obj(data->check_layout, NULL, FCheck);
    layout_dbind_obj(data->radio_layout, NULL, FRadio);
    layout_dbind_obj(data->tool_layout, NULL, FTool);
    layout_dbind_obj(data->popup_layout, NULL, FPopUp);
    layout_dbind_obj(data->edit_layout, NULL, FEdit);
    layout_dbind_obj(data->combo_layout, NULL, FCombo);
    layout_dbind_obj(data->listbox_layout, NULL, FListBox);
    layout_dbind_obj(data->slider_layout, NULL, FSlider);
    layout_dbind_obj(data->vslider_layout, NULL, FVSlider);
    layout_dbind_obj(data->progress_layout, NULL, FProgress);
    layout_dbind_obj(data->view_layout, NULL, FView);
    layout_dbind_obj(data->text_layout, NULL, FText);
    layout_dbind_obj(data->image_layout, NULL, FImage);
    layout_dbind_obj(data->table_layout, NULL, FTable);

    /* i_no_sel_layout */
    if (sel->flayout == NULL)
    {
        panel_visible_layout(panel, 0);
        designer_clipboard_controls(data->app, FALSE, FALSE);
    }
    /* i_layout_layout */
    else if (sel->elem != ekLAYELEM_CELL)
    {
        char_t text[64];
        uint32_t ncols = flayout_ncols(sel->flayout);
        uint32_t nrows = flayout_nrows(sel->flayout);

        if (ncols == 1 && nrows == 1)
            blib_strcpy(text, sizeof(text), gui_text(TEXT_LTYPE_SING));
        else if (ncols == 1)
            bstd_sprintf(text, sizeof(text), gui_text(TEXT_LTYPE_VERT), nrows);
        else if (nrows == 1)
            bstd_sprintf(text, sizeof(text), gui_text(TEXT_LTYPE_HORZ), ncols);
        else
            bstd_sprintf(text, sizeof(text), gui_text(TEXT_LTYPE_GRID), ncols, nrows);

        label_text(data->layout_type_label, text);
        i_column_selector(data);
        i_row_selector(data);
        layout_dbind_obj(data->layout_layout, sel->flayout, FLayout);
        panel_visible_layout(panel, 1);
        designer_clipboard_controls(data->app, TRUE, FALSE);
    }
    /* i_cell_layout */
    else
    {
        FCell *cell = dform_sel_fcell(sel);
        const char_t *type = dform_cell_type(cell->type);
        const char_t *folder_path = designer_folder_path(data->app);
        char_t text[64];
        bstd_sprintf(text, sizeof(text), "%s (%d,%d)", type, sel->col, sel->row);
        label_text(data->cell_type_label, text);
        panel_visible_layout(panel, 2);
        layout_dbind_obj(data->cell_layout, cell, FCell);
        if (cell->type == ekCELL_TYPE_EMPTY)
        {
            panel_visible_layout(data->cell_panel, 0);
        }
        else if (cell->type == ekCELL_TYPE_LAYOUT)
        {
            panel_visible_layout(data->cell_panel, 1);
        }
        else if (cell->type == ekCELL_TYPE_LABEL)
        {
            layout_dbind_obj(data->label_layout, cell->widget.label, FLabel);
            panel_visible_layout(data->cell_panel, 2);
        }
        else if (cell->type == ekCELL_TYPE_BUTTON)
        {
            layout_dbind_obj(data->button_layout, cell->widget.button, FButton);
            panel_visible_layout(data->cell_panel, 3);
        }
        else if (cell->type == ekCELL_TYPE_CHECK)
        {
            layout_dbind_obj(data->check_layout, cell->widget.check, FCheck);
            panel_visible_layout(data->cell_panel, 4);
        }
        else if (cell->type == ekCELL_TYPE_RADIO)
        {
            layout_dbind_obj(data->radio_layout, cell->widget.radio, FRadio);
            panel_visible_layout(data->cell_panel, 5);
        }
        else if (cell->type == ekCELL_TYPE_TOOL)
        {
            layout_dbind_obj(data->tool_layout, cell->widget.tool, FTool);
            i_update_icon(data->view_tool_icon, data->label_tool_icon, data->button_tool_icon, folder_path, tc(cell->widget.tool->path));
            panel_visible_layout(data->cell_panel, 6);
        }
        else if (cell->type == ekCELL_TYPE_POPUP)
        {
            layout_dbind_obj(data->popup_layout, cell->widget.popup, FPopUp);
            i_update_elem_list(cell->widget.popup->elems, data->popup_list, folder_path);
            panel_visible_layout(data->cell_panel, 7);
        }
        else if (cell->type == ekCELL_TYPE_EDIT)
        {
            layout_dbind_obj(data->edit_layout, cell->widget.edit, FEdit);
            panel_visible_layout(data->cell_panel, 8);
        }
        else if (cell->type == ekCELL_TYPE_COMBO)
        {
            layout_dbind_obj(data->combo_layout, cell->widget.combo, FCombo);
            panel_visible_layout(data->cell_panel, 9);
        }
        else if (cell->type == ekCELL_TYPE_LISTBOX)
        {
            layout_dbind_obj(data->listbox_layout, cell->widget.listbox, FListBox);
            i_update_elem_list(cell->widget.listbox->elems, data->listbox_list, folder_path);
            panel_visible_layout(data->cell_panel, 10);
        }
        else if (cell->type == ekCELL_TYPE_SLIDER)
        {
            layout_dbind_obj(data->slider_layout, cell->widget.slider, FSlider);
            panel_visible_layout(data->cell_panel, 11);
        }
        else if (cell->type == ekCELL_TYPE_VSLIDER)
        {
            layout_dbind_obj(data->vslider_layout, cell->widget.vslider, FVSlider);
            panel_visible_layout(data->cell_panel, 12);
        }
        else if (cell->type == ekCELL_TYPE_PROGRESS)
        {
            layout_dbind_obj(data->progress_layout, cell->widget.progress, FProgress);
            panel_visible_layout(data->cell_panel, 13);
        }
        else if (cell->type == ekCELL_TYPE_VIEW)
        {
            layout_dbind_obj(data->view_layout, cell->widget.view, FView);
            panel_visible_layout(data->cell_panel, 14);
        }
        else if (cell->type == ekCELL_TYPE_TEXT)
        {
            layout_dbind_obj(data->text_layout, cell->widget.text, FText);
            panel_visible_layout(data->cell_panel, 15);
        }
        else if (cell->type == ekCELL_TYPE_IMAGE)
        {
            layout_dbind_obj(data->image_layout, cell->widget.image, FImage);
            i_update_icon(data->view_image_icon, data->label_image_icon, data->button_image_icon, folder_path, tc(cell->widget.image->path));
            panel_visible_layout(data->cell_panel, 16);
        }
        else if (cell->type == ekCELL_TYPE_TABLEVIEW)
        {
            layout_dbind_obj(data->table_layout, cell->widget.table, FTable);
            i_update_header_list(cell->widget.table->headers, data->table_list, data->header_layout);
            panel_visible_layout(data->cell_panel, 17);
        }
        else
        {
            cassert(FALSE);
            panel_visible_layout(data->cell_panel, 0);
        }

        if (cell->type == ekCELL_TYPE_EMPTY)
            designer_clipboard_controls(data->app, FALSE, TRUE);
        else
            designer_clipboard_controls(data->app, TRUE, FALSE);

        panel_update(data->cell_panel);
    }

    panel_update(panel);
}
