/* Dialog boxes */

#include "dialogs.h"
#include "dgui.h"
#include "res_designer.h"
#include <nflib/nflib.h>
#include <nflib/flayout.h>
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
#include <core/dbind.h>
#include <core/event.h>
#include <core/strings.h>
#include <sewer/cassert.h>
#include <sewer/bmem.h>

typedef struct _dialoglayout_t DialogLayout;
typedef struct _dialogdata_t DialogData;

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
    Button *defbutton;
    Label *label;
    ImageView *imgview;
    Window *window;
    Edit *edit;
};

#define BUTTON_OK 1000
#define BUTTON_CANCEL 1001
#define BUTTON_SAVE 1002
#define BUTTON_NO_SAVE 1003
#define BUTTON_HEADER_CLOSE 1004

/*---------------------------------------------------------------------------*/

void dialog_dbind(void)
{
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
    button_min_width(button1, 70);
    button_min_width(button2, 80);
    layout_button(layout, button1, 1, 0);
    layout_button(layout, button2, 2, 0);
    layout_hexpand(layout, 0);

    if (ok_default == TRUE)
        data->defbutton = button1;
    else
        data->defbutton = button2;

    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_ok(DialogData *data)
{
    Layout *layout = layout_create(1, 1);
    Button *button1 = button_push();
    cassert_no_null(data);
    button_text(button1, "Ok");
    button_tag(button1, BUTTON_OK);
    button_OnClick(button1, listener(data, i_OnClick, DialogData));
    layout_button(layout, button1, 0, 0);
    data->defbutton = button1;
    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_save_buttons(DialogData *data)
{
    Layout *layout = layout_create(3, 1);
    Button *button1 = button_push();
    Button *button2 = button_push();
    Button *button3 = button_push();
    cassert_no_null(data);
    button_text(button1, "Save changes");
    button_text(button2, "NO save changes");
    button_text(button3, "Cancel");
    button_tag(button1, BUTTON_SAVE);
    button_tag(button2, BUTTON_NO_SAVE);
    button_tag(button3, BUTTON_CANCEL);
    button_OnClick(button1, listener(data, i_OnClick, DialogData));
    button_OnClick(button2, listener(data, i_OnClick, DialogData));
    button_OnClick(button3, listener(data, i_OnClick, DialogData));
    layout_button(layout, button1, 0, 0);
    layout_button(layout, button2, 1, 0);
    layout_button(layout, button3, 2, 0);
    data->defbutton = button1;
    return layout;
}

/*---------------------------------------------------------------------------*/

String *dialog_form_name(Window *parent, const char_t *name)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 1);
    Layout *layout3 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Edit *edit = edit_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    uint32_t ret = 0;
    String *fname = NULL;
    data.window = window;
    data.edit = edit;
    if (str_empty_c(name) == TRUE)
    {
        caption = str_c("Set name for new form");
    }
    else
    {
        caption = str_c("Change the name of the form");
        edit_text(edit, name);
    }
    label_text(label1, tc(caption));
    label_text(label2, "Name:");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_edit(layout2, edit, 1, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_layout(layout1, layout3, 0, 2);
    layout_vmargin(layout1, 0, 5);
    layout_vmargin(layout1, 1, 5);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret == BUTTON_OK)
        fname = str_c(edit_get_text(data.edit));
    else
        fname = str_c("");

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fname;
}

/*---------------------------------------------------------------------------*/

FLabel *dialog_new_label(Window *parent, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 4);
    Layout *layout2 = layout_create(2, 1);
    Layout *layout3 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Button *check = button_check();
    Edit *edit = edit_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FLabel *flabel = dbind_create(FLabel);
    uint32_t ret = 0;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    caption = str_printf("New Label widget in (%d, %d) of '%s'", sel->col, sel->row, tc(sel->flayout->name));
    label_text(label1, tc(caption));
    label_text(label2, "Text:");
    button_text(check, "Multiline");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_edit(layout2, edit, 1, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_button(layout1, check, 0, 2);
    layout_layout(layout1, layout3, 0, 3);
    layout_vmargin(layout1, 0, 5);
    layout_vmargin(layout1, 1, 5);
    panel_layout(panel, layout1);
    cell_dbind(layout_cell(layout2, 1, 0), FLabel, String *, text);
    cell_dbind(layout_cell(layout1, 0, 2), FLabel, bool_t, multiline);
    layout_dbind(layout1, NULL, FLabel);
    layout_dbind_obj(layout1, flabel, FLabel);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret != BUTTON_OK)
        dbind_destroy(&flabel, FLabel);

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return flabel;
}

