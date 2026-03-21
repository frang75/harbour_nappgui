/* Form layout */

#include "flayout.h"
#include "nflib.h"
#include "nflib.inl"
#include "fcheck.h"
#include "fcombo.h"
#include "fbutton.h"
#include "fedit.h"
#include "flabel.h"
#include "flistbox.h"
#include "fimage.h"
#include "fpopup.h"
#include "fprogress.h"
#include "fradio.h"
#include "fslider.h"
#include "fvslider.h"
#include "ftable.h"
#include "ftext.h"
#include "ftool.h"
#include "fview.h"
#include "fsview.h"
#include "fhline.h"
#include "fvline.h"
#include <gui/gui.h>
#include <gui/button.h>
#include <gui/cell.h>
#include <gui/combo.h>
#include <gui/label.h>
#include <gui/layout.h>
#include <gui/layouth.h>
#include <gui/line.h>
#include <gui/listbox.h>
#include <gui/edit.h>
#include <gui/textview.h>
#include <gui/imageview.h>
#include <gui/progress.h>
#include <gui/popup.h>
#include <gui/slider.h>
#include <gui/tableview.h>
#include <gui/view.h>
#include <draw2d/color.h>
#include <draw2d/image.h>
#include <geom2d/s2d.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <core/heap.h>
#include <core/stream.h>
#include <core/strings.h>
#include <sewer/cassert.h>
#include <sewer/bmem.h>
#include <sewer/ptr.h>

/*---------------------------------------------------------------------------*/

static uint16_t i_VERSION = 10;
static void i_write_layout(Stream *stm, const FLayout *layout);

/*---------------------------------------------------------------------------*/

static void i_remove_column(FColumn *column)
{
    dbind_remove(column, FColumn);
}

/*---------------------------------------------------------------------------*/

static void i_remove_row(FRow *row)
{
    dbind_remove(row, FRow);
}

/*---------------------------------------------------------------------------*/

static void i_remove_cell(FCell *cell)
{
    /* TODO: Remove when 'dbind()' support unions */
    cassert_no_null(cell);
    str_destroy(&cell->name);
    switch (cell->type)
    {
    case ekCELL_TYPE_EMPTY:
        break;

    case ekCELL_TYPE_LABEL:
        flabel_destroy(&cell->widget.label);
        break;

    case ekCELL_TYPE_BUTTON:
        fbutton_destroy(&cell->widget.button);
        break;

    case ekCELL_TYPE_CHECK:
        fcheck_destroy(&cell->widget.check);
        break;

    case ekCELL_TYPE_RADIO:
        fradio_destroy(&cell->widget.radio);
        break;

    case ekCELL_TYPE_TOOL:
        ftool_destroy(&cell->widget.tool);
        break;

    case ekCELL_TYPE_POPUP:
        fpopup_destroy(&cell->widget.popup);
        break;

    case ekCELL_TYPE_EDIT:
        fedit_destroy(&cell->widget.edit);
        break;

    case ekCELL_TYPE_COMBO:
        fcombo_destroy(&cell->widget.combo);
        break;

    case ekCELL_TYPE_LISTBOX:
        flistbox_destroy(&cell->widget.listbox);
        break;

    case ekCELL_TYPE_SLIDER:
        fslider_destroy(&cell->widget.slider);
        break;

    case ekCELL_TYPE_VSLIDER:
        fvslider_destroy(&cell->widget.vslider);
        break;

    case ekCELL_TYPE_PROGRESS:
        fprogress_destroy(&cell->widget.progress);
        break;

    case ekCELL_TYPE_VIEW:
        fview_destroy(&cell->widget.view);
        break;

    case ekCELL_TYPE_SCROLL_VIEW:
        fsview_destroy(&cell->widget.sview);
        break;

    case ekCELL_TYPE_TEXT:
        ftext_destroy(&cell->widget.text);
        break;

    case ekCELL_TYPE_IMAGE:
        fimage_destroy(&cell->widget.image);
        break;

    case ekCELL_TYPE_TABLEVIEW:
        ftable_destroy(&cell->widget.table);
        break;

    case ekCELL_TYPE_HLINE:
        fhline_destroy(&cell->widget.hline);
        break;

    case ekCELL_TYPE_VLINE:
        fvline_destroy(&cell->widget.vline);
        break;

    case ekCELL_TYPE_LAYOUT:
        flayout_destroy(&cell->widget.layout);
        break;

    default:
        cassert_default(cell->type);
    }

    cassert(cell->widget.label == NULL);
    cassert(cell->widget.button == NULL);
    cassert(cell->widget.check == NULL);
    cassert(cell->widget.radio == NULL);
    cassert(cell->widget.tool == NULL);
    cassert(cell->widget.popup == NULL);
    cassert(cell->widget.edit == NULL);
    cassert(cell->widget.combo == NULL);
    cassert(cell->widget.listbox == NULL);
    cassert(cell->widget.slider == NULL);
    cassert(cell->widget.vslider == NULL);
    cassert(cell->widget.progress == NULL);
    cassert(cell->widget.text == NULL);
    cassert(cell->widget.image == NULL);
    cassert(cell->widget.table == NULL);
    cassert(cell->widget.hline == NULL);
    cassert(cell->widget.vline == NULL);
    cassert(cell->widget.layout == NULL);
}

/*---------------------------------------------------------------------------*/

FLayout *flayout_create(const uint32_t ncols, const uint32_t nrows)
{
    FLayout *layout = dbind_create(FLayout);
    cassert(ncols > 0);
    cassert(nrows > 0);

    /* Add columns to layout */
    {
        uint32_t i = 0;
        for (i = 0; i < ncols; ++i)
        {
            FColumn *col = arrst_new(layout->cols, FColumn);
            dbind_init(col, FColumn);
        }
    }

    /* Add rows to layout */
    {
        uint32_t i = 0;
        for (i = 0; i < nrows; ++i)
        {
            FRow *row = arrst_new(layout->rows, FRow);
            dbind_init(row, FRow);
        }
    }

    /* Add cells to layout */
    {
        uint32_t n = ncols * nrows, i = 0;
        for (i = 0; i < n; ++i)
        {
            FCell *cell = arrst_new0(layout->cells, FCell);
            cell->name = str_c("");
            cell->tabstop = TRUE;
            cell->type = ekCELL_TYPE_EMPTY;
            cell->halign = ekHALIGN_LEFT;
            cell->valign = ekVALIGN_TOP;
            /* TODO: Use dbind_init() when dbind support unions */
        }
    }

    return layout;
}

/*---------------------------------------------------------------------------*/

