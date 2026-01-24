/* Dialog boxes */

#include "dialogs.h"
#include "dgui.h"
#include "res_designer.h"
#include <nflib/nflib.h>
#include <nflib/fbutton.h>
#include <nflib/fcheck.h>
#include <nflib/fcombo.h>
#include <nflib/fedit.h>
#include <nflib/fimage.h>
#include <nflib/flabel.h>
#include <nflib/flistbox.h>
#include <nflib/fradio.h>
#include <nflib/ftool.h>
#include <nflib/flayout.h>
#include <nflib/fpopup.h>
#include <nflib/fprogress.h>
#include <nflib/fslider.h>
#include <nflib/fvslider.h>
#include <nflib/ftable.h>
#include <nflib/ftext.h>
#include <nflib/fview.h>
#include <nflib/fsview.h>
#include <gui/button.h>
#include <gui/cell.h>
#include <gui/comwin.h>
#include <gui/edit.h>
#include <gui/gui.h>
#include <gui/label.h>
#include <gui/layout.h>
#include <gui/imageview.h>
#include <gui/panel.h>
#include <gui/popup.h>
#include <gui/updown.h>
#include <gui/window.h>
#include <draw2d/image.h>
#include <geom2d/s2d.h>
#include <core/dbind.h>
#include <core/event.h>
#include <core/strings.h>
#include <sewer/cassert.h>
#include <sewer/bmem.h>

typedef struct _dialogprops_t DialogProps;
typedef struct _dialoglayout_t DialogLayout;
typedef struct _dialogdata_t DialogData;

typedef enum _dbuttons_t
{
    ekDBUT_ONLY_OK,
    ekDBUT_OK_CANCEL_DEF_OK,
    ekDBUT_OK_CANCEL_DEF_CANCEL,
    ekDBUT_SAVE_OPTS
} dbuttons_t;

struct _dialogprops_t
{
    String *filename;
    String *description;
};

struct _dialoglayout_t
{
    uint32_t ncols;
    uint32_t nrows;
};

struct _dialogdata_t
{
    const char_t *path;
    FImage *fimage;
    FElem *felem;
    FTool *ftool;
    Button *defbutton;
    Label *label;
    ImageView *imageview;
    Button *toolbutton;
    Window *window;
    Edit *edit;
};

#define BUTTON_OK 1000
#define BUTTON_CANCEL 1001
#define BUTTON_SAVE 1002
#define BUTTON_NO_SAVE 1003
#define BUTTON_HEADER_CLOSE 1004
static const real32_t i_LABEL_MARGIN = 10;

/*---------------------------------------------------------------------------*/

void dialog_dbind(void)
{
    dbind(DialogProps, String*, filename);
    dbind(DialogProps, String*, description);
    dbind(DialogLayout, uint32_t, ncols);
    dbind(DialogLayout, uint32_t, nrows);
    dbind_range(DialogLayout, uint32_t, ncols, 1, 20);
    dbind_range(DialogLayout, uint32_t, nrows, 1, 20);
}

/*---------------------------------------------------------------------------*/

static DialogData i_dialog_data(void)
{
    DialogData data;
    bmem_zero(&data, DialogData);
    return data;
}

/*---------------------------------------------------------------------------*/

static void i_remove_dialog_data(DialogData *data)
{
    bmem_zero(data, DialogData);
}

/*---------------------------------------------------------------------------*/

static void i_remove_dialog_props(DialogProps *props)
{
    cassert_no_null(props);
    str_destroy(&props->filename);
    str_destroy(&props->description);
    bmem_zero(props, DialogProps);
}

/*---------------------------------------------------------------------------*/

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

static void i_OnClick(DialogData *data, Event *e)
{
    Button *button = event_sender(e, Button);
    uint32_t tag = button_get_tag(button);
    cassert_no_null(data);
    window_stop_modal(data->window, tag);
}

/*---------------------------------------------------------------------------*/