/*---------------------------------------------------------------------------*/

FButton *dialog_new_button(Window *parent, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 1);
    Layout *layout3 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Edit *edit = edit_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FButton *fbutton = dbind_create(FButton);
    uint32_t ret = 0;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    caption = str_printf("New Button widget in (%d, %d) of '%s'", sel->col, sel->row, tc(sel->flayout->name));
    label_text(label1, tc(caption));
    label_text(label2, "Text:");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_edit(layout2, edit, 1, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_layout(layout1, layout3, 0, 2);
    layout_vmargin(layout1, 0, 5);
    layout_vmargin(layout1, 1, 5);
    panel_layout(panel, layout1);
    cell_dbind(layout_cell(layout2, 1, 0), FButton, String *, text);
    layout_dbind(layout1, NULL, FButton);
    layout_dbind_obj(layout1, fbutton, FButton);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret != BUTTON_OK)
        dbind_destroy(&fbutton, FButton);

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fbutton;
}

/*---------------------------------------------------------------------------*/

FCheck *dialog_new_check(Window *parent, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 1);
    Layout *layout3 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Edit *edit = edit_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FCheck *fcheck = dbind_create(FCheck);
    uint32_t ret = 0;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    caption = str_printf("New Checkbox widget in (%d, %d) of '%s'", sel->col, sel->row, tc(sel->flayout->name));
    label_text(label1, tc(caption));
    label_text(label2, "Text:");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_edit(layout2, edit, 1, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_layout(layout1, layout3, 0, 2);
    layout_vmargin(layout1, 0, 5);
    layout_vmargin(layout1, 1, 5);
    panel_layout(panel, layout1);
    cell_dbind(layout_cell(layout2, 1, 0), FCheck, String *, text);
    layout_dbind(layout1, NULL, FCheck);
    layout_dbind_obj(layout1, fcheck, FCheck);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret != BUTTON_OK)
        dbind_destroy(&fcheck, FCheck);

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fcheck;
}

/*---------------------------------------------------------------------------*/

FEdit *dialog_new_edit(Window *parent, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 5);
    Layout *layout2 = layout_create(2, 1);
    Layout *layout3 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Button *button1 = button_check();
    Button *button2 = button_check();
    PopUp *popup = popup_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FEdit *fedit = dbind_create(FEdit);
    uint32_t ret = 0;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    caption = str_printf("New Editbox widget in (%d, %d) of '%s'", sel->col, sel->row, tc(sel->flayout->name));
    label_text(label1, tc(caption));
    label_text(label2, "Text align");
    button_text(button1, "Passmode");
    button_text(button2, "Autosel");
    layout_label(layout1, label1, 0, 0);
    layout_button(layout1, button1, 0, 1);
    layout_button(layout1, button2, 0, 2);
    layout_label(layout2, label2, 0, 0);
    layout_popup(layout2, popup, 1, 0);
    layout_layout(layout1, layout2, 0, 3);
    layout_layout(layout1, layout3, 0, 4);
    layout_vmargin(layout1, 0, 5);
    layout_vmargin(layout1, 1, 5);
    panel_layout(panel, layout1);
    cell_dbind(layout_cell(layout1, 0, 1), FEdit, bool_t, passmode);
    cell_dbind(layout_cell(layout1, 0, 2), FEdit, bool_t, autosel);
    cell_dbind(layout_cell(layout2, 1, 0), FEdit, halign_t, text_align);
    layout_dbind(layout1, NULL, FEdit);
    layout_dbind_obj(layout1, fedit, FEdit);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret != BUTTON_OK)
        dbind_destroy(&fedit, FEdit);

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fedit;
}

/*---------------------------------------------------------------------------*/

static Layout *i_value_updown_layout(void)
{
    Layout *layout = layout_create(2, 1);
    Edit *edit = edit_create();
    UpDown *updown = updown_create();
    layout_edit(layout, edit, 0, 0);
    layout_updown(layout, updown, 1, 0);
    layout_hexpand(layout, 0);
    return layout;
}

/*---------------------------------------------------------------------------*/