static void i_read_col(Stream *stm, FColumn *col, const uint16_t *vers)
{
    cassert_no_null(col);
    cassert_no_null(vers);
    if (*vers >= 5)
        col->expand = stm_read_bool(stm);
    else
        col->expand = FALSE;

    col->forced_width = stm_read_r32(stm);
    col->margin_right = stm_read_r32(stm);
}

/*---------------------------------------------------------------------------*/

static void i_read_row(Stream *stm, FRow *row, const uint16_t *vers)
{
    cassert_no_null(row);
    cassert_no_null(vers);
    if (*vers >= 5)
        row->expand = stm_read_bool(stm);
    else
        row->expand = FALSE;

    row->forced_height = stm_read_r32(stm);
    row->margin_bottom = stm_read_r32(stm);
}

/*---------------------------------------------------------------------------*/

static FLabel *i_read_label(Stream *stm, const uint16_t vers)
{
    FLabel *label = heap_new0(FLabel);
    label->text = str_read(stm);
    if (vers >= 3)
    {
        label->multiline = stm_read_bool(stm);
        label->min_width = stm_read_r32(stm);
        label->align = stm_read_enum(stm, halign_t);
    }
    else
    {
        label->multiline = FALSE;
        label->min_width = 0;
        label->align = ekHALIGN_LEFT;
    }
    return label;
}

/*---------------------------------------------------------------------------*/

static FButton *i_read_button(Stream *stm, const uint16_t vers)
{
    FButton *button = heap_new0(FButton);
    button->text = str_read(stm);
    if (vers >= 6)
        button->tooltip = str_read(stm);
    else
        button->tooltip = str_c("");

    if (vers >= 1)
        button->min_width = stm_read_r32(stm);
    else
        button->min_width = 0;

    if (vers >= 4)
    {
        button->hpadding = stm_read_r32(stm);
        button->vpadding = stm_read_r32(stm);
    }
    else
    {
        button->hpadding = -1;
        button->vpadding = -1;
    }

    return button;
}

/*---------------------------------------------------------------------------*/

static FCheck *i_read_check(Stream *stm)
{
    FCheck *check = heap_new0(FCheck);
    check->text = str_read(stm);
    return check;
}

/*---------------------------------------------------------------------------*/

static FRadio *i_read_radio(Stream *stm)
{
    FRadio *radio = heap_new0(FRadio);
    radio->text = str_read(stm);
    return radio;
}

/*---------------------------------------------------------------------------*/

static FTool *i_read_tool(Stream *stm, const uint16_t vers)
{
    FTool *tool = heap_new0(FTool);
    tool->path = str_read(stm);
    if (vers >= 6)
        tool->tooltip = str_read(stm);
    else
        tool->tooltip = str_c("");

    tool->hpadding = stm_read_r32(stm);
    tool->vpadding = stm_read_r32(stm);
    return tool;
}

/*---------------------------------------------------------------------------*/

static void i_read_elem(Stream *stm, FElem *elem)
{
    elem->text = str_read(stm);
    elem->iconpath = str_read(stm);
}

/*---------------------------------------------------------------------------*/

static FPopUp *i_read_popup(Stream *stm)
{
    FPopUp *popup = heap_new0(FPopUp);
    popup->elems = arrst_read(stm, i_read_elem, FElem);
    return popup;
}

/*---------------------------------------------------------------------------*/

static FEdit *i_read_edit(Stream *stm, const uint16_t vers)
{
    FEdit *edit = heap_new0(FEdit);
    edit->passmode = stm_read_bool(stm);
    edit->autosel = stm_read_bool(stm);
    edit->text_align = stm_read_enum(stm, halign_t);

    if (vers >= 2)
        edit->min_width = stm_read_r32(stm);
    else
        edit->min_width = 100;

    return edit;
}

/*---------------------------------------------------------------------------*/

static FCombo *i_read_combo(Stream *stm)
{
    FCombo *combo = heap_new0(FCombo);
    combo->passmode = stm_read_bool(stm);
    combo->autosel = stm_read_bool(stm);
    combo->text_align = stm_read_enum(stm, halign_t);
    combo->min_width = stm_read_r32(stm);
    return combo;
}

/*---------------------------------------------------------------------------*/

static FListBox *i_read_listbox(Stream *stm)
{
    FListBox *listbox = heap_new0(FListBox);
    listbox->min_width = stm_read_r32(stm);
    listbox->min_height = stm_read_r32(stm);
    listbox->elems = arrst_read(stm, i_read_elem, FElem);
    return listbox;
}

/*---------------------------------------------------------------------------*/

static FSlider *i_read_slider(Stream *stm)
{
    FSlider *slider = heap_new0(FSlider);
    slider->min_width = stm_read_r32(stm);
    return slider;
}

/*---------------------------------------------------------------------------*/

static FVSlider *i_read_vslider(Stream *stm)
{
    FVSlider *slider = heap_new0(FVSlider);
    slider->min_height = stm_read_r32(stm);
    return slider;
}

/*---------------------------------------------------------------------------*/

static FProgress *i_read_progress(Stream *stm)
{
    FProgress *progress = heap_new0(FProgress);
    progress->min_width = stm_read_r32(stm);
    return progress;
}

/*---------------------------------------------------------------------------*/

static FView *i_read_view(Stream *stm)
{
    FView *view = heap_new0(FView);
    view->min_width = stm_read_r32(stm);
    view->min_height = stm_read_r32(stm);
    return view;
}

/*---------------------------------------------------------------------------*/

static FSView *i_read_sview(Stream *stm)
{
    FSView *sview = heap_new0(FSView);
    sview->min_width = stm_read_r32(stm);
    sview->min_height = stm_read_r32(stm);
    return sview;
}

/*---------------------------------------------------------------------------*/

static FText *i_read_text(Stream *stm)
{
    FText *text = heap_new0(FText);
    text->read_only = stm_read_bool(stm);
    text->min_width = stm_read_r32(stm);
    text->min_height = stm_read_r32(stm);
    return text;
}

/*---------------------------------------------------------------------------*/

static FImage *i_read_image(Stream *stm)
{
    FImage *image = heap_new0(FImage);
    image->path = str_read(stm);
    image->scale = stm_read_enum(stm, scale_t);
    image->min_width = stm_read_r32(stm);
    image->min_height = stm_read_r32(stm);
    return image;
}

/*---------------------------------------------------------------------------*/

static void i_read_header(Stream *stm, FHeader *header)
{
    header->title = str_read(stm);
    header->align = stm_read_enum(stm, halign_t);
    header->dalign = stm_read_enum(stm, halign_t);
    header->resizable = stm_read_bool(stm);
    header->width = stm_read_r32(stm);
    header->min_width = stm_read_r32(stm);
    header->max_width = stm_read_r32(stm);
}

/*---------------------------------------------------------------------------*/