static Layout *i_ok(DialogData *data)
{
    Layout *layout = layout_create(1, 1);
    Button *button1 = button_push();
    cassert_no_null(data);
    button_text(button1, gui_text(TEXT_OK));
    button_tag(button1, BUTTON_OK);
    button_OnClick(button1, listener(data, i_OnClick, DialogData));
    button_hpadding(button1, 40);
    layout_button(layout, button1, 0, 0);
    data->defbutton = button1;
    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_ok_cancel(DialogData *data, const bool_t ok_default)
{
    Layout *layout = layout_create(3, 1);
    Button *button1 = button_push();
    Button *button2 = button_push();
    cassert_no_null(data);
    button_text(button1, gui_text(TEXT_OK));
    button_text(button2, gui_text(TEXT_CANCEL));
    button_tag(button1, BUTTON_OK);
    button_tag(button2, BUTTON_CANCEL);
    button_OnClick(button1, listener(data, i_OnClick, DialogData));
    button_OnClick(button2, listener(data, i_OnClick, DialogData));
    button_hpadding(button1, 40);
    button_hpadding(button2, 40);
    layout_button(layout, button1, 1, 0);
    layout_button(layout, button2, 2, 0);
    layout_hmargin(layout, 1, 5);
    layout_hexpand(layout, 0);

    if (ok_default == TRUE)
        data->defbutton = button1;
    else
        data->defbutton = button2;

    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_save_buttons(DialogData *data)
{
    Layout *layout = layout_create(4, 1);
    Button *button1 = button_push();
    Button *button2 = button_push();
    Button *button3 = button_push();
    cassert_no_null(data);
    button_text(button1, gui_text(TEXT_SAVE_CHANGES));
    button_text(button2, gui_text(TEXT_DISC_CHANGES));
    button_text(button3, gui_text(TEXT_CANCEL));
    button_tag(button1, BUTTON_SAVE);
    button_tag(button2, BUTTON_NO_SAVE);
    button_tag(button3, BUTTON_CANCEL);
    button_OnClick(button1, listener(data, i_OnClick, DialogData));
    button_OnClick(button2, listener(data, i_OnClick, DialogData));
    button_OnClick(button3, listener(data, i_OnClick, DialogData));
    layout_button(layout, button1, 1, 0);
    layout_button(layout, button2, 2, 0);
    layout_button(layout, button3, 3, 0);
    layout_hmargin(layout, 1, 5);
    layout_hmargin(layout, 2, 5);
    layout_hexpand(layout, 0);
    data->defbutton = button1;
    return layout;
}

/*---------------------------------------------------------------------------*/

static void i_OnHeaderClose(DialogData *data, Event *e)
{
    cassert_no_null(data);
    unref(e);
    window_stop_modal(data->window, BUTTON_HEADER_CLOSE);
}

/*---------------------------------------------------------------------------*/

static Layout *i_buttons_layout(DialogData *data, const dbuttons_t buttons)
{
    switch (buttons)
    {
    case ekDBUT_ONLY_OK:
        return i_ok(data);
    case ekDBUT_OK_CANCEL_DEF_OK:
        return i_ok_cancel(data, TRUE);
    case ekDBUT_OK_CANCEL_DEF_CANCEL:
        return i_ok_cancel(data, FALSE);
    case ekDBUT_SAVE_OPTS:
        return i_save_buttons(data);
    default:
        cassert_default(buttons);
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_modal_launch(Window *parent, DialogData *data, Layout *inner_layout, const Font *font, const ResId icon_id, const ResId header_id, const char_t *caption, const dbuttons_t buttons)
{
    Layout *layout1 = layout_create(1, 4);
    Layout *layout2 = layout_create(2, 1);
    Layout *layout3 = inner_layout;
    Layout *layout4 = i_buttons_layout(data, buttons);
    ImageView *icon = imageview_create();               
    View *header = dgui_panel_header(gui_text(header_id), font, listener(data, i_OnHeaderClose, DialogData));
    Label *label = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_RETURN | ekWINDOW_ESC | ekWINDOW_EDGE);
    uint32_t ret = 0;
    cassert_no_null(data);
    data->window = window;
    imageview_scale(icon, ekGUI_SCALE_ADJUST);
    imageview_image(icon, gui_image(icon_id));
    label_text(label, caption);
    label_multiline(label, TRUE);
    label_min_width(label, 200);
    layout_imageview(layout2, icon, 0, 0);
    layout_label(layout2, label, 1, 0);
    layout_view(layout1, header, 0, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_layout(layout1, layout3, 0, 2);
    layout_layout(layout1, layout4, 0, 3);
    layout_hmargin(layout2, 0, 10);
    layout_margin4(layout2, 10, 10, 10, 10);
    layout_margin4(layout3, 0, 30, 0, 60);    
    layout_halign(layout1, 0, 3, ekRIGHT);
    layout_margin4(layout4, 30, 10, 10, 10);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data->defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);
    window_destroy(&window);
    return ret;
}

/*---------------------------------------------------------------------------*/

void dialog_form_name_exists(Window *parent, const Font *font, const char_t *name)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(1, 1);
    String *caption = NULL;
    caption = str_printf(gui_text(TEXT_NAME_EXISTS), name);
    i_modal_launch(parent, &data, layout, font, ERROR24_PNG, TEXT_ERROR_NAME, tc(caption), ekDBUT_ONLY_OK);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
}

/*---------------------------------------------------------------------------*/

void dialog_no_remove_col(Window *parent, const Font *font, const char_t *layout_name)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(1, 1);
    String *caption = NULL;
    caption = str_printf(gui_text(TEXT_NO_REMOVE_COL), layout_name);
    i_modal_launch(parent, &data, layout, font, ERROR24_PNG, TEXT_REMOVE_COLC, tc(caption), ekDBUT_ONLY_OK);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
}

/*---------------------------------------------------------------------------*/

void dialog_no_remove_row(Window *parent, const Font *font, const char_t *layout_name)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(1, 1);
    String *caption = NULL;
    caption = str_printf(gui_text(TEXT_NO_REMOVE_ROW), layout_name);
    i_modal_launch(parent, &data, layout, font, ERROR24_PNG, TEXT_REMOVE_ROWC, tc(caption), ekDBUT_ONLY_OK);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
}

/*---------------------------------------------------------------------------*/

static bool_t i_form_dialog(Window *parent, const Font *font, const char_t *caption, ResId icon_id, ResId title_id, String **filename, String **desc)
{
    bool_t ok = FALSE;
    DialogProps props;
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(2, 2);
    uint32_t ret = 0;
    cassert_no_null(filename);
    cassert_no_null(desc);
    props.filename = str_empty(*filename) ? str_c("") : str_copy(*filename);
    props.description = str_empty(*desc) ?  str_c("") : str_copy(*desc);

    /* Form layout */
    {
        Label *label1 = label_create();
        Label *label2 = label_create();
        Edit *edit1 = edit_create();
        Edit *edit2 = edit_multiline();
        label_text(label1, gui_text(TEXT_FILENAME));
        label_text(label2, gui_text(TEXT_DESCRIPTION));
        edit_min_width(edit2, 200);
        edit_min_height(edit2, 100);
        layout_label(layout, label1, 0, 0);
        layout_label(layout, label2, 0, 1);
        layout_edit(layout, edit1, 1, 0);
        layout_edit(layout, edit2, 1, 1);
        layout_hmargin(layout, 0, 5);
        layout_vmargin(layout, 0, 5);
        cell_dbind(layout_cell(layout, 1, 0), DialogProps, String *, filename);
        cell_dbind(layout_cell(layout, 1, 1), DialogProps, String *, description);
        layout_dbind(layout, NULL, DialogProps);
        layout_dbind_obj(layout, &props, DialogProps);
    }

    ret = i_modal_launch(parent, &data, layout, font, icon_id, title_id, caption, ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK && ret != ekGUI_CLOSE_INTRO)
    {
        ok = FALSE;
    }
    else
    {
        str_upd(filename, tc(props.filename));
        str_upd(desc, tc(props.description));
        ok = TRUE;
    }

    i_remove_dialog_props(&props);
    i_remove_dialog_data(&data);
    return ok;
}

