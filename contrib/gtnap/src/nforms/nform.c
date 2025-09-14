/* NAppGUI form */

#include "nform.h"
#include <nflib/fform.h>
#include <nflib/flayout.h>
#include <gui/button.h>
#include <gui/edit.h>
#include <gui/guicontrol.h>
#include <gui/label.h>
#include <gui/listbox.h>
#include <gui/panel.h>
#include <gui/popup.h>
#include <gui/slider.h>
#include <gui/progress.h>
#include <gui/textview.h>
#include <gui/window.h>
#include <geom2d/r2d.h>
#include <core/heap.h>
#include <core/strings.h>
#include <core/stream.h>
#include <sewer/bmath.h>
#include <sewer/bstd.h>
#include <sewer/cassert.h>

struct _nform_t
{
    FForm *fform;
    Layout *glayout;
};

/*---------------------------------------------------------------------------*/

static NForm *i_from_stream(Stream **stm)
{
    cassert_no_null(stm);
    if (*stm != NULL)
    {
        NForm *form = heap_new0(NForm);
        form->fform = fform_read(*stm);
        stm_close(stm);
        return form;
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

NForm *nform_from_data(const byte_t *data, const uint32_t size)
{
    Stream *stm = stm_from_block(data, size);
    return i_from_stream(&stm);
}

/*---------------------------------------------------------------------------*/

NForm *nform_from_file(const char_t *pathname, ferror_t *error)
{
    Stream *stm = stm_from_file(pathname, error);
    return i_from_stream(&stm);
}

/*---------------------------------------------------------------------------*/

void nform_destroy(NForm **form)
{
    cassert_no_null(form);
    cassert_no_null(*form);
    fform_destroy(&(*form)->fform);
    (*form)->glayout = NULL;
    heap_delete(form, NForm);
}

/*---------------------------------------------------------------------------*/

Window *nform_window(const NForm *form, const uint32_t flags, const char_t *resource_path)
{
    Layout *layout = NULL;
    Panel *panel = NULL;
    Window *window = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    layout = flayout_to_gui(form->fform->layout, resource_path, 40.f, 20.f);
    panel = panel_create();
    window = window_create(flags);
    panel_layout(panel, layout);
    window_panel(window, panel);
    cast(form, NForm)->glayout = layout;
    return window;
}

/*---------------------------------------------------------------------------*/

void nform_set_control_str(NForm *form, const char_t *cell_name, const char_t *value)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    if (control != NULL)
    {
        Button *button = guicontrol_button(control);
        Label *label = guicontrol_label(control);
        Edit *edit = guicontrol_edit(control);
        TextView *text = guicontrol_textview(control);

        if (button != NULL)
        {
            button_text(button, value);
        }
        else if (label != NULL)
        {
            label_text(label, value);
        }
        else if (edit != NULL)
        {
            edit_text(edit, value);
        }
        else if (text != NULL)
        {
            textview_clear(text);
            textview_writef(text, value);
        }
    }
}

/*---------------------------------------------------------------------------*/

void nform_add_control_str(NForm *form, const char_t *cell_name, const char_t *value)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    if (control != NULL)
    {
        TextView *text = guicontrol_textview(control);
        if (text != NULL)
            textview_writef(text, value);
    }
}

/*---------------------------------------------------------------------------*/

void nform_set_control_bool(NForm *form, const char_t *cell_name, const bool_t value)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    if (control != NULL)
    {
        Button *button = guicontrol_button(control);
        if (button != NULL)
            button_state(button, value == TRUE ? ekGUI_ON : ekGUI_OFF);
    }
}

/*---------------------------------------------------------------------------*/