FText *dialog_new_text(Window *parent, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 2);
    Layout *layout3 = i_value_updown_layout();
    Layout *layout4 = i_value_updown_layout();
    Layout *layout5 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FText *ftext = dbind_create(FText);
    uint32_t ret = 0;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    caption = str_printf("New TextView widget in (%d, %d) of '%s'", sel->col, sel->row, tc(sel->flayout->name));
    label_text(label1, tc(caption));
    label_text(label2, "Min width");
    label_text(label3, "Min height");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_label(layout2, label3, 0, 1);
    layout_layout(layout2, layout3, 1, 0);
    layout_layout(layout2, layout4, 1, 1);
    layout_layout(layout1, layout2, 0, 1);
    layout_layout(layout1, layout5, 0, 2);
    layout_vmargin(layout1, 0, 5);
    layout_vmargin(layout1, 0, 5);
    panel_layout(panel, layout1);
    cell_dbind(layout_cell(layout2, 1, 0), FText, real32_t, min_width);
    cell_dbind(layout_cell(layout2, 1, 1), FText, real32_t, min_height);
    layout_dbind(layout1, NULL, FText);
    layout_dbind_obj(layout1, ftext, FText);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret != BUTTON_OK)
        dbind_destroy(&ftext, FText);

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return ftext;
}

/*---------------------------------------------------------------------------*/

static void i_OnLoadImage(DialogData *data, Event *e)
{
    const char_t *imgpath = NULL;
    cassert_no_null(data);
    cassert_no_null(data->path);
    imgpath = comwin_open_file(data->window, NULL, 0, data->path);
    if (imgpath != NULL)
    {
        Image *image = image_from_file(imgpath, NULL);
        if (image != NULL)
        {
            String **path = NULL;
            imageview_image(data->imgview, image);
            if (data->fimage != NULL)
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

static Layout *i_image_layout(DialogData *data)
{
    Layout *layout = layout_create(1, 3);
    ImageView *view = imageview_create();
    Label *label = label_create();
    Button *button = button_push();
    const Image *image = nflib_default_image();
    cassert_no_null(data);
    imageview_image(view, image);
    imageview_scale(view, ekGUI_SCALE_ASPECT);
    label_text(label, "default");
    button_text(button, "Load image");
    button_OnClick(button, listener(data, i_OnLoadImage, DialogData));
    label_align(label, ekCENTER);
    layout_imageview(layout, view, 0, 0);
    layout_label(layout, label, 0, 1);
    layout_button(layout, button, 0, 2);
    layout_halign(layout, 0, 1, ekJUSTIFY);
    data->label = label;
    data->imgview = view;
    return layout;
}

/*---------------------------------------------------------------------------*/

FImage *dialog_new_image(Window *parent, const DSelect *sel, const char_t *folder_path)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 4);
    Layout *layout2 = layout_create(2, 2);
    Layout *layout3 = i_value_updown_layout();
    Layout *layout4 = i_value_updown_layout();
    Layout *layout5 = i_image_layout(&data);
    Layout *layout6 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FImage *fimage = dbind_create(FImage);
    uint32_t ret = 0;
    data.path = folder_path;
    data.fimage = fimage;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    caption = str_printf("New ImageView widget in (%d, %d) of '%s'", sel->col, sel->row, tc(sel->flayout->name));
    label_text(label1, tc(caption));
    label_text(label2, "Min width");
    label_text(label3, "Min height");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_label(layout2, label3, 0, 1);
    layout_layout(layout2, layout3, 1, 0);
    layout_layout(layout2, layout4, 1, 1);
    layout_layout(layout1, layout2, 0, 1);
    layout_layout(layout1, layout5, 0, 2);
    layout_layout(layout1, layout6, 0, 3);
    layout_vmargin(layout1, 0, 5);
    layout_vmargin(layout1, 0, 5);
    panel_layout(panel, layout1);
    cell_dbind(layout_cell(layout2, 1, 0), FImage, real32_t, min_width);
    cell_dbind(layout_cell(layout2, 1, 1), FImage, real32_t, min_height);
    layout_dbind(layout1, NULL, FImage);
    layout_dbind_obj(layout1, fimage, FImage);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret != BUTTON_OK)
        dbind_destroy(&fimage, FImage);

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fimage;
}

/*---------------------------------------------------------------------------*/

FSlider *dialog_new_slider(Window *parent, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 2);
    Layout *layout2 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FSlider *fslider = dbind_create(FSlider);
    uint32_t ret = 0;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    caption = str_printf("New Slider widget in (%d, %d) of '%s'", sel->col, sel->row, tc(sel->flayout->name));
    label_text(label1, tc(caption));
    layout_label(layout1, label1, 0, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_vmargin(layout1, 0, 5);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret != BUTTON_OK)
        dbind_destroy(&fslider, FSlider);

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fslider;
}

/*---------------------------------------------------------------------------*/

