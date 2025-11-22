/* Design form */

#include "dform.h"
#include "designer.h"
#include "dlayout.h"
#include "dialogs.h"
#include "inspect.h"
#include "propedit.h"
#include "res_designer.h"
#include <nflib/nflib.h>
#include <nflib/fbutton.h>
#include <nflib/fcheck.h>
#include <nflib/fcombo.h>
#include <nflib/fedit.h>
#include <nflib/fform.h>
#include <nflib/fimage.h>
#include <nflib/flabel.h>
#include <nflib/flayout.h>
#include <nflib/flistbox.h>
#include <nflib/fpopup.h>
#include <nflib/fprogress.h>
#include <nflib/fradio.h>
#include <nflib/fslider.h>
#include <nflib/fvslider.h>
#include <nflib/ftable.h>
#include <nflib/ftext.h>
#include <nflib/ftool.h>
#include <gui/guicontrol.h>
#include <gui/button.h>
#include <gui/edit.h>
#include <gui/combo.h>
#include <gui/gui.h>
#include <gui/popup.h>
#include <gui/label.h>
#include <gui/listbox.h>
#include <gui/imageview.h>
#include <gui/tableview.h>
#include <gui/textview.h>
#include <gui/layout.h>
#include <gui/layouth.h>
#include <gui/panel.h>
#include <gui/panel.inl>
#include <gui/slider.h>
#include <gui/progress.h>
#include <gui/window.h>
#include <draw2d/image.h>
#include <geom2d/v2d.h>
#include <geom2d/s2d.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <core/heap.h>
#include <core/stream.h>
#include <core/strings.h>
#include <sewer/bstd.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>

typedef struct _undoframe_t UndoFrame;

struct _undoframe_t
{
    FForm *fform;
    V2Df cellpos;
};

struct _dform_t
{
    Designer *app;
    FForm *fform;
    DLayout *dlayout;
    Layout *glayout;
    Window *window;
    V2Df origin;
    DSelect hover;
    DSelect sel;
    ArrSt(UndoFrame) *undo_stack;
    ArrSt(DSelect) *temp_path;
    ArrSt(DSelect) *sel_path;
    uint32_t undo_pos;
    uint32_t layout_id;
    uint32_t cell_id;
    bool_t need_save;
};

/*---------------------------------------------------------------------------*/

DeclSt(UndoFrame);
static real32_t i_EMPTY_CELL_WIDTH = 40;
static real32_t i_EMPTY_CELL_HEIGHT = 20;

/*---------------------------------------------------------------------------*/

static void i_remove_undo_frame(UndoFrame *frame)
{
    dbind_destroy(&frame->fform, FForm);
}
    
/*---------------------------------------------------------------------------*/

static void i_cell_obj_name(DForm *form, FLayout *flayout, const uint32_t col, const uint32_t row)
{
    char_t name[64];
    FCell *fcell = flayout_cell(flayout, col, row);
    cassert_no_null(form);
    bstd_sprintf(name, sizeof(name), "cell%d", form->cell_id);
    str_upd(&fcell->name, name);
    form->cell_id += 1;
}

/*---------------------------------------------------------------------------*/

static void i_layout_obj_names(DForm *form, FLayout *flayout)
{
    cassert_no_null(form);

    {
        char_t name[64];
        bstd_sprintf(name, sizeof(name), "layout%d", form->layout_id);
        str_upd(&flayout->name, name);
        form->layout_id += 1;
    }

    {
        uint32_t i, ncols = flayout_ncols(flayout);
        uint32_t j, nrows = flayout_nrows(flayout);
        for (j = 0; j < nrows; ++j)
            for (i = 0; i < ncols; ++i)
                i_cell_obj_name(form, flayout, i, j);
    }
}

/*---------------------------------------------------------------------------*/

static void i_undo_add_frame(DForm *form)
{
    uint32_t n = 0;
    UndoFrame *frame = NULL;
    cassert_no_null(form);
    n = arrst_size(form->undo_stack, UndoFrame);

    /* Remove all redo operations before current stack position */
    if (form->undo_pos != UINT32_MAX)
    {
        uint32_t i, rn = 0;
        cassert(form->undo_pos < n);
        rn = n - form->undo_pos - 1;
        for (i = 0; i < rn; ++i)
        {
            arrst_delete(form->undo_stack, form->undo_pos, i_remove_undo_frame, UndoFrame);
            n -= 1;
        }
    }

    form->undo_pos = n;
    frame = arrst_new0(form->undo_stack, UndoFrame);
    frame->fform = dbind_copy(form->fform, FForm);
    if (form->sel.dlayout != NULL)
    {
        R2Df rect = dlayout_sel_rect(&form->sel);
        frame->cellpos = rect.pos;
        frame->cellpos.x -= form->origin.x;
        frame->cellpos.y -= form->origin.y;
    }
    else
    {
        frame->cellpos.x = -1;
        frame->cellpos.y = -1;
    }
}
    
/*---------------------------------------------------------------------------*/

static void i_need_save(DForm *form, const bool_t undo)
{
    cassert_no_null(form);
    form->need_save = TRUE;
    designer_need_save(form->app);
    if (undo == TRUE)
    {
        i_undo_add_frame(form);
        designer_undo_controls(form->app, TRUE, FALSE);
    }
}

/*---------------------------------------------------------------------------*/

static DForm *i_dform(Designer *app, FForm **fform)
{
    DForm *form = heap_new0(DForm);
    form->app = app;
    form->fform = ptr_dget(fform, FForm);
    form->undo_stack = arrst_create(UndoFrame);
    form->temp_path = arrst_create(DSelect);
    form->sel_path = arrst_create(DSelect);
    form->undo_pos = UINT32_MAX;
    return form;
}

/*---------------------------------------------------------------------------*/