static FTable *i_read_table(Stream *stm)
{
    FTable *table = heap_new0(FTable);
    table->min_width = stm_read_r32(stm);
    table->min_height = stm_read_r32(stm);
    table->headers = arrst_read(stm, i_read_header, FHeader);
    return table;
}

/*---------------------------------------------------------------------------*/

static FHline *i_read_hline(Stream *stm)
{
    FHline *line = heap_new0(FHline);
    line->length = stm_read_r32(stm);
    return line;
}

/*---------------------------------------------------------------------------*/

static FVline *i_read_vline(Stream *stm)
{
    FVline *line = heap_new0(FVline);
    line->length = stm_read_r32(stm);
    return line;
}

/*---------------------------------------------------------------------------*/

static void i_read_cell(Stream *stm, FCell *cell, const uint16_t *vers)
{
    cassert_no_null(cell);
    cassert_no_null(vers);
    bmem_zero(cell, FCell);
    cell->name = str_read(stm);

    if (*vers >= 7)
        cell->tabstop = stm_read_bool(stm);
    else
        cell->tabstop = TRUE;

    cell->type = stm_read_enum(stm, celltype_t);
    cell->halign = stm_read_enum(stm, halign_t);
    cell->valign = stm_read_enum(stm, valign_t);

    if (*vers >= 9)
    {
        cell->padding_left = stm_read_r32(stm);
        cell->padding_top = stm_read_r32(stm);
        cell->padding_right = stm_read_r32(stm);
        cell->padding_bottom = stm_read_r32(stm);
    }
    else
    {
        cell->padding_left = 0;
        cell->padding_top = 0;
        cell->padding_right = 0;
        cell->padding_bottom = 0;
    }

    switch (cell->type)
    {
    case ekCELL_TYPE_EMPTY:
        break;
    case ekCELL_TYPE_LABEL:
        cell->widget.label = i_read_label(stm, *vers);
        break;
    case ekCELL_TYPE_BUTTON:
        cell->widget.button = i_read_button(stm, *vers);
        break;
    case ekCELL_TYPE_CHECK:
        cell->widget.check = i_read_check(stm);
        break;
    case ekCELL_TYPE_RADIO:
        cell->widget.radio = i_read_radio(stm);
        break;
    case ekCELL_TYPE_TOOL:
        cell->widget.tool = i_read_tool(stm, *vers);
        break;
    case ekCELL_TYPE_POPUP:
        cell->widget.popup = i_read_popup(stm);
        break;
    case ekCELL_TYPE_EDIT:
        cell->widget.edit = i_read_edit(stm, *vers);
        break;
    case ekCELL_TYPE_COMBO:
        cell->widget.combo = i_read_combo(stm);
        break;
    case ekCELL_TYPE_LISTBOX:
        cell->widget.listbox = i_read_listbox(stm);
        break;
    case ekCELL_TYPE_SLIDER:
        cell->widget.slider = i_read_slider(stm);
        break;
    case ekCELL_TYPE_VSLIDER:
        cell->widget.vslider = i_read_vslider(stm);
        break;
    case ekCELL_TYPE_PROGRESS:
        cell->widget.progress = i_read_progress(stm);
        break;
    case ekCELL_TYPE_VIEW:
        cell->widget.view = i_read_view(stm);
        break;
    case ekCELL_TYPE_SCROLL_VIEW:
        cell->widget.sview = i_read_sview(stm);
        break;
    case ekCELL_TYPE_TEXT:
        cell->widget.text = i_read_text(stm);
        break;
    case ekCELL_TYPE_IMAGE:
        cell->widget.image = i_read_image(stm);
        break;
    case ekCELL_TYPE_TABLEVIEW:
        cell->widget.table = i_read_table(stm);
        break;
    case ekCELL_TYPE_HLINE:
        cell->widget.hline = i_read_hline(stm);
        break;
    case ekCELL_TYPE_VLINE:
        cell->widget.vline = i_read_vline(stm);
        break;
    case ekCELL_TYPE_LAYOUT:
        if (*vers >= 8)
            cell->widget.layout = flayout_read_with_vers(stm, *vers);
        else
            cell->widget.layout = flayout_read(stm);
        break;
    default:
        cassert_default(cell->type);
    }
}

/*---------------------------------------------------------------------------*/

FLayout *flayout_read(Stream *stm)
{
    uint16_t vers = stm_read_u16(stm);
    return flayout_read_with_vers(stm, vers);
}

/*---------------------------------------------------------------------------*/

FLayout *flayout_read_with_vers(Stream *stm, const uint16_t vers)
{
    if (vers <= i_VERSION)
    {
        FLayout *layout = heap_new0(FLayout);
        layout->name = str_read(stm);

        if (vers >= 7)
            layout->row_tabstop = stm_read_bool(stm);
        else
            layout->row_tabstop = TRUE;

        layout->margin_left = stm_read_r32(stm);
        layout->margin_top = stm_read_r32(stm);
        layout->margin_right = stm_read_r32(stm);
        layout->margin_bottom = stm_read_r32(stm);

        if (vers >= 10)
        {
            layout->with_border = stm_read_bool(stm);
            layout->with_background = stm_read_bool(stm);
            layout->with_group = stm_read_bool(stm);
            layout->border_light = stm_read_u32(stm);
            layout->border_dark = stm_read_u32(stm);
            layout->backgd_light = stm_read_u32(stm);
            layout->backgd_dark = stm_read_u32(stm);
            layout->group_title = str_read(stm);
        }
        else
        {
            layout->with_border = FALSE;
            layout->with_background = FALSE;
            layout->with_group = FALSE;
            layout->border_light = color_rgb(225, 225, 225);
            layout->border_dark = color_rgb(100, 100, 100);
            layout->backgd_light = color_rgb(225, 225, 225);
            layout->backgd_dark = color_rgb(100, 100, 100);
            layout->group_title = str_c("");
        }

        layout->cols = arrst_read_ex(stm, i_read_col, &vers, FColumn, uint16_t);
        layout->rows = arrst_read_ex(stm, i_read_row, &vers, FRow, uint16_t);
        layout->cells = arrst_read_ex(stm, i_read_cell, &vers, FCell, uint16_t);
        return layout;
    }
    else
    {
        stm_corrupt(stm);
        return NULL;
    }
}

/*---------------------------------------------------------------------------*/

void flayout_destroy(FLayout **layout)
{
    cassert_no_null(layout);
    str_destroy(&(*layout)->name);
    arrst_destroy(&(*layout)->cols, i_remove_column, FColumn);
    arrst_destroy(&(*layout)->rows, i_remove_row, FRow);
    arrst_destroy(&(*layout)->cells, i_remove_cell, FCell);
    heap_delete(layout, FLayout);
    /* TODO: Change by */
    /* dbind_destroy(layout, FLayout); */
}

