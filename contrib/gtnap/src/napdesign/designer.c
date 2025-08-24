/* NAppGUI Designer Application */

#include <nflib/nflib.h>
#include <nappgui.h>
#include "res_designer.h"
#include "dlayout.h"
#include "dform.h"
#include "dgui.h"
#include "dialogs.h"
#include "propedit.h"
#include "inspect.h"

typedef struct _config_t Config;
typedef struct _wdrawer_t WDrawer;
typedef struct _bwidget_t BWidget;

struct _config_t
{
    uint16_t vers;
    String *folder_path;
    uint32_t sel_form;
    widget_t swidget;
    real32_t wx;
    real32_t wy;
    real32_t wwidth;
    real32_t wheight;
    real32_t split1_pos;
    real32_t split2_pos;
    real32_t split3_pos;
    real32_t split4_pos;
    bool_t show_forms;
    bool_t show_widgets;
    bool_t show_inspectr;
    bool_t show_propedit;
};

struct _wdrawer_t
{
    drawer_t type;
    ResId labelid;
    Panel *panel;
    bool_t opened;
};

struct _bwidget_t
{
    widget_t twidget;
    drawer_t drawer;
    ResId labelid;
    ResId imageid;
    Button *button;
    Label *label;
    bool_t opened;
};

struct _desiger_t
{
    Window *window;
    Config config;
    Menu *menu;
    ArrPt(DForm) *forms;
    ArrSt(WDrawer) *wdrawers;
    ArrSt(BWidget) *bwidgets;
    ListBox *form_list;
    Label *status_label;
    Label *cells_label;
    Progress *progress;
    View *canvas;
    Panel *inspect;
    Panel *propedit;
    Cell *open_form_cell;
    Cell *save_form_cell;
    Cell *run_form_cell;
    Cell *add_form_cell;
    Cell *remove_form_cell;
    Cell *rename_form_cell;
    SplitView *split1;
    SplitView *split2;
    SplitView *split3;
    SplitView *split4;
    MenuItem *show_forms;
    MenuItem *show_widgets;
    MenuItem *show_inspectr;
    MenuItem *show_propedit;
    Image *add_icon;
    Font *default_font;
    Font *bold_font;
    bool_t dragging;
    V2Df drag_mouse;
    V2Df drag_form;
};

/*---------------------------------------------------------------------------*/

static const uint16_t i_CONFIG_VERS = 0;
static const split_mode_t i_SPLIT1_MODE = ekSPLIT_FIXED0;
static const split_mode_t i_SPLIT2_MODE = ekSPLIT_FIXED0;
static const split_mode_t i_SPLIT3_MODE = ekSPLIT_FIXED1;
static const split_mode_t i_SPLIT4_MODE = ekSPLIT_FIXED0;
static const char_t *i_FILE_EXT = "nfm";
static const char_t *i_SAVE_MARK = "• ";
DeclPt(DForm);
DeclSt(BWidget);
DeclSt(WDrawer);

/*---------------------------------------------------------------------------*/

static void i_OnShowForms(Designer *, Event *);
static void i_OnShowWidgets(Designer *, Event *);
static void i_OnShowInspectr(Designer *, Event *);
static void i_OnShowPropEdit(Designer *, Event *);

/*---------------------------------------------------------------------------*/

static void i_dbind(void)
{
}

/*---------------------------------------------------------------------------*/

static void i_destroy_form_opt(DForm **form)
{
    cassert_no_null(form);
    if (*form != NULL)
        dform_destroy(form);
}

/*---------------------------------------------------------------------------*/

static ___INLINE const char_t *i_list_text(const ListBox *listbox, const uint32_t index)
{
    const char_t *name = listbox_get_text(listbox, index);
    if (str_is_prefix(name, i_SAVE_MARK) == TRUE)
        name += str_len_c(i_SAVE_MARK);
    return name;
}

/*---------------------------------------------------------------------------*/

static ___INLINE bool_t i_with_save_mark(const ListBox *listbox, const uint32_t index)
{
    const char_t *name = listbox_get_text(listbox, index);
    return str_is_prefix(name, i_SAVE_MARK);
}

/*---------------------------------------------------------------------------*/

static void i_need_save_mark(ListBox *listbox, const uint32_t pos, const bool_t needs_save)
{
    const char_t *name = i_list_text(listbox, pos);
    bool_t with_bullet = i_with_save_mark(listbox, pos);
    if (needs_save != with_bullet)
    {
        if (needs_save == TRUE)
        {
            String *nname = str_printf("%s%s", i_SAVE_MARK, name);
            listbox_set_elem(listbox, pos, tc(nname), NULL);
            str_destroy(&nname);
        }
        else
        {
            listbox_set_elem(listbox, pos, name, NULL);
        }
    }
}

/*---------------------------------------------------------------------------*/

static bool_t i_need_save(Designer *app)
{
    cassert_no_null(app);
    cassert(arrpt_size(app->forms, DForm) == listbox_count(app->form_list));
    arrpt_foreach(form, app->forms, DForm)
        if (form != NULL && dform_need_save(form) == TRUE)
            return TRUE;
    arrpt_end()
    return FALSE;
}

/*---------------------------------------------------------------------------*/

static void i_update_form_controls(Designer *app, const bool_t enable)
{
    bool_t enable_save = FALSE;
    bool_t enable_run = FALSE;
    bool_t enable_remove = FALSE;
    bool_t enable_rename = FALSE;
    cassert_no_null(app);

    if (enable == TRUE)
    {
        enable_save = i_need_save(app);

        if (app->config.sel_form != UINT32_MAX)
        {
            enable_run = TRUE;
            enable_remove = TRUE;
            enable_rename = TRUE;
        }
    }

    arrpt_foreach(form, app->forms, DForm)
        bool_t need_save = FALSE;
        if (form != NULL)
            need_save = dform_need_save(form);
        i_need_save_mark(app->form_list, form_i, need_save);
    arrpt_end()

    /*cell_enabled(app->open_form_cell, enable);*/
    cell_enabled(app->save_form_cell, enable_save);
    cell_enabled(app->run_form_cell, enable_run);
    cell_enabled(app->add_form_cell, enable);
    cell_enabled(app->remove_form_cell, enable_remove);
    cell_enabled(app->rename_form_cell, enable_rename);
}

/*---------------------------------------------------------------------------*/

static void i_open_form(Designer *app, const uint32_t index)
{
    DForm *form = NULL;
    cassert_no_null(app);
    if (index != UINT32_MAX)
    {
        form = arrpt_get(app->forms, index, DForm);
        if (form == NULL)
        {
            const char_t *name = i_list_text(app->form_list, index);
            String *path = str_cpath("%s/%s.%s", tc(app->config.folder_path), name, i_FILE_EXT);
            Stream *stm = NULL;
            DForm **forms = arrpt_all(app->forms, DForm);
            stm = stm_from_file(tc(path), NULL);
            if (stm != NULL)
            {
                form = dform_read(stm, app);
                stm_close(&stm);
            }

            forms[index] = form;
            str_destroy(&path);
        }

        if (form != NULL)
        {
            dform_compose(form);
            dform_set(form, app->inspect, app->propedit);
        }
    }

    view_update(app->canvas);
}

