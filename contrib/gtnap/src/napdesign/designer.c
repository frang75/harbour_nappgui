/* NAppGUI Designer Application */

#include <nflib/nflib.h>
#include <nappgui.h>
#include "designer.h"
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
    bool_t is_maximized;
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

struct _designer_t
{
    Window *window;
    Config config;
    Menu *menu;
    ArrPt(DForm) *forms;
    ArrSt(WDrawer) *wdrawers;
    ArrSt(BWidget) *bwidgets;
    DClipBoard clipboard;
    cmode_t cmode;
    ListBox *form_list;
    Label *undo_label;
    Label *cells_label;
    Progress *progress;
    View *canvas;
    Panel *inspect;
    Panel *propedit;
    Cell *open_form_cell;
    Cell *save_form_cell;
    Cell *run_form_cell;
    Cell *resize_form_cell;
    Cell *add_form_cell;
    Cell *skeleton_form_cell;
    Cell *remove_form_cell;
    Cell *rename_form_cell;
    Cell *cut_cell;
    Cell *copy_cell;
    Cell *paste_cell;
    Cell *undo_cell;
    Cell *redo_cell;
    SplitView *split1;
    SplitView *split2;
    SplitView *split3;
    SplitView *split4;
    MenuItem *show_forms_item;
    MenuItem *show_widgets_item;
    MenuItem *show_inspectr_item;
    MenuItem *show_propedit_item;
    MenuItem *layout_skeleton_item;
    MenuItem *cut_item;
    MenuItem *copy_item;
    MenuItem *paste_item;
    MenuItem *undo_item;
    MenuItem *redo_item;
    Font *default_font;
    Font *bold_font;
    bool_t dragging;
    bool_t focus;
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
static DColors i_COLORS;
DeclPt(DForm);
DeclSt(BWidget);
DeclSt(WDrawer);

/*---------------------------------------------------------------------------*/

static bool_t i_close_app(Designer *);
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
    cell_enabled(app->resize_form_cell, enable_run);
    cell_enabled(app->add_form_cell, enable);
    cell_enabled(app->skeleton_form_cell, enable_run);
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
            bool_t can_undo = dform_can_undo(form);
            bool_t can_redo = dform_can_redo(form);
            dform_compose(form);
            dform_set(form, app->inspect, app->propedit);
            dform_origin(form, v2df(50, 50));
            designer_undo_controls(app, can_undo, can_redo);
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
        uint8_t ret = dialog_unsaved_changes(app->window, app->default_font, gui_text(TEXT_UNSAVED1));
        if (ret == 1)
            i_save_forms(app);
        else if (ret == 2)
            can_open = FALSE;
    }

    if (can_open == TRUE)
    {
        const char_t *folder = comwin_select_dir(app->window, gui_text(TEXT_OPEN_CAPTION), tc(app->config.folder_path));
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

static void i_OnQuitClick(Designer *app, Event *e)
{
    i_close_app(app);
    unref(e);
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
    String *filename = NULL;
    String *desc = NULL;
    bool_t ok = FALSE;
    cassert_no_null(app);
    unref(e);
    ok = dialog_new_form(app->window, app->default_font, &filename, &desc);
    if (ok == TRUE)
    {
        if (i_exists_form_name(app, tc(filename)) == FALSE)
        {
            uint32_t n = listbox_count(app->form_list);
            DForm *form = dform_empty(app);
            dform_description(form, tc(desc));
            dform_compose(form);
            cassert(n == arrpt_size(app->forms, DForm));
            listbox_add_elem(app->form_list, tc(filename), NULL);
            listbox_select(app->form_list, n, TRUE);
            arrpt_append(app->forms, form, DForm);
            app->config.sel_form = n;
            i_update_form_controls(app, TRUE);
            view_update(app->canvas);
        }
        else
        {
            dialog_form_name_exists(app->window, app->default_font, tc(filename));
        }
    }

    str_destopt(&filename);
    str_destopt(&desc);
}

/*---------------------------------------------------------------------------*/

static void i_OnPropsFormClick(Designer *app, Event *e)
{
    DForm *form = NULL;
    const char_t *name = NULL;
    String *filename = NULL;
    String *desc = NULL;
    bool_t ok = FALSE;
    cassert_no_null(app);
    unref(e);
    form = arrpt_get(app->forms, app->config.sel_form, DForm);
    name = i_list_text(app->form_list, app->config.sel_form);
    filename = str_c(name);
    desc = str_c(dform_get_description(form));
    ok = dialog_props_form(app->window, app->default_font, &filename, &desc);
    if (ok == TRUE)
    {
        if (str_equ(filename, name) == TRUE)
        {
            dform_description(form, tc(desc));
        }
        else if (i_exists_form_name(app, tc(filename)) == FALSE)
        {
            String *oldpath = str_cpath("%s/%s.%s", tc(app->config.folder_path), name, i_FILE_EXT);
            bool_t renamed = FALSE;
            if (hfile_exists(tc(oldpath), NULL) == TRUE)
            {
                String *newpath = str_cpath("%s/%s.%s", tc(app->config.folder_path), tc(filename), i_FILE_EXT);
                renamed = bfile_rename(tc(oldpath), tc(newpath), NULL);
                str_destroy(&newpath);
            }

            if (renamed == TRUE)
            {
                bool_t with_bullet = i_with_save_mark(app->form_list, app->config.sel_form);
                if (with_bullet == TRUE)
                {
                    String *rname = str_printf("%s%s", i_SAVE_MARK, tc(filename));
                    listbox_set_elem(app->form_list, app->config.sel_form, tc(rname), NULL);
                    str_destroy(&rname);
                }
                else
                {
                    listbox_set_elem(app->form_list, app->config.sel_form, tc(filename), NULL);
                }

                dform_description(form, tc(desc));
            }
            else
            {
                /* RENAME ERROR */
                dialog_form_name_exists(app->window, app->default_font, tc(filename));
            }

            str_destroy(&oldpath);
        }
        else
        {
            dialog_form_name_exists(app->window, app->default_font, tc(filename));
        }

        i_update_form_controls(app, TRUE);
    }

    str_destroy(&filename);
    str_destroy(&desc);
}

/*---------------------------------------------------------------------------*/

static void i_OnSimulateClick(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        const char_t *name = i_list_text(app->form_list, app->config.sel_form);
        dform_simulate(form, name, app->window);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnResizableClick(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        const char_t *name = i_list_text(app->form_list, app->config.sel_form);
        dform_simulate_resizable(form, name, app->window);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnSkeletonClick(Designer *app, Event *e)
{
    Button *button = NULL;
    cassert_no_null(app);
    unref(e);
    button = cell_button(app->skeleton_form_cell);
    if (app->cmode == ekCMODE_DETAIL)
    {
        app->cmode = ekCMODE_SKELETON;
        button_state(button, ekGUI_ON);
        menuitem_state(app->layout_skeleton_item, ekGUI_ON);
    }
    else
    {
        app->cmode = ekCMODE_DETAIL;
        button_state(button, ekGUI_OFF);
        menuitem_state(app->layout_skeleton_item, ekGUI_OFF);
    }

    view_update(app->canvas);
}

/*---------------------------------------------------------------------------*/

static void i_OnRemoveClick(Designer *app, Event *e)
{
    const char_t *name = NULL;
    cassert_no_null(app);
    unref(e);
    name = i_list_text(app->form_list, app->config.sel_form);
    if (dialog_remove_form(app->window, app->default_font, name) == TRUE)
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

static void i_OnCutClick(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        bool_t ok = dform_OnCopy(form, &app->clipboard);
        cassert_unref(ok == TRUE, ok);
        ok = dform_OnSupr(form, app->inspect, app->propedit);
        cassert_unref(ok == TRUE, ok);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnCopyClick(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        bool_t ok = dform_OnCopy(form, &app->clipboard);
        cassert_unref(ok == TRUE, ok);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnPasteClick(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        bool_t ok = dform_OnPaste(form, &app->clipboard, app->inspect, app->propedit);
        cassert_unref(ok == TRUE, ok);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnUndoClick(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        bool_t ok = dform_OnUndo(form, app->inspect, app->propedit);
        if (ok == TRUE)
        {
            bool_t can_undo = dform_can_undo(form);
            bool_t can_redo = dform_can_redo(form);
            designer_undo_controls(app, can_undo, can_redo);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnRedoClick(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        bool_t ok = dform_OnRedo(form, app->inspect, app->propedit);
        if (ok == TRUE)
        {
            bool_t can_undo = dform_can_undo(form);
            bool_t can_redo = dform_can_redo(form);
            designer_undo_controls(app, can_undo, can_redo);
        }
    }
}

/*---------------------------------------------------------------------------*/

static Layout *i_tools_layout(Designer *app)
{
    Layout *layout = layout_create(14, 1);
    Button *button1 = button_flat();
    Button *button2 = button_flat();
    Button *button3 = button_flat();
    Button *button4 = button_flat();
    Button *button5 = button_flat();
    Button *button6 = button_flat();
    Button *button7 = button_flatgle();
    Button *button8 = button_flat();
    Button *button9 = button_flat();
    Button *button10 = button_flat();
    Button *button11 = button_flat();
    Button *button12 = button_flat();
    Button *button13 = button_flat();
    cassert_no_null(app);
    button_image(button1, cast_const(OPEN_PNG, Image));
    button_image(button2, cast_const(SAVE_PNG, Image));
    button_image(button3, cast_const(NEW_PNG, Image));
    button_image(button4, cast_const(PROPS_PNG, Image));
    button_image(button5, cast_const(SHOW_PNG, Image));
    button_image(button6, cast_const(RESIZE24_PNG, Image));
    button_image(button7, cast_const(LSKEL24_PNG, Image));
    button_image(button8, cast_const(REMOVE_PNG, Image));
    button_image(button9, cast_const(CUT_PNG, Image));
    button_image(button10, cast_const(COPY_PNG, Image));
    button_image(button11, cast_const(PASTE_PNG, Image));
    button_image(button12, cast_const(UNDO_PNG, Image));
    button_image(button13, cast_const(REDO_PNG, Image));
    button_hpadding(button1, 6);
    button_hpadding(button2, 6);
    button_hpadding(button3, 6);
    button_hpadding(button4, 6);
    button_hpadding(button5, 6);
    button_hpadding(button6, 6);
    button_hpadding(button7, 6);
    button_hpadding(button8, 6);
    button_hpadding(button9, 6);
    button_hpadding(button10, 6);
    button_hpadding(button11, 6);
    button_hpadding(button12, 6);
    button_hpadding(button13, 6);
    button_vpadding(button1, 6);
    button_vpadding(button2, 6);
    button_vpadding(button3, 6);
    button_vpadding(button4, 6);
    button_vpadding(button5, 6);
    button_vpadding(button6, 6);
    button_vpadding(button7, 6);
    button_vpadding(button8, 6);
    button_vpadding(button9, 6);
    button_vpadding(button10, 6);
    button_vpadding(button11, 6);
    button_vpadding(button12, 6);
    button_vpadding(button13, 6);
    button_OnClick(button1, listener(app, i_OnOpenFormsClick, Designer));
    button_OnClick(button2, listener(app, i_OnSaveFormsClick, Designer));
    button_OnClick(button3, listener(app, i_OnAddFormClick, Designer));
    button_OnClick(button4, listener(app, i_OnPropsFormClick, Designer));
    button_OnClick(button5, listener(app, i_OnSimulateClick, Designer));
    button_OnClick(button6, listener(app, i_OnResizableClick, Designer));
    button_OnClick(button7, listener(app, i_OnSkeletonClick, Designer));
    button_OnClick(button8, listener(app, i_OnRemoveClick, Designer));
    button_OnClick(button9, listener(app, i_OnCutClick, Designer));
    button_OnClick(button10, listener(app, i_OnCopyClick, Designer));
    button_OnClick(button11, listener(app, i_OnPasteClick, Designer));
    button_OnClick(button12, listener(app, i_OnUndoClick, Designer));
    button_OnClick(button13, listener(app, i_OnRedoClick, Designer));
    button_tooltip(button1, gui_text(TOOLBAR_OPEN));
    button_tooltip(button2, gui_text(TOOLBAR_SAVE));
    button_tooltip(button3, gui_text(TOOLBAR_NEW));
    button_tooltip(button4, gui_text(TOOLBAR_PROPS));
    button_tooltip(button5, gui_text(TOOLBAR_SIMULATE));
    button_tooltip(button6, gui_text(TOOLBAR_RESIZABLE));
    button_tooltip(button7, gui_text(TOOLBAR_LSKEL));
    button_tooltip(button8, gui_text(TOOLBAR_REMOVE));
    button_tooltip(button9, gui_text(TEXT_CUT));
    button_tooltip(button10, gui_text(TEXT_COPY));
    button_tooltip(button11, gui_text(TEXT_PASTE));
    button_tooltip(button12, gui_text(TEXT_UNDO));
    button_tooltip(button13, gui_text(TEXT_REDO));
    layout_button(layout, button1, 0, 0);
    layout_button(layout, button2, 1, 0);
    layout_button(layout, button3, 2, 0);
    layout_button(layout, button4, 3, 0);
    layout_button(layout, button5, 4, 0);
    layout_button(layout, button6, 5, 0);
    layout_button(layout, button7, 6, 0);
    layout_button(layout, button8, 7, 0);
    layout_button(layout, button9, 8, 0);
    layout_button(layout, button10, 9, 0);
    layout_button(layout, button11, 10, 0);
    layout_button(layout, button12, 11, 0);
    layout_button(layout, button13, 12, 0);
    layout_margin4(layout, 0, 0, 0, 10);
    layout_hexpand(layout, 13);
    layout_hmargin(layout, 5, 15);
    layout_hmargin(layout, 6, 15);
    layout_hmargin(layout, 7, 15);
    layout_hmargin(layout, 10, 15);
    app->open_form_cell = layout_cell(layout, 0, 0);
    app->save_form_cell = layout_cell(layout, 1, 0);
    app->add_form_cell = layout_cell(layout, 2, 0);
    app->rename_form_cell = layout_cell(layout, 3, 0);
    app->run_form_cell = layout_cell(layout, 4, 0);
    app->resize_form_cell = layout_cell(layout, 5, 0);
    app->skeleton_form_cell = layout_cell(layout, 6, 0);
    app->remove_form_cell = layout_cell(layout, 7, 0);
    app->cut_cell = layout_cell(layout, 8, 0);
    app->copy_cell = layout_cell(layout, 9, 0);
    app->paste_cell = layout_cell(layout, 10, 0);
    app->undo_cell = layout_cell(layout, 11, 0);
    app->redo_cell = layout_cell(layout, 12, 0);
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
                button_vpadding(button, 4);
                button_hpadding(button, 4);
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
    case ekDRAWER_WIDGET_DISPLAY:
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
    case ekDRAWER_VIEW_PROPS:
    case ekDRAWER_SVIEW_PROPS:
    case ekDRAWER_TEXT_PROPS:
    case ekDRAWER_IMAGE_PROPS:
    case ekDRAWER_TABLE_FRAME_PROPS:
    case ekDRAWER_TABLE_COLS_PROPS:
    case ekDRAWER_HLINE_PROPS:
    case ekDRAWER_VLINE_PROPS:
        return FALSE;
    default:
        cassert_default(drawer);
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
    draw_clear(p->ctx, i_COLORS.canvas);
    if (app->config.sel_form != UINT32_MAX)
    {
        if (app->config.sel_form < arrpt_size(app->forms, DForm))
        {
            DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
            const char_t *name = i_list_text(app->form_list, app->config.sel_form);
            dform_draw(form, app->config.swidget, app->default_font, app->bold_font, app->cmode, &i_COLORS, name, app->focus, p->ctx);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnOverlay(Designer *app, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    cassert_no_null(app);
    if (app->focus == TRUE)
    {
        draw_image_align(p->ctx, ekRIGHT, ekBOTTOM);
        draw_image(p->ctx, gui_image(KEYBOARD_PNG), p->width - 5, p->height - 5);
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

static void i_OnKey(Designer *app, Event *e)
{
    const EvKey *p = event_params(e, EvKey);
    cassert_no_null(app);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        bool_t update = FALSE;
        if (p->key == ekKEY_SUPR)
            update = dform_OnSupr(form, app->inspect, app->propedit);
        else if (p->key == ekKEY_UP || p->key == ekKEY_DOWN || p->key == ekKEY_LEFT || p->key == ekKEY_RIGHT)
            update = dform_OnCursorNav(form, p->key, app->inspect, app->propedit);

        if (update == TRUE)
            view_update(app->canvas);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnFocus(Designer *app, Event *e)
{
    const bool_t *focus = event_params(e, bool_t);
    cassert_no_null(app);
    if (app->focus != *focus)
    {
        app->focus = *focus;
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
    View *view = view_custom(FALSE, TRUE);
    view_size(view, s2df(450, 200));
    view_OnDraw(view, listener(app, i_OnDraw, Designer));
    view_OnOverlay(view, listener(app, i_OnOverlay, Designer));
    view_OnMove(view, listener(app, i_OnMove, Designer));
    view_OnDrag(view, listener(app, i_OnDrag, Designer));
    view_OnUp(view, listener(app, i_OnUp, Designer));
    view_OnExit(view, listener(app, i_OnExit, Designer));
    view_OnClick(view, listener(app, i_OnClick, Designer));
    view_OnKeyDown(view, listener(app, i_OnKey, Designer));
    view_OnFocus(view, listener(app, i_OnFocus, Designer));
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
    splitview_view(split3, view, TRUE);
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
    label_size_text(label1, "Undo Stack: 999999MMM");
    label_text(label2, "status-2");

    layout_margin4(layout, 0, 5, 5, 5);
    /* Keep the controls for futher updates */
    app->undo_label = label1;
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
    layout_tabstop(layout1, 0, 0, FALSE);

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
    i_swap_show_item(app, &app->config.show_forms, app->show_forms_item);
    window_update(app->window);
}

/*---------------------------------------------------------------------------*/

static void i_OnShowWidgets(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    i_swap_show_item(app, &app->config.show_widgets, app->show_widgets_item);
    window_update(app->window);
}

/*---------------------------------------------------------------------------*/

static void i_OnShowInspectr(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    i_swap_show_item(app, &app->config.show_inspectr, app->show_inspectr_item);
    window_update(app->window);
}

/*---------------------------------------------------------------------------*/

static void i_OnShowPropEdit(Designer *app, Event *e)
{
    cassert_no_null(app);
    unref(e);
    i_swap_show_item(app, &app->config.show_propedit, app->show_propedit_item);
    window_update(app->window);
}

/*---------------------------------------------------------------------------*/

static Menu *i_file_menu(Designer *app)
{
    Menu *menu = menu_create();
    MenuItem *item1 = menuitem_create();
    MenuItem *item2 = menuitem_create();
    MenuItem *item3 = menuitem_create();
    MenuItem *item4 = menuitem_create();
    cassert_no_null(app);
    menuitem_text(item1, gui_text(TOOLBAR_NEW_FORM));
    menuitem_text(item2, gui_text(TOOLBAR_OPEN));
    menuitem_text(item3, gui_text(TOOLBAR_SAVE));
    menuitem_text(item4, gui_text(TEXT_QUIT));
    menuitem_image(item1, gui_image(NEW16_PNG));
    menuitem_image(item2, gui_image(OPEN16_PNG));
    menuitem_image(item3, gui_image(SAVE16_PNG));
    menuitem_OnClick(item1, listener(app, i_OnAddFormClick, Designer));
    menuitem_OnClick(item2, listener(app, i_OnOpenFormsClick, Designer));
    menuitem_OnClick(item3, listener(app, i_OnSaveFormsClick, Designer));
    menuitem_OnClick(item4, listener(app, i_OnQuitClick, Designer));
    menu_add_item(menu, item1);
    menu_add_item(menu, item2);
    menu_add_item(menu, menuitem_separator());
    menu_add_item(menu, item3);
    menu_add_item(menu, menuitem_separator());
    menu_add_item(menu, item4);
    return menu;
}

/*---------------------------------------------------------------------------*/

static Menu *i_edit_menu(Designer *app)
{
    Menu *menu = menu_create();
    MenuItem *item1 = menuitem_create();
    MenuItem *item2 = menuitem_create();
    MenuItem *item3 = menuitem_create();
    MenuItem *item4 = menuitem_create();
    MenuItem *item5 = menuitem_create();
    menuitem_text(item1, gui_text(TEXT_UNDO));
    menuitem_text(item2, gui_text(TEXT_REDO));
    menuitem_text(item3, gui_text(TEXT_CUT));
    menuitem_text(item4, gui_text(TEXT_COPY));
    menuitem_text(item5, gui_text(TEXT_PASTE));
    menuitem_image(item1, gui_image(UNDO16_PNG));
    menuitem_image(item2, gui_image(REDO16_PNG));
    menuitem_image(item3, gui_image(CUT16_PNG));
    menuitem_image(item4, gui_image(COPY16_PNG));
    menuitem_image(item5, gui_image(PASTE16_PNG));
    menuitem_key(item1, ekKEY_Z, ekMKEY_CONTROL);
    menuitem_key(item2, ekKEY_Y, ekMKEY_CONTROL);
    menuitem_key(item3, ekKEY_X, ekMKEY_CONTROL);
    menuitem_key(item4, ekKEY_C, ekMKEY_CONTROL);
    menuitem_key(item5, ekKEY_V, ekMKEY_CONTROL);
    menuitem_OnClick(item1, listener(app, i_OnUndoClick, Designer));
    menuitem_OnClick(item2, listener(app, i_OnRedoClick, Designer));
    menuitem_OnClick(item3, listener(app, i_OnCutClick, Designer));
    menuitem_OnClick(item4, listener(app, i_OnCopyClick, Designer));
    menuitem_OnClick(item5, listener(app, i_OnPasteClick, Designer));
    menu_add_item(menu, item1);
    menu_add_item(menu, item2);
    menu_add_item(menu, menuitem_separator());
    menu_add_item(menu, item3);
    menu_add_item(menu, item4);
    menu_add_item(menu, item5);
    app->undo_item = item1;
    app->redo_item = item2;
    app->cut_item = item3;
    app->copy_item = item4;
    app->paste_item = item5;
    return menu;
}

/*---------------------------------------------------------------------------*/

static Menu *i_form_menu(Designer *app)
{
    Menu *menu = menu_create();
    MenuItem *item1 = menuitem_create();
    MenuItem *item2 = menuitem_create();
    MenuItem *item3 = menuitem_create();
    MenuItem *item4 = menuitem_create();
    MenuItem *item5 = menuitem_create();
    cassert_no_null(app);
    menuitem_text(item1, gui_text(TEXT_FORM_PROPS));
    menuitem_text(item2, gui_text(TOOLBAR_SIMULATE));
    menuitem_text(item3, gui_text(TOOLBAR_RESIZABLE));
    menuitem_text(item4, gui_text(TOOLBAR_LSKEL));
    menuitem_text(item5, gui_text(TOOLBAR_REMOVE));
    menuitem_image(item1, gui_image(PROPS16_PNG));
    menuitem_image(item2, gui_image(SHOW16_PNG));
    menuitem_image(item3, gui_image(RESIZE16_PNG));
    menuitem_image(item4, gui_image(LSKEL16_PNG));
    menuitem_image(item5, gui_image(REMOVE16_PNG));
    menuitem_key(item4, ekKEY_F5, 0);
    menuitem_OnClick(item1, listener(app, i_OnPropsFormClick, Designer));
    menuitem_OnClick(item2, listener(app, i_OnSimulateClick, Designer));
    menuitem_OnClick(item3, listener(app, i_OnResizableClick, Designer));
    menuitem_OnClick(item4, listener(app, i_OnSkeletonClick, Designer));
    menuitem_OnClick(item5, listener(app, i_OnRemoveClick, Designer));
    menu_add_item(menu, item1);
    menu_add_item(menu, item2);
    menu_add_item(menu, item3);
    menu_add_item(menu, menuitem_separator());
    menu_add_item(menu, item4);
    menu_add_item(menu, menuitem_separator());
    menu_add_item(menu, item5);
    app->layout_skeleton_item = item4;
    return menu;
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
    menuitem_text(item1, gui_text(TEXT_BOX_FORM_BOX));
    menuitem_text(item2, gui_text(TEXT_BOX_WIDGET_BOX));
    menuitem_text(item3, gui_text(TEXT_BOX_INSPECTOR));
    menuitem_text(item4, gui_text(TEXT_BOX_PROPEDIT));
    menuitem_OnClick(item1, listener(app, i_OnShowForms, Designer));
    menuitem_OnClick(item2, listener(app, i_OnShowWidgets, Designer));
    menuitem_OnClick(item3, listener(app, i_OnShowInspectr, Designer));
    menuitem_OnClick(item4, listener(app, i_OnShowPropEdit, Designer));
    menu_add_item(menu, item1);
    menu_add_item(menu, item2);
    menu_add_item(menu, item3);
    menu_add_item(menu, item4);
    app->show_forms_item = item1;
    app->show_widgets_item = item2;
    app->show_inspectr_item = item3;
    app->show_propedit_item = item4;
    return menu;
}

/*---------------------------------------------------------------------------*/

static Menu *i_menu(Designer *app)
{
    Menu *menu = menu_create();
    Menu *submenu1 = i_file_menu(app);
    Menu *submenu2 = i_edit_menu(app);
    Menu *submenu3 = i_form_menu(app);
    Menu *submenu4 = i_view_menu(app);
    MenuItem *item1 = menuitem_create();
    MenuItem *item2 = menuitem_create();
    MenuItem *item3 = menuitem_create();
    MenuItem *item4 = menuitem_create();
    menuitem_text(item1, gui_text(TEXT_FILE));
    menuitem_text(item2, gui_text(TEXT_EDIT));
    menuitem_text(item3, gui_text(TEXT_FORM));
    menuitem_text(item4, gui_text(TEXT_VIEW));
    menuitem_submenu(item1, &submenu1);
    menuitem_submenu(item2, &submenu2);
    menuitem_submenu(item3, &submenu3);
    menuitem_submenu(item4, &submenu4);
    
#if defined(__APPLE__)
    {
        MenuItem *appitem = menuitem_create();
        menu_add_item(menu, appitem);
    }
#endif
    
    menu_add_item(menu, item1);
    menu_add_item(menu, item2);
    menu_add_item(menu, item3);
    menu_add_item(menu, item4);
    return menu;
}

/*---------------------------------------------------------------------------*/

static void i_default_win_frame(Config *config)
{
    cassert_no_null(config);
    config->wx = 100;
    config->wy = 100;
    config->wwidth = 850;
    config->wheight = 500;
}

/*---------------------------------------------------------------------------*/

static void i_update_config(Designer *app)
{
    cassert_no_null(app);
    if (window_get_maximize(app->window) == TRUE || window_get_minimize(app->window) == TRUE)
    {
        i_default_win_frame(&app->config);
    }
    else
    {
        V2Df pos = window_get_origin(app->window);
        S2Df size = window_get_client_size(app->window);
        app->config.wx = pos.x;
        app->config.wy = pos.y;
        app->config.wwidth = size.width;
        app->config.wheight = size.height;
    }

    app->config.is_maximized = window_get_maximize(app->window);
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
        stm_write_bool(stm, app->config.is_maximized);
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
    i_default_win_frame(&app->config);
    app->config.vers = i_CONFIG_VERS;
    app->config.folder_path = str_c("");
    app->config.sel_form = UINT32_MAX;
    app->config.swidget = ekWIDGET_SELECT;
    app->config.split1_pos = 200;
    app->config.split2_pos = 200;
    app->config.split3_pos = 200;
    app->config.split4_pos = 200;
    app->config.is_maximized = FALSE;
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
            app->config.is_maximized = stm_read_bool(stm);
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
    window_client_size(app->window, s2df(app->config.wwidth, app->config.wheight));
    window_origin(app->window, v2df(app->config.wx, app->config.wy));

    if (app->config.is_maximized == TRUE)
        window_maximize(app->window);

    i_restore_splits(app);
    i_set_bwidget(app->config.swidget, app->bwidgets, app->default_font, app->bold_font);
    menuitem_state(app->show_forms_item, i_bool_state(app->config.show_forms));
    menuitem_state(app->show_widgets_item, i_bool_state(app->config.show_widgets));
    menuitem_state(app->show_inspectr_item, i_bool_state(app->config.show_inspectr));
    menuitem_state(app->show_propedit_item, i_bool_state(app->config.show_propedit));
    window_update(app->window);
}

/*---------------------------------------------------------------------------*/

static bool_t i_close_app(Designer *app)
{
    bool_t close = TRUE;
    if (i_need_save(app) == TRUE)
    {
        uint8_t ret = dialog_unsaved_changes(app->window, app->default_font, gui_text(TEXT_UNSAVED2));
        if (ret == 1)
            i_save_forms(app);
        else if (ret == 2)
            close = FALSE;
    }

    if (close == TRUE)
    {
        i_update_config(app);
        i_save_config(app);
        osapp_finish();
    }

    return close;
}

/*---------------------------------------------------------------------------*/

static void i_OnWindowClose(Designer *app, Event *e)
{
    bool_t *close = event_result(e, bool_t);
    *close = i_close_app(app);
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
    nflib_start();
    i_dbind();
    dialog_dbind();
    dlayout_global_init();

    if (gui_dark_mode() == TRUE)
    {
        i_COLORS.canvas = color_rgb(0x25, 0x29, 0x2E);
        i_COLORS.panel = color_rgb(0x56, 0x56, 0x56);
        i_COLORS.back = color_rgb(0x23, 0x23, 0x23);
        i_COLORS.main = color_rgb(0xCC, 0xCC, 0xCC);
        i_COLORS.select = gui_link_color();
        i_COLORS.title0 = color_rgb(0x39, 0x3C, 0x42);
        i_COLORS.title1 = color_rgb(0x2D, 0x30, 0x34);
        i_COLORS.cell = color_rgb(0x6E, 0x1C, 0x87);
        i_COLORS.cellhot = color_rgb(0x8F, 0x8E, 0x30);
        i_COLORS.col = color_rgb(0x8D, 0xDB, 0x45);
        i_COLORS.row = color_rgb(0x6A, 0x8E, 0xD8);
        i_COLORS.header0 = color_rgb(0x35, 0x2A, 0x27);
        i_COLORS.header1 = color_rgb(0x2C, 0x31, 0x37);
        i_COLORS.drawer0 = color_rgb(0x39, 0x3C, 0x42);
        i_COLORS.drawer1 = color_rgb(0x2D, 0x30, 0x34);
    }
    else
    {
        i_COLORS.canvas = color_rgb(0xA0, 0xA0, 0xA0);
        i_COLORS.panel = color_rgb(0xF0, 0xF0, 0xF0);
        i_COLORS.back = color_rgb(0xE0, 0xE0, 0XE0);
        i_COLORS.main = kCOLOR_BLACK;
        i_COLORS.select = gui_link_color();
        i_COLORS.title0 = color_rgb(0x99, 0xB5, 0xD1);
        i_COLORS.title1 = color_rgb(0xB7, 0xCF, 0xE8);
        i_COLORS.cell = color_rgb(0xFF, 0xE0, 0x82);
        i_COLORS.cellhot = color_rgb(0xFF, 0xA0, 0x00);
        i_COLORS.col = color_rgb(0x8E, 0x03, 0xA3);
        i_COLORS.row = kCOLOR_BLUE;
        i_COLORS.header0 = color_rgb(0xDE, 0xDE, 0xDE);
        i_COLORS.header1 = color_rgb(0xDA, 0xDA, 0xDA);
        i_COLORS.drawer0 = color_rgb(0xF2, 0xF2, 0xF2);
        i_COLORS.drawer1 = color_rgb(0xE4, 0xE4, 0xE4);
    }

    i_COLORS.add_icon = gui_image(PLUS16_PNG);
    i_COLORS.nap_icon = gui_image(NAPP_PNG);
    dgui_init(&i_COLORS);
    app->forms = arrpt_create(DForm);
    app->default_font = font_system(font_regular_size(), 0);
    app->bold_font = font_system(font_regular_size(), ekFBOLD);
    app->wdrawers = arrst_create(WDrawer);
    app->bwidgets = arrst_create(BWidget);
    app->cmode = ekCMODE_DETAIL;
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_SELECT, "");
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_LAYOUTS, TEXT_LAYOUTS);
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_BUTTONS, TEXT_BUTTONS);
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_TEXT, TEXT_TEXT_WIDGETS);
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_ITEMS, TEXT_ITEM_WIDGETS);
    i_add_drawer(app->wdrawers, ekDRAWER_WIDGET_DISPLAY, TEXT_DISPLAY_WIDGETS);
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
    i_add_drawer(app->wdrawers, ekDRAWER_VIEW_PROPS, TEXT_VIEW_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_SVIEW_PROPS, TEXT_SVIEW_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_TEXT_PROPS, TEXT_TEXT_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_IMAGE_PROPS, TEXT_IMAGE_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_TABLE_FRAME_PROPS, TEXT_TABLE_PROPS);
    i_add_drawer(app->wdrawers, ekDRAWER_TABLE_COLS_PROPS, TEXT_COLUMN_PROPS);
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
    i_add_widget(app->bwidgets, ekWIDGET_IMAGEVIEW, TEXT_IMAGE_VIEW, IMAGEVIEW_PNG, ekDRAWER_WIDGET_DISPLAY);
    i_add_widget(app->bwidgets, ekWIDGET_CUSTOMVIEW, TEXT_CUSTOM_VIEW, VIEW_PNG, ekDRAWER_WIDGET_DISPLAY);
    i_add_widget(app->bwidgets, ekWIDGET_SCROLLVIEW, TEXT_SCROLL_VIEW, SVIEW_PNG, ekDRAWER_WIDGET_DISPLAY);
    i_add_widget(app->bwidgets, ekWIDGET_HORZ_SLIDER, TEXT_HORZ_SLIDER, HORSLIDER_PNG, ekDRAWER_WIDGET_OTHERS);
    i_add_widget(app->bwidgets, ekWIDGET_VERT_SLIDER, TEXT_VERT_SLIDER, VERSLIDER_PNG, ekDRAWER_WIDGET_OTHERS);
    i_add_widget(app->bwidgets, ekWIDGET_PROGRESS, TEXT_PROGRESS_BAR, PROGRESSBAR_PNG, ekDRAWER_WIDGET_OTHERS);
    i_add_widget(app->bwidgets, ekWIDGET_HORZ_LINE, TEXT_HORZ_LINE, HLINE_PNG, ekDRAWER_WIDGET_OTHERS);
    i_add_widget(app->bwidgets, ekWIDGET_VERT_LINE, TEXT_VERT_LINE, VLINE_PNG, ekDRAWER_WIDGET_OTHERS);   
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
    window_title(app->window, gui_text(TEXT_APP_TITLE));
    window_OnClose(app->window, listener(app, i_OnWindowClose, Designer));
    i_apply_config(app);
    designer_clipboard_controls(app, FALSE, FALSE);
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
    font_destroy(&(*app)->default_font);
    font_destroy(&(*app)->bold_font);
    arrst_destroy(&(*app)->wdrawers, NULL, WDrawer);
    arrst_destroy(&(*app)->bwidgets, NULL, BWidget);
    arrpt_destroy(&(*app)->forms, i_destroy_form_opt, DForm);
    dbind_destopt(&(*app)->clipboard.fcell, FCell);
    dbind_destopt(&(*app)->clipboard.flayout, FLayout);
    menu_destroy(&(*app)->menu);
    window_destroy(&(*app)->window);
    nflib_finish();
    dgui_finish();
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

static bool_t i_has_clipboard(Designer *app)
{
    cassert_no_null(app);
    if (app->clipboard.fcell != NULL)
    {
        cassert(app->clipboard.flayout == NULL);
        return TRUE;
    }
    else if (app->clipboard.flayout != NULL)
    {
        return TRUE;
    }

    return FALSE;
}

/*---------------------------------------------------------------------------*/

void designer_clipboard_controls(Designer *app, const bool_t can_copy, const bool_t can_paste)
{
    bool_t paste = can_paste && i_has_clipboard(app);
    cassert_no_null(app);
    menuitem_enabled(app->cut_item, can_copy);
    menuitem_enabled(app->copy_item, can_copy);
    menuitem_enabled(app->paste_item, paste);
    cell_enabled(app->cut_cell, can_copy);
    cell_enabled(app->copy_cell, can_copy);
    cell_enabled(app->paste_cell, paste);
}

/*---------------------------------------------------------------------------*/

void designer_undo_controls(Designer *app, const bool_t can_undo, const bool_t can_redo)
{
    cassert_no_null(app);
    menuitem_enabled(app->undo_item, can_undo);
    menuitem_enabled(app->redo_item, can_redo);
    cell_enabled(app->undo_cell, can_undo);
    cell_enabled(app->redo_cell, can_redo);
}

/*---------------------------------------------------------------------------*/

void designer_undo_stack(Designer *app, const uint32_t size)
{
    String *str = NULL;
    cassert_no_null(app);
    if (size < 1024 * 1024)
    {
        real32_t kb = (real32_t)size / (real32_t)1024;
        str = str_printf("%s: %.1fK", gui_text(TEXT_UNDO_STACK), kb);
    }
    else if (size < 1024 * 1024 * 1024)
    {
        real32_t mb = (real32_t)size / (real32_t)(1024 * 1024);
        str = str_printf("%s: %.1fM", gui_text(TEXT_UNDO_STACK), mb);
    }
    else
    {
        real32_t gb = (real32_t)size / (real32_t)(1024 * 1024 * 1024);
        str = str_printf("%s: %.1fG", gui_text(TEXT_UNDO_STACK), gb);
    }

    label_text(app->undo_label, tc(str));
    str_destroy(&str);
}

/*---------------------------------------------------------------------------*/

void designer_promote_left(Designer *app, const DSelect *sel)
{
    cassert_no_null(app);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        if (dform_OnPromoteLeft(form, sel, app->inspect, app->propedit) == TRUE)
            view_update(app->canvas);
    }
}

/*---------------------------------------------------------------------------*/

void designer_promote_right(Designer *app, const DSelect *sel)
{
    cassert_no_null(app);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        if (dform_OnPromoteRight(form, sel, app->inspect, app->propedit) == TRUE)
            view_update(app->canvas);
    }
}

/*---------------------------------------------------------------------------*/

void designer_promote_top(Designer *app, const DSelect *sel)
{
    cassert_no_null(app);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        if (dform_OnPromoteTop(form, sel, app->inspect, app->propedit) == TRUE)
            view_update(app->canvas);
    }
}

/*---------------------------------------------------------------------------*/

void designer_promote_bottom(Designer *app, const DSelect *sel)
{
    cassert_no_null(app);
    if (app->config.sel_form != UINT32_MAX)
    {
        DForm *form = arrpt_get(app->forms, app->config.sel_form, DForm);
        if (dform_OnPromoteBottom(form, sel, app->inspect, app->propedit) == TRUE)
            view_update(app->canvas);
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

const DColors *designer_colors(const Designer *app)
{
    unref(app);
    return &i_COLORS;
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