/*---------------------------------------------------------------------------*/

static void i_write_col(Stream *stm, const FColumn *col)
{
    cassert_no_null(col);
    stm_write_bool(stm, col->expand);
    stm_write_r32(stm, col->forced_width);
    stm_write_r32(stm, col->margin_right);
}

/*---------------------------------------------------------------------------*/

static void i_write_row(Stream *stm, const FRow *row)
{
    cassert_no_null(row);
    stm_write_bool(stm, row->expand);
    stm_write_r32(stm, row->forced_height);
    stm_write_r32(stm, row->margin_bottom);
}

/*---------------------------------------------------------------------------*/

static void i_write_label(Stream *stm, const FLabel *label)
{
    cassert_no_null(label);
    str_write(stm, label->text);
    stm_write_bool(stm, label->multiline);
    stm_write_r32(stm, label->min_width);
    stm_write_enum(stm, label->align, halign_t);
}

/*---------------------------------------------------------------------------*/

static void i_write_buttom(Stream *stm, const FButton *button)
{
    cassert_no_null(button);
    str_write(stm, button->text);
    str_write(stm, button->tooltip);
    stm_write_r32(stm, button->min_width);
    stm_write_r32(stm, button->hpadding);
    stm_write_r32(stm, button->vpadding);
}

/*---------------------------------------------------------------------------*/

static void i_write_check(Stream *stm, const FCheck *check)
{
    cassert_no_null(check);
    str_write(stm, check->text);
}

/*---------------------------------------------------------------------------*/

static void i_write_radio(Stream *stm, const FRadio *radio)
{
    cassert_no_null(radio);
    str_write(stm, radio->text);
}

/*---------------------------------------------------------------------------*/

static void i_write_tool(Stream *stm, const FTool *tool)
{
    cassert_no_null(tool);
    str_write(stm, tool->path);
    str_write(stm, tool->tooltip);
    stm_write_r32(stm, tool->hpadding);
    stm_write_r32(stm, tool->vpadding);
}

/*---------------------------------------------------------------------------*/

static void i_write_elem(Stream *stm, const FElem *elem)
{
    str_write(stm, elem->text);
    str_write(stm, elem->iconpath);
}

/*---------------------------------------------------------------------------*/

static void i_write_popup(Stream *stm, const FPopUp *popup)
{
    cassert_no_null(popup);
    arrst_write(stm, popup->elems, i_write_elem, FElem);
}

/*---------------------------------------------------------------------------*/

static void i_write_edit(Stream *stm, const FEdit *edit)
{
    cassert_no_null(edit);
    stm_write_bool(stm, edit->passmode);
    stm_write_bool(stm, edit->autosel);
    stm_write_enum(stm, edit->text_align, halign_t);
    stm_write_r32(stm, edit->min_width);
}

/*---------------------------------------------------------------------------*/

static void i_write_combo(Stream *stm, const FCombo *combo)
{
    cassert_no_null(combo);
    stm_write_bool(stm, combo->passmode);
    stm_write_bool(stm, combo->autosel);
    stm_write_enum(stm, combo->text_align, halign_t);
    stm_write_r32(stm, combo->min_width);
}

/*---------------------------------------------------------------------------*/

static void i_write_listbox(Stream *stm, const FListBox *listbox)
{
    cassert_no_null(listbox);
    stm_write_r32(stm, listbox->min_width);
    stm_write_r32(stm, listbox->min_height);
    arrst_write(stm, listbox->elems, i_write_elem, FElem);
}

/*---------------------------------------------------------------------------*/

static void i_write_slider(Stream *stm, const FSlider *slider)
{
    cassert_no_null(slider);
    stm_write_r32(stm, slider->min_width);
}

/*---------------------------------------------------------------------------*/

static void i_write_vslider(Stream *stm, const FVSlider *slider)
{
    cassert_no_null(slider);
    stm_write_r32(stm, slider->min_height);
}

/*---------------------------------------------------------------------------*/

static void i_write_progress(Stream *stm, const FProgress *progress)
{
    cassert_no_null(progress);
    stm_write_r32(stm, progress->min_width);
}

/*---------------------------------------------------------------------------*/

static void i_write_view(Stream *stm, const FView *view)
{
    cassert_no_null(view);
    stm_write_r32(stm, view->min_width);
    stm_write_r32(stm, view->min_height);
}

/*---------------------------------------------------------------------------*/

static void i_write_sview(Stream *stm, const FSView *sview)
{
    cassert_no_null(sview);
    stm_write_r32(stm, sview->min_width);
    stm_write_r32(stm, sview->min_height);
}

/*---------------------------------------------------------------------------*/

static void i_write_text(Stream *stm, const FText *text)
{
    cassert_no_null(text);
    stm_write_bool(stm, text->read_only);
    stm_write_r32(stm, text->min_width);
    stm_write_r32(stm, text->min_height);
}

/*---------------------------------------------------------------------------*/

static void i_write_image(Stream *stm, const FImage *image)
{
    cassert_no_null(image);
    str_write(stm, image->path);
    stm_write_enum(stm, image->scale, scale_t);
    stm_write_r32(stm, image->min_width);
    stm_write_r32(stm, image->min_height);
}

/*---------------------------------------------------------------------------*/

static void i_write_header(Stream *stm, const FHeader *header)
{
    cassert_no_null(header);
    str_write(stm, header->title);
    stm_write_enum(stm, header->align, halign_t);
    stm_write_enum(stm, header->dalign, halign_t);
    stm_write_bool(stm, header->resizable);
    stm_write_r32(stm, header->width);
    stm_write_r32(stm, header->min_width);
    stm_write_r32(stm, header->max_width);
}

/*---------------------------------------------------------------------------*/

static void i_write_table(Stream *stm, const FTable *table)
{
    cassert_no_null(table);
    stm_write_r32(stm, table->min_width);
    stm_write_r32(stm, table->min_height);
    arrst_write(stm, table->headers, i_write_header, FHeader);
}

/*---------------------------------------------------------------------------*/

static void i_write_hline(Stream *stm, const FHline *hline)
{
    cassert_no_null(hline);
    stm_write_r32(stm, hline->length);
}

/*---------------------------------------------------------------------------*/

static void i_write_vline(Stream *stm, const FVline *vline)
{
    cassert_no_null(vline);
    stm_write_r32(stm, vline->length);
}

/*---------------------------------------------------------------------------*/