/*---------------------------------------------------------------------------*/

static void i_init_forms(Designer *app, const char_t *path)
{
    ArrSt(DirEntry) *files = NULL;
    ferror_t err = ekFNOPATH;
    cassert_no_null(app);
    if (str_empty_c(path) == FALSE)
        files = hfile_dir_list(path, FALSE, &err);

    arrpt_clear(app->forms, i_destroy_form_opt, DForm);
    listbox_clear(app->form_list);

    if (err == ekFOK)
    {
        uint32_t n = UINT32_MAX;

        str_upd(&app->config.folder_path, path);
        arrst_foreach(file, files, DirEntry)
            String *fil = NULL;
            String *ext = NULL;
            str_split_pathext(tc(file->name), NULL, &fil, &ext);
            if (str_equ_c(tc(ext), i_FILE_EXT) == TRUE)
            {
                listbox_add_elem(app->form_list, tc(fil), NULL);
                arrpt_append(app->forms, NULL, DForm);
            }

            str_destroy(&fil);
            str_destroy(&ext);
        arrst_end()

        n = arrpt_size(app->forms, DForm);
        if (n > 0)
        {
            if (app->config.sel_form >= n)
                app->config.sel_form = 0;
            i_open_form(app, app->config.sel_form);
            listbox_select(app->form_list, app->config.sel_form, TRUE);
        }
        else
        {
            app->config.sel_form = UINT32_MAX;
        }

        i_update_form_controls(app, TRUE);

        {
            Button *button = cell_button(app->open_form_cell);
            String *tooltip = str_printf("Open forms folder (%s)", tc(app->config.folder_path));
            button_tooltip(button, tc(tooltip));
            str_destroy(&tooltip);
        }
    }
    else
    {
        Button *button = cell_button(app->open_form_cell);
        i_update_form_controls(app, FALSE);
        button_tooltip(button, "Open forms folder (No path selected)");
    }

    view_update(app->canvas);
    arrst_destopt(&files, hfile_dir_entry_remove, DirEntry);
}

/*---------------------------------------------------------------------------*/

static void i_save_forms(Designer *app)
{
    cassert_no_null(app);
    arrpt_foreach(form, app->forms, DForm)
        bool_t need_save = FALSE;
        if (form != NULL)
            need_save = dform_need_save(form);

        if (need_save == TRUE)
        {
            const char_t *name = i_list_text(app->form_list, form_i);
            String *path = str_cpath("%s/%s.%s", tc(app->config.folder_path), name, i_FILE_EXT);
            Stream *stm = stm_to_file(tc(path), NULL);
            if (stm != NULL)
            {
                dform_write(stm, form);
                stm_close(&stm);
            }

            str_destroy(&path);
        }
    arrpt_end()
}

/*---------------------------------------------------------------------------*/