/*---------------------------------------------------------------------------*/

bool_t dialog_new_form(Window *parent, const Font *font, String **filename, String **desc)
{
    return i_form_dialog(parent, font, gui_text(TEXT_NEW_FORM), NEW_PNG, TEXT_FORM, filename, desc);
}

/*---------------------------------------------------------------------------*/

bool_t dialog_props_form(Window *parent, const Font *font, String **filename, String **desc)
{
    return i_form_dialog(parent, font, gui_text(TEXT_PROPS_FORM), PROPS_PNG, TEXT_FORM_PROPS, filename, desc);
}

/*---------------------------------------------------------------------------*/

bool_t dialog_remove_form(Window *parent, const Font *font, const char_t *name)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(1, 1);
    String *caption = NULL;
    uint32_t ret = 0;
    caption = str_printf(gui_text(TEXT_REMOVE_FORM), name);
    ret = i_modal_launch(parent, &data, layout, font, REMOVE_PNG, TEXT_FORM_REMOVE, tc(caption), ekDBUT_OK_CANCEL_DEF_CANCEL);
    str_destroy(&caption);
    i_remove_dialog_data(&data);

    if (ret != BUTTON_OK && ret != ekGUI_CLOSE_INTRO)
        return FALSE;
    else
        return TRUE;
}

/*---------------------------------------------------------------------------*/

bool_t dialog_remove_col(Window *parent, const Font *font, const char_t *layout_name, const uint32_t col)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(1, 1);
    String *caption = NULL;
    uint32_t ret = 0;
    caption = str_printf(gui_text(TEXT_REMOVE_COL), col, layout_name);
    ret = i_modal_launch(parent, &data, layout, font, REMOVE_PNG, TEXT_COL_REMOVE, tc(caption), ekDBUT_OK_CANCEL_DEF_CANCEL);
    str_destroy(&caption);
    i_remove_dialog_data(&data);

    if (ret != BUTTON_OK && ret != ekGUI_CLOSE_INTRO)
        return FALSE;
    else
        return TRUE;    
}

/*---------------------------------------------------------------------------*/

bool_t dialog_remove_row(Window *parent, const Font *font, const char_t *layout_name, const uint32_t row)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(1, 1);
    String *caption = NULL;
    uint32_t ret = 0;
    caption = str_printf(gui_text(TEXT_REMOVE_ROW), row, layout_name);
    ret = i_modal_launch(parent, &data, layout, font, REMOVE_PNG, TEXT_ROW_REMOVE, tc(caption), ekDBUT_OK_CANCEL_DEF_CANCEL);
    str_destroy(&caption);
    i_remove_dialog_data(&data);

    if (ret != BUTTON_OK && ret != ekGUI_CLOSE_INTRO)
        return FALSE;
    else
        return TRUE;    
}

/*---------------------------------------------------------------------------*/

uint8_t dialog_unsaved_changes(Window *parent, const Font *font, const char_t *caption)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(1, 1);
    uint32_t ret = 0;
    ret = i_modal_launch(parent, &data, layout, font, QUEST24_PNG, TEXT_UNSAVE_CHANGES, caption, ekDBUT_SAVE_OPTS);
    i_remove_dialog_data(&data);

    /* Save changes */
    if (ret == BUTTON_SAVE)
        return 1;
    /* Don't save changes */
    else if (ret == BUTTON_NO_SAVE)
        return 0;
    /* Cancel action */
    else
        return 2;
}

/*---------------------------------------------------------------------------*/

FLabel *dialog_new_label(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(2, 2);
    FLabel *flabel = flabel_create();
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label1 = label_create();
        Label *label2 = label_create();
        Edit *edit = edit_create();
        Button *check = button_check();
        label_text(label1, gui_text(TEXT_TEXT));
        label_text(label2, gui_text(TEXT_MULTILINE));
        button_text(check, "");
        layout_label(layout, label1, 0, 0);
        layout_label(layout, label2, 0, 1);
        layout_edit(layout, edit, 1, 0);
        layout_button(layout, check, 1, 1);
        layout_hmargin(layout, 0, 5);
        cell_dbind(layout_cell(layout, 1, 0), FLabel, String *, text);
        cell_dbind(layout_cell(layout, 1, 1), FLabel, bool_t, multiline);
        layout_dbind(layout, NULL, FLabel);
        layout_dbind_obj(layout, flabel, FLabel);
    }

    caption = str_printf(gui_text(TEXT_NEW_LABEL), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout, font, LABEL_PNG, TEXT_LABEL, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK && ret != ekGUI_CLOSE_INTRO)
        flabel_destroy(&flabel);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return flabel;
}

/*---------------------------------------------------------------------------*/

FButton *dialog_new_button(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(2, 1);
    FButton *fbutton = fbutton_create();
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label = label_create();
        Edit *edit = edit_create();
        label_text(label, gui_text(TEXT_TEXT));
        layout_label(layout, label, 0, 0);
        layout_edit(layout, edit, 1, 0);
        layout_hmargin(layout, 0, 5);
        cell_dbind(layout_cell(layout, 1, 0), FButton, String *, text);
        layout_dbind(layout, NULL, FButton);
        layout_dbind_obj(layout, fbutton, FButton);
    }

    caption = str_printf(gui_text(TEXT_NEW_BUTTON), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout, font, PUSHBUT_PNG, TEXT_PUSH_BUTTON, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK && ret != ekGUI_CLOSE_INTRO)
        fbutton_destroy(&fbutton);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fbutton;
}