FProgress *dialog_new_progress(Window *parent, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 2);
    Layout *layout2 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FProgress *fprogress = dbind_create(FProgress);
    uint32_t ret = 0;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    caption = str_printf("New Progress widget in (%d, %d) of '%s'", sel->col, sel->row, tc(sel->flayout->name));
    label_text(label1, tc(caption));
    layout_label(layout1, label1, 0, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_vmargin(layout1, 0, 5);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret != BUTTON_OK)
        dbind_destroy(&fprogress, FProgress);

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fprogress;
}

/*---------------------------------------------------------------------------*/

FPopUp *dialog_new_popup(Window *parent, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 2);
    Layout *layout2 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FPopUp *fpopup = dbind_create(FPopUp);
    uint32_t ret = 0;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    caption = str_printf("New Popup widget in (%d, %d) of '%s'", sel->col, sel->row, tc(sel->flayout->name));
    label_text(label1, tc(caption));
    layout_label(layout1, label1, 0, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_vmargin(layout1, 0, 5);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret != BUTTON_OK)
        dbind_destroy(&fpopup, FPopUp);

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return fpopup;
}

/*---------------------------------------------------------------------------*/

FListBox *dialog_new_listbox(Window *parent, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 2);
    Layout *layout3 = i_value_updown_layout();
    Layout *layout4 = i_value_updown_layout();
    Layout *layout5 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FListBox *flistbox = dbind_create(FListBox);
    uint32_t ret = 0;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    caption = str_printf("New ListBox widget in (%d, %d) of '%s'", sel->col, sel->row, tc(sel->flayout->name));
    label_text(label1, tc(caption));
    label_text(label2, "Min width");
    label_text(label3, "Min height");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_label(layout2, label3, 0, 1);
    layout_layout(layout2, layout3, 1, 0);
    layout_layout(layout2, layout4, 1, 1);
    layout_layout(layout1, layout2, 0, 1);
    layout_layout(layout1, layout5, 0, 2);
    layout_vmargin(layout1, 0, 5);
    layout_vmargin(layout1, 1, 5);
    cell_dbind(layout_cell(layout2, 1, 0), FListBox, real32_t, min_width);
    cell_dbind(layout_cell(layout2, 1, 1), FListBox, real32_t, min_height);
    layout_dbind(layout1, NULL, FListBox);
    layout_dbind_obj(layout1, flistbox, FListBox);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret != BUTTON_OK)
        dbind_destroy(&flistbox, FListBox);

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return flistbox;
}

/*---------------------------------------------------------------------------*/

FElem *dialog_new_elem(Window *parent, const char_t *folder_path)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 4);
    Layout *layout2 = layout_create(2, 1);
    Layout *layout3 = i_image_layout(&data);
    Layout *layout4 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Edit *edit = edit_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FElem *felem = dbind_create(FElem);
    uint32_t ret = 0;
    data.path = folder_path;
    data.felem = felem;
    data.window = window;
    caption = str_printf("Add new elem to Popup");
    label_text(label1, tc(caption));
    label_text(label2, "Text");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_edit(layout2, edit, 1, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_layout(layout1, layout3, 0, 2);
    layout_layout(layout1, layout4, 0, 3);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret == BUTTON_OK)
    {
        const char_t *text = edit_get_text(edit);
        str_upd(&felem->text, text);
    }
    else
    {
        dbind_destroy(&felem, FElem);
    }

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return felem;
}

/*---------------------------------------------------------------------------*/

FTable *dialog_new_table(Window *parent, const DSelect *sel)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = layout_create(2, 2);
    Layout *layout3 = i_value_updown_layout();
    Layout *layout4 = i_value_updown_layout();
    Layout *layout5 = i_ok_cancel(&data, TRUE);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Label *label3 = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    FTable *ftable = dbind_create(FTable);
    uint32_t ret = 0;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    caption = str_printf("New Table widget in (%d, %d) of '%s'", sel->col, sel->row, tc(sel->flayout->name));
    label_text(label1, tc(caption));
    label_text(label2, "Min width");
    label_text(label3, "Min height");
    layout_label(layout1, label1, 0, 0);
    layout_label(layout2, label2, 0, 0);
    layout_label(layout2, label3, 0, 1);
    layout_layout(layout2, layout3, 1, 0);
    layout_layout(layout2, layout4, 1, 1);
    layout_layout(layout1, layout2, 0, 1);
    layout_layout(layout1, layout5, 0, 2);
    layout_vmargin(layout1, 0, 5);
    layout_vmargin(layout1, 1, 5);
    cell_dbind(layout_cell(layout2, 1, 0), FTable, real32_t, min_width);
    cell_dbind(layout_cell(layout2, 1, 1), FTable, real32_t, min_height);
    layout_dbind(layout1, NULL, FTable);
    layout_dbind_obj(layout1, ftable, FTable);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);

    if (ret != BUTTON_OK)
        dbind_destroy(&ftable, FTable);

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return ftable;
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
    cell_dbind(layout_cell(layout, 1, 0), DialogLayout, uint32_t, ncols);
    cell_dbind(layout_cell(layout, 2, 0), DialogLayout, uint32_t, ncols);
    cell_dbind(layout_cell(layout, 1, 1), DialogLayout, uint32_t, nrows);
    cell_dbind(layout_cell(layout, 2, 1), DialogLayout, uint32_t, nrows);
    layout_dbind(layout, NULL, DialogLayout);
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