DForm *dform_empty(Designer *app)
{
    FForm *fform = fform_create();
    DForm *form = i_dform(app, &fform);
    cassert_no_null(form->fform);
    i_layout_obj_names(form, form->fform->layout);
    i_need_save(form, TRUE);
    return form;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_num_cells(const FLayout *flayout)
{
    uint32_t n = 0;
    cassert_no_null(flayout);
    n = arrst_size(flayout->cells, FCell);
    arrst_foreach_const(fcell, flayout->cells, FCell)
        if (fcell->type == ekCELL_TYPE_LAYOUT)
            n += i_num_cells(fcell->widget.layout);
    arrst_end()
    return n;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_num_layouts(const FLayout *flayout)
{
    uint32_t n = 1;
    cassert_no_null(flayout);
    arrst_foreach_const(fcell, flayout->cells, FCell)
        if (fcell->type == ekCELL_TYPE_LAYOUT)
            n += i_num_layouts(fcell->widget.layout);
    arrst_end()
    return n;
}

/*---------------------------------------------------------------------------*/

DForm *dform_read(Stream *stm, Designer *app)
{
    FForm *fform = fform_read(stm);
    if (stm_state(stm) == ekSTOK)
    {
        DForm *form = i_dform(app, &fform);
        cassert_no_null(form);
        cassert_no_null(form->fform->layout);
        form->cell_id = i_num_cells(form->fform->layout);
        form->layout_id = i_num_layouts(form->fform->layout);
        i_undo_add_frame(form);
        return form;
    }
    else
    {
        ptr_destopt(fform_destroy, &fform, FForm);
        return NULL;
    }
}

/*---------------------------------------------------------------------------*/

void dform_destroy(DForm **form)
{
    cassert_no_null(form);
    cassert_no_null(*form);
    fform_destroy(&(*form)->fform);
    arrst_destroy(&(*form)->undo_stack, i_remove_undo_frame, UndoFrame);
    arrst_destroy(&(*form)->temp_path, NULL, DSelect);
    arrst_destroy(&(*form)->sel_path, NULL, DSelect);
    if ((*form)->window != NULL)
    {
        cassert((*form)->glayout != NULL);
        dlayout_destroy(&(*form)->dlayout);
        window_destroy(&(*form)->window);
    }
    else
    {
        cassert((*form)->dlayout == NULL);
        cassert((*form)->glayout == NULL);
    }

    heap_delete(form, DForm);
}

/*---------------------------------------------------------------------------*/

void dform_write(Stream *stm, DForm *form)
{
    cassert_no_null(form);
    fform_write(stm, form->fform);
    form->need_save = FALSE;
}

/*---------------------------------------------------------------------------*/

void dform_compose(DForm *form)
{
    cassert_no_null(form);
    if (form->glayout == NULL)
    {
        Panel *panel = panel_create();
        const char_t *resource_path = designer_folder_path(form->app);
        const DColors *colors = designer_colors(form->app);
        cassert_no_null(form->fform);
        cassert(form->window == NULL);
        cassert(form->dlayout == NULL);
        form->dlayout = dlayout_from_flayout(form->fform->layout, resource_path, colors);
        form->glayout = flayout_to_gui(form->fform->layout, resource_path, i_EMPTY_CELL_WIDTH, i_EMPTY_CELL_HEIGHT);
        panel_layout(panel, form->glayout);
        form->window = window_create(ekWINDOW_STD);
        window_panel(form->window, panel);
    }

    window_update(form->window);
    dlayout_synchro_visual(form->dlayout, form->glayout, form->origin);
}

/*---------------------------------------------------------------------------*/

void dform_description(DForm *form, const char_t *desc)
{
    cassert_no_null(form);
    cassert_no_null(form->fform);
    if (str_equ(form->fform->description, desc) == FALSE)
    {
        str_upd(&form->fform->description, desc);
        form->need_save = TRUE;
    }
}

/*---------------------------------------------------------------------------*/

const char_t *dform_get_description(const DForm *form)
{
    cassert_no_null(form);
    cassert_no_null(form->fform);
    return tc(form->fform->description);
}

/*---------------------------------------------------------------------------*/

void dform_set(DForm *form, Panel *inspect, Panel *propedit)
{
    cassert_no_null(form);
    inspect_set(inspect, form);
    propedit_set(propedit, form, &form->sel);
}

/*---------------------------------------------------------------------------*/

static bool_t i_sel_equ(const DSelect *sel1, const DSelect *sel2)
{
    cassert_no_null(sel1);
    cassert_no_null(sel2);
    if (sel1->dlayout != sel2->dlayout)
        return FALSE;

    if (sel1->dlayout == NULL && sel2->dlayout == NULL)
        return TRUE;

    if (sel1->elem == sel2->elem
        && sel1->col == sel2->col
        && sel1->row == sel2->row)
        return TRUE;
    else
        return FALSE;
}

/*---------------------------------------------------------------------------*/

static void i_elem_at_mouse(DLayout *dlayout, FLayout *flayout, Layout *glayout, const real32_t mouse_x, const real32_t mouse_y, ArrSt(DSelect) *selpath, DSelect *sel)
{
    cassert_no_null(sel);
    arrst_clear(selpath, NULL, DSelect);
    dlayout_elem_at_pos(dlayout, flayout, glayout, mouse_x, mouse_y, selpath);
    if (arrst_size(selpath, DSelect) > 0)
    {
        *sel = *arrst_last(selpath, DSelect);
    }
    else
    {
        sel->dlayout = NULL;
        sel->flayout = NULL;
        sel->glayout = NULL;
        sel->elem = ENUM_MAX(layelem_t);
        sel->col = UINT32_MAX;
        sel->row = UINT32_MAX;
    }
}

/*---------------------------------------------------------------------------*/

bool_t dform_need_save(const DForm *form)
{
    cassert_no_null(form);
    return form->need_save;
}

/*---------------------------------------------------------------------------*/

bool_t dform_can_undo(const DForm *form)
{
    cassert_no_null(form);
    return (form->undo_pos > 0);
}

/*---------------------------------------------------------------------------*/

bool_t dform_can_redo(const DForm *form)
{
    uint32_t n = 0;
    cassert_no_null(form);
    n = arrst_size(form->undo_stack, UndoFrame);
    if (n > 0 && form->undo_pos < n - 1)
        return TRUE;
    else
        return FALSE;
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnMove(DForm *form, const real32_t mouse_x, const real32_t mouse_y)
{
    DSelect hover;
    bool_t equ = TRUE;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    i_elem_at_mouse(form->dlayout, form->fform->layout, form->glayout, mouse_x, mouse_y, form->temp_path, &hover);
    equ = i_sel_equ(&form->hover, &hover);
    form->hover = hover;
    return !equ;
}

/*---------------------------------------------------------------------------*/

static align_t i_halign(const halign_t halign)
{
    switch(halign) {
    case ekHALIGN_LEFT:
        return ekLEFT;
    case ekHALIGN_CENTER:
        return ekCENTER;
    case ekHALIGN_RIGHT:
        return ekRIGHT;
    case ekHALIGN_JUSTIFY:
        return ekJUSTIFY;
    default:
        cassert_default(halign);
    }
    return ekLEFT;
}

/*---------------------------------------------------------------------------*/

static align_t i_valign(const valign_t valign)
{
    switch(valign) {
    case ekVALIGN_TOP:
        return ekTOP;
    case ekVALIGN_CENTER:
        return ekCENTER;
    case ekVALIGN_BOTTOM:
        return ekBOTTOM;
    case ekVALIGN_JUSTIFY:
        return ekJUSTIFY;
    default:
        cassert_default(valign);
    }
    return ekTOP;
}

/*---------------------------------------------------------------------------*/

static void i_sel_remove_cell(const DSelect *sel)
{
    cassert_no_null(sel);
    flayout_remove_cell(sel->flayout, sel->col, sel->row);
    dlayout_remove_cell(sel->dlayout, sel->col, sel->row);
    layout_remove_cell(sel->glayout, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_copy_cell_props(const FCell *fcell, const DSelect *sel)
{
    FCell *sfcell = NULL;
    cassert_no_null(fcell);
    cassert_no_null(sel);
    sfcell = flayout_cell(sel->flayout, sel->col, sel->row);
    cassert(sfcell->type == fcell->type);
    str_upd(&sfcell->name, tc(fcell->name));
    sfcell->halign = fcell->halign;
    sfcell->valign = fcell->valign;
}

/*---------------------------------------------------------------------------*/

static void i_sel_synchro_cell(const DSelect *sel)
{
    const FCell *fcell = NULL;
    align_t halign = ENUM_MAX(align_t);
    align_t valign = ENUM_MAX(align_t);
    cassert_no_null(sel);
    fcell = flayout_ccell(sel->flayout, sel->col, sel->row);
    halign = i_halign(fcell->halign);
    valign = i_valign(fcell->valign);
    layout_halign(sel->glayout, sel->col, sel->row, halign);
    layout_valign(sel->glayout, sel->col, sel->row, valign);
}

/*---------------------------------------------------------------------------*/

static bool_t i_sel_empty_cell(const DSelect *sel)
{
    cassert_no_null(sel);
    if (sel->flayout != NULL)
    {
        cassert_no_null(sel->glayout);
        cassert_no_null(sel->dlayout);
        if (sel->elem == ekLAYELEM_CELL)
        {
            const FCell *cell = flayout_ccell(sel->flayout, sel->col, sel->row);
            cassert_no_null(cell);
            if (cell->type == ekCELL_TYPE_EMPTY)
                return TRUE;
        }
    }
    else
    {
        cassert(sel->glayout == NULL);
        cassert(sel->dlayout == NULL);
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

static void i_after_new_widget(DForm *form, Panel *inspect, Panel *propedit, DSelect *sel)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    i_sel_synchro_cell(sel);
    dform_compose(form);
    propedit_set(propedit, form, sel);
    inspect_set(inspect, form);
    form->sel = *sel;
    i_need_save(form, TRUE);
}

/*---------------------------------------------------------------------------*/

static void i_new_label(FLabel *flabel, const DSelect *sel)
{
    Label *label = label_create();
    cassert_no_null(sel);
    flabel_synchro(flabel, label);
    flayout_add_label(sel->flayout, flabel, sel->col, sel->row);
    layout_label(sel->glayout, label, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_button(FButton *fbutton, const DSelect *sel)
{
    Button *button = button_push();
    cassert_no_null(sel);
    fbutton_synchro(fbutton, button);
    flayout_add_button(sel->flayout, fbutton, sel->col, sel->row);
    layout_button(sel->glayout, button, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_check(FCheck *fcheck, const DSelect *sel)
{
    Button *button = button_check();
    cassert_no_null(sel);
    fcheck_synchro(fcheck, button);
    flayout_add_check(sel->flayout, fcheck, sel->col, sel->row);
    layout_button(sel->glayout, button, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_radio(FRadio *fradio, const DSelect *sel)
{
    Button *button = button_radio();
    cassert_no_null(sel);
    fradio_synchro(fradio, button);
    flayout_add_radio(sel->flayout, fradio, sel->col, sel->row);
    layout_button(sel->glayout, button, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_tool(FTool *ftool, const DSelect *sel, const char_t *folder_path, const DColors *colors)
{
    Button *button = button_flat();
    const Image *image = NULL;
    cassert_no_null(sel);
    ftool_synchro(ftool, button, folder_path);
    flayout_add_tool(sel->flayout, ftool, sel->col, sel->row);
    layout_button(sel->glayout, button, sel->col, sel->row);
    image = button_get_image(button);
    dlayout_set_image(sel->dlayout, image, sel->col, sel->row, colors);
}

/*---------------------------------------------------------------------------*/

static void i_new_popup(FPopUp *fpopup, const DSelect *sel, const char_t *folder_path, const DColors *colors)
{
    PopUp *popup = popup_create();
    cassert_no_null(sel);
    fpopup_synchro(fpopup, popup, folder_path);
    dlayout_synchro_elems(sel->dlayout, sel->col, sel->row, fpopup->elems, folder_path, colors);
    flayout_add_popup(sel->flayout, fpopup, sel->col, sel->row);
    layout_popup(sel->glayout, popup, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_edit(FEdit *fedit, const DSelect *sel)
{
    Edit *edit = edit_create();
    cassert_no_null(sel);
    fedit_synchro(fedit, edit);
    flayout_add_edit(sel->flayout, fedit, sel->col, sel->row);
    layout_edit(sel->glayout, edit, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_combo(FCombo *fcombo, const DSelect *sel)
{
    Combo *combo = combo_create();
    cassert_no_null(sel);
    fcombo_synchro(fcombo, combo);
    flayout_add_combo(sel->flayout, fcombo, sel->col, sel->row);
    layout_combo(sel->glayout, combo, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_listbox(FListBox *flistbox, const DSelect *sel, const char_t *folder_path, const DColors *colors)
{
    ListBox *listbox = listbox_create();
    cassert_no_null(sel);
    flistbox_synchro(flistbox, listbox, folder_path);
    dlayout_synchro_elems(sel->dlayout, sel->col, sel->row, flistbox->elems, folder_path, colors);
    flayout_add_listbox(sel->flayout, flistbox, sel->col, sel->row);
    layout_listbox(sel->glayout, listbox, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_slider(FSlider *fslider, const DSelect *sel)
{
    Slider *slider = slider_create();
    cassert_no_null(sel);
    fslider_synchro(fslider, slider);
    flayout_add_slider(sel->flayout, fslider, sel->col, sel->row);
    layout_slider(sel->glayout, slider, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_vslider(FVSlider *fvslider, const DSelect *sel)
{
    Slider *slider = slider_vertical();
    cassert_no_null(sel);
    fvslider_synchro(fvslider, slider);
    flayout_add_vslider(sel->flayout, fvslider, sel->col, sel->row);
    layout_slider(sel->glayout, slider, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_progress(FProgress *fprogress, const DSelect *sel)
{
    Progress *progress = progress_create();
    cassert_no_null(sel);
    fprogress_synchro(fprogress, progress);
    flayout_add_progress(sel->flayout, fprogress, sel->col, sel->row);
    layout_progress(sel->glayout, progress, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_text(FText *ftext, const DSelect *sel)
{
    TextView *text = textview_create();
    cassert_no_null(sel);
    ftext_synchro(ftext, text);
    flayout_add_text(sel->flayout, ftext, sel->col, sel->row);
    layout_textview(sel->glayout, text, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_image(FImage *fimage, const DSelect *sel, const char_t *folder_path, const DColors *colors)
{
    ImageView *view = imageview_create();
    cassert_no_null(sel);
    fimage_synchro(fimage, view, folder_path);
    flayout_add_image(sel->flayout, fimage, sel->col, sel->row);
    layout_imageview(sel->glayout, view, sel->col, sel->row);
    dlayout_set_image(sel->dlayout, imageview_get_image(view), sel->col, sel->row, colors);
}

/*---------------------------------------------------------------------------*/

static void i_new_table(FTable *ftable, const DSelect *sel)
{
    TableView *view = tableview_create();
    cassert_no_null(sel);
    ftable_synchro(ftable, view);
    flayout_add_table(sel->flayout, ftable, sel->col, sel->row);
    layout_tableview(sel->glayout, view, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

static void i_new_sublayout(FLayout *fsublayout, const DSelect *sel, const char_t *folder_path, const DColors *colors)
{
    DLayout *dsublayout = dlayout_from_flayout(fsublayout, folder_path, colors);
    Layout *gsublayout = flayout_to_gui(fsublayout, folder_path, i_EMPTY_CELL_WIDTH, i_EMPTY_CELL_HEIGHT);
    cassert_no_null(sel);
    dlayout_add_layout(sel->dlayout, dsublayout, sel->col, sel->row);
    flayout_add_layout(sel->flayout, fsublayout, sel->col, sel->row);
    layout_layout(sel->glayout, gsublayout, sel->col, sel->row);
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnClick(DForm *form, Window *window, Panel *inspect, Panel *propedit, const Font *font, const widget_t widget, const real32_t mouse_x, const real32_t mouse_y, const gui_mouse_t mbutton)
{
    cassert_no_null(form);
    cassert_no_null(form->fform);
    if (mbutton == ekGUI_MOUSE_LEFT)
    {
        DSelect sel;
        i_elem_at_mouse(form->dlayout, form->fform->layout, form->glayout, mouse_x, mouse_y, form->sel_path, &sel);
        inspect_set(inspect, form);
        if (i_sel_empty_cell(&sel) == TRUE)
        {
            const char_t *folder_path = designer_folder_path(form->app);
            const DColors *colors = designer_colors(form->app);
            cassert_no_null(form->dlayout);

            switch(widget) {
            case ekWIDGET_SELECT:
                break;

            case ekWIDGET_LABEL:
            {
                FLabel *flabel = dialog_new_label(window, font, &sel);
                if (flabel != NULL)
                {
                    i_new_label(flabel, &sel);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_PUSH_BUTTON:
            {
                FButton *fbutton = dialog_new_button(window, font, &sel);
                if (fbutton != NULL)
                {
                    i_new_button(fbutton, &sel);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_CHECK_BUTTON:
            {
                FCheck *fcheck = dialog_new_check(window, font, &sel);
                if (fcheck != NULL)
                {
                    i_new_check(fcheck, &sel);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_RADIO_BUTTON:
            {
                FRadio *fradio = dialog_new_radio(window, font, &sel);
                if (fradio != NULL)
                {
                    i_new_radio(fradio, &sel);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_TOOL_BUTTON:
            {
                FTool *ftool = dialog_new_tool(window, font, &sel, folder_path);
                if (ftool != NULL)
                {
                    i_new_tool(ftool, &sel, folder_path, colors);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_POPUP:
            {
                FPopUp *fpopup = dialog_new_popup(window, font, &sel);
                if (fpopup != NULL)
                {
                    i_new_popup(fpopup, &sel, folder_path, colors);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_EDITBOX:
            {
                FEdit *fedit = dialog_new_edit(window, font, &sel);
                if (fedit != NULL)
                {
                    i_new_edit(fedit, &sel);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_COMBOBOX:
            {
                FCombo *fcombo = dialog_new_combo(window, font, &sel);
                if (fcombo != NULL)
                {
                    i_new_combo(fcombo, &sel);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_LISTBOX:
            {
                FListBox *flistbox = dialog_new_listbox(window, font, &sel);
                if (flistbox != NULL)
                {
                    i_new_listbox(flistbox, &sel, folder_path, colors);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_HORZ_SLIDER:
            {
                FSlider *fslider = dialog_new_slider(window, font, &sel);
                if (fslider != NULL)
                {
                    i_new_slider(fslider, &sel);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_VERT_SLIDER:
            {
                FVSlider *fvslider = dialog_new_vslider(window, font, &sel);
                if (fvslider != NULL)
                {
                    i_new_vslider(fvslider, &sel);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_PROGRESS:
            {
                FProgress *fprogress = dialog_new_progress(window, font, &sel);
                if (fprogress != NULL)
                {
                    i_new_progress(fprogress, &sel);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            case ekWIDGET_TEXTVIEW:
            {
                FText *ftext = dialog_new_text(window, font, &sel);
                if (ftext != NULL)
                {
                    i_new_text(ftext, &sel);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

			case ekWIDGET_IMAGEVIEW:
			{
				FImage *fimage = dialog_new_image(window, font, &sel, folder_path);
                if (fimage != NULL)
                {
                    i_new_image(fimage, &sel, folder_path, colors);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
			}

            case ekWIDGET_TABLEVIEW:
            {
                FTable *ftable = dialog_new_table(window, font, &sel);
                if (ftable != NULL)
                {
                    i_new_table(ftable, &sel);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }
    
            case ekWIDGET_VERT_LAYOUT:
            case ekWIDGET_HORZ_LAYOUT:
            case ekWIDGET_GRID_LAYOUT:
            {
                FLayout *fsublayout = NULL;
                if (widget == ekWIDGET_VERT_LAYOUT)
                    fsublayout = dialog_vertical_layout(window, font, &sel);
                else if (widget == ekWIDGET_HORZ_LAYOUT)
                    fsublayout = dialog_horizontal_layout(window, font, &sel);
                else
                    fsublayout = dialog_grid_layout(window, font, &sel);

                if (fsublayout != NULL)
                {
                    i_layout_obj_names(form, fsublayout);
                    i_new_sublayout(fsublayout, &sel, folder_path, colors);
                    i_after_new_widget(form, inspect, propedit, &sel);
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
            }

            default:
                break;
            }
        }

        /* No new component added, just select */
        {
            bool_t equ = i_sel_equ(&form->sel, &sel);
            propedit_set(propedit, form, &sel);
            form->sel = sel;
            return !equ;
        }
    }
    else
    {
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnExit(DForm *form)
{
    DSelect sel;
    bool_t equ = TRUE;
    sel.dlayout = NULL;
    sel.flayout = NULL;
    sel.glayout = NULL;
    sel.elem = ENUM_MAX(layelem_t);
    sel.col = UINT32_MAX;
    sel.row = UINT32_MAX;
    equ = i_sel_equ(&form->hover, &sel);
    form->hover = sel;
    return !equ;
}

/*---------------------------------------------------------------------------*/

static const DSelect *i_parent_sel(const ArrSt(DSelect) *path, const DSelect *sel)
{
    cassert_no_null(sel);
    arrst_foreach_const(nsel, path, DSelect)
        if (nsel->dlayout == sel->dlayout)
        {
            if (nsel_i > 0)
                return nsel - 1;
        }
    arrst_end()
    return NULL;
}

/*---------------------------------------------------------------------------*/

static DCell *i_up_cell(const ArrSt(DSelect) *path, const DSelect *sel)
{
    cassert_no_null(sel);
    cassert(sel->elem == ekLAYELEM_CELL);
    while (sel != NULL)
    {
        if (sel->row > 0)
        {
            DCell *cell = dlayout_cell(sel->dlayout, sel->col, sel->row - 1);
            while (cell->sublayout != NULL)
            {
                uint32_t n = dlayout_nrows(cell->sublayout);
                cell = dlayout_cell(cell->sublayout, 0, n - 1);
            }

            return cell;
        }

        sel = i_parent_sel(path, sel);
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

static DCell *i_down_cell(const ArrSt(DSelect) *path, const DSelect *sel)
{
    cassert_no_null(sel);
    cassert(sel->elem == ekLAYELEM_CELL);
    while (sel != NULL)
    {
        uint32_t n = dlayout_nrows(sel->dlayout);
        if (sel->row < n - 1)
        {
            DCell *cell = dlayout_cell(sel->dlayout, sel->col, sel->row + 1);
            while (cell->sublayout != NULL)
                cell = dlayout_cell(cell->sublayout, 0, 0);
            return cell;
        }

        sel = i_parent_sel(path, sel);
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

static DCell *i_left_cell(const ArrSt(DSelect) *path, const DSelect *sel)
{
    cassert_no_null(sel);
    cassert(sel->elem == ekLAYELEM_CELL);
    while (sel != NULL)
    {
        if (sel->col > 0)
        {
            DCell *cell = dlayout_cell(sel->dlayout, sel->col - 1, sel->row);
            while (cell->sublayout != NULL)
            {
                uint32_t n = dlayout_ncols(cell->sublayout);
                cell = dlayout_cell(cell->sublayout, n - 1, 0);
            }

            return cell;
        }

        sel = i_parent_sel(path, sel);
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

static DCell *i_right_cell(const ArrSt(DSelect) *path, const DSelect *sel)
{
    cassert_no_null(sel);
    cassert(sel->elem == ekLAYELEM_CELL);
    while (sel != NULL)
    {
        uint32_t n = dlayout_ncols(sel->dlayout);
        if (sel->col < n - 1)
        {
            DCell *cell = dlayout_cell(sel->dlayout, sel->col + 1, sel->row);
            while (cell->sublayout != NULL)
                cell = dlayout_cell(cell->sublayout, 0, 0);
            return cell;
        }

        sel = i_parent_sel(path, sel);
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

static DCell *i_first_cell(DLayout *dlayout)
{
    DCell *cell = dlayout_cell(dlayout, 0, 0);
    while (cell->sublayout != NULL)
        cell = dlayout_cell(cell->sublayout, 0, 0);
    return cell;
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnCursorNav(DForm *form, const vkey_t key, Panel *inspect, Panel *propedit)
{
    const DCell *dcell = NULL;
    cassert_no_null(form);

    if (form->sel.elem == ekLAYELEM_CELL)
    {
        if (key == ekKEY_UP)
            dcell = i_up_cell(form->sel_path, &form->sel);
        else if (key == ekKEY_DOWN)
            dcell = i_down_cell(form->sel_path, &form->sel);
        else if (key == ekKEY_LEFT)
            dcell = i_left_cell(form->sel_path, &form->sel);
        else if (key == ekKEY_RIGHT)
            dcell = i_right_cell(form->sel_path, &form->sel);
    }
    else
    {
        dcell = i_first_cell(form->dlayout);
    }

    if (dcell != NULL)
    {
        DCell *ccell = NULL;
        
        if (form->sel.elem == ekLAYELEM_CELL)
            ccell = dlayout_cell(form->sel.dlayout, form->sel.col, form->sel.row);

        if (dcell != ccell)
        {
            /* We reuse the click process to create the selection path and update property editor / inspector */
            DSelect sel;
            i_elem_at_mouse(form->dlayout, form->fform->layout, form->glayout, dcell->rect.pos.x + 1, dcell->rect.pos.y + 1, form->sel_path, &sel);
            inspect_set(inspect, form);
            propedit_set(propedit, form, &sel);
            form->sel = sel;
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnSupr(DForm *form, Panel *inspect, Panel *propedit)
{
    const DSelect *sel = NULL;
    cassert_no_null(form);
    if (form->sel.dlayout != NULL)
    {
        if (form->sel.elem == ekLAYELEM_CELL)
            sel = &form->sel;
        else
            sel = i_parent_sel(form->sel_path, &form->sel);
    }

    if (sel != NULL)
    {
        cassert(sel->elem == ekLAYELEM_CELL);
        cassert_no_null(sel->flayout);
        cassert_no_null(sel->glayout);
        if (i_sel_empty_cell(sel) == FALSE)
        {
            /* Remove all inspector path steps after deleted cell */
            {
                uint32_t n = arrst_size(form->sel_path, DSelect);
                while (n > 0)
                {
                    const DSelect *last = arrst_last_const(form->sel_path, DSelect);
                    if (i_sel_equ(sel, last) == TRUE)
                        break;

                    arrst_delete(form->sel_path, n - 1, NULL, DSelect);
                    n -= 1;
                }
            }

            /* Remove the cell itself */
            {
                Cell *cell = layout_cell(sel->glayout, sel->col, sel->row);
                i_sel_remove_cell(sel);
                cell_force_size(cell, i_EMPTY_CELL_WIDTH, i_EMPTY_CELL_HEIGHT);
                i_sel_synchro_cell(sel);
                dform_compose(form);
                dform_need_save(form);
                propedit_set(propedit, form, sel);
                inspect_set(inspect, form);
            }

            i_need_save(form, TRUE);
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnCopy(DForm *form, DClipBoard *clipboard)
{
    cassert_no_null(form);
    cassert_no_null(clipboard);
    if (form->sel.elem == ekLAYELEM_CELL)
    {
        const FCell *cell = flayout_ccell(form->sel.flayout, form->sel.col, form->sel.row);
        FCell *ccell = dbind_copy(cell, FCell);
        cassert_no_null(ccell);
        dbind_destopt(&clipboard->fcell, FCell);
        dbind_destopt(&clipboard->flayout, FLayout);
        clipboard->fcell = ccell;
        return TRUE;
    }
    else
    {
        FLayout *clayout = dbind_copy(form->sel.flayout, FLayout);
        dbind_destopt(&clipboard->fcell, FCell);
        dbind_destopt(&clipboard->flayout, FLayout);
        clipboard->flayout = clayout;
        return TRUE;
    }
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnPaste(DForm *form, const DClipBoard *clipboard, Panel *inspect, Panel *propedit)
{
    cassert_no_null(form);
    cassert_no_null(clipboard);
    if (i_sel_empty_cell(&form->sel) == TRUE)
    {
        const char_t *folder_path = designer_folder_path(form->app);
        const DColors *colors = designer_colors(form->app);

        if (clipboard->fcell != NULL)
        {
            switch (clipboard->fcell->type)
            {
            case ekCELL_TYPE_EMPTY:
                break;

            case ekCELL_TYPE_LABEL:
            {
                FLabel *flabel = dbind_copy(clipboard->fcell->widget.label, FLabel);
                i_new_label(flabel, &form->sel);
                break;
            }

            case ekCELL_TYPE_BUTTON:
            {
                FButton *fbutton = dbind_copy(clipboard->fcell->widget.button, FButton);
                i_new_button(fbutton, &form->sel);
                break;
            }

            case ekCELL_TYPE_CHECK:
            {
                FCheck *fcheck = dbind_copy(clipboard->fcell->widget.check, FCheck);
                i_new_check(fcheck, &form->sel);
                break;
            }

            case ekCELL_TYPE_RADIO:
            {
                FRadio *fradio = dbind_copy(clipboard->fcell->widget.radio, FRadio);
                i_new_radio(fradio, &form->sel);
                break;
            }

            case ekCELL_TYPE_TOOL:
            {
                FTool *ftool = dbind_copy(clipboard->fcell->widget.tool, FTool);
                i_new_tool(ftool, &form->sel, folder_path, colors);
                break;
            }

            case ekCELL_TYPE_POPUP:
            {
                FPopUp *fpopup = dbind_copy(clipboard->fcell->widget.popup, FPopUp);
                i_new_popup(fpopup, &form->sel, folder_path, colors);
                break;
            }

            case ekCELL_TYPE_EDIT:
            {
                FEdit *fedit = dbind_copy(clipboard->fcell->widget.edit, FEdit);
                i_new_edit(fedit, &form->sel);
                break;
            }

            case ekCELL_TYPE_COMBO:
            {
                FCombo *fcombo = dbind_copy(clipboard->fcell->widget.combo, FCombo);
                i_new_combo(fcombo, &form->sel);
                break;
            }

            case ekCELL_TYPE_LISTBOX:
            {
                FListBox *flistbox = dbind_copy(clipboard->fcell->widget.listbox, FListBox);
                i_new_listbox(flistbox, &form->sel, folder_path, colors);
                break;
            }

            case ekCELL_TYPE_SLIDER:
            {
                FSlider *fslider = dbind_copy(clipboard->fcell->widget.slider, FSlider);
                i_new_slider(fslider, &form->sel);
                break;
            }

            case ekCELL_TYPE_VSLIDER:
            {
                FVSlider *fvslider = dbind_copy(clipboard->fcell->widget.vslider, FVSlider);
                i_new_vslider(fvslider, &form->sel);
                break;
            }

            case ekCELL_TYPE_PROGRESS:
            {
                FProgress *fprogress = dbind_copy(clipboard->fcell->widget.progress, FProgress);
                i_new_progress(fprogress, &form->sel);
                break;
            }

            case ekCELL_TYPE_TEXT:
            {
                FText *ftext = dbind_copy(clipboard->fcell->widget.text, FText);
                i_new_text(ftext, &form->sel);
                break;
            }

            case ekCELL_TYPE_IMAGE:
            {
                FImage *fimage = dbind_copy(clipboard->fcell->widget.image, FImage);
                i_new_image(fimage, &form->sel, folder_path, colors);
                break;
            }

            case ekCELL_TYPE_TABLEVIEW:
            {
                FTable *ftable = dbind_copy(clipboard->fcell->widget.table, FTable);
                i_new_table(ftable, &form->sel);
                break;
            }

            case ekCELL_TYPE_LAYOUT:
            {
                FLayout *fsublayout = dbind_copy(clipboard->fcell->widget.layout, FLayout);
                i_new_sublayout(fsublayout, &form->sel, folder_path, colors);
                break;
            }

            default:
                cassert_default(clipboard->fcell->type);
            }

            i_copy_cell_props(clipboard->fcell, &form->sel);
            i_after_new_widget(form, inspect, propedit, &form->sel);
            return TRUE;            
        }
        else if (clipboard->flayout != NULL)
        {
            FLayout *fsublayout = dbind_copy(clipboard->flayout, FLayout);
            i_new_sublayout(fsublayout, &form->sel, folder_path, colors);
            i_after_new_widget(form, inspect, propedit, &form->sel);
            return TRUE;            
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

static void i_apply_undo_frame(DForm *form, const uint32_t pos, Panel *inspect, Panel *propedit)
{
    const UndoFrame *frame = NULL;

    cassert_no_null(form);
    frame = arrst_get_const(form->undo_stack, pos, UndoFrame);
    dbind_destroy(&form->fform, FForm);

    if (form->window != NULL)
    {
        window_destroy(&form->window);
        dlayout_destroy(&form->dlayout);
        form->glayout = NULL;
    }
    else
    {
        cassert(form->glayout == NULL);
        cassert(form->dlayout == NULL);
    }

    form->fform = dbind_copy(frame->fform, FForm);
    form->undo_pos = pos;
    dform_compose(form);

    {
        DSelect sel;
        V2Df cpos = frame->cellpos;
        if (cpos.x >= 0)
        {
            cpos.x += form->origin.x + 1;
            cpos.y += form->origin.y + 1;
        }

        i_elem_at_mouse(form->dlayout, form->fform->layout, form->glayout, cpos.x, cpos.y, form->sel_path, &sel);
        inspect_set(inspect, form);
        propedit_set(propedit, form, &sel);
        form->sel = sel;
    }
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnUndo(DForm *form, Panel *inspect, Panel *propedit)
{
    cassert_no_null(form);
    if (form->undo_pos > 0)
    {
        i_apply_undo_frame(form, form->undo_pos - 1, inspect, propedit);
        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

bool_t dform_OnRedo(DForm *form, Panel *inspect, Panel *propedit)
{
    uint32_t n = 0;
    cassert_no_null(form);
    n = arrst_size(form->undo_stack, UndoFrame);
    if (n > 0 && form->undo_pos < n - 1)
    {
        i_apply_undo_frame(form, form->undo_pos + 1, inspect, propedit);
        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

V2Df dform_get_origin(const DForm *form)
{
    cassert_no_null(form);
    return form->origin;
}

/*---------------------------------------------------------------------------*/

DSelect dform_get_sel(const DForm *form)
{
    cassert_no_null(form);
    return form->sel;
}

/*---------------------------------------------------------------------------*/

void dform_origin(DForm *form, const V2Df origin)
{
    cassert_no_null(form);
    form->origin = origin;
    dlayout_synchro_visual(form->dlayout, form->glayout, form->origin);
}

/*---------------------------------------------------------------------------*/

void dform_insert_col(DForm *form, const DSelect *sel, const uint32_t col_id)
{
    uint32_t i, n;
    cassert_no_null(form);
    cassert_no_null(sel);
    flayout_insert_col(sel->flayout, col_id);
    layout_insert_col(sel->glayout, col_id);
    dlayout_insert_col(sel->dlayout, col_id);

    n = layout_nrows(sel->glayout);
    cassert(n == flayout_nrows(sel->flayout));
    cassert(n == dlayout_nrows(sel->dlayout));
    for (i = 0; i < n; ++i)
    {
        Cell *cell = layout_cell(sel->glayout, col_id, i);
        cell_force_size(cell, i_EMPTY_CELL_WIDTH, i_EMPTY_CELL_HEIGHT);
        i_cell_obj_name(form, sel->flayout, col_id, i);
    }

    dform_compose(form);
    i_need_save(form, TRUE);
    form->sel = *sel;
    form->sel.col = col_id;
}

/*---------------------------------------------------------------------------*/

void dform_insert_row(DForm *form, const DSelect *sel, const uint32_t row_id)
{
    uint32_t i, n;
    cassert_no_null(form);
    cassert_no_null(sel);
    flayout_insert_row(sel->flayout, row_id);
    layout_insert_row(sel->glayout, row_id);
    dlayout_insert_row(sel->dlayout, row_id);

    n = layout_ncols(sel->glayout);
    cassert(n == flayout_ncols(sel->flayout));
    cassert(n == dlayout_ncols(sel->dlayout));
    for (i = 0; i < n; ++i)
    {
        Cell *cell = layout_cell(sel->glayout, i, row_id);
        cell_force_size(cell, i_EMPTY_CELL_WIDTH, i_EMPTY_CELL_HEIGHT);
        i_cell_obj_name(form, sel->flayout, i, row_id);
    }

    dform_compose(form);
    i_need_save(form, TRUE);
    form->sel = *sel;
    form->sel.row = row_id;
}

/*---------------------------------------------------------------------------*/

void dform_remove_col(DForm *form, const DSelect *sel, const uint32_t col_id)
{
    uint32_t n, col = col_id;
    cassert_no_null(form);
    cassert_no_null(sel);
    flayout_remove_col(sel->flayout, col_id);
    layout_remove_col(sel->glayout, col_id);
    dlayout_remove_col(sel->dlayout, col_id);
    n = layout_ncols(sel->glayout);
    cassert(n == flayout_ncols(sel->flayout));
    cassert(n == dlayout_ncols(sel->dlayout));
    cassert(n > 0);

    if (col_id == n)
        col = col_id - 1;

    dform_compose(form);
    i_need_save(form, TRUE);
    form->sel = *sel;
    form->sel.col = col;
}

/*---------------------------------------------------------------------------*/

void dform_remove_row(DForm *form, const DSelect *sel, const uint32_t row_id)
{
    uint32_t n, row = row_id;
    cassert_no_null(form);
    cassert_no_null(sel);
    flayout_remove_row(sel->flayout, row_id);
    layout_remove_row(sel->glayout, row_id);
    dlayout_remove_row(sel->dlayout, row_id);
    n = layout_nrows(sel->glayout);
    cassert(n == flayout_nrows(sel->flayout));
    cassert(n == dlayout_nrows(sel->dlayout));
    cassert(n > 0);

    if (row_id == n)
        row = row_id - 1;

    dform_compose(form);
    i_need_save(form, TRUE);
    form->sel = *sel;
    form->sel.row = row;
}

/*---------------------------------------------------------------------------*/

static FCell *i_sel_fcell(const DSelect *sel)
{
    cassert_no_null(sel);
    if (sel->flayout != NULL)
    {
        cassert_no_null(sel->glayout);
        cassert_no_null(sel->dlayout);
        if (sel->elem == ekLAYELEM_CELL)
        {
            return flayout_cell(sel->flayout, sel->col, sel->row);
        }
    }
    else
    {
        cassert(sel->glayout == NULL);
        cassert(sel->dlayout == NULL);
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

void dform_synchro_cell_image(DForm *form, const DSelect *sel, const Image *image, const char_t *imgname)
{
    FCell *cell = i_sel_fcell(sel);
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    i_need_save(form, TRUE);
    if (cell->type == ekCELL_TYPE_IMAGE)
    {
        ImageView *imgview = layout_get_imageview(sel->glayout, sel->col, sel->row);
        imageview_image(imgview, image);
        str_upd(&cell->widget.image->path, imgname);
    }
    else
    {
        cassert(FALSE);
    }
}

/*---------------------------------------------------------------------------*/

void dform_synchro_label(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Label *label = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_LABEL);
    i_need_save(form, TRUE);
    label = layout_get_label(sel->glayout, sel->col, sel->row);
    flabel_synchro(cell->widget.label, label);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_button(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Button *button = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_BUTTON);
    i_need_save(form, TRUE);
    button = layout_get_button(sel->glayout, sel->col, sel->row);
    fbutton_synchro(cell->widget.button, button);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_check(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Button *button = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_CHECK);
    i_need_save(form, TRUE);
    button = layout_get_button(sel->glayout, sel->col, sel->row);
    fcheck_synchro(cell->widget.check, button);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_radio(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Button *button = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_RADIO);
    i_need_save(form, TRUE);
    button = layout_get_button(sel->glayout, sel->col, sel->row);
    fradio_synchro(cell->widget.radio, button);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_tool(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Button *button = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_TOOL);
    i_need_save(form, TRUE);
    button = layout_get_button(sel->glayout, sel->col, sel->row);
    ftool_synchro(cell->widget.tool, button, NULL);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_popup(DForm *form, const DSelect *sel, const char_t *resource_path)
{
    FCell *cell = i_sel_fcell(sel);
    PopUp *popup = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_POPUP);
    i_need_save(form, TRUE);
    popup = layout_get_popup(sel->glayout, sel->col, sel->row);
    fpopup_synchro(cell->widget.popup, popup, resource_path);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_edit(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Edit *edit = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_EDIT);
    i_need_save(form, TRUE);
    edit = layout_get_edit(sel->glayout, sel->col, sel->row);
    fedit_synchro(cell->widget.edit, edit);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_combo(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Combo *combo = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_COMBO);
    i_need_save(form, TRUE);
    combo = layout_get_combo(sel->glayout, sel->col, sel->row);
    fcombo_synchro(cell->widget.combo, combo);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_listbox(DForm *form, const DSelect *sel, const char_t *resource_path)
{
    FCell *cell = i_sel_fcell(sel);
    ListBox *listbox = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_LISTBOX);
    i_need_save(form, TRUE);
    listbox = layout_get_listbox(sel->glayout, sel->col, sel->row);
    flistbox_synchro(cell->widget.listbox, listbox, resource_path);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_slider(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Slider *slider = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_SLIDER);
    i_need_save(form, TRUE);
    slider = layout_get_slider(sel->glayout, sel->col, sel->row);
    fslider_synchro(cell->widget.slider, slider);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_vslider(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Slider *slider = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_VSLIDER);
    i_need_save(form, TRUE);
    slider = layout_get_slider(sel->glayout, sel->col, sel->row);
    fvslider_synchro(cell->widget.vslider, slider);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_progress(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    Progress *progress = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_PROGRESS);
    i_need_save(form, TRUE);
    progress = layout_get_progress(sel->glayout, sel->col, sel->row);
    fprogress_synchro(cell->widget.progress, progress);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_textview(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    TextView *view = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_TEXT);
    i_need_save(form, TRUE);
    view = layout_get_textview(sel->glayout, sel->col, sel->row);
    ftext_synchro(cell->widget.text, view);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_imageview(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    ImageView *view = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_IMAGE);
    i_need_save(form, TRUE);
    view = layout_get_imageview(sel->glayout, sel->col, sel->row);
    fimage_synchro(cell->widget.image, view, NULL);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_table(DForm *form, const DSelect *sel)
{
    FCell *cell = i_sel_fcell(sel);
    TableView *view = NULL;
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(cell);
    cassert(cell->type == ekCELL_TYPE_TABLEVIEW);
    i_need_save(form, TRUE);
    view = layout_get_tableview(sel->glayout, sel->col, sel->row);
    ftable_synchro(cell->widget.table, view);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_layout_margin(DForm *form, const DSelect *sel)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    i_need_save(form, TRUE);
    layout_margin4(sel->glayout, sel->flayout->margin_top, sel->flayout->margin_right, sel->flayout->margin_bottom, sel->flayout->margin_left);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_column_margin(DForm *form, const DSelect *sel, const FColumn *fcol, const uint32_t col)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(fcol);
    cassert(flayout_column(cast(sel->flayout, FLayout), col) == fcol);
    i_need_save(form, TRUE);
    layout_hmargin(sel->glayout, col, fcol->margin_right);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_column_width(DForm *form, const DSelect *sel, const FColumn *fcol, const uint32_t col)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(fcol);
    cassert(flayout_column(cast(sel->flayout, FLayout), col) == fcol);
    i_need_save(form, TRUE);
    layout_hsize(sel->glayout, col, fcol->forced_width);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_row_margin(DForm *form, const DSelect *sel, const FRow *frow, const uint32_t row)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(frow);
    cassert(flayout_row(cast(sel->flayout, FLayout), row) == frow);
    i_need_save(form, TRUE);
    layout_vmargin(sel->glayout, row, frow->margin_bottom);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_row_height(DForm *form, const DSelect *sel, const FRow *frow, const uint32_t row)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(frow);
    cassert(flayout_row(cast(sel->flayout, FLayout), row) == frow);
    i_need_save(form, TRUE);
    layout_vsize(sel->glayout, row, frow->forced_height);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_cell_halign(DForm *form, const DSelect *sel, const FCell *fcell, const uint32_t col, const uint32_t row)
{
    align_t align = ENUM_MAX(align_t);
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(fcell);
    cassert(flayout_cell(cast(sel->flayout, FLayout), col, row) == fcell);
    i_need_save(form, TRUE);
    align = i_halign(fcell->halign);
    layout_halign(sel->glayout, col, row, align);
}

/*---------------------------------------------------------------------------*/

void dform_synchro_cell_valign(DForm *form, const DSelect *sel, const FCell *fcell, const uint32_t col, const uint32_t row)
{
    align_t align = ENUM_MAX(align_t);
    cassert_no_null(form);
    cassert_no_null(sel);
    cassert_no_null(fcell);
    cassert(flayout_cell(cast(sel->flayout, FLayout), col, row) == fcell);
    i_need_save(form, TRUE);
    align = i_valign(fcell->valign);
    layout_valign(sel->glayout, col, row, align);
}

/*---------------------------------------------------------------------------*/

FCell *dform_sel_fcell(const DSelect *sel)
{
    return i_sel_fcell(sel);
}

/*---------------------------------------------------------------------------*/

void dform_draw(const DForm *form, const widget_t swidget, const Font *default_font, const Font *bold_font, const cmode_t cmode, const DColors *colors, const char_t *form_name, const bool_t focus, DCtx *ctx)
{
    cassert_no_null(form);
    cassert_no_null(form->fform);
    dlayout_draw(form->dlayout, form->fform->layout, form->glayout, &form->hover, &form->sel, swidget, default_font, bold_font, cmode, colors, form_name, focus, ctx);
}

/*---------------------------------------------------------------------------*/

uint32_t dform_selpath_size(const DForm *form)
{
    uint32_t n = 0;
    cassert_no_null(form);
    n = arrst_size(form->sel_path, DSelect);
    if (n > 0)
    {
        const DSelect *last = arrst_last_const(form->sel_path, DSelect);
        cassert(last->dlayout != NULL);
        if (last->elem == ekLAYELEM_CELL)
            return n * 2;
        else
            return (n - 1) * 2 + 1;
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

const char_t* dform_cell_type(const celltype_t type)
{
    switch(type)
    {
    case ekCELL_TYPE_EMPTY:
        return gui_text(TEXT_CELL_EMPTY);
    case ekCELL_TYPE_LABEL:
        return gui_text(TEXT_CELL_LABEL);
    case ekCELL_TYPE_BUTTON:
        return gui_text(TEXT_CELL_BUTTON);
    case ekCELL_TYPE_CHECK:
        return gui_text(TEXT_CELL_CHECK);
    case ekCELL_TYPE_RADIO:
        return gui_text(TEXT_CELL_RADIO);
    case ekCELL_TYPE_TOOL:
        return gui_text(TEXT_CELL_TOOL);        
    case ekCELL_TYPE_POPUP:
        return gui_text(TEXT_CELL_POPUP);
    case ekCELL_TYPE_EDIT:
        return gui_text(TEXT_CELL_EDIT);
    case ekCELL_TYPE_COMBO:
        return gui_text(TEXT_CELL_COMBO);
    case ekCELL_TYPE_LISTBOX:
        return gui_text(TEXT_CELL_LISTBOX);
    case ekCELL_TYPE_SLIDER:
        return gui_text(TEXT_CELL_SLIDER);
    case ekCELL_TYPE_VSLIDER:
        return gui_text(TEXT_CELL_VSLIDER);
    case ekCELL_TYPE_PROGRESS:
        return gui_text(TEXT_CELL_PROGRESS);
    case ekCELL_TYPE_TEXT:
        return gui_text(TEXT_CELL_TEXT);
    case ekCELL_TYPE_IMAGE:
        return gui_text(TEXT_CELL_IMAGE);
    case ekCELL_TYPE_TABLEVIEW:
        return gui_text(TEXT_CELL_TABLE);
    case ekCELL_TYPE_LAYOUT:
        return gui_text(TEXT_CELL_LAYOUT);
    default:
        cassert_default(type);
    }

    return "";
}

/*---------------------------------------------------------------------------*/

const Image *dform_cell_icon(const celltype_t type)
{
    switch(type)
    {
    case ekCELL_TYPE_EMPTY:
        return NULL;
    case ekCELL_TYPE_LABEL:
        return gui_image(LABEL16_PNG);
    case ekCELL_TYPE_BUTTON:
        return gui_image(PUSHBUT16_PNG);
    case ekCELL_TYPE_CHECK:
        return gui_image(CHECBUT16_PNG);
    case ekCELL_TYPE_RADIO:
        return gui_image(RADBUT16_PNG);
    case ekCELL_TYPE_TOOL:
        return gui_image(TOOLBUT16_PNG);        
    case ekCELL_TYPE_POPUP:
        return gui_image(POPUP16_PNG);
    case ekCELL_TYPE_EDIT:
        return gui_image(EDITBOX16_PNG);
    case ekCELL_TYPE_COMBO:
        return gui_image(COMBOBOX16_PNG);
    case ekCELL_TYPE_LISTBOX:
        return gui_image(LISTVIEW16_PNG);
    case ekCELL_TYPE_SLIDER:
        return gui_image(HORSLIDER16_PNG);
    case ekCELL_TYPE_VSLIDER:
        return gui_image(VERSLIDER16_PNG);
    case ekCELL_TYPE_PROGRESS:
        return gui_image(PROGRESSBAR16_PNG);
    case ekCELL_TYPE_TEXT:
        return gui_image(TEXTVIEW16_PNG);
    case ekCELL_TYPE_IMAGE:
        return gui_image(IMAGEVIEW16_PNG);
    case ekCELL_TYPE_TABLEVIEW:
        return gui_image(TABLEVIEW16_PNG);
    case ekCELL_TYPE_LAYOUT:
        return gui_image(LCELL16_PNG);
    default:
        cassert_default(type);
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

const char_t *dform_selpath_caption(const DForm *form, const uint32_t col, const uint32_t row)
{
    const DSelect *sel = NULL;
    cassert_no_null(form);
    sel = arrst_get_const(form->sel_path, row / 2, DSelect);
    cassert(col <= 1);
    cassert_no_null(sel);
    cassert_no_null(sel->dlayout);
    cassert_no_null(sel->flayout);

    /* Even rows == layout */
    if (row % 2 == 0)
    {
        if (col == 0)
        {
            return tc(sel->flayout->name);
        }
        else
        {
            if (arrst_size(sel->flayout->cols, FColumn) == 1 && arrst_size(sel->flayout->rows, FRow) == 1)
                return gui_text(TEXT_SINGLE_LAYOUT);
            else if (arrst_size(sel->flayout->cols, FColumn) == 1)
                return gui_text(TEXT_VERT_LAYOUT);
            else if (arrst_size(sel->flayout->rows, FRow) == 1)
                return gui_text(TEXT_HORZ_LAYOUT);
            else
                return gui_text(TEXT_GRID_LAYOUT);
        }
    }
    /* Odd rows == cell */
    else
    {
        const FCell *cell = i_sel_fcell(sel);
        if (col == 0)
        {
            return tc(cell->name);
        }
        else
        {
            cassert(col == 1);
            return dform_cell_type(cell->type);
        }
    }
}

/*---------------------------------------------------------------------------*/

const Image *dform_selpath_icon(const DForm *form, const uint32_t col, const uint32_t row)
{
    const DSelect *sel = NULL;
    cassert_no_null(form);
    sel = arrst_get_const(form->sel_path, row / 2, DSelect);
    cassert(col <= 1);
    cassert_no_null(sel);
    cassert_no_null(sel->dlayout);
    cassert_no_null(sel->flayout);

    if (col == 0)
    {
        /* Even rows == layout */
        if (row % 2 == 0)
        {
            if (arrst_size(sel->flayout->cols, FColumn) == 1 && arrst_size(sel->flayout->rows, FRow) == 1)
                return gui_image(SLAYOUT16_PNG);
            else if (arrst_size(sel->flayout->cols, FColumn) == 1)
                return gui_image(VLAYOUT16_PNG);
            else if (arrst_size(sel->flayout->rows, FRow) == 1)
                return gui_image(HLAYOUT16_PNG);
            else
                return gui_image(GLAYOUT16_PNG);
        }
        /* Odd rows == cell */
        else
        {
            const FCell *cell = i_sel_fcell(sel);
            return dform_cell_icon(cell->type);
        }
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

void dform_inspect_select(DForm *form, Panel *propedit, const uint32_t row)
{
    const DSelect *sel = NULL;
    bool_t laysel = FALSE;
    uint32_t i = row / 2;
    uint32_t n;
    cassert_no_null(form);
    sel = arrst_get_const(form->sel_path, i, DSelect);
    n = arrst_size(form->sel_path, DSelect);

    {
        const DSelect *lsel = arrst_last_const(form->sel_path, DSelect);
        cassert_no_null(lsel);
        laysel = lsel->elem != ekLAYELEM_CELL;
    }

    /* Even rows == layout */
    if (row % 2 == 0)
    {
        /* This layout has a cell below */
        if (laysel == FALSE || i < n - 1)
        {
            DSelect propsel;
            propsel.dlayout = sel->dlayout;
            propsel.flayout = sel->flayout;
            propsel.glayout = sel->glayout;
            propsel.elem = ekLAYELEM_LAYOUT;
            propsel.col = sel->col;
            propsel.row = sel->row;
            form->sel = propsel;
            propedit_set(propedit, form, &propsel);
        }
        else
        {
            /* This layout is the final leaf in object inspector */
            cassert(sel->elem != ekLAYELEM_CELL);
            form->sel = *sel;
            propedit_set(propedit, form, sel);
        }
    }
    /* Odd rows == cell */
    else
    {
        cassert(sel->elem == ekLAYELEM_CELL);
        form->sel = *sel;
        propedit_set(propedit, form, sel);
    }
}

/*---------------------------------------------------------------------------*/
/* Unify with 'dialogs' code */
static void i_center_window(const Window *parent, Window *window)
{
    V2Df p1 = window_get_origin(parent);
    S2Df s1 = window_get_size(parent);
    S2Df s2 = window_get_size(window);
    V2Df p2;
    p2.x = p1.x + (s1.width - s2.width) / 2;
    p2.y = p1.y + (s1.height - s2.height) / 2;
    window_origin(window, p2);
}

/*---------------------------------------------------------------------------*/

void dform_set_need_save(DForm *form)
{
    i_need_save(form, TRUE);
}

/*---------------------------------------------------------------------------*/

void dform_update_sel(DForm *form, const DSelect *sel)
{
    cassert_no_null(form);
    cassert_no_null(sel);
    form->sel = *sel;
}

/*---------------------------------------------------------------------------*/

void dform_simulate(DForm *form, const char_t *form_name, Window *window)
{
    cassert_no_null(form);
    if (form->window != NULL)
    {
        String *name = str_printf("%s - %s", gui_text(TEXT_FORM), form_name);
        i_center_window(window, form->window);
        window_title(form->window, tc(name));
        window_modal(form->window, window);
        str_destroy(&name);
    }
}