/*---------------------------------------------------------------------------*/

FCheck *dialog_new_check(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(2, 1);
    FCheck *fcheck = fcheck_create();
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label = label_create();
        Edit *edit = edit_create();
        label_text(label, gui_text(TEXT_TEXT));
        layout_label(layout, label, 0, 0);
        layout_edit(layout, edit, 1, 0);
        layout_hmargin(layout, 0, 5);
        cell_dbind(layout_cell(layout, 1, 0), FCheck, String *, text);
        layout_dbind(layout, NULL, FCheck);
        layout_dbind_obj(layout, fcheck, FCheck);
    }

    caption = str_printf(gui_text(TEXT_NEW_CHECK), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout, font, CHECBUT_PNG, TEXT_CHECK_BOX, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        fcheck_destroy(&fcheck);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fcheck;
}

/*---------------------------------------------------------------------------*/

FRadio *dialog_new_radio(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(2, 1);
    FRadio *fradio = fradio_create();
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label = label_create();
        Edit *edit = edit_create();
        label_text(label, gui_text(TEXT_TEXT));
        layout_label(layout, label, 0, 0);
        layout_edit(layout, edit, 1, 0);
        layout_hmargin(layout, 0, 5);
        cell_dbind(layout_cell(layout, 1, 0), FRadio, String *, text);
        layout_dbind(layout, NULL, FRadio);
        layout_dbind_obj(layout, fradio, FRadio);
    }

    caption = str_printf(gui_text(TEXT_NEW_RADIO), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout, font, RADBUT_PNG, TEXT_RADIO_BUTTON, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        fradio_destroy(&fradio);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fradio;
}

/*---------------------------------------------------------------------------*/

static void i_OnLoadImage(DialogData *data, Event *e)
{
    const char_t *imgpath = NULL;
    cassert_no_null(data);
    cassert_no_null(data->path);
    imgpath = comwin_open_file(data->window, NULL, NULL, 0, data->path, NULL);
    if (imgpath != NULL)
    {
        Image *image = image_from_file(imgpath, NULL);
        if (image != NULL)
        {
            String **path = NULL;
            if (data->imageview != NULL)
            {
                cassert(data->toolbutton == NULL);
                imageview_image(data->imageview, image);
            }
            else
            {
                cassert(data->toolbutton != NULL);
                button_image(data->toolbutton, image);
            }

            if (data->ftool != NULL)
            {
                cassert(data->fimage == NULL);
                cassert(data->felem == NULL);
                path = &data->ftool->path;
            }
            else if (data->fimage != NULL)
            {
                cassert(data->felem == NULL);
                path = &data->fimage->path;
            }
            else
            {
                cassert_no_null(data->felem);
                path = &data->felem->iconpath;
            }

            str_destroy(path);
            *path = str_relpath(ekLINUX, data->path, imgpath);
            label_text(data->label, tc(*path));
            window_update(data->window);
            image_destroy(&image);
        }
    }
    unref(e);
}

/*---------------------------------------------------------------------------*/

FTool *dialog_new_tool(Window *parent, const Font *font, const DSelect *sel, const char_t *folder_path)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 2);
    FTool *ftool = ftool_create();
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    data.path = folder_path;
    data.ftool = ftool;

    /* Widget layout */
    {
        Layout *layout2 = layout_create(2, 1);
        Label *label1 = label_create();
        Label *label2 = label_create();
        Label *label3 = label_create();
        Button *button1 = button_flat();
        Button *button2 = button_push();
        const Image *image = nflib_default_icon();
        label_text(label1, gui_text(TEXT_TOOL_BUTTON));
        label_text(label2, gui_text(TEXT_ICON_PATH));
        label_text(label3, gui_text(TEXT_DEFAULT));
        /* label_ellipsis(label3, ekELLIPBEGIN); When NAppGUI supports */
        label_min_width(label3, 150);
        button_image(button1, image);
        button_text(button2, "...");
        button_tooltip(button2, gui_text(TEXT_LOAD_ICON));
        button_hpadding(button2, 20);
        button_OnClick(button2, listener(&data, i_OnLoadImage, DialogData));
        layout_label(layout1, label1, 0, 0);
        layout_label(layout1, label2, 0, 1);
        layout_button(layout2, button1, 0, 0);
        layout_button(layout2, button2, 1, 0);
        layout_layout(layout1, layout2, 1, 0);
        layout_label(layout1, label3, 1, 1);
        layout_tabstop(layout2, 0, 0, FALSE);
        layout_hmargin(layout1, 0, 5);
        layout_hmargin(layout2, 0, 5);
        layout_hexpand(layout1, 1);
        layout_halign(layout1, 1, 0, ekLEFT);
        data.label = label3;
        data.toolbutton = button1;
    }

    caption = str_printf(gui_text(TEXT_NEW_TOOL), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, TOOLBUT_PNG, TEXT_TOOL_BUTTON, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        ftool_destroy(&ftool);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return ftool;
}

/*---------------------------------------------------------------------------*/