static FLayout *i_dialog_layout(Window *parent, const Font *font, const DSelect *sel, Layout *grid_layout, const ResId icon_id, const ResId text_id, DialogLayout *diag)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 4);
    Layout *layout2 = layout_create(2, 1);
    Layout *layout3 = grid_layout;
    Layout *layout4 = i_ok_cancel(&data, TRUE);
    ImageView *icon = imageview_create();               
    View *header = dgui_panel_header(gui_text(text_id), font, listener(&data, i_OnHeaderClose, DialogData));
    Label *label = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_RETURN | ekWINDOW_ESC);
    String *caption = NULL;
    FLayout *flayout = NULL;
    uint32_t ret = 0;
    data.window = window;
    cassert_no_null(sel);
    cassert_no_null(sel->flayout);
    cassert_no_null(diag);
    imageview_scale(icon, ekGUI_SCALE_ADJUST);
    imageview_image(icon, gui_image(icon_id));
    caption = str_printf(gui_text(TEXT_NEW_SUBLAYOUT), sel->col, sel->row, tc(sel->flayout->name));
    label_text(label, tc(caption));
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
    layout_hmargin(layout3, 0, 10);
    layout_margin4(layout3, 0, 30, 0, 60);
    layout_hexpand(layout3, 1);
    layout_hmargin(layout4, 1, 5);
    layout_margin4(layout4, 30, 10, 10, 10);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    layout_dbind_obj(layout3, diag, DialogLayout);
    ret = window_modal(window, parent);

    if (ret == BUTTON_OK || ret == ekGUI_CLOSE_INTRO)
    {
        cassert(diag->ncols > 0);
        cassert(diag->nrows > 0);
        flayout = flayout_create(diag->ncols, diag->nrows);
    }

    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
    return flayout;
}

/*---------------------------------------------------------------------------*/

FLayout *dialog_grid_layout(Window *parent, const Font *font, const DSelect *sel)
{
    DialogLayout diag = {3, 2};
    return i_dialog_layout(parent, font, sel, i_grid_layout(), GLAYOUT_PNG, TEXT_GRID_LAYOUT, &diag);
}

/*---------------------------------------------------------------------------*/

uint8_t dialog_unsaved_changes(Window *parent)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = i_save_buttons(&data);
    Label *label = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    uint32_t ret = 0;
    data.window = window;
    label_text(label, "There are unsaved changes in your current forms.\nChange will be lost if you open another project folder.\nDo you want to save your changes?");
    label_multiline(label, TRUE);
    layout_label(layout1, label, 0, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_vmargin(layout1, 0, 5);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);
    window_destroy(&window);
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

bool_t dialog_remove_form(Window *parent, const char_t *name)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = i_ok_cancel(&data, FALSE);
    Label *label = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    uint32_t ret = 0;
    data.window = window;
    caption = str_printf("Do you really want to delete the '%s' form?", name);
    label_text(label, tc(caption));
    layout_label(layout1, label, 0, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_vmargin(layout1, 0, 5);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    ret = window_modal(window, parent);
    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);

    if (ret == BUTTON_OK)
        return TRUE;

    return FALSE;
}

/*---------------------------------------------------------------------------*/

void dialog_name_already_exists(Window *parent, const char_t *name)
{
    DialogData data = i_dialog_data();
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = i_ok(&data);
    Label *label = label_create();
    Panel *panel = panel_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_ESC);
    String *caption = NULL;
    data.window = window;
    caption = str_printf("Form name '%s' already exists", name);
    label_text(label, tc(caption));
    layout_label(layout1, label, 0, 0);
    layout_layout(layout1, layout2, 0, 1);
    layout_vmargin(layout1, 0, 5);
    panel_layout(panel, layout1);
    window_panel(window, panel);
    window_defbutton(window, data.defbutton);
    i_center_window(parent, window);
    window_modal(window, parent);
    window_destroy(&window);
    str_destroy(&caption);
    i_remove_dialog_data(&data);
}