static void i_write_cell(Stream *stm, const FCell *cell)
{
    cassert_no_null(cell);
    str_write(stm, cell->name);
    stm_write_bool(stm, cell->tabstop);
    stm_write_enum(stm, cell->type, celltype_t);
    stm_write_enum(stm, cell->halign, halign_t);
    stm_write_enum(stm, cell->valign, valign_t);
    stm_write_r32(stm, cell->padding_left);
    stm_write_r32(stm, cell->padding_top);
    stm_write_r32(stm, cell->padding_right);
    stm_write_r32(stm, cell->padding_bottom);

    switch (cell->type)
    {
    case ekCELL_TYPE_EMPTY:
        break;
    case ekCELL_TYPE_LABEL:
        i_write_label(stm, cell->widget.label);
        break;
    case ekCELL_TYPE_BUTTON:
        i_write_buttom(stm, cell->widget.button);
        break;
    case ekCELL_TYPE_CHECK:
        i_write_check(stm, cell->widget.check);
        break;
    case ekCELL_TYPE_RADIO:
        i_write_radio(stm, cell->widget.radio);
        break;
    case ekCELL_TYPE_TOOL:
        i_write_tool(stm, cell->widget.tool);
        break;
    case ekCELL_TYPE_POPUP:
        i_write_popup(stm, cell->widget.popup);
        break;
    case ekCELL_TYPE_EDIT:
        i_write_edit(stm, cell->widget.edit);
        break;
    case ekCELL_TYPE_COMBO:
        i_write_combo(stm, cell->widget.combo);
        break;
    case ekCELL_TYPE_LISTBOX:
        i_write_listbox(stm, cell->widget.listbox);
        break;
    case ekCELL_TYPE_SLIDER:
        i_write_slider(stm, cell->widget.slider);
        break;
    case ekCELL_TYPE_VSLIDER:
        i_write_vslider(stm, cell->widget.vslider);
        break;
    case ekCELL_TYPE_PROGRESS:
        i_write_progress(stm, cell->widget.progress);
        break;
     case ekCELL_TYPE_VIEW:
        i_write_view(stm, cell->widget.view);
        break;
     case ekCELL_TYPE_SCROLL_VIEW:
        i_write_sview(stm, cell->widget.sview);
        break;
    case ekCELL_TYPE_TEXT:
        i_write_text(stm, cell->widget.text);
        break;
    case ekCELL_TYPE_IMAGE:
        i_write_image(stm, cell->widget.image);
        break;
    case ekCELL_TYPE_TABLEVIEW:
        i_write_table(stm, cell->widget.table);
        break;
    case ekCELL_TYPE_HLINE:
        i_write_hline(stm, cell->widget.hline);
        break;
    case ekCELL_TYPE_VLINE:
        i_write_vline(stm, cell->widget.vline);
        break;
    case ekCELL_TYPE_LAYOUT:
        i_write_layout(stm, cell->widget.layout);
        break;
    default:
        cassert_default(cell->type);
    }
}

/*---------------------------------------------------------------------------*/

static void i_write_layout(Stream *stm, const FLayout *layout)
{
    cassert_no_null(layout);
    str_write(stm, layout->name);
    stm_write_bool(stm, layout->row_tabstop);
    stm_write_r32(stm, layout->margin_left);
    stm_write_r32(stm, layout->margin_top);
    stm_write_r32(stm, layout->margin_right);
    stm_write_r32(stm, layout->margin_bottom);
    stm_write_bool(stm, layout->with_border);
    stm_write_bool(stm, layout->with_background);
    stm_write_bool(stm, layout->with_group);
    stm_write_u32(stm, layout->border_light);
    stm_write_u32(stm, layout->border_dark);
    stm_write_u32(stm, layout->backgd_light);
    stm_write_u32(stm, layout->backgd_dark);
    str_write(stm, layout->group_title);
    arrst_write(stm, layout->cols, i_write_col, FColumn);
    arrst_write(stm, layout->rows, i_write_row, FRow);
    arrst_write(stm, layout->cells, i_write_cell, FCell);
}

/*---------------------------------------------------------------------------*/

void flayout_write(Stream *stm, const FLayout *layout)
{
    stm_write_u16(stm, i_VERSION);
    i_write_layout(stm, layout);
}

/*---------------------------------------------------------------------------*/

void flayout_insert_col(FLayout *layout, const uint32_t col)
{
    uint32_t ncols = 0, nrows = 0, i = 0;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, FColumn);
    nrows = arrst_size(layout->rows, FRow);

    /* Insert and init (empty) the new cells */
    for (i = 0; i < nrows; ++i)
    {
        uint32_t inspos = ((ncols + 1) * i) + col;
        FCell *cell = arrst_insert_n(layout->cells, inspos, 1, FCell);
        dbind_init(cell, FCell);
    }

    /* Add a new column */
    {
        FColumn *ncol = arrst_insert_n(layout->cols, col, 1, FColumn);
        dbind_init(ncol, FColumn);
    }
}

/*---------------------------------------------------------------------------*/

void flayout_remove_col(FLayout *layout, const uint32_t col)
{
    uint32_t ncols = 0, nrows = 0, i = 0;
    cassert_no_null(layout);
    cassert(arrst_size(layout->cols, FColumn) >= 1);
    cassert(col < arrst_size(layout->cols, FColumn));
    ncols = arrst_size(layout->cols, FColumn);
    nrows = arrst_size(layout->rows, FRow);

    /* Destroy the column cells */
    for (i = 0; i < nrows; ++i)
    {
        uint32_t delrow = nrows - i - 1;
        uint32_t delpos = (ncols * delrow) + col;
        arrst_delete(layout->cells, delpos, i_remove_cell, FCell);
    }

    /* Destroy the column */
    arrst_delete(layout->cols, col, i_remove_column, FColumn);
}

/*---------------------------------------------------------------------------*/

void flayout_insert_row(FLayout *layout, const uint32_t row)
{
    uint32_t ncols = 0, i = 0;
    uint32_t inspos = 0;
    FCell *cells = NULL;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, FColumn);
    /* Cells insert position */
    inspos = row * ncols;
    /* Cells array is in row-major order. All row cells are together in memory */
    cells = arrst_insert_n(layout->cells, inspos, ncols, FCell);

    /* Initialize the new cells (empty) */
    for (i = 0; i < ncols; ++i)
        dbind_init(cells + i, FCell);

    /* Add a new row */
    {
        FRow *nrow = arrst_insert_n(layout->rows, row, 1, FRow);
        dbind_init(nrow, FRow);
    }
}

/*---------------------------------------------------------------------------*/

void flayout_remove_row(FLayout *layout, const uint32_t row)
{
    uint32_t i, ncols = 0;
    cassert_no_null(layout);
    cassert(arrst_size(layout->rows, FRow) >= 1);
    cassert(row < arrst_size(layout->rows, FRow));
    ncols = arrst_size(layout->cols, FColumn);

    /* Destroy the row cells */
    for (i = 0; i < ncols; ++i)
    {
        uint32_t delcol = ncols - i - 1;
        uint32_t delpos = (ncols * row) + delcol;
        arrst_delete(layout->cells, delpos, i_remove_cell, FCell);
    }

    /* Destroy the row */
    arrst_delete(layout->rows, row, i_remove_row, FRow);
}