FElem *dialog_new_elem(Window *parent, const Font *font, const char_t *caption, const ResId iconId, const ResId headerId, const char_t *folder_path)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 3);
    FElem *felem = dbind_create(FElem);
    uint32_t ret = 0;
    data.path = folder_path;
    data.felem = felem;

    /* Widget layout */
    {
        Layout *layout2 = layout_create(2, 1);
        Label *label1 = label_create();
        Label *label2 = label_create();
        Label *label3 = label_create();
        Label *label4 = label_create();
        Button *button1 = button_flat();
        Button *button2 = button_push();
        Edit *edit = edit_create();
        const Image *image = nflib_default_icon();
        label_text(label1, gui_text(TEXT_TEXT));
        label_text(label2, gui_text(TEXT_ICON));
        label_text(label3, gui_text(TEXT_ICON_PATH));
        label_text(label4, gui_text(TEXT_DEFAULT));
        /* label_ellipsis(label3, ekELLIPBEGIN); When NAppGUI supports */
        label_min_width(label4, 150);
        button_image(button1, image);
        button_text(button2, "...");
        button_tooltip(button2, gui_text(TEXT_LOAD_ICON));
        button_hpadding(button2, 20);
        button_OnClick(button2, listener(&data, i_OnLoadImage, DialogData));
        layout_label(layout1, label1, 0, 0);
        layout_label(layout1, label2, 0, 1);
        layout_label(layout1, label3, 0, 2);
        layout_edit(layout1, edit, 1, 0);
        layout_button(layout2, button1, 0, 0);
        layout_button(layout2, button2, 1, 0);
        layout_layout(layout1, layout2, 1, 1);
        layout_label(layout1, label4, 1, 2);
        layout_tabstop(layout2, 0, 0, FALSE);
        layout_hmargin(layout1, 0, 5);
        layout_hmargin(layout2, 0, 5);
        layout_hexpand(layout1, 1);
        layout_halign(layout1, 1, 1, ekLEFT);
        cell_dbind(layout_cell(layout1, 1, 0), FElem, String *, text);
        layout_dbind(layout1, NULL, FElem);
        layout_dbind_obj(layout1, felem, FElem);
        data.label = label4;
        data.toolbutton = button1;
    }

    ret = i_modal_launch(parent, &data, layout1, font, iconId, headerId, caption, ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        dbind_destroy(&felem, FElem);

    i_remove_dialog_data(&data);
    return felem;
}

/*---------------------------------------------------------------------------*/

FPopUp *dialog_new_popup(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout = layout_create(1, 1);
    FPopUp *fpopup = fpopup_create();
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    caption = str_printf(gui_text(TEXT_NEW_POPUP), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout, font, POPUP_PNG, TEXT_POPUP_BUTTON, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        fpopup_destroy(&fpopup);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fpopup;
}

/*---------------------------------------------------------------------------*/

static Layout *i_value_updown_layout(void)
{
    Layout *layout = layout_create(2, 1);
    Edit *edit = edit_create();
    UpDown *updown = updown_create();
    edit_align(edit, ekRIGHT);
    layout_edit(layout, edit, 0, 0);
    layout_updown(layout, updown, 1, 0);
    layout_hexpand(layout, 0);
    return layout;
}

/*---------------------------------------------------------------------------*/

FEdit *dialog_new_edit(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 4);
    FEdit *fedit = fedit_create();
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label1 = label_create();
        Label *label2 = label_create();
        Label *label3 = label_create();
        Label *label4 = label_create();        
        PopUp *popup = popup_create();
        Button *button1 = button_check();
        Button *button2 = button_check();
        Layout *layout2 = i_value_updown_layout();
        label_text(label1, gui_text(TEXT_TEXT_ALIGN));
        label_text(label2, gui_text(TEXT_MIN_WIDTH));
        label_text(label3, gui_text(TEXT_PASSMODE));
        label_text(label4, gui_text(TEXT_AUTOSELECT));
        layout_label(layout1, label1, 0, 0);
        layout_label(layout1, label2, 0, 1);
        layout_label(layout1, label3, 0, 2);
        layout_label(layout1, label4, 0, 3);
        layout_popup(layout1, popup, 1, 0);
        layout_layout(layout1, layout2, 1, 1);
        layout_button(layout1, button1, 1, 2);
        layout_button(layout1, button2, 1, 3);
        layout_hmargin(layout1, 0, 5);
        cell_dbind(layout_cell(layout1, 1, 0), FEdit, halign_t, text_align);
        cell_dbind(layout_cell(layout1, 1, 1), FEdit, real32_t, min_width);
        cell_dbind(layout_cell(layout1, 1, 2), FEdit, bool_t, passmode);
        cell_dbind(layout_cell(layout1, 1, 3), FEdit, bool_t, autosel);
        layout_dbind(layout1, NULL, FEdit);
        layout_dbind_obj(layout1, fedit, FEdit);
    }

    caption = str_printf(gui_text(TEXT_NEW_EDIT), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, EDITBOX_PNG, TEXT_EDIT_BOX, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        fedit_destroy(&fedit);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fedit;
}

/*---------------------------------------------------------------------------*/

FCombo *dialog_new_combo(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 4);
    FCombo *fcombo = fcombo_create();    
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label1 = label_create();
        Label *label2 = label_create();
        Label *label3 = label_create();
        Label *label4 = label_create();        
        PopUp *popup = popup_create();
        Button *button1 = button_check();
        Button *button2 = button_check();
        Layout *layout2 = i_value_updown_layout();
        label_text(label1, gui_text(TEXT_TEXT_ALIGN));
        label_text(label2, gui_text(TEXT_MIN_WIDTH));
        label_text(label3, gui_text(TEXT_PASSMODE));
        label_text(label4, gui_text(TEXT_AUTOSELECT));
        layout_label(layout1, label1, 0, 0);
        layout_label(layout1, label2, 0, 1);
        layout_label(layout1, label3, 0, 2);
        layout_label(layout1, label4, 0, 3);
        layout_popup(layout1, popup, 1, 0);
        layout_layout(layout1, layout2, 1, 1);
        layout_button(layout1, button1, 1, 2);
        layout_button(layout1, button2, 1, 3);
        layout_hmargin(layout1, 0, 5);
        cell_dbind(layout_cell(layout1, 1, 0), FCombo, halign_t, text_align);
        cell_dbind(layout_cell(layout1, 1, 1), FCombo, real32_t, min_width);
        cell_dbind(layout_cell(layout1, 1, 2), FCombo, bool_t, passmode);
        cell_dbind(layout_cell(layout1, 1, 3), FCombo, bool_t, autosel);
        layout_dbind(layout1, NULL, FCombo);
        layout_dbind_obj(layout1, fcombo, FCombo);
    }

    caption = str_printf(gui_text(TEXT_NEW_COMBO), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, COMBOBOX_PNG, TEXT_COMBO_BOX, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        fcombo_destroy(&fcombo);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fcombo;
}