void nform_set_control_int(NForm *form, const char_t *cell_name, const int32_t value)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    if (control != NULL)
    {
        Label *label = guicontrol_label(control);
        Edit *edit = guicontrol_edit(control);
        PopUp *popup = guicontrol_popup(control);
        ListBox *listbox = guicontrol_listbox(control);

        if (label != NULL)
        {
            char_t text[32];
            bstd_sprintf(text, sizeof(text), "%d", value);
            label_text(label, text);
        }
        else if (edit != NULL)
        {
            char_t text[32];
            bstd_sprintf(text, sizeof(text), "%d", value);
            edit_text(edit, text);
        }
        else if (popup != NULL)
        {
            uint32_t n = popup_count(popup);
            if (n > 0)
            {
                uint32_t v = 0;
                if (value > 0)
                    v = (uint32_t)value - 1;
                if (v >= n)
                    v = n - 1;
                popup_selected(popup, v);
            }
        }
        else if (listbox != NULL)
        {
            uint32_t n = listbox_count(listbox);
            if (n > 0)
            {
                uint32_t v = 0;
                if (value > 0)
                    v = (uint32_t)value - 1;
                if (v >= n)
                    v = n - 1;
                listbox_select(listbox, v, TRUE);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void nform_set_control_real(NForm *form, const char_t *cell_name, const real32_t value)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    if (control != NULL)
    {
        Slider *slider = guicontrol_slider(control);
        Progress *progress = guicontrol_progress(control);
        if (slider != NULL)
        {
            real32_t cvalue = bmath_clampf(value, 0, 1);
            slider_value(slider, cvalue);
        }
        else if (progress != NULL)
        {
            real32_t cvalue = bmath_clampf(value, 0, 1);
            progress_value(progress, cvalue);
        }
    }
}

/*---------------------------------------------------------------------------*/

bool_t nform_get_control_str(const NForm *form, const char_t *cell_name, const char_t **value)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    cassert_no_null(value);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    if (control != NULL)
    {
        Edit *edit = guicontrol_edit(control);
        TextView *text = guicontrol_textview(control);
        if (edit != NULL)
        {
            *value = edit_get_text(edit);
            return TRUE;
        }
        else if (text != NULL)
        {
            *value = textview_get_text(text);
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

bool_t nform_get_control_bool(const NForm *form, const char_t *cell_name, bool_t *value)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    cassert_no_null(value);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    if (control != NULL)
    {
        Button *button = guicontrol_button(control);
        if (button != NULL)
        {
            gui_state_t state = button_get_state(button);
            *value = (state == ekGUI_OFF) ? FALSE : TRUE;
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

bool_t nform_get_control_int(const NForm *form, const char_t *cell_name, int32_t *value)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    cassert_no_null(value);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    if (control != NULL)
    {
        Edit *edit = guicontrol_edit(control);
        PopUp *popup = guicontrol_popup(control);
        ListBox *listbox = guicontrol_listbox(control);
        if (edit != NULL)
        {
            const char_t *text = edit_get_text(edit);
            bool_t error = FALSE;
            int32_t v = str_to_i32(text, 10, &error);
            if (error == FALSE)
            {
                *value = v;
                return TRUE;
            }
        }
        else if (popup != NULL)
        {
            if (popup_count(popup) > 0)
                *value = (int32_t)popup_get_selected(popup) + 1;
            else
                *value = 0;

            return TRUE;
        }
        else if (listbox != NULL)
        {
            if (listbox_count(listbox) > 0)
                *value = (int32_t)listbox_get_selected(listbox) + 1;
            else
                *value = 0;

            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

bool_t nform_get_control_real(const NForm *form, const char_t *cell_name, real32_t *value)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    cassert_no_null(value);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    if (control != NULL)
    {
        Slider *slider = guicontrol_slider(control);
        Progress *progress = guicontrol_progress(control);
        if (slider != NULL)
        {
            *value = slider_get_value(slider);
            return TRUE;
        }
        else if (progress != NULL)
        {
            *value = .5f;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

bool_t nform_set_listener(NForm *form, const char_t *cell_name, Listener *listener)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    if (control != NULL)
    {
        Button *button = guicontrol_button(control);
        if (button != NULL)
        {
            button_OnClick(button, listener);
            return TRUE;
        }
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

R2Df nform_get_control_frame(NForm *form, const char_t *cell_name, Window *window)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    if (control != NULL)
    {
        R2Df frame = window_control_frame(window, control);
        frame.pos = window_client_to_screen(window, frame.pos);
        return frame;
    }

    return kR2D_ZEROf;
}

/*---------------------------------------------------------------------------*/

TableView *nform_get_tableview(NForm *form, const char_t *cell_name)
{
    GuiControl *control = NULL;
    cassert_no_null(form);
    cassert_no_null(form->fform);
    cassert_no_null(form->glayout);
    control = flayout_search_gui_control(form->fform->layout, form->glayout, cell_name);
    return guicontrol_tableview(control);
}