/*---------------------------------------------------------------------------*/

static ___INLINE FCell *i_cell(FLayout *layout, const uint32_t col, const uint32_t row)
{
    uint32_t ncols = UINT32_MAX;
    uint32_t pos = UINT32_MAX;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, FColumn);
    pos = row * ncols + col;
    return arrst_get(layout->cells, pos, FCell);
}

/*---------------------------------------------------------------------------*/

void flayout_remove_cell(FLayout *layout, const uint32_t col, const uint32_t row)
{
    String *name = NULL;
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    name = str_c(tc(cell->name));
    i_remove_cell(cell);
    cell->type = ekCELL_TYPE_EMPTY;
    str_upd(&cell->name, tc(name));
    str_destroy(&name);
}

/*---------------------------------------------------------------------------*/

void flayout_add_label(FLayout *layout, FLabel *label, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(label);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_LABEL;
    cell->halign = ekHALIGN_LEFT;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.label = label;
}

/*---------------------------------------------------------------------------*/

void flayout_add_button(FLayout *layout, FButton *button, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(button);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_BUTTON;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.button = button;
}

/*---------------------------------------------------------------------------*/

void flayout_add_check(FLayout *layout, FCheck *check, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(check);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_CHECK;
    cell->halign = ekHALIGN_LEFT;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.check = check;
}

/*---------------------------------------------------------------------------*/

void flayout_add_radio(FLayout *layout, FRadio *radio, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(radio);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_RADIO;
    cell->halign = ekHALIGN_LEFT;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.radio = radio;
}

/*---------------------------------------------------------------------------*/

void flayout_add_tool(FLayout *layout, FTool *tool, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(tool);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_TOOL;
    cell->halign = ekHALIGN_CENTER;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.tool = tool;
}

/*---------------------------------------------------------------------------*/

void flayout_add_popup(FLayout *layout, FPopUp *popup, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(popup);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_POPUP;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.popup = popup;
}

/*---------------------------------------------------------------------------*/

void flayout_add_edit(FLayout *layout, FEdit *edit, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(edit);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_EDIT;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.edit = edit;
}

/*---------------------------------------------------------------------------*/

void flayout_add_combo(FLayout *layout, FCombo *combo, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(combo);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_COMBO;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.combo = combo;
}

/*---------------------------------------------------------------------------*/

void flayout_add_listbox(FLayout *layout, FListBox *listbox, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(listbox);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_LISTBOX;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_JUSTIFY;
    cell->widget.listbox = listbox;
}

/*---------------------------------------------------------------------------*/

void flayout_add_slider(FLayout *layout, FSlider *slider, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(slider);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_SLIDER;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.slider = slider;
}

/*---------------------------------------------------------------------------*/

void flayout_add_vslider(FLayout *layout, FVSlider *slider, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(slider);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_VSLIDER;
    cell->halign = ekHALIGN_CENTER;
    cell->valign = ekVALIGN_JUSTIFY;
    cell->widget.vslider = slider;
}

/*---------------------------------------------------------------------------*/

void flayout_add_progress(FLayout *layout, FProgress *progress, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(progress);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_PROGRESS;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.progress = progress;
}

/*---------------------------------------------------------------------------*/

void flayout_add_view(FLayout *layout, FView *view, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(view);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_VIEW;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_JUSTIFY;
    cell->widget.view = view;
}

/*---------------------------------------------------------------------------*/

void flayout_add_sview(FLayout *layout, FSView *sview, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(sview);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_SCROLL_VIEW;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_JUSTIFY;
    cell->widget.sview = sview;
}

/*---------------------------------------------------------------------------*/

void flayout_add_text(FLayout *layout, FText *text, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(text);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_TEXT;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_JUSTIFY;
    cell->widget.text = text;
}

/*---------------------------------------------------------------------------*/

void flayout_add_image(FLayout *layout, FImage *image, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(image);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_IMAGE;
    cell->halign = ekHALIGN_CENTER;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.image = image;
}

/*---------------------------------------------------------------------------*/

void flayout_add_table(FLayout *layout, FTable *table, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(table);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_TABLEVIEW;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_JUSTIFY;
    cell->widget.table = table;
}

/*---------------------------------------------------------------------------*/

void flayout_add_hline(FLayout *layout, FHline *hline, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(hline);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_HLINE;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_CENTER;
    cell->widget.hline = hline;
}

/*---------------------------------------------------------------------------*/

void flayout_add_vline(FLayout *layout, FVline *vline, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(vline);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_VLINE;
    cell->halign = ekHALIGN_CENTER;
    cell->valign = ekVALIGN_JUSTIFY;
    cell->widget.vline = vline;
}

/*---------------------------------------------------------------------------*/

void flayout_add_layout(FLayout *layout, FLayout *sublayout, const uint32_t col, const uint32_t row)
{
    FCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(sublayout);
    cassert(cell->type == ekCELL_TYPE_EMPTY);
    cell->type = ekCELL_TYPE_LAYOUT;
    cell->halign = ekHALIGN_JUSTIFY;
    cell->valign = ekVALIGN_JUSTIFY;
    cell->widget.layout = sublayout;
}

/*---------------------------------------------------------------------------*/

uint32_t flayout_ncols(const FLayout *layout)
{
    cassert_no_null(layout);
    return arrst_size(layout->cols, FColumn);
}

/*---------------------------------------------------------------------------*/

uint32_t flayout_nrows(const FLayout *layout)
{
    cassert_no_null(layout);
    return arrst_size(layout->rows, FRow);
}

/*---------------------------------------------------------------------------*/

FColumn *flayout_column(FLayout *layout, const uint32_t col)
{
    cassert_no_null(layout);
    return arrst_get(layout->cols, col, FColumn);
}

/*---------------------------------------------------------------------------*/

const FColumn *flayout_ccolumn(const FLayout *layout, const uint32_t col)
{
    cassert_no_null(layout);
    return arrst_get_const(layout->cols, col, FColumn);
}

/*---------------------------------------------------------------------------*/

FRow *flayout_row(FLayout *layout, const uint32_t row)
{
    cassert_no_null(layout);
    return arrst_get(layout->rows, row, FRow);
}

/*---------------------------------------------------------------------------*/

const FRow *flayout_crow(const FLayout *layout, const uint32_t row)
{
    cassert_no_null(layout);
    return arrst_get_const(layout->rows, row, FRow);
}