/*---------------------------------------------------------------------------*/

FListBox *dialog_new_listbox(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 2);
    FListBox *flistbox = flistbox_create();    
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label1 = label_create();
        Label *label2 = label_create();
        Layout *layout2 = i_value_updown_layout();
        Layout *layout3 = i_value_updown_layout();
        label_text(label1, gui_text(TEXT_MIN_WIDTH));
        label_text(label2, gui_text(TEXT_MIN_HEIGHT));
        layout_label(layout1, label1, 0, 0);
        layout_label(layout1, label2, 0, 1);
        layout_layout(layout1, layout2, 1, 0);
        layout_layout(layout1, layout3, 1, 1);
        layout_hmargin(layout1, 0, 5);
        cell_dbind(layout_cell(layout1, 1, 0), FListBox, real32_t, min_width);
        cell_dbind(layout_cell(layout1, 1, 1), FListBox, real32_t, min_height);
        layout_dbind(layout1, NULL, FListBox);
        layout_dbind_obj(layout1, flistbox, FListBox);
    }

    caption = str_printf(gui_text(TEXT_NEW_LISTBOX), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, LISTVIEW_PNG, TEXT_LIST_BOX, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        flistbox_destroy(&flistbox);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return flistbox;
}

/*---------------------------------------------------------------------------*/

FSlider *dialog_new_slider(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 1);
    FSlider *fslider = fslider_create();    
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label1 = label_create();
        Layout *layout2 = i_value_updown_layout();
        label_text(label1, gui_text(TEXT_MIN_WIDTH));
        layout_label(layout1, label1, 0, 0);
        layout_layout(layout1, layout2, 1, 0);
        layout_hmargin(layout1, 0, 5);
        cell_dbind(layout_cell(layout1, 1, 0), FSlider, real32_t, min_width);
        layout_dbind(layout1, NULL, FSlider);
        layout_dbind_obj(layout1, fslider, FSlider);
    }

    caption = str_printf(gui_text(TEXT_NEW_SLIDER), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, HORSLIDER_PNG, TEXT_HORZ_SLIDER, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        fslider_destroy(&fslider);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fslider;
}

/*---------------------------------------------------------------------------*/

FVSlider *dialog_new_vslider(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 1);
    FVSlider *fvslider = fvslider_create();    
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label1 = label_create();
        Layout *layout2 = i_value_updown_layout();
        label_text(label1, gui_text(TEXT_MIN_HEIGHT));
        layout_label(layout1, label1, 0, 0);
        layout_layout(layout1, layout2, 1, 0);
        layout_hmargin(layout1, 0, 5);
        cell_dbind(layout_cell(layout1, 1, 0), FVSlider, real32_t, min_height);
        layout_dbind(layout1, NULL, FVSlider);
        layout_dbind_obj(layout1, fvslider, FVSlider);
    }

    caption = str_printf(gui_text(TEXT_NEW_VSLIDER), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, VERSLIDER_PNG, TEXT_VERT_SLIDER, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        fvslider_destroy(&fvslider);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fvslider;
}

/*---------------------------------------------------------------------------*/

FProgress *dialog_new_progress(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 1);
    FProgress *fprogress = fprogress_create();    
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label1 = label_create();
        Layout *layout2 = i_value_updown_layout();
        label_text(label1, gui_text(TEXT_MIN_WIDTH));
        layout_label(layout1, label1, 0, 0);
        layout_layout(layout1, layout2, 1, 0);
        layout_hmargin(layout1, 0, 5);
        cell_dbind(layout_cell(layout1, 1, 0), FProgress, real32_t, min_width);
        layout_dbind(layout1, NULL, FProgress);
        layout_dbind_obj(layout1, fprogress, FProgress);
    }

    caption = str_printf(gui_text(TEXT_NEW_PROGRESS), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, PROGRESSBAR_PNG, TEXT_PROGRESS_BAR, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        fprogress_destroy(&fprogress);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fprogress;
}

/*---------------------------------------------------------------------------*/

FView *dialog_new_view(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 2);
    FView *fview = fview_create();    
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label1 = label_create();
        Label *label2 = label_create();
        Layout *layout2 = i_value_updown_layout();
        Layout *layout3 = i_value_updown_layout();
        label_text(label1, gui_text(TEXT_MIN_WIDTH));
        label_text(label2, gui_text(TEXT_MIN_HEIGHT));
        layout_label(layout1, label1, 0, 0);
        layout_label(layout1, label2, 0, 1);
        layout_layout(layout1, layout2, 1, 0);
        layout_layout(layout1, layout3, 1, 1);
        layout_hmargin(layout1, 0, 5);
        cell_dbind(layout_cell(layout1, 1, 0), FView, real32_t, min_width);
        cell_dbind(layout_cell(layout1, 1, 1), FView, real32_t, min_height);
        layout_dbind(layout1, NULL, FView);
        layout_dbind_obj(layout1, fview, FView);
    }

    caption = str_printf(gui_text(TEXT_NEW_CUSTOMVIEW), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, VIEW_PNG, TEXT_CUSTOM_VIEW, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        fview_destroy(&fview);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fview;
}

/*---------------------------------------------------------------------------*/