static void i_OnOpenFormsClick(Designer *app, Event *e)
{
    bool_t can_open = TRUE;
    cassert_no_null(app);
    unref(e);
    if (i_need_save(app) == TRUE)
    {
        uint8_t ret = dialog_unsaved_changes(app->window);
        if (ret == 1)
            i_save_forms(app);
        else if (ret == 2)
            can_open = FALSE;
    }

    if (can_open == TRUE)
    {
        const char_t *ftype = "..DIR..";
        const char_t *folder = comwin_open_file(app->window, &ftype, 1, tc(app->config.folder_path));
        if (folder != NULL)
            i_init_forms(app, folder);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnSaveFormsClick(Designer *app, Event *e)
{
    i_save_forms(app);
    i_update_form_controls(app, TRUE);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_OnSimulateClick(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        dform_simulate(form, app->window);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnRemoveClick(Designer *app, Event *e)
{
    const char_t *name = NULL;
    cassert_no_null(app);
    unref(e);
    name = i_list_text(app->form_list, app->config.sel_form);
    if (dialog_remove_form(app->window, name) == TRUE)
    {
        String *path = str_cpath("%s/%s.%s", tc(app->config.folder_path), name, i_FILE_EXT);
        bool_t removed = TRUE;

        if (hfile_exists(tc(path), NULL) == TRUE)
            removed = bfile_delete(tc(path), NULL);

        if (removed == TRUE)
        {
            uint32_t n = UINT32_MAX;
            listbox_del_elem(app->form_list, app->config.sel_form);
            arrpt_delete(app->forms, app->config.sel_form, i_destroy_form_opt, DForm);
            n = arrpt_size(app->forms, DForm);

            if (n > 0)
            {
                if (app->config.sel_form >= n)
                    app->config.sel_form = n - 1;
                listbox_select(app->form_list, app->config.sel_form, TRUE);
            }
            else
            {
                app->config.sel_form = UINT32_MAX;
            }

            i_open_form(app, app->config.sel_form);
            i_update_form_controls(app, TRUE);
        }

        str_destroy(&path);
    }
}

/*---------------------------------------------------------------------------*/

static bool_t i_exists_form_name(Designer *app, const char_t *name)
{
    uint32_t i, n;
    cassert_no_null(app);
    n = listbox_count(app->form_list);
    cassert(n == arrpt_size(app->forms, DForm));
    for (i = 0; i < n; ++i)
    {
        const char_t *lname = i_list_text(app->form_list, i);
        if (str_equ_c(lname, name) == TRUE)
            return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

static void i_OnAddFormClick(Designer *app, Event *e)
{
    String *fname = NULL;
    cassert_no_null(app);
    unref(e);
    fname = dialog_form_name(app->window, NULL);
    if (str_empty(fname) == FALSE)
    {
        if (i_exists_form_name(app, tc(fname)) == FALSE)
        {
            uint32_t n = listbox_count(app->form_list);
            DForm *form = dform_empty(app);
            dform_compose(form);
            cassert(n == arrpt_size(app->forms, DForm));
            listbox_add_elem(app->form_list, tc(fname), NULL);
            listbox_select(app->form_list, n, TRUE);
            arrpt_append(app->forms, form, DForm);
            app->config.sel_form = n;
            i_update_form_controls(app, TRUE);
            view_update(app->canvas);
        }
        else
        {
            dialog_name_already_exists(app->window, tc(fname));
        }
    }

    str_destroy(&fname);
}

/*---------------------------------------------------------------------------*/

static void i_OnRenameFormClick(Designer *app, Event *e)
{
    const char_t *name = NULL;
    String *fname = NULL;
    cassert_no_null(app);
    unref(e);
    name = i_list_text(app->form_list, app->config.sel_form);
    fname = dialog_form_name(app->window, name);
    if (str_empty(fname) == FALSE)
    {
        if (i_exists_form_name(app, tc(fname)) == FALSE)
        {
            String *oldpath = str_cpath("%s/%s.%s", tc(app->config.folder_path), name, i_FILE_EXT);
            if (hfile_exists(tc(oldpath), NULL) == TRUE)
            {
                String *newpath = str_cpath("%s/%s.%s", tc(app->config.folder_path), tc(fname), i_FILE_EXT);
                bfile_rename(tc(oldpath), tc(newpath), NULL);
                str_destroy(&newpath);
            }

            {
                bool_t with_bullet = i_with_save_mark(app->form_list, app->config.sel_form);
                if (with_bullet == TRUE)
                {
                    String *rname = str_printf("%s%s", i_SAVE_MARK, tc(fname));
                    listbox_set_elem(app->form_list, app->config.sel_form, tc(rname), NULL);
                    str_destroy(&rname);
                }
                else
                {
                    listbox_set_elem(app->form_list, app->config.sel_form, tc(fname), NULL);
                }
            }

            str_destroy(&oldpath);
        }
        else
        {
            dialog_name_already_exists(app->window, tc(fname));
        }
    }

    str_destroy(&fname);
}

/*---------------------------------------------------------------------------*/

static Layout *i_tools_layout(Designer *app)
{
    Layout *layout = layout_create(9, 1);
    Button *button1 = button_flat();
    Button *button2 = button_flat();
    Button *button3 = button_flat();
    Button *button4 = button_flat();
    Button *button5 = button_flat();
    Button *button6 = button_flat();
    Button *button7 = button_flat();
    Button *button8 = button_flat();
    cassert_no_null(app);
    button_image(button1, cast_const(FOLDER24_PNG, Image));
    button_image(button2, cast_const(DISK24_PNG, Image));
    button_image(button3, cast_const(PLUS24_PNG, Image));
    button_image(button4, cast_const(EDIT24_PNG, Image));
    button_image(button5, cast_const(SEARCH24_PNG, Image));
    button_image(button6, cast_const(ERROR24_PNG, Image));
    button_image(button7, cast_const(PLUS24_PNG, Image));
    button_image(button8, cast_const(ERROR24_PNG, Image));
    button_OnClick(button1, listener(app, i_OnOpenFormsClick, Designer));
    button_OnClick(button2, listener(app, i_OnSaveFormsClick, Designer));
    button_OnClick(button3, listener(app, i_OnAddFormClick, Designer));
    button_OnClick(button4, listener(app, i_OnRenameFormClick, Designer));
    button_OnClick(button5, listener(app, i_OnSimulateClick, Designer));
    button_OnClick(button6, listener(app, i_OnRemoveClick, Designer));
    button_tooltip(button1, "Open forms folder");
    button_tooltip(button2, "Save all forms");
    button_tooltip(button3, "Add new form");
    button_tooltip(button4, "Rename form");
    button_tooltip(button5, "Simulate current form");
    button_tooltip(button6, "Remove current form");
    layout_button(layout, button1, 0, 0);
    layout_button(layout, button2, 1, 0);
    layout_button(layout, button3, 2, 0);
    layout_button(layout, button4, 3, 0);
    layout_button(layout, button5, 4, 0);
    layout_button(layout, button6, 5, 0);
    layout_button(layout, button7, 7, 0);
    layout_button(layout, button8, 8, 0);
    layout_hexpand(layout, 6);
    app->open_form_cell = layout_cell(layout, 0, 0);
    app->save_form_cell = layout_cell(layout, 1, 0);
    app->add_form_cell = layout_cell(layout, 2, 0);
    app->rename_form_cell = layout_cell(layout, 3, 0);
    app->run_form_cell = layout_cell(layout, 4, 0);
    app->remove_form_cell = layout_cell(layout, 5, 0);
    return layout;
}

/*---------------------------------------------------------------------------*/

static void i_set_bwidget(const widget_t swidget, ArrSt(BWidget) *bwidgets, const Font *font, const Font *bold_font)
{
    arrst_foreach(bwidget, bwidgets, BWidget)
        button_state(bwidget->button, bwidget->twidget == swidget ? ekGUI_ON : ekGUI_OFF);
        label_font(bwidget->label, bwidget->twidget == swidget ? bold_font : font);
    arrst_end()
}

/*---------------------------------------------------------------------------*/

static void i_OnWidgetButtonClick(Designer *app, Event *e)
{
    Button *sender = event_sender(e, Button);
    cassert_no_null(app);
    arrst_foreach(bwidget, app->bwidgets, BWidget)
        if (bwidget->button == sender)
        {
            app->config.swidget = bwidget->twidget;
            break;
        }
    arrst_end()

    i_set_bwidget(app->config.swidget, app->bwidgets, app->default_font, app->bold_font);
}

/*---------------------------------------------------------------------------*/

static void i_OnWidgetLabelClick(Designer *app, Event *e)
{
    Label *sender = event_sender(e, Label);
    cassert_no_null(app);
    arrst_foreach(bwidget, app->bwidgets, BWidget)
        if (bwidget->label == sender)
        {
            app->config.swidget = bwidget->twidget;
            break;
        }
    arrst_end()

    i_set_bwidget(app->config.swidget, app->bwidgets, app->default_font, app->bold_font);
}

/*---------------------------------------------------------------------------*/

static Panel *i_drawer_widget_panel(Designer *app, const drawer_t drawer)
{
    Panel *panel = panel_create();
    uint32_t i = 0, n = 0;    
    cassert_no_null(app);

    /* Number of widgets for this drawer */
    arrst_foreach_const(bwidget, app->bwidgets, BWidget)
        if (bwidget->drawer == drawer)
            n += 1;
    arrst_end()

    if (n > 0)
    {
        Layout *layout = layout_create(2, n);
        arrst_foreach(bwidget, app->bwidgets, BWidget)
            if (bwidget->drawer == drawer)
            {
                Button *button = button_flatgle();
                Label *label = label_create();
                cassert(bwidget->button == NULL);
                cassert(bwidget->label == NULL);
                button_OnClick(button, listener(app, i_OnWidgetButtonClick, Designer));               
                button_image(button, gui_image(bwidget->imageid));
                button_vpadding(button, 0);
                button_hpadding(button, 0);
                label_text(label, gui_text(bwidget->labelid));
                label_style_over(label, ekFUNDERLINE);
                label_OnClick(label, listener(app, i_OnWidgetLabelClick, Designer));               
                layout_button(layout, button, 0, i);
                layout_label(layout, label, 1, i);
                layout_tabstop(layout, 0, i, FALSE);
                layout_halign(layout, 1, i, ekJUSTIFY);
                bwidget->button = button;
                bwidget->label = label;
                i += 1;
            }
        arrst_end()
        layout_margin4(layout, 0, 0, 0, 5);
        layout_hmargin(layout, 0, 10);
        layout_hexpand(layout, 1);
        panel_layout(panel, layout);
    }
    else
    {
        cassert(FALSE);
    }

    return panel;
}

/*---------------------------------------------------------------------------*/

static WDrawer *i_find_drawer_by_panel(Designer *app, Panel *panel)
{
    cassert_no_null(app);
    arrst_foreach(wdrawer, app->wdrawers, WDrawer)
        if (wdrawer->panel == panel)
            return wdrawer;
    arrst_end()
    return NULL;
}

/*---------------------------------------------------------------------------*/

static WDrawer *i_find_drawer_by_type(Designer *app, const drawer_t type)
{
    cassert_no_null(app);
    arrst_foreach(wdrawer, app->wdrawers, WDrawer)
        if (wdrawer->type == type)
            return wdrawer;
    arrst_end()
    return NULL;
}

/*---------------------------------------------------------------------------*/

static bool_t i_is_widget_drawer(const drawer_t drawer)
{
    switch (drawer)
    {
    case ekDRAWER_WIDGET_SELECT:
    case ekDRAWER_WIDGET_LAYOUTS:
    case ekDRAWER_WIDGET_BUTTONS:
    case ekDRAWER_WIDGET_TEXT:
    case ekDRAWER_WIDGET_ITEMS:
    case ekDRAWER_WIDGET_OTHERS:
        return TRUE;
    case ekDRAWER_LAYOUT_PROPS:
    case ekDRAWER_COLUMN_PROPS:
    case ekDRAWER_ROW_PROPS:
    case ekDRAWER_CELL_PROPS:
    case ekDRAWER_LABEL_PROPS:
    case ekDRAWER_BUTTON_PROPS:
    case ekDRAWER_CHECKBOX_PROPS:
    case ekDRAWER_RADIO_PROPS:
    case ekDRAWER_TOOL_PROPS:
    case ekDRAWER_POPUP_PROPS:
    case ekDRAWER_EDIT_PROPS:
    case ekDRAWER_COMBO_PROPS:
    case ekDRAWER_LIST_PROPS:
    case ekDRAWER_HSLIDER_PROPS:
    case ekDRAWER_VSLIDER_PROPS:
    case ekDRAWER_PROGRESS_PROPS:
    case ekDRAWER_TEXT_PROPS:
    case ekDRAWER_IMAGE_PROPS:
        return FALSE;
        cassert_default();
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

static void i_OnDrawerChange(Designer *app, Event *e)
{
    Panel *sender = event_sender(e, Panel);
    WDrawer *drawer = i_find_drawer_by_panel(app, sender);
    const bool_t *p = event_params(e, bool_t);
    cassert(drawer->opened != *p);
    drawer->opened = *p;
}
    
/*---------------------------------------------------------------------------*/

static Panel *i_widgets_panel(Designer *app)
{
    Panel *panel = panel_custom(FALSE, TRUE, FALSE);
    uint32_t n = 0;
    cassert_no_null(app);       

    arrst_foreach_const(wdrawer, app->wdrawers, WDrawer)
        if (i_is_widget_drawer(wdrawer->type) == TRUE)
            n += 1;
    arrst_end()

    {
        uint32_t i = 0;
        Layout *layout = layout_create(1, n + 1);
        arrst_foreach(wdrawer, app->wdrawers, WDrawer)
            if (i_is_widget_drawer(wdrawer->type) == TRUE)
            {
                Panel *dpanel = NULL;
                Panel *ipanel = i_drawer_widget_panel(app, wdrawer->type);
                const char_t *dlabel = gui_text(wdrawer->labelid);
                cassert(wdrawer->panel == NULL);
                if (str_empty_c(dlabel) == FALSE)
                {
                    dpanel = dgui_drawer(dlabel, app->default_font, ipanel, wdrawer->opened, listener(app, i_OnDrawerChange, Designer));
                    wdrawer->panel = dpanel;
                }
                else
                {
                    dpanel = ipanel;
                }
                layout_panel(layout, dpanel, 0, i);
                i += 1;
            }
        arrst_end()
        layout_vexpand(layout, n);
        panel_layout(panel, layout);
    }

    return panel;
}

/*---------------------------------------------------------------------------*/

static Panel *i_widgets_box(Designer *app)
{
    Panel *panel1 = panel_create();
    Panel *panel2 = i_widgets_panel(app);
    Layout *layout = layout_create(1, 2);
    View *header = dgui_panel_header(gui_text(TEXT_BOX_WIDGETS), app->default_font, listener(app, i_OnShowWidgets, Designer));
    layout_view(layout, header, 0, 0);
    layout_panel(layout, panel2, 0, 1);
    layout_vmargin(layout, 0, 2);
    layout_vexpand(layout, 1);
    layout_margin4(layout, 0, 5, 5, 1);
    panel_layout(panel1, layout);
    return panel1;
}

/*---------------------------------------------------------------------------*/

static void i_OnFormSelect(Designer *app, Event *e)
{
    const EvButton *p = event_params(e, EvButton);
    cassert_no_null(app);
    app->config.sel_form = p->index;
    i_open_form(app, app->config.sel_form);
    i_update_form_controls(app, TRUE);
}

/*---------------------------------------------------------------------------*/

static Panel *i_forms_box(Designer *app)
{
    Panel *panel = panel_create();
    Layout *layout = layout_create(1, 2);
    View *header = dgui_panel_header(gui_text(TEXT_BOX_FORMS), app->default_font, listener(app, i_OnShowForms, Designer));
    ListBox *list = listbox_create();
    cassert_no_null(app);
    listbox_size(list, s2df(150, 100));
    listbox_OnSelect(list, listener(app, i_OnFormSelect, Designer));
    layout_view(layout, header, 0, 0);
    layout_listbox(layout, list, 0, 1);
    layout_vmargin(layout, 0, 2);
    layout_vexpand(layout, 1);
    layout_margin4(layout, 0, 5, 5, 1);
    panel_layout(panel, layout);
    app->form_list = list;
    return panel;
}

/*---------------------------------------------------------------------------*/

static Panel *i_inspector_box(Designer *app)
{
    Panel *panel1 = panel_create();
    Panel *panel2 = inspect_create(app);
    Layout *layout = layout_create(1, 2);
    View *header = dgui_panel_header(gui_text(TEXT_BOX_INSPECTOR), app->default_font, listener(app, i_OnShowInspectr, Designer));
    cassert_no_null(app);
    layout_view(layout, header, 0, 0);
    layout_panel(layout, panel2, 0, 1);
    layout_vmargin(layout, 0, 2);
    layout_vexpand(layout, 1);
    layout_margin4(layout, 0, 1, 5, 5);
    panel_layout(panel1, layout);
    app->inspect = panel2;
    return panel1;
}

/*---------------------------------------------------------------------------*/

static Panel *i_propedit_box(Designer *app)
{
    Panel *panel1 = panel_create();
    Panel *panel2 = propedit_create(app);
    Layout *layout = layout_create(1, 2);
    View *header = dgui_panel_header(gui_text(TEXT_BOX_PROPEDIT), app->default_font, listener(app, i_OnShowPropEdit, Designer));
    cassert_no_null(app);
    layout_view(layout, header, 0, 0);
    layout_panel(layout, panel2, 0, 1);
    layout_vmargin(layout, 0, 2);
    layout_vexpand(layout, 1);
    layout_margin4(layout, 0, 1, 5, 5);
    panel_layout(panel1, layout);
    app->propedit = panel2;
    return panel1;
}

/*---------------------------------------------------------------------------*/

static void i_OnDraw(Designer *app, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    cassert_no_null(app);
    draw_clear(p->ctx, kCOLOR_YELLOW);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        dform_draw(form, app->config.swidget, app->add_icon, app->default_font, p->ctx);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnMove(Designer *app, Event *e)
{
    cassert_no_null(app);
    if (app->config.sel_form != UINT32_MAX)
    {
        const EvMouse *p = event_params(e, EvMouse);
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        if (dform_OnMove(form, p->x, p->y) == TRUE)
            view_update(app->canvas);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnDrag(Designer *app, Event *e)
{
    cassert_no_null(app);
    if (app->config.sel_form != UINT32_MAX)
    {
        const EvMouse *p = event_params(e, EvMouse);
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        if (app->dragging == FALSE)
        {
            app->drag_mouse = v2df(p->x, p->y);
            app->drag_form = dform_get_origin(form);
            app->dragging = TRUE;
        }
        else
        {
            V2Df origin;
            origin.x = app->drag_form.x + (p->x - app->drag_mouse.x);
            origin.y = app->drag_form.y + (p->y - app->drag_mouse.y);
            dform_origin(form, origin);
            view_update(app->canvas);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnUp(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    app->dragging = FALSE;
}

/*---------------------------------------------------------------------------*/

static void i_OnExit(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        if (dform_OnExit(form) == TRUE)
            view_update(app->canvas);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnClick(Designer *app, Event *e)
{
    cassert_no_null(app);
    if (app->config.sel_form != UINT32_MAX)
    {
        const EvMouse *p = event_params(e, EvMouse);
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        if (dform_OnClick(form, app->window, app->inspect, app->propedit, app->default_font, app->config.swidget, p->x, p->y, p->button) == TRUE)
            view_update(app->canvas);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnSize(Designer *app, Event *e)
{
    unref(app);
    unref(e);
}

/*---------------------------------------------------------------------------*/

static View *i_canvas_view(Designer *app)
{
    View *view = view_scroll();
    view_size(view, s2df(450, 200));
    view_OnDraw(view, listener(app, i_OnDraw, Designer));
    view_OnMove(view, listener(app, i_OnMove, Designer));
    view_OnDrag(view, listener(app, i_OnDrag, Designer));
    view_OnUp(view, listener(app, i_OnUp, Designer));
    view_OnExit(view, listener(app, i_OnExit, Designer));
    view_OnClick(view, listener(app, i_OnClick, Designer));
    view_OnSize(view, listener(app, i_OnSize, Designer));
    app->canvas = view;
    return view;
}

/*---------------------------------------------------------------------------*/

static SplitView *i_middle_view(Designer *app)
{
    SplitView *split1 = splitview_vertical();
    SplitView *split2 = splitview_horizontal();
    SplitView *split3 = splitview_vertical();
    SplitView *split4 = splitview_horizontal();
    Panel *panel1 = i_forms_box(app);
    Panel *panel2 = i_widgets_box(app);
    Panel *panel3 = i_inspector_box(app);
    Panel *panel4 = i_propedit_box(app);
    View *view = i_canvas_view(app);
    splitview_splitview(split1, split2);
    splitview_splitview(split1, split3);
    splitview_panel(split2, panel1);
    splitview_panel(split2, panel2);
    splitview_view(split3, view, FALSE);
    splitview_splitview(split3, split4);
    splitview_panel(split4, panel3);
    splitview_panel(split4, panel4);
    splitview_minsize0(split1, 100);
    splitview_minsize0(split2, 100);
    splitview_minsize1(split2, 100);
    splitview_minsize0(split3, 50);
    splitview_minsize1(split3, 100);
    splitview_minsize0(split4, 100);
    splitview_minsize1(split4, 100);
    app->split1 = split1;
    app->split2 = split2;
    app->split3 = split3;
    app->split4 = split4;
    return split1;
}

/*---------------------------------------------------------------------------*/

static Layout *i_statusbar_layout(Designer *app)
{
    Layout *layout = layout_create(4, 1);
    Label *label1 = label_create();
    Label *label2 = label_create();
    Progress *progress = progress_create();

    label_align(label2, ekRIGHT);
    layout_label(layout, label1, 0, 0);
    layout_label(layout, label2, 3, 0);
    layout_progress(layout, progress, 1, 0);
    layout_hmargin(layout, 0, 10);

    /* All the horizontal expansion will be done in empty column-cell(2) */
    layout_hexpand(layout, 2);
    label_text(label1, "status-1");
    label_text(label2, "status-2");

    layout_margin4(layout, 0, 5, 5, 5);
    /* Keep the controls for futher updates */
    app->status_label = label1;
    app->cells_label = label2;
    app->progress = progress;
    return layout;
}

/*---------------------------------------------------------------------------*/

static Layout *i_main_layout(Designer *app)
{
    Layout *layout1 = layout_create(1, 3);
    Layout *layout2 = i_tools_layout(app);
    Layout *layout4 = i_statusbar_layout(app);
    SplitView *view = i_middle_view(app);
    layout_layout(layout1, layout2, 0, 0);
    layout_splitview(layout1, view, 0, 1);
    layout_layout(layout1, layout4, 0, 2);
    /*
     * All the vertical expansion will be done in the middle layout
     * tools_layout (top) and statusbar_layout (bottom) will preserve
     * the 'natural' height
     */
    layout_vexpand(layout1, 1);

    /* A vertical margins between middle and (controls, info) */
    layout_vmargin(layout1, 0, 5);
    layout_vmargin(layout1, 1, 5);

    return layout1;
}

/*---------------------------------------------------------------------------*/

static Panel *i_panel(Designer *app)
{
    Panel *panel = panel_create();
    Layout *layout = i_main_layout(app);
    panel_layout(panel, layout);
    return panel;
}

/*---------------------------------------------------------------------------*/

static void i_OnHotKey(Designer *app, Event *e)
{
    const EvKey *p = event_params(e, EvKey);
    cassert_no_null(app);
    if (p->key == ekKEY_SUPR)
    {
        if (app->config.sel_form != UINT32_MAX)
        {
            DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
            if (dform_OnSupr(form, app->inspect, app->propedit) == TRUE)
                view_update(app->canvas);
        }
    }
}

/*---------------------------------------------------------------------------*/

static gui_state_t i_bool_state(const bool_t state)
{
    if (state == TRUE)
        return ekGUI_ON;
    else 
        return ekGUI_OFF;
}

/*---------------------------------------------------------------------------*/

/* TODO: Remove when splits cache the divpos on hide */
static void i_save_splits(Designer *app)
{
    cassert_no_null(app);
    if (app->config.show_forms == TRUE && app->config.show_widgets == TRUE)
        app->config.split2_pos = splitview_get_pos(app->split2, i_SPLIT2_MODE);
    if (app->config.show_forms == TRUE || app->config.show_widgets == TRUE)
        app->config.split1_pos = splitview_get_pos(app->split1, i_SPLIT1_MODE);
    if (app->config.show_inspectr == TRUE && app->config.show_propedit == TRUE)
        app->config.split4_pos = splitview_get_pos(app->split4, i_SPLIT4_MODE);
    if (app->config.show_inspectr == TRUE || app->config.show_propedit == TRUE)
        app->config.split3_pos = splitview_get_pos(app->split3, i_SPLIT3_MODE);
}

/*---------------------------------------------------------------------------*/

static void i_restore_splits(Designer *app)
{
    cassert_no_null(app);
    if (app->config.show_forms == TRUE || app->config.show_widgets == TRUE)
    {
        splitview_visible0(app->split1, TRUE);
        splitview_pos(app->split1, i_SPLIT1_MODE, app->config.split1_pos);
    }
    else
    {
        splitview_visible0(app->split1, FALSE);
    }

    splitview_visible0(app->split2, app->config.show_forms);
    splitview_visible1(app->split2, app->config.show_widgets);

    if (app->config.show_forms == TRUE && app->config.show_widgets == TRUE)
        splitview_pos(app->split2, i_SPLIT2_MODE, app->config.split2_pos);        

    if (app->config.show_inspectr == TRUE || app->config.show_propedit == TRUE)
    {
        splitview_visible1(app->split3, TRUE);
        splitview_pos(app->split3, i_SPLIT3_MODE, app->config.split3_pos);
    }
    else
    {
        splitview_visible1(app->split3, FALSE);
    }

    splitview_visible0(app->split4, app->config.show_inspectr);
    splitview_visible1(app->split4, app->config.show_propedit);

    if (app->config.show_inspectr == TRUE && app->config.show_propedit == TRUE)
        splitview_pos(app->split4, i_SPLIT4_MODE, app->config.split4_pos);        
}

/*---------------------------------------------------------------------------*/

static void i_swap_show_item(Designer *app, bool_t *value, MenuItem *item)
{
    cassert_no_null(value);
    i_save_splits(app);
    *value = !*value;
    menuitem_state(item, i_bool_state(*value));
    i_restore_splits(app);
}

/*---------------------------------------------------------------------------*/

static void i_OnShowForms(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    i_swap_show_item(app, &app->config.show_forms, app->show_forms);
    window_update(app->window);
}

/*---------------------------------------------------------------------------*/

static void i_OnShowWidgets(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    i_swap_show_item(app, &app->config.show_widgets, app->show_widgets);
    window_update(app->window);
}

/*---------------------------------------------------------------------------*/

static void i_OnShowInspectr(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    i_swap_show_item(app, &app->config.show_inspectr, app->show_inspectr);
    window_update(app->window);
}

/*---------------------------------------------------------------------------*/

static void i_OnShowPropEdit(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    i_swap_show_item(app, &app->config.show_propedit, app->show_propedit);
    window_update(app->window);
}

/*---------------------------------------------------------------------------*/

static Menu *i_view_menu(Designer *app)
{
    Menu *menu = menu_create();
    MenuItem *item1 = menuitem_create();
    MenuItem *item2 = menuitem_create();
    MenuItem *item3 = menuitem_create();
    MenuItem *item4 = menuitem_create();
    cassert_no_null(app);
    menuitem_text(item1, "Forms box");
    menuitem_text(item2, "Widgets box");
    menuitem_text(item3, "Object inspector");
    menuitem_text(item4, "Property editor");
    menuitem_OnClick(item1, listener(app, i_OnShowForms, Designer));
    menuitem_OnClick(item2, listener(app, i_OnShowWidgets, Designer));
    menuitem_OnClick(item3, listener(app, i_OnShowInspectr, Designer));
    menuitem_OnClick(item4, listener(app, i_OnShowPropEdit, Designer));
    menu_add_item(menu, item1);
    menu_add_item(menu, item2);
    menu_add_item(menu, item3);
    menu_add_item(menu, item4);
    app->show_forms = item1;
    app->show_widgets = item2;
    app->show_inspectr = item3;
    app->show_propedit = item4;
    return menu;
}

/*---------------------------------------------------------------------------*/

static Menu *i_menu(Designer *app)
{
    Menu *menu = menu_create();
    Menu *submenu1 = i_view_menu(app);
    MenuItem *item1 = menuitem_create();
    menuitem_text(item1, "View");
    menuitem_submenu(item1, &submenu1);
    menu_add_item(menu, item1);
    return menu;
}

/*---------------------------------------------------------------------------*/

static void i_update_config(Designer *app)
{
    V2Df pos;
    S2Df size;
    cassert_no_null(app);
    pos = window_get_origin(app->window);
    size = window_get_size(app->window);
    app->config.wx = pos.x;
    app->config.wy = pos.y;
    app->config.wwidth = size.width;
    app->config.wheight = size.height;
    app->config.split1_pos = splitview_get_pos(app->split1, i_SPLIT1_MODE);
    app->config.split2_pos = splitview_get_pos(app->split2, i_SPLIT2_MODE);
    app->config.split3_pos = splitview_get_pos(app->split3, i_SPLIT3_MODE);
    app->config.split4_pos = splitview_get_pos(app->split4, i_SPLIT4_MODE);
}

/*---------------------------------------------------------------------------*/

static void i_save_config(const Designer *app)
{
    String *cfile = hfile_appdata("config.bin");
    Stream *stm = stm_to_file(tc(cfile), NULL);
    cassert_no_null(app);
    
    if (stm != NULL)
    {
        stm_write_u16(stm, i_CONFIG_VERS);
        str_write(stm, app->config.folder_path);
        stm_write_u32(stm, app->config.sel_form);
        stm_write_enum(stm, app->config.swidget, widget_t);
        stm_write_r32(stm, app->config.wx);
        stm_write_r32(stm, app->config.wy);
        stm_write_r32(stm, app->config.wwidth);
        stm_write_r32(stm, app->config.wheight);
        stm_write_r32(stm, app->config.split1_pos);
        stm_write_r32(stm, app->config.split2_pos);
        stm_write_r32(stm, app->config.split3_pos);
        stm_write_r32(stm, app->config.split4_pos);
        stm_write_bool(stm, app->config.show_forms);
        stm_write_bool(stm, app->config.show_widgets);
        stm_write_bool(stm, app->config.show_inspectr);
        stm_write_bool(stm, app->config.show_propedit);
        arrst_foreach_const(wdrawer, app->wdrawers, WDrawer)
            stm_write_bool(stm, wdrawer->opened);
        arrst_end()
        stm_close(&stm);
    }

    str_destroy(&cfile);
}

/*---------------------------------------------------------------------------*/

static void i_remove_config(Config *config)
{
    cassert_no_null(config);
    str_destopt(&config->folder_path);
}

/*---------------------------------------------------------------------------*/

static void i_default_config(Designer *app)
{
    cassert_no_null(app);
    i_remove_config(&app->config);
    app->config.vers = i_CONFIG_VERS;
    app->config.folder_path = str_c("");
    app->config.sel_form = UINT32_MAX;
    app->config.swidget = ekWIDGET_SELECT;
    app->config.wx = 100;
    app->config.wy = 100;
    app->config.wwidth = 850;
    app->config.wheight = 500;
    app->config.split1_pos = 200;
    app->config.split2_pos = 200;
    app->config.split3_pos = 200;
    app->config.split4_pos = 200;
    app->config.show_forms = TRUE;
    app->config.show_widgets = TRUE;
    app->config.show_inspectr = TRUE;
    app->config.show_propedit = TRUE;

    arrst_foreach(wdrawer, app->wdrawers, WDrawer)
        wdrawer->opened = FALSE;
    arrst_end()
}
    
/*---------------------------------------------------------------------------*/

static void i_load_config(Designer *app)
{
    String *cfile = hfile_appdata("config.bin");
    Stream *stm = stm_from_file(tc(cfile), NULL);
    bool_t ok = FALSE;
    cassert_no_null(app);
    i_remove_config(&app->config);
    if (stm != NULL)
    {
        app->config.vers = stm_read_u16(stm);
        if (app->config.vers <= i_CONFIG_VERS)
        {
            app->config.folder_path = str_read(stm);
            app->config.sel_form = stm_read_u32(stm);
            app->config.swidget = stm_read_enum(stm, widget_t);
            app->config.wx = stm_read_r32(stm);
            app->config.wy = stm_read_r32(stm);
            app->config.wwidth = stm_read_r32(stm);
            app->config.wheight = stm_read_r32(stm);
            app->config.split1_pos = stm_read_r32(stm);
            app->config.split2_pos = stm_read_r32(stm);
            app->config.split3_pos = stm_read_r32(stm);
            app->config.split4_pos = stm_read_r32(stm);
            app->config.show_forms = stm_read_bool(stm);
            app->config.show_widgets = stm_read_bool(stm);
            app->config.show_inspectr = stm_read_bool(stm);
            app->config.show_propedit = stm_read_bool(stm);
            arrst_foreach(wdrawer, app->wdrawers, WDrawer)
                wdrawer->opened = stm_read_bool(stm);
            arrst_end()
            ok = stm_state(stm) == ekSTOK;
        }

        stm_close(&stm);
    }

    if (ok == FALSE)
        i_default_config(app);

    str_destroy(&cfile);
}
    
/*---------------------------------------------------------------------------*/

static void i_apply_config(Designer *app)
{
    cassert_no_null(app);
    window_origin(app->window, v2df(app->config.wx, app->config.wy));
    window_size(app->window, s2df(app->config.wwidth, app->config.wheight));
    i_restore_splits(app);
    i_set_bwidget(app->config.swidget, app->bwidgets, app->default_font, app->bold_font);
    menuitem_state(app->show_forms, i_bool_state(app->config.show_forms));
    menuitem_state(app->show_widgets, i_bool_state(app->config.show_widgets));
    menuitem_state(app->show_inspectr, i_bool_state(app->config.show_inspectr));
    menuitem_state(app->show_propedit, i_bool_state(app->config.show_propedit));
    window_update(app->window);
}

/*---------------------------------------------------------------------------*/

static void i_OnClose(Designer *app, Event *e)
{
    i_update_config(app);
    i_save_config(app);
    osapp_finish();
    unref(e);
}

/*---------------------------------------------------------------------------*/

static void i_add_drawer(ArrSt(WDrawer) *wdrawers, const drawer_t type, ResId labelid)
{
    WDrawer *wdrawer = arrst_new0(wdrawers, WDrawer);
    wdrawer->type = type;
    wdrawer->labelid = labelid;
}

/*---------------------------------------------------------------------------*/

static void i_add_widget(ArrSt(BWidget) *bwidgets, const widget_t twidget, ResId labelid, ResId imageid, const drawer_t drawer)
{
    BWidget *bwidget = arrst_new0(bwidgets, BWidget);
    bwidget->twidget = twidget;
    bwidget->labelid = labelid;
    bwidget->imageid = imageid;
    bwidget->drawer = drawer;
}

/*---------------------------------------------------------------------------*/

static Designer *i_app(void)
{
    Designer *app = heap_new0(Designer);
    gui_respack(res_designer_respack);
    gui_language("");
    dgui_init();
    nflib_start();
    i_dbind();
    dialog_dbind();    
    dlayout_global_init();
    app->forms = arrpt_create(DForm);
    app->add_icon = image_copy(gui_image(PLUS16_PNG));
    app->default_font = font_system(font_regular_size(), 0);
    app->bold_font = font_system(font_regular_size(), ekFBOLD);
    app->wdrawers = arrst_create(WDrawer);
    app->bwidgets = arrst_create(BWidget);
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_SELECT, "");
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_LAYOUTS, TEXT_LAYOUTS);
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_BUTTONS, TEXT_BUTTONS);
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_TEXT, TEXT_TEXT_WIDGETS);
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_ITEMS, TEXT_ITEM_WIDGETS);
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_OTHERS, TEXT_OTHER_WIDGETS);
    i_add_drawer(app->wdrawers, ekDRAWER_LAYOUT_PROPS, TEXT_LAYOUT_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_COLUMN_PROPS, TEXT_COLUMN_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_ROW_PROPS, TEXT_ROW_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_CELL_PROPS, TEXT_CELL_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_LABEL_PROPS, TEXT_LABEL_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_BUTTON_PROPS, TEXT_BUTTON_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_CHECKBOX_PROPS, TEXT_CHECK_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_RADIO_PROPS, TEXT_RADIO_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_TOOL_PROPS, TEXT_TOOL_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_POPUP_PROPS, TEXT_POPUP_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_EDIT_PROPS, TEXT_EDIT_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_COMBO_PROPS, TEXT_COMBO_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_LIST_PROPS, TEXT_LIST_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_HSLIDER_PROPS, TEXT_SLIDER_PROPS);    
    i_add_drawer(app->wdrawers, ekDRAWER_VSLIDER_PROPS, TEXT_VSLIDER_PROPS);  
    i_add_drawer(app->wdrawers, ekDRAWER_PROGRESS_PROPS, TEXT_PROGRESS_PROPS);  
    i_add_drawer(app->wdrawers, ekDRAWER_TEXT_PROPS, TEXT_TEXT_PROPS);  
    i_add_drawer(app->wdrawers, ekDRAWER_IMAGE_PROPS, TEXT_IMAGE_PROPS);  
    i_add_widget(app->bwidgets, ekWIDGET_SELECT, TEXT_SELECT, CURSOR_PNG, ekDRAWER_WIDGET_SELECT);
    i_add_widget(app->bwidgets, ekWIDGET_VERT_LAYOUT, TEXT_VERT_LAYOUT, VLAYOUT_PNG, ekDRAWER_WIDGET_LAYOUTS);
    i_add_widget(app->bwidgets, ekWIDGET_HORZ_LAYOUT, TEXT_HORZ_LAYOUT, HLAYOUT_PNG, ekDRAWER_WIDGET_LAYOUTS);
    i_add_widget(app->bwidgets, ekWIDGET_GRID_LAYOUT, TEXT_GRID_LAYOUT, GLAYOUT_PNG, ekDRAWER_WIDGET_LAYOUTS);
    i_add_widget(app->bwidgets, ekWIDGET_PUSH_BUTTON, TEXT_PUSH_BUTTON, PUSHBUT_PNG, ekDRAWER_WIDGET_BUTTONS);
    i_add_widget(app->bwidgets, ekWIDGET_CHECK_BUTTON, TEXT_CHECK_BOX, CHECBUT_PNG, ekDRAWER_WIDGET_BUTTONS);
    i_add_widget(app->bwidgets, ekWIDGET_RADIO_BUTTON, TEXT_RADIO_BUTTON, RADBUT_PNG, ekDRAWER_WIDGET_BUTTONS);
    i_add_widget(app->bwidgets, ekWIDGET_TOOL_BUTTON, TEXT_TOOL_BUTTON, TOOLBUT_PNG, ekDRAWER_WIDGET_BUTTONS);
    i_add_widget(app->bwidgets, ekWIDGET_LABEL, TEXT_LABEL, LABEL_PNG, ekDRAWER_WIDGET_TEXT);
    i_add_widget(app->bwidgets, ekWIDGET_EDITBOX, TEXT_EDIT_BOX, EDITBOX_PNG, ekDRAWER_WIDGET_TEXT);
    i_add_widget(app->bwidgets, ekWIDGET_COMBOBOX, TEXT_COMBO_BOX, COMBOBOX_PNG, ekDRAWER_WIDGET_TEXT);
    i_add_widget(app->bwidgets, ekWIDGET_TEXTVIEW, TEXT_TEXT_VIEW, TEXTVIEW_PNG, ekDRAWER_WIDGET_TEXT);
    i_add_widget(app->bwidgets, ekWIDGET_LISTBOX, TEXT_LIST_BOX, LISTVIEW_PNG, ekDRAWER_WIDGET_ITEMS);
    i_add_widget(app->bwidgets, ekWIDGET_POPUP, TEXT_POPUP_BUTTON, POPUP_PNG, ekDRAWER_WIDGET_ITEMS);
    i_add_widget(app->bwidgets, ekWIDGET_TABLEVIEW, TEXT_TABLE_VIEW, TABLEVIEW_PNG, ekDRAWER_WIDGET_ITEMS);
    i_add_widget(app->bwidgets, ekWIDGET_IMAGEVIEW, TEXT_IMAGE_VIEW, IMAGEVIEW_PNG, ekDRAWER_WIDGET_OTHERS);
    i_add_widget(app->bwidgets, ekWIDGET_HORZ_SLIDER, TEXT_HORZ_SLIDER, HORSLIDER_PNG, ekDRAWER_WIDGET_OTHERS);
    i_add_widget(app->bwidgets, ekWIDGET_VERT_SLIDER, TEXT_VERT_SLIDER, VERSLIDER_PNG, ekDRAWER_WIDGET_OTHERS);
    i_add_widget(app->bwidgets, ekWIDGET_PROGRESS, TEXT_PROGRESS_BAR, PROGRESSBAR_PNG, ekDRAWER_WIDGET_OTHERS);
    i_load_config(app);
    return app;
}

/*---------------------------------------------------------------------------*/

static Designer *i_create(void)
{
    Designer *app = i_app();
    Panel *panel = i_panel(app);
    app->window = window_create(ekWINDOW_STDRES);
    app->menu = i_menu(app);
    window_panel(app->window, panel);
    window_title(app->window, "GTNAP Designer");
    window_OnClose(app->window, listener(app, i_OnClose, Designer));
    window_hotkey(app->window, ekKEY_SUPR, 0, listener(app, i_OnHotKey, Designer));
    i_apply_config(app);
    window_show(app->window);
    osapp_menubar(app->menu, app->window);
    i_init_forms(app, tc(app->config.folder_path));
    return app;
}

/*---------------------------------------------------------------------------*/

static void i_destroy(Designer **app)
{
    cassert_no_null(app);
    cassert_no_null(*app);
    i_remove_config(&(*app)->config);
    image_destroy(&(*app)->add_icon);
    font_destroy(&(*app)->default_font);
    font_destroy(&(*app)->bold_font);
    arrst_destroy(&(*app)->wdrawers, NULL, WDrawer);
    arrst_destroy(&(*app)->bwidgets, NULL, BWidget);
    arrpt_destroy(&(*app)->forms, i_destroy_form_opt, DForm);
    menu_destroy(&(*app)->menu);
    window_destroy(&(*app)->window);
    nflib_finish();
    heap_delete(app, Designer);
}

/*---------------------------------------------------------------------------*/

static void i_update(Designer *app, const real64_t prtime, const real64_t ctime)
{
    unref(app);
    unref(prtime);
    unref(ctime);
}

/*---------------------------------------------------------------------------*/

void designer_need_save(Designer *app)
{
    i_update_form_controls(app, TRUE);
}

/*---------------------------------------------------------------------------*/

void designer_canvas_update(Designer *app)
{
    cassert_no_null(app);
    view_update(app->canvas);
}

/*---------------------------------------------------------------------------*/

void designer_inspect_update(Designer *app)
{
    cassert_no_null(app);
    inspect_update(app->inspect);
}

/*---------------------------------------------------------------------------*/

void designer_inspect_select(Designer *app, const uint32_t row)
{
    cassert_no_null(app);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        dform_inspect_select(form, app->propedit, row);
    }
}

/*---------------------------------------------------------------------------*/

const char_t *designer_folder_path(const Designer *app)
{
    cassert_no_null(app);
    return tc(app->config.folder_path);
}

/*---------------------------------------------------------------------------*/

const Font *designer_default_font(const Designer *app)
{
    cassert_no_null(app);
    return app->default_font;
}

/*---------------------------------------------------------------------------*/

Window *designer_main_window(const Designer *app)
{
    cassert_no_null(app);
    return app->window;
}

/*---------------------------------------------------------------------------*/

Panel *designer_drawer(Designer *app, Panel *child, const drawer_t drawer)
{
    WDrawer *wdrawer = i_find_drawer_by_type(app, drawer);
    cassert_no_null(wdrawer);
    cassert(wdrawer->panel == NULL);
    wdrawer->panel = dgui_drawer(gui_text(wdrawer->labelid), app->default_font, child, wdrawer->opened, listener(app, i_OnDrawerChange, Designer));    
    return wdrawer->panel;
}

/*---------------------------------------------------------------------------*/

#include <osapp/osmain.h>
osmain_sync(0.1, i_create, i_destroy, i_update, "", Designer)