/*---------------------------------------------------------------------------*/

FCell *flayout_cell(FLayout *layout, const uint32_t col, const uint32_t row)
{
    return i_cell(layout, col, row);
}

/*---------------------------------------------------------------------------*/

const FCell *flayout_ccell(const FLayout *layout, const uint32_t col, const uint32_t row)
{
    return i_cell(cast(layout, FLayout), col, row);
}

/*---------------------------------------------------------------------------*/

void flayout_synchro(const FLayout *layout, Layout *glayout)
{
    cassert_no_null(layout);
    layout_margin4(glayout, layout->margin_top, layout->margin_right, layout->margin_bottom, layout->margin_left);
    layout_taborder(glayout, layout->row_tabstop ? ekGUI_HORIZONTAL : ekGUI_VERTICAL);
    
    if (layout->with_border == TRUE)
        layout_skcolor(glayout, gui_alt_color(layout->border_light, layout->border_dark));
    else
        layout_skcolor(glayout, 0);

    if (layout->with_background == TRUE)
        layout_bgcolor(glayout, gui_alt_color(layout->backgd_light, layout->backgd_dark));
    else
        layout_bgcolor(glayout, 0);

    /* TODO: NAppGUI support for groups */
    /* layout->group_title, layout->with_group */
}

/*---------------------------------------------------------------------------*/

void flayout_col_synchro(const FLayout *layout, Layout *glayout, const uint32_t col)
{
    const FColumn *fcol = flayout_ccolumn(layout, col);
    uint32_t ncols = layout_ncols(glayout);
    cassert_no_null(fcol);
    cassert(ncols == flayout_ncols(layout));
    if (col < ncols - 1)
        layout_hmargin(glayout, col, fcol->margin_right);
    layout_hsize(glayout, col, fcol->forced_width);
}

/*---------------------------------------------------------------------------*/

void flayout_row_synchro(const FLayout *layout, Layout *glayout, const uint32_t row)
{
    const FRow *frow = flayout_crow(layout, row);
    uint32_t nrows = layout_nrows(glayout);
    cassert_no_null(frow);
    cassert(nrows == flayout_nrows(layout));
    if (row < nrows - 1)
        layout_vmargin(glayout, row, frow->margin_bottom);
    layout_vsize(glayout, row, frow->forced_height);
}

/*---------------------------------------------------------------------------*/

void flayout_cols_expand(const FLayout *layout, Layout *glayout)
{
    uint32_t ncols = 0, i, tr = 0;
    uint32_t index[256];
    real32_t exp[256];
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, FColumn);
    cassert(ncols == layout_ncols(glayout));
    cassert(ncols < 256);

    /* Column expansion */
    arrst_foreach_const(col, layout->cols, FColumn)
        index[col_i] = col_i;
        if (col->expand == TRUE)
        {
            exp[col_i] = 1;
            tr += 1;
        }
        else
        {
            exp[col_i] = 0;
        }
    arrst_end()

    for (i = 0; i < ncols; ++i)
    {
        if (tr == 0)
            exp[i] = 1 / (real32_t)ncols;
        else if (exp[i] > 0)
            exp[i] = 1 / (real32_t)tr;
    }

    layout_hexpandn(glayout, ncols, index, exp);
}

/*---------------------------------------------------------------------------*/

void flayout_rows_expand(const FLayout *layout, Layout *glayout)
{
    uint32_t nrows = 0, i, tr = 0;
    uint32_t index[256];
    real32_t exp[256];
    cassert_no_null(layout);
    nrows = arrst_size(layout->rows, FRow);
    cassert(nrows == layout_nrows(glayout));
    cassert(nrows < 256);

    arrst_foreach_const(row, layout->rows, FRow)
        index[row_i] = row_i;
        if (row->expand == TRUE)
        {
            exp[row_i] = 1;
            tr += 1;
        }
        else
        {
            exp[row_i] = 0;
        }
    arrst_end()

    for (i = 0; i < nrows; ++i)
    {
        if (tr == 0)
            exp[i] = 1 / (real32_t)nrows;
        else if (exp[i] > 0)
            exp[i] = 1 / (real32_t)tr;
    }

    layout_vexpandn(glayout, nrows, index, exp);
}

/*---------------------------------------------------------------------------*/

void flayout_cell_synchro(const FLayout *layout, Layout *glayout, const uint32_t col, const uint32_t row)
{
    const FCell *fcell = NULL;
    Cell *cell = NULL;
    align_t halign, valign;
    cassert_no_null(layout);
    fcell = flayout_ccell(layout, col, row);
    cell = layout_cell(glayout, col, row);
    halign = _nflib_halign(fcell->halign);
    valign = _nflib_valign(fcell->valign);
    layout_halign(glayout, col, row, halign);
    layout_valign(glayout, col, row, valign);
    layout_tabstop(glayout, col, row, fcell->tabstop);
    cell_padding4(cell, fcell->padding_top, fcell->padding_left, fcell->padding_bottom, fcell->padding_left);
}

/*---------------------------------------------------------------------------*/