FSView *dialog_new_sview(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 2);
    FSView *fsview = fsview_create();    
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label1 = label_create();
        Label *label2 = label_create();
        Layout *layout2 = i_value_updown_layout();
        Layout *layout3 = i_value_updown_layout();
        label_text(label1, gui_text(TEXT_MIN_WIDTH));
        label_text(label2, gui_text(TEXT_MIN_HEIGHT));
        layout_label(layout1, label1, 0, 0);
        layout_label(layout1, label2, 0, 1);
        layout_layout(layout1, layout2, 1, 0);
        layout_layout(layout1, layout3, 1, 1);
        layout_hmargin(layout1, 0, 5);
        cell_dbind(layout_cell(layout1, 1, 0), FSView, real32_t, min_width);
        cell_dbind(layout_cell(layout1, 1, 1), FSView, real32_t, min_height);
        layout_dbind(layout1, NULL, FSView);
        layout_dbind_obj(layout1, fsview, FSView);
    }

    caption = str_printf(gui_text(TEXT_NEW_SCROLLVIEW), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, SVIEW_PNG, TEXT_SCROLL_VIEW, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        fsview_destroy(&fsview);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fsview;
}

/*---------------------------------------------------------------------------*/

FText *dialog_new_text(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 2);
    FText *ftext = ftext_create();    
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label1 = label_create();
        Label *label2 = label_create();
        Layout *layout2 = i_value_updown_layout();
        Layout *layout3 = i_value_updown_layout();
        label_text(label1, gui_text(TEXT_MIN_WIDTH));
        label_text(label2, gui_text(TEXT_MIN_HEIGHT));
        layout_label(layout1, label1, 0, 0);
        layout_label(layout1, label2, 0, 1);
        layout_layout(layout1, layout2, 1, 0);
        layout_layout(layout1, layout3, 1, 1);
        layout_hmargin(layout1, 0, 5);
        cell_dbind(layout_cell(layout1, 1, 0), FText, real32_t, min_width);
        cell_dbind(layout_cell(layout1, 1, 1), FText, real32_t, min_height);
        layout_dbind(layout1, NULL, FText);
        layout_dbind_obj(layout1, ftext, FText);
    }

    caption = str_printf(gui_text(TEXT_NEW_TEXTVIEW), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, TEXTVIEW_PNG, TEXT_TEXT_VIEW, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        ftext_destroy(&ftext);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return ftext;
}

/*---------------------------------------------------------------------------*/

FImage *dialog_new_image(Window *parent, const Font *font, const DSelect *sel, const char_t *folder_path)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 4);
    FImage *fimage = fimage_create();
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    data.path = folder_path;
    data.fimage = fimage;

    /* Widget layout */
    {
        Label *label1 = label_create();
        Label *label2 = label_create();
        Label *label3 = label_create();
        Label *label4 = label_create();
        Label *label5 = label_create();
        Layout *layout2 = i_value_updown_layout();
        Layout *layout3 = i_value_updown_layout();
        Layout *layout4 = layout_create(3, 1);
        Button *button = button_push();
        ImageView *view = imageview_create();
        const Image *image = nflib_default_image();
        label_text(label1, gui_text(TEXT_MIN_WIDTH));
        label_text(label2, gui_text(TEXT_MIN_HEIGHT));
        label_text(label3, gui_text(TEXT_PREV_IMAGE));
        label_text(label4, gui_text(TEXT_IMAGE_PATH));
        label_text(label5, gui_text(TEXT_DEFAULT));
        /* label_ellipsis(label5, ekELLIPBEGIN); When NAppGUI supports */
        label_min_width(label5, 150);
        button_text(button, "...");
        button_tooltip(button, gui_text(TEXT_LOAD_IMAGE));
        button_hpadding(button, 20);
        button_OnClick(button, listener(&data, i_OnLoadImage, DialogData));
        imageview_image(view, image);
        imageview_scale(view, ekGUI_SCALE_ASPECT);
        imageview_size(view, s2df(128, 96));
        layout_label(layout1, label1, 0, 0);
        layout_label(layout1, label2, 0, 1);
        layout_label(layout1, label3, 0, 2);
        layout_label(layout1, label4, 0, 3);
        layout_layout(layout1, layout2, 1, 0);
        layout_layout(layout1, layout3, 1, 1);
        layout_imageview(layout4, view, 0, 0);
        layout_button(layout4, button, 2, 0);
        layout_layout(layout1, layout4, 1, 2);
        layout_label(layout1, label5, 1, 3);
        layout_hexpand(layout4, 1);
        layout_hmargin(layout1, 0, 5);
        layout_hmargin(layout4, 0, 5);
        layout_vmargin(layout1, 1, 5);
        layout_vmargin(layout1, 2, 5);
        layout_halign(layout1, 1, 2, ekLEFT);
        cell_dbind(layout_cell(layout1, 1, 0), FImage, real32_t, min_width);
        cell_dbind(layout_cell(layout1, 1, 1), FImage, real32_t, min_height);
        layout_dbind(layout1, NULL, FImage);
        layout_dbind_obj(layout1, fimage, FImage);
        data.label = label5;
        data.imageview = view;
    }

    caption = str_printf(gui_text(TEXT_NEW_IMAGEVIEW), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, IMAGEVIEW_PNG, TEXT_IMAGE_VIEW, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        fimage_destroy(&fimage);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fimage;
}

/*---------------------------------------------------------------------------*/

FTable *dialog_new_table(Window *parent, const Font *font, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(2, 2);
    FTable *ftable = ftable_create();    
    String *caption = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);

    /* Widget layout */
    {
        Label *label1 = label_create();
        Label *label2 = label_create();
        Layout *layout2 = i_value_updown_layout();
        Layout *layout3 = i_value_updown_layout();
        label_text(label1, gui_text(TEXT_MIN_WIDTH));
        label_text(label2, gui_text(TEXT_MIN_HEIGHT));
        layout_label(layout1, label1, 0, 0);
        layout_label(layout1, label2, 0, 1);
        layout_layout(layout1, layout2, 1, 0);
        layout_layout(layout1, layout3, 1, 1);
        layout_hmargin(layout1, 0, 5);
        cell_dbind(layout_cell(layout1, 1, 0), FTable, real32_t, min_width);
        cell_dbind(layout_cell(layout1, 1, 1), FTable, real32_t, min_height);
        layout_dbind(layout1, NULL, FTable);
        layout_dbind_obj(layout1, ftable, FTable);
    }

    caption = str_printf(gui_text(TEXT_NEW_TABLEVIEW), sel->col, sel->row, tc(sel->flayout->name));
    ret = i_modal_launch(parent, &data, layout1, font, TABLEVIEW_PNG, TEXT_TABLE_VIEW, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret != BUTTON_OK)
        ftable_destroy(&ftable);

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return ftable;
}

