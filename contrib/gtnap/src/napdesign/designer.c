/* NAppGUI Designer Application */

#include <nflib/nflib.h>
#include <nappgui.h>
#include "res_designer.h"
#include "dlayout.h"
#include "dform.h"
#include "dialogs.h"
#include "propedit.h"
#include "inspect.h"

typedef struct _config_t Config;

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

struct _desiger_t
{
    Window *window;
    Config config;
    Menu *menu;
    ArrPt(DForm) *forms;
    ListBox *form_list;
    Label *status_label;
    Label *cells_label;
    Progress *progress;
    View *canvas;
    Panel *inspect;
    Panel *propedit;
    Layout *widgets_layout;
    Cell *widgets_cell;
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

/*---------------------------------------------------------------------------*/

static void i_dbind(void)
{
    dbind_enum(widget_t, ekWIDGET_SELECT, "");
    dbind_enum(widget_t, ekWIDGET_GRID_LAYOUT, "");
    dbind_enum(widget_t, ekWIDGET_LABEL, "");
    dbind_enum(widget_t, ekWIDGET_BUTTON, "");
    dbind_enum(widget_t, ekWIDGET_CHECKBOX, "");
    dbind_enum(widget_t, ekWIDGET_EDITBOX, "");
    dbind_enum(widget_t, ekWIDGET_TEXTVIEW, "");
    dbind_enum(widget_t, ekWIDGET_IMAGEVIEW, "");
    dbind_enum(widget_t, ekWIDGET_SLIDER, "");
    dbind_enum(widget_t, ekWIDGET_PROGRESS, "");
    dbind_enum(widget_t, ekWIDGET_POPUP, "");
    dbind_enum(widget_t, ekWIDGET_LISTBOX, "");
    dbind_enum(widget_t, ekWIDGET_TABLEVIEW, "");    
    dbind(Designer, widget_t, config.swidget);
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
    const char_t *name = listbox_text(listbox, index);
    if (str_is_prefix(name, i_SAVE_MARK) == TRUE)
        name += str_len_c(i_SAVE_MARK);
    return name;
}

/*---------------------------------------------------------------------------*/

static ___INLINE bool_t i_with_save_mark(const ListBox *listbox, const uint32_t index)
{
    const char_t *name = listbox_text(listbox, index);
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
    cell_enabled(app->widgets_cell, enable);
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

static Panel *i_widgets_panel(Designer *app)
{
    Panel *panel = panel_custom(FALSE, TRUE, FALSE);
    Layout *layout1 = layout_create(1, 1);
    Layout *layout2 = layout_create(1, 13);
    Button *radio1 = button_radio();
    Button *radio2 = button_radio();
    Button *radio3 = button_radio();
    Button *radio4 = button_radio();
    Button *radio5 = button_radio();
    Button *radio6 = button_radio();
    Button *radio7 = button_radio();
    Button *radio8 = button_radio();
    Button *radio9 = button_radio();
    Button *radio10 = button_radio();
    Button *radio11 = button_radio();
    Button *radio12 = button_radio();
    Button *radio13 = button_radio();
    cassert_no_null(app);
    button_text(radio1, "Select");
    button_text(radio2, "Grid layout");
    button_text(radio3, "Label");
    button_text(radio4, "Button");
    button_text(radio5, "Checkbox");
    button_text(radio6, "Editbox");
    button_text(radio7, "TextView");
    button_text(radio8, "ImageView");
    button_text(radio9, "Slider");
    button_text(radio10, "Progress");
    button_text(radio11, "PopUp");
    button_text(radio12, "ListBox");
    button_text(radio13, "TableView");
    layout_button(layout2, radio1, 0, 0);
    layout_button(layout2, radio2, 0, 1);
    layout_button(layout2, radio3, 0, 2);
    layout_button(layout2, radio4, 0, 3);
    layout_button(layout2, radio5, 0, 4);
    layout_button(layout2, radio6, 0, 5);
    layout_button(layout2, radio7, 0, 6);
    layout_button(layout2, radio8, 0, 7);
    layout_button(layout2, radio9, 0, 8);
    layout_button(layout2, radio10, 0, 9);
    layout_button(layout2, radio11, 0, 10);
    layout_button(layout2, radio12, 0, 11);
    layout_button(layout2, radio13, 0, 12);
    layout_vmargin(layout2, 0, 5);
    layout_vmargin(layout2, 1, 5);
    layout_vmargin(layout2, 2, 5);
    layout_vmargin(layout2, 3, 5);
    layout_vmargin(layout2, 4, 5);
    layout_vmargin(layout2, 5, 5);
    layout_vmargin(layout2, 6, 5);
    layout_vmargin(layout2, 7, 5);
    layout_vmargin(layout2, 8, 5);
    layout_vmargin(layout2, 9, 5);
    layout_vmargin(layout2, 10, 5);
    layout_vmargin(layout2, 11, 5);
    layout_valign(layout1, 0, 0, ekTOP);
    layout_layout(layout1, layout2, 0, 0);
    panel_layout(panel, layout1);
    panel_size(panel, s2df(-1, 200));
    cell_dbind(layout_cell(layout1, 0, 0), Designer, widget_t, config.swidget);
    app->widgets_layout = layout2;
    app->widgets_cell = layout_cell(layout1, 0, 0);
    return panel;
}

/*---------------------------------------------------------------------------*/

static Panel *i_widgets_box(Designer *app)
{
    Panel *panel1 = panel_create();
    Panel *panel2 = i_widgets_panel(app);
    Layout *layout = layout_create(1, 2);
    Label *label = label_create();
    label_text(label, "Widgets");
    layout_label(layout, label, 0, 0);
    layout_panel(layout, panel2, 0, 1);
    layout_vmargin(layout, 0, 5);
    layout_vexpand(layout, 1);
    layout_margin4(layout, 0, 5, 5, 5);
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
    Label *label = label_create();
    ListBox *list = listbox_create();
    cassert_no_null(app);
    label_text(label, "Forms");
    listbox_size(list, s2df(150, 100));
    listbox_OnSelect(list, listener(app, i_OnFormSelect, Designer));
    layout_label(layout, label, 0, 0);
    layout_listbox(layout, list, 0, 1);
    layout_vmargin(layout, 0, 5);
    layout_vexpand(layout, 1);
    layout_margin4(layout, 0, 5, 5, 5);
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
    Label *label = label_create();
    cassert_no_null(app);
    label_text(label, "Object inspector");
    layout_label(layout, label, 0, 0);
    layout_panel(layout, panel2, 0, 1);
    layout_vmargin(layout, 0, 5);
    layout_vexpand(layout, 1);
    layout_margin4(layout, 0, 5, 5, 5);
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
    Label *label = label_create();
    cassert_no_null(app);
    label_text(label, "Property editor");
    layout_label(layout, label, 0, 0);
    layout_panel(layout, panel2, 0, 1);
    layout_vmargin(layout, 0, 5);
    layout_vexpand(layout, 1);
    layout_margin4(layout, 0, 0, 5, 5);
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
    const EvMouse *p = event_params(e, EvMouse);
    cassert_no_null(app);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        if (dform_OnMove(form, p->x, p->y) == TRUE)
            view_update(app->canvas);
    }
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
        if (dform_OnClick(form, app->window, app->inspect, app->propedit, app->config.swidget, p->x, p->y, p->button) == TRUE)
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
    splitview_split(split1, split2);
    splitview_split(split1, split3);
    splitview_panel(split2, panel1);
    splitview_panel(split2, panel2);
    splitview_view(split3, view, FALSE);
    splitview_split(split3, split4);
    splitview_panel(split4, panel3);
    splitview_panel(split4, panel4);
    splitview_mode(split1, i_SPLIT1_MODE);
    splitview_mode(split2, i_SPLIT2_MODE);
    splitview_mode(split3, i_SPLIT3_MODE);
    splitview_mode(split4, i_SPLIT4_MODE);    
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

static void i_save_config(const Config *config)
{
    String *cfile = hfile_appdata("config.bin");
    Stream *stm = stm_to_file(tc(cfile), NULL);
    cassert_no_null(config);
    
    if (stm != NULL)
    {
        stm_write_u16(stm, i_CONFIG_VERS);
        str_write(stm, config->folder_path);
        stm_write_u32(stm, config->sel_form);
        stm_write_enum(stm, config->swidget, widget_t);
        stm_write_r32(stm, config->wx);
        stm_write_r32(stm, config->wy);
        stm_write_r32(stm, config->wwidth);
        stm_write_r32(stm, config->wheight);
        stm_write_r32(stm, config->split1_pos);
        stm_write_r32(stm, config->split2_pos);
        stm_write_r32(stm, config->split3_pos);
        stm_write_r32(stm, config->split4_pos);
        stm_write_bool(stm, config->show_forms);
        stm_write_bool(stm, config->show_widgets);
        stm_write_bool(stm, config->show_inspectr);
        stm_write_bool(stm, config->show_propedit);
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

static void i_default_config(Config *config)
{
    cassert_no_null(config);
    i_remove_config(config);
    config->vers = i_CONFIG_VERS;
    config->folder_path = str_c("");
    config->sel_form = UINT32_MAX;
    config->swidget = ekWIDGET_SELECT;
    config->wx = 100;
    config->wy = 100;
    config->wwidth = 850;
    config->wheight = 500;
    config->split1_pos = 200;
    config->split2_pos = 200;
    config->split3_pos = 200;
    config->split4_pos = 200;
    config->show_forms = TRUE;
    config->show_widgets = TRUE;
    config->show_inspectr = TRUE;
    config->show_propedit = TRUE;
}

/*---------------------------------------------------------------------------*/

static void i_load_config(Config *config)
{
    String *cfile = hfile_appdata("config.bin");
    Stream *stm = stm_from_file(tc(cfile), NULL);
    bool_t ok = FALSE;
    cassert_no_null(config);
    i_remove_config(config);
    if (stm != NULL)
    {
        config->vers = stm_read_u16(stm);
        if (config->vers <= i_CONFIG_VERS)
        {
            config->folder_path = str_read(stm);
            config->sel_form = stm_read_u32(stm);
            config->swidget = stm_read_enum(stm, widget_t);
            config->wx = stm_read_r32(stm);
            config->wy = stm_read_r32(stm);
            config->wwidth = stm_read_r32(stm);
            config->wheight = stm_read_r32(stm);
            config->split1_pos = stm_read_r32(stm);
            config->split2_pos = stm_read_r32(stm);
            config->split3_pos = stm_read_r32(stm);
            config->split4_pos = stm_read_r32(stm);
            config->show_forms = stm_read_bool(stm);
            config->show_widgets = stm_read_bool(stm);
            config->show_inspectr = stm_read_bool(stm);
            config->show_propedit = stm_read_bool(stm);
            ok = stm_state(stm) == ekSTOK;
        }

        stm_close(&stm);
    }

    if (ok == FALSE)
        i_default_config(config);

    str_destroy(&cfile);
}

/*---------------------------------------------------------------------------*/

static void i_apply_config(Designer *app)
{
    cassert_no_null(app);
    window_origin(app->window, v2df(app->config.wx, app->config.wy));
    window_size(app->window, s2df(app->config.wwidth, app->config.wheight));
    i_restore_splits(app);
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
    i_save_config(&app->config);
    osapp_finish();
    unref(e);
}

/*---------------------------------------------------------------------------*/

static Designer *i_app(void)
{
    Designer *app = heap_new0(Designer);
    gui_respack(res_designer_respack);
    gui_language("");
    nflib_start();
    i_dbind();
    dialog_dbind();    
    i_load_config(&app->config);
    dlayout_global_init();
    app->forms = arrpt_create(DForm);
    app->add_icon = image_copy(gui_image(PLUS16_PNG));
    app->default_font = font_system(font_regular_size(), 0);
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
    window_origin(app->window, v2df(500, 200));
    window_OnClose(app->window, listener(app, i_OnClose, Designer));
    window_hotkey(app->window, ekKEY_SUPR, 0, listener(app, i_OnHotKey, Designer));
    window_show(app->window);
    osapp_menubar(app->menu, app->window);
    i_apply_config(app);
    layout_dbind(app->widgets_layout, NULL, Designer);
    layout_dbind_obj(app->widgets_layout, app, Designer);
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

Window *designer_main_window(const Designer *app)
{
    cassert_no_null(app);
    return app->window;
}

/*---------------------------------------------------------------------------*/

#include <osapp/osmain.h>
osmain_sync(0.1, i_create, i_destroy, i_update, "", Designer)