Layout *flayout_to_gui(const FLayout *layout, const char_t *resource_path, const real32_t empty_width, const real32_t empty_height)
{
    uint32_t ncols = 0, nrows = 0;
    Layout *glayout = NULL;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, FColumn);
    nrows = arrst_size(layout->rows, FRow);
    glayout = layout_create(ncols, nrows);

    flayout_synchro(layout, glayout);

    /* Column properties */
    arrst_foreach_const(col, layout->cols, FColumn)
        flayout_col_synchro(layout, glayout, col_i);
        if (col_i == col_total - 1)
            cast(col, FColumn)->margin_right = 0;
    arrst_end()

    /* Row properties */
    arrst_foreach_const(row, layout->rows, FRow)
        flayout_row_synchro(layout, glayout, row_i);
        if (row_i == row_total - 1)
            cast(row, FRow)->margin_bottom = 0;
    arrst_end()

    /* Cell expansion */
    flayout_cols_expand(layout, glayout);
    flayout_rows_expand(layout, glayout);

    /* Cells */
    {
        uint32_t i, j;
        const FCell *cells = arrst_all_const(layout->cells, FCell);
        for (j = 0; j < nrows; ++j)
        {
            for (i = 0; i < ncols; ++i)
            {
                flayout_cell_synchro(layout, glayout, i, j);

                switch (cells->type)
                {
                case ekCELL_TYPE_EMPTY:
                {
                    Cell *gcell = layout_cell(glayout, i, j);
                    cell_force_size(gcell, empty_width, empty_height);
                    break;
                }

                case ekCELL_TYPE_LABEL:
                {
                    Label *label = label_create();
                    flabel_synchro(cells->widget.label, label);
                    layout_label(glayout, label, i, j);
                    break;
                }

                case ekCELL_TYPE_BUTTON:
                {
                    Button *button = button_push();
                    fbutton_synchro(cells->widget.button, button);
                    layout_button(glayout, button, i, j);
                    break;
                }

                case ekCELL_TYPE_CHECK:
                {
                    Button *button = button_check();
                    fcheck_synchro(cells->widget.check, button);
                    layout_button(glayout, button, i, j);
                    break;
                }

                case ekCELL_TYPE_RADIO:
                {
                    Button *button = button_radio();
                    fradio_synchro(cells->widget.radio, button);
                    layout_button(glayout, button, i, j);
                    break;
                }

                case ekCELL_TYPE_TOOL:
                {
                    Button *button = button_flat();
                    ftool_synchro(cells->widget.tool, button, resource_path);
                    layout_button(glayout, button, i, j);
                    break;
                }

                case ekCELL_TYPE_POPUP:
                {
                    PopUp *popup = popup_create();
                    fpopup_synchro(cells->widget.popup, popup, resource_path);
                    layout_popup(glayout, popup, i, j);
                    break;
                }

                case ekCELL_TYPE_EDIT:
                {
                    Edit *edit = edit_create();
                    fedit_synchro(cells->widget.edit, edit);
                    layout_edit(glayout, edit, i, j);
                    break;
                }

                case ekCELL_TYPE_COMBO:
                {
                    Combo *combo = combo_create();
                    fcombo_synchro(cells->widget.combo, combo);
                    layout_combo(glayout, combo, i, j);
                    break;
                }

                case ekCELL_TYPE_LISTBOX:
                {
                    ListBox *listbox = listbox_create();
                    flistbox_synchro(cells->widget.listbox, listbox, resource_path);
                    layout_listbox(glayout, listbox, i, j);
                    break;
                }

                case ekCELL_TYPE_SLIDER:
                {
                    Slider *slider = slider_create();
                    fslider_synchro(cells->widget.slider, slider);
                    layout_slider(glayout, slider, i, j);
                    break;
                }

                case ekCELL_TYPE_VSLIDER:
                {
                    Slider *slider = slider_vertical();
                    fvslider_synchro(cells->widget.vslider, slider);
                    layout_slider(glayout, slider, i, j);
                    break;
                }

                case ekCELL_TYPE_PROGRESS:
                {
                    Progress *progress = progress_create();
                    fprogress_synchro(cells->widget.progress, progress);
                    layout_progress(glayout, progress, i, j);
                    break;
                }

                case ekCELL_TYPE_VIEW:
                {
                    View *view = view_create();
                    fview_synchro(cells->widget.view, view);
                    layout_view(glayout, view, i, j);
                    break;
                }

                case ekCELL_TYPE_SCROLL_VIEW:
                {
                    View *view = view_scroll();
                    fsview_synchro(cells->widget.sview, view);
                    layout_view(glayout, view, i, j);
                    break;
                }

                case ekCELL_TYPE_TEXT:
                {
                    TextView *view = textview_create();
                    ftext_synchro(cells->widget.text, view);
                    layout_textview(glayout, view, i, j);
                    break;
                }

                case ekCELL_TYPE_IMAGE:
                {
                    ImageView *view = imageview_create();
                    fimage_synchro(cells->widget.image, view, resource_path);
                    layout_imageview(glayout, view, i, j);
                    break;
                }

                case ekCELL_TYPE_TABLEVIEW:
                {
                    TableView *view = tableview_create();
                    ftable_synchro(cells->widget.table, view);
                    layout_tableview(glayout, view, i, j);
                    break;
                }

                case ekCELL_TYPE_HLINE:
                {
                    Line *line = line_horizontal();
                    fhline_synchro(cells->widget.hline, line);
                    layout_line(glayout, line, i, j);
                    break;
                }

                case ekCELL_TYPE_VLINE:
                {
                    Line *line = line_vertical();
                    fvline_synchro(cells->widget.vline, line);
                    layout_line(glayout, line, i, j);
                    break;
                }

                case ekCELL_TYPE_LAYOUT:
                {
                    Layout *gsublayout = flayout_to_gui(cells->widget.layout, resource_path, empty_width, empty_height);
                    layout_layout(glayout, gsublayout, i, j);
                    break;
                }

                default:
                    cassert_default(cells->type);
                }

                cells += 1;
            }
        }
    }

    return glayout;
}

/*---------------------------------------------------------------------------*/

GuiControl *flayout_search_gui_control(const FLayout *layout, Layout *gui_layout, const char_t *cell_name)
{
    const FCell *cells = NULL;
    uint32_t i, j, ncols = 0, nrows = 0;
    cassert_no_null(layout);
    cells = arrst_all_const(layout->cells, FCell);
    ncols = arrst_size(layout->cols, FColumn);
    nrows = arrst_size(layout->rows, FRow);

    for (j = 0; j < nrows; ++j)
    {
        for (i = 0; i < ncols; ++i)
        {
            if (str_equ(cells->name, cell_name) == TRUE)
            {
                switch (cells->type)
                {
                case ekCELL_TYPE_LABEL:
                case ekCELL_TYPE_BUTTON:
                case ekCELL_TYPE_CHECK:
                case ekCELL_TYPE_RADIO:
                case ekCELL_TYPE_TOOL:
                case ekCELL_TYPE_POPUP:
                case ekCELL_TYPE_EDIT:
                case ekCELL_TYPE_COMBO:
                case ekCELL_TYPE_LISTBOX:
                case ekCELL_TYPE_SLIDER:
                case ekCELL_TYPE_VSLIDER:
                case ekCELL_TYPE_PROGRESS:
                case ekCELL_TYPE_VIEW:
                case ekCELL_TYPE_SCROLL_VIEW:
                case ekCELL_TYPE_TEXT:
                case ekCELL_TYPE_IMAGE:
                case ekCELL_TYPE_TABLEVIEW:
                case ekCELL_TYPE_HLINE:
                case ekCELL_TYPE_VLINE:
                {
                    Cell *gcell = layout_cell(gui_layout, i, j);
                    return cell_control(gcell);
                }

                case ekCELL_TYPE_LAYOUT:
                case ekCELL_TYPE_EMPTY:
                    break;
                default:
                    cassert_default(cells->type);
                }
            }

            if (cells->type == ekCELL_TYPE_LAYOUT)
            {
                Cell *gcell = layout_cell(gui_layout, i, j);
                Layout *gsub_layout = cell_layout(gcell);
                GuiControl *control = flayout_search_gui_control(cells->widget.layout, gsub_layout, cell_name);
                if (control != NULL)
                    return control;
            }

            cells += 1;
        }
    }

    return NULL;
}