/*---------------------------------------------------------------------------*/

static Layout *i_vertical_layout(void)
{
    Layout *layout = layout_create(3, 1);
    Label *label1 = label_create();
    Edit *edit1 = edit_create();
    UpDown *updown1 = updown_create();
    edit_align(edit1, ekRIGHT);
    label_text(label1, gui_text(TEXT_ROWS));
    layout_label(layout, label1, 0, 0);
    layout_edit(layout, edit1, 1, 0);
    layout_updown(layout, updown1, 2, 0);
    layout_hmargin(layout, 0, i_LABEL_MARGIN);
    layout_hexpand(layout, 1);
    cell_dbind(layout_cell(layout, 1, 0), DialogLayout, uint32_t, nrows);
    cell_dbind(layout_cell(layout, 2, 0), DialogLayout, uint32_t, nrows);
    layout_dbind(layout, NULL, DialogLayout);
    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_horizontal_layout(void)
{
    Layout *layout = layout_create(3, 1);
    Label *label1 = label_create();
    Edit *edit1 = edit_create();
    UpDown *updown1 = updown_create();
    edit_align(edit1, ekRIGHT);
    label_text(label1, gui_text(TEXT_COLUMNS));
    layout_label(layout, label1, 0, 0);
    layout_edit(layout, edit1, 1, 0);
    layout_updown(layout, updown1, 2, 0);
    layout_hmargin(layout, 0, i_LABEL_MARGIN);
    layout_hexpand(layout, 1);
    cell_dbind(layout_cell(layout, 1, 0), DialogLayout, uint32_t, ncols);
    cell_dbind(layout_cell(layout, 2, 0), DialogLayout, uint32_t, ncols);
    layout_dbind(layout, NULL, DialogLayout);
    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_grid_layout(void)
{
    Layout *layout = layout_create(3, 2);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Edit *edit1 = edit_create();
    Edit *edit2 = edit_create();
    UpDown *updown1 = updown_create();
    UpDown *updown2 = updown_create();
    edit_align(edit1, ekRIGHT);
    edit_align(edit2, ekRIGHT);
    label_text(label1, gui_text(TEXT_COLUMNS));
    label_text(label2, gui_text(TEXT_ROWS));
    layout_label(layout, label1, 0, 0);
    layout_label(layout, label2, 0, 1);
    layout_edit(layout, edit1, 1, 0);
    layout_edit(layout, edit2, 1, 1);
    layout_updown(layout, updown1, 2, 0);
    layout_updown(layout, updown2, 2, 1);
    layout_hmargin(layout, 0, i_LABEL_MARGIN);
    layout_hexpand(layout, 1);
    cell_dbind(layout_cell(layout, 1, 0), DialogLayout, uint32_t, ncols);
    cell_dbind(layout_cell(layout, 2, 0), DialogLayout, uint32_t, ncols);
    cell_dbind(layout_cell(layout, 1, 1), DialogLayout, uint32_t, nrows);
    cell_dbind(layout_cell(layout, 2, 1), DialogLayout, uint32_t, nrows);
    layout_dbind(layout, NULL, DialogLayout);
    return layout;
}

/*---------------------------------------------------------------------------*/

static FLayout *i_dialog_new_layout(Window *parent, const Font *font, const DSelect *sel, Layout *grid_layout, const ResId icon_id, const ResId header_id, DialogLayout *diag)
{
    DialogData data = i_dialog_data();
    String *caption = NULL;
    FLayout *flayout = NULL;
    uint32_t ret = 0;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    cassert_no_null(diag);
    caption = str_printf(gui_text(TEXT_NEW_SUBLAYOUT), sel->col, sel->row, tc(sel->flayout->name));
    layout_dbind_obj(grid_layout, diag, DialogLayout);
    ret = i_modal_launch(parent, &data, grid_layout, font, icon_id, header_id, tc(caption), ekDBUT_OK_CANCEL_DEF_OK);

    if (ret == BUTTON_OK || ret == ekGUI_CLOSE_INTRO)
    {
        cassert(diag->ncols > 0);
        cassert(diag->nrows > 0);
        flayout = flayout_create(diag->ncols, diag->nrows);
    }

    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return flayout;
}

/*---------------------------------------------------------------------------*/

FLayout *dialog_vertical_layout(Window *parent, const Font *font, const DSelect *sel)
{
    DialogLayout diag = {1, 3};
    return i_dialog_new_layout(parent, font, sel, i_vertical_layout(), VLAYOUT_PNG, TEXT_VERT_LAYOUT, &diag);
}

/*---------------------------------------------------------------------------*/

FLayout* dialog_horizontal_layout(Window* parent, const Font* font, const DSelect* sel)
{
    DialogLayout diag = {3, 1};
    return i_dialog_new_layout(parent, font, sel, i_horizontal_layout(), HLAYOUT_PNG, TEXT_HORZ_LAYOUT, &diag);
}

/*---------------------------------------------------------------------------*/

FLayout *dialog_grid_layout(Window *parent, const Font *font, const DSelect *sel)
{
    DialogLayout diag = {3, 2};
    return i_dialog_new_layout(parent, font, sel, i_grid_layout(), GLAYOUT_PNG, TEXT_GRID_LAYOUT, &diag);
}

