/*
    This is part of gtnap
    TODO: More info
    Commit - 2
*/

#include "gtnap.h"
#include "gtnap.inl"
#include "gtnap.ch"
#include "hbnap.ch"
#include "nap_menu.inl"
#include "nap_debugger.inl"
#include <nforms/nforms.h>
#include <nforms/nform.h>
#include <deblib/deblib.h>
#include <osapp/osmain.h>
#include <osapp/osapp.h>
#include <gui/button.h>
#include <gui/drawctrl.inl>
#include <gui/edit.h>
#include <gui/gui.h>
#include <gui/imageview.h>
#include <gui/label.h>
#include <gui/layout.h>
#include <gui/menu.h>
#include <gui/menuitem.h>
#include <gui/panel.h>
#include <gui/tableview.h>
#include <gui/tableviewh.h>
#include <gui/textview.h>
#include <gui/view.h>
#include <gui/window.h>
#include <draw2d/draw2d.h>
#include <draw2d/color.h>
#include <draw2d/font.h>
#include <draw2d/image.h>
#include <draw2d/draw.h>
#include <draw2d/dctx.h>
#include <geom2d/r2d.h>
#include <geom2d/s2d.h>
#include <geom2d/t2d.h>
#include <geom2d/v2d.h>
#include <core/arrst.h>
#include <core/arrpt.h>
#include <core/setst.h>
#include <core/event.h>
#include <core/heap.h>
#include <core/hfile.h>
#include <core/strings.h>
#include <core/stream.h>
#include <core/treest.h>
#include <osbs/bfile.h>
#include <osbs/btime.h>
#include <osbs/log.h>
#include <osbs/osbs.h>
#include <sewer/bmath.h>
#include <sewer/blib.h>
#include <sewer/bstd.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>
#include <sewer/unicode.h>

#include "hbapiitm.h"
#include "hbapirdd.h"
#include "hbapistr.h"
#include "hbdate.h"
#include "hbset.h"

typedef struct _gui_component_t GuiComponent;
typedef struct _gtnap_callback_t GtNapCallback;
typedef struct _gtnap_key_t GtNapKey;
typedef struct _gtnap_column_t GtNapColumn;
typedef struct _gtnap_toolbar_t GtNapToolbar;
typedef struct _gtnap_area_t GtNapArea;
typedef struct _gtnap_object_t GtNapObject;
typedef struct _gtnap_geom_t GtNapGeom;
typedef struct _gtnap_window_t GtNapWindow;
typedef struct _gtnap_bind_t GtNapBind;
typedef struct _gtnap_fcolumn_t GtNapFColumn;
typedef struct _gtnap_fbdconn_t GtNapFDBConn;
typedef struct _gtnap_farea_t GtNapFArea;
typedef struct _gtnap_farea2_t GtNapFArea2;
typedef struct _gtnap_fnode_t GtNapFNode;
typedef struct _gtnap_prop_t GtNapProp;
typedef struct _gtnap_t GtNap;

typedef void (*FPtr_gtnap_callback)(GtNapCallback *callback, Event *event);

typedef enum _objtype_t
{
    ekOBJ_LABEL,
    ekOBJ_EDIT,
    ekOBJ_BUTTON,
    ekOBJ_MENU,
    ekOBJ_TABLEVIEW,
    ekOBJ_TEXTVIEW,
    ekOBJ_IMAGE
} objtype_t;

typedef enum _datatype_t
{
    ekTYPE_CHARACTER,
    ekTYPE_DATE
} datatype_t;

struct _gtnap_callback_t
{
    GtNapWindow *gtwin;
    GtNapForm *form;
    GtNapMenuItem *menuitem;
    HB_ITEM *block;
    int32_t key;
    uint32_t autoclose_id;
};

struct _gtnap_key_t
{
    int32_t key; /* inkey.ch */
    vkey_t vkey;
    uint32_t modifiers;
};

struct _gtnap_column_t
{
    uint32_t fixed_chars;
    uint32_t header_lines;
    real32_t widthf;
    align_t align;
    String *title;
    HB_ITEM *block;
};

struct _gtnap_toolbar_t
{
    real32_t button_widthf;
    real32_t heightf;
    ArrPt(GuiComponent) *items;
};

struct _gtnap_area_t
{
    AREA *area;
    HB_ULONG cache_recno; /* Store the DB recno while table drawing */
    GtNapObject *gtobj;
    ArrSt(uint32_t) *records; /* Records visible in table (index, deleted, filters) */
    HB_ITEM *while_block;
};

struct _gtnap_object_t
{
    objtype_t type;
    datatype_t dtype;
    int32_t top;
    int32_t left;
    V2Df pos;
    S2Df size;
    bool_t multisel;
    bool_t autoclose;
    bool_t is_last_edit;
    bool_t in_scroll;
    bool_t can_auto_lista;
    bool_t has_focus;
    uint32_t max_chars;
    uint32_t editBoxIndexForButton;
    String *text;
    PHB_ITEM text_block;
    PHB_ITEM get_set_block;
    PHB_ITEM is_editable_block;
    PHB_ITEM when_block;
    PHB_ITEM valida_block;
    PHB_ITEM message_block;
    PHB_ITEM keyfilter_block;
    PHB_ITEM auto_block;
    PHB_ITEM wizard_block;
    ArrSt(GtNapColumn) *columns;
    GuiComponent *component;
    GtNapWindow *gtwin;
};

struct _gtnap_window_t
{
    uint32_t id;
    uint32_t parent_id;
    int32_t top;
    int32_t left;
    int32_t bottom;
    int32_t right;
    int32_t cursor_row;
    int32_t cursor_col;
    int32_t scroll_top;
    int32_t scroll_left;
    int32_t scroll_bottom;
    int32_t scroll_right;
    String *nameid;
    HB_ITEM *is_editable_block;
    HB_ITEM *confirm_block;
    HB_ITEM *desist_block;
    HB_ITEM *error_date_block;
    bool_t is_configured;
    bool_t is_closed_by_esc;
    bool_t modal_window_alive;
    bool_t buttons_navigation;
    bool_t border;
    uint32_t message_label_id;
    uint32_t default_button;
    GtNapObject *wizard_obj;
    Window *window;
    S2Df panel_size;
    Panel *panel;
    Panel *scrolled_panel;
    GtNapToolbar *toolbar;
    GtNapArea *gtarea;
    uint32_t num_rows;
    ArrPt(GuiComponent) *tabstops;
    ArrPt(GtNapObject) *objects;
    ArrPt(GtNapCallback) *callbacks;
};

struct _gtnap_modal_t
{
    uint64_t timestamp;
    uint32_t close_seconds;
    GtNapWindow *gtwin;
};

struct _gtnap_bind_t
{
    String *gui_id;
    PHB_ITEM value;
    Listener *listener;
};

struct _gtnap_fcolumn_t
{
    align_t align;
    HB_ITEM *block;
};

struct _gtnap_fnode_t
{
    GtNapFArea2 *area;
    HB_ULONG recno;
    bool_t expanded;
};

struct _gtnap_fbdconn_t
{
    GtNapForm *form;
    String *cellname;
    TableView *table;
    ArrSt(GtNapFArea2) *areas;
    TreeSt(GtNapFNode) *tdata;
};

struct _gtnap_farea2_t
{
    AREA *area;
    ArrSt(GtNapFColumn) *columns;
    HB_ITEM *relfrom;
    HB_ITEM *relto;
};

struct _gtnap_farea_t
{
    GtNapForm *form;
    String *cellname;
    TableView *table;
    AREA *area;
    HB_ULONG cache_recno;     /* Store the DB recno while table drawing */
    ArrSt(uint32_t) *records; /* Records visible in table (index, deleted, filters) */
    ArrSt(GtNapFColumn) *columns;
};

struct _gtnap_form_t
{
    NForm *form;
    Window *window;
    String *nameid;
    String *respath;
    uint32_t modal_ret;
    GtNapFArea *area;
    GtNapFDBConn *dbconn;
    HB_ITEM *OnClose_block;
    bool_t is_resizable;
    ArrSt(GtNapBind) *binds;
    ArrPt(GtNapCallback) *callbacks;
};

struct _gtnap_prop_t
{
    String *key;
    String *value;
};

struct _gtnap_t
{
    Font *global_font;
    Font *button_font;
    Font *edit_font;
    uint8_t date_digits;
    uint8_t date_chars;
    String *title;
    String *working_path;
    uint32_t rows;
    uint32_t cols;
    real32_t cell_x_sizef;
    real32_t cell_y_sizef;
    real32_t label_y_sizef;
    real32_t button_y_sizef;
    real32_t edit_y_sizef;
    uint64_t modal_timestamp;
    uint32_t modal_delay_seconds;
    GtNapWindow *modal_time_window;
    ArrPt(GtNapWindow) *windows;
    ArrPt(GtNapCallback) *menu_callbacks;
    String *debugger_path;
    bool_t debugger_visible;
    GtNapDebugger *debugger;
    SetSt(GtNapProp) *properties;
};

/*---------------------------------------------------------------------------*/

DeclPt(GtNapCallback);
DeclSt(GtNapColumn);
DeclSt(GtNapFColumn);
DeclPt(GtNapArea);
DeclPt(GtNapObject);
DeclSt(GtNapBind);
DeclPt(GtNapWindow);
DeclPt(GuiComponent);
DeclSt(GtNapProp);
DeclSt(GtNapFArea2);
DeclSt(GtNapFNode);

/*---------------------------------------------------------------------------*/

static const GtNapKey KEYMAPS[] = {
    {K_F1, ekKEY_F1, 0},
    {K_F2, ekKEY_F2, 0},
    {K_F3, ekKEY_F3, 0},
    {K_F4, ekKEY_F4, 0},
    {K_F5, ekKEY_F5, 0},
    {K_F6, ekKEY_F6, 0},
    {K_F7, ekKEY_F7, 0},
    {K_F8, ekKEY_F8, 0},
    {K_F9, ekKEY_F9, 0},
    {K_F10, ekKEY_F10, 0},
    {K_F11, ekKEY_F11, 0},
    {K_F12, ekKEY_F12, 0},

    {K_ENTER, ekKEY_RETURN, 0},
    {K_SPACE, ekKEY_SPACE, 0},

    {'a', ekKEY_A, 0},
    {'b', ekKEY_B, 0},
    {'c', ekKEY_C, 0},
    {'d', ekKEY_D, 0},
    {'e', ekKEY_E, 0},
    {'f', ekKEY_F, 0},
    {'g', ekKEY_G, 0},
    {'h', ekKEY_H, 0},
    {'i', ekKEY_I, 0},
    {'j', ekKEY_J, 0},
    {'k', ekKEY_K, 0},
    {'l', ekKEY_L, 0},
    {'m', ekKEY_M, 0},
    {'n', ekKEY_N, 0},
    {'o', ekKEY_O, 0},
    {'p', ekKEY_P, 0},
    {'q', ekKEY_Q, 0},
    {'r', ekKEY_R, 0},
    {'s', ekKEY_S, 0},
    {'t', ekKEY_T, 0},
    {'u', ekKEY_U, 0},
    {'v', ekKEY_V, 0},
    {'w', ekKEY_W, 0},
    {'x', ekKEY_X, 0},
    {'y', ekKEY_Y, 0},
    {'z', ekKEY_Z, 0},

    {'A', ekKEY_A, 0},
    {'B', ekKEY_B, 0},
    {'C', ekKEY_C, 0},
    {'D', ekKEY_D, 0},
    {'E', ekKEY_E, 0},
    {'F', ekKEY_F, 0},
    {'G', ekKEY_G, 0},
    {'H', ekKEY_H, 0},
    {'I', ekKEY_I, 0},
    {'J', ekKEY_J, 0},
    {'K', ekKEY_K, 0},
    {'L', ekKEY_L, 0},
    {'M', ekKEY_M, 0},
    {'N', ekKEY_N, 0},
    {'O', ekKEY_O, 0},
    {'P', ekKEY_P, 0},
    {'Q', ekKEY_Q, 0},
    {'R', ekKEY_R, 0},
    {'S', ekKEY_S, 0},
    {'T', ekKEY_T, 0},
    {'U', ekKEY_U, 0},
    {'V', ekKEY_V, 0},
    {'W', ekKEY_W, 0},
    {'X', ekKEY_X, 0},
    {'Y', ekKEY_Y, 0},
    {'Z', ekKEY_Z, 0},

    {'0', ekKEY_0, 0},
    {'1', ekKEY_1, 0},
    {'2', ekKEY_2, 0},
    {'3', ekKEY_3, 0},
    {'4', ekKEY_4, 0},
    {'5', ekKEY_5, 0},
    {'6', ekKEY_6, 0},
    {'7', ekKEY_7, 0},
    {'8', ekKEY_8, 0},
    {'9', ekKEY_9, 0}};

/*---------------------------------------------------------------------------*/

/* Identify the main window in a debugging process */
static const uint32_t i_MAIN_WINDOW_HASH = 1234567;
static const uint32_t i_DEBUGGER_WINDOW_HASH = 7654321;
static const bool_t i_LOG_HBFUNCS = FALSE;
static const bool_t i_FULL_HBFUNCS = FALSE;

/* Harbour colors sensible to light/dark themes */
static color_t i_COLORS[16];
static const real32_t i_MINIMAL_FONT_SIZE = 5;
static const real32_t i_MAX_SCREEN_HEIGHT_TOLERANCE_PX = 30;
static const real32_t i_UNDEF_R32 = REAL32_MAX;
static const real32_t i_MAXIMIZED_SIZE = 1e10f;
static const char_t *i_XPOS_PROP = "XPOS";
static const char_t *i_YPOS_PROP = "YPOS";
static const char_t *i_WIDTH_PROP = "WIDTH";
static const char_t *i_HEIGHT_PROP = "HEIGHT";
static const char_t *i_FONT_REF_TEXT = "exibicao/edicao de texto em memoria";

/*---------------------------------------------------------------------------*/

__EXTERN_C

/*
 * These are internal, non-documented functions of NAppGUI.
 * They are used for direct handling of widgets, avoiding the 'layout' layer.
 */
void _component_detach_from_panel(GuiComponent *panel_component, GuiComponent *child_component);
void _component_set_frame(GuiComponent *component, const V2Df *origin, const S2Df *size);
void _component_visible(GuiComponent *component, const bool_t visible);
void _component_destroy(GuiComponent **component);
void _component_taborder(GuiComponent *component, Window *window);
const char_t *_component_type(const GuiComponent *component);
void *_component_ositem(const GuiComponent *component);
void _panel_attach_component(Panel *panel, GuiComponent *component);
void _panel_dettach_component(Panel *panel, GuiComponent *component);
void _panel_destroy_component(Panel *panel, GuiComponent *component);
void _panel_compose(Panel *panel, const S2Df *required_size, S2Df *final_size);
void _panel_locate(Panel *panel);
void _window_taborder(Window *window, void *ositem);
void _window_focus(Window *window, GuiComponent *component);
void _panel_content_size(Panel *panel, const real32_t width, const real32_t height);
View *_view_create(const uint32_t flags);

__END_C

/*---------------------------------------------------------------------------*/

static GtNap *GTNAP_GLOBAL = NULL;
static char_t INIT_TITLE[128];
static PHB_ITEM INIT_CODEBLOCK = NULL;
static uint32_t INIT_ROWS = 0;
static uint32_t INIT_COLS = 0;

#define STATIC_TEXT_SIZE 1024
char_t TEMP_BUFFER[STATIC_TEXT_SIZE];

/*---------------------------------------------------------------------------*/

static void i_destroy_callback(GtNapCallback **callback)
{
    cassert_no_null(callback);
    cassert_no_null(*callback);
    if ((*callback)->block != NULL)
        hb_itemRelease((*callback)->block);
    heap_delete(callback, GtNapCallback);
}

/*---------------------------------------------------------------------------*/

static void i_remove_column(GtNapColumn *column)
{
    cassert_no_null(column);
    str_destopt(&column->title);
    if (column->block != NULL)
        hb_itemRelease(column->block);
}

/*---------------------------------------------------------------------------*/

static void i_remove_toolbar(GtNapToolbar *toolbar, Panel *panel, const bool_t is_configured)
{
    cassert_no_null(toolbar);
    arrpt_foreach(item, toolbar->items, GuiComponent)
        if (item != NULL)
        {
            GuiComponent *ditem = item;

            if (is_configured == TRUE)
                _panel_destroy_component(panel, ditem);
            else
                _component_destroy(&ditem);
        }
    arrpt_end();
    arrpt_destroy(&toolbar->items, NULL, GuiComponent);
}

/*---------------------------------------------------------------------------*/

static void i_destroy_area(GtNapArea **area)
{
    cassert_no_null(area);
    cassert_no_null(*area);
    arrst_destroy(&(*area)->records, NULL, uint32_t);

    if ((*area)->while_block)
    {
        hb_itemRelease((*area)->while_block);
        (*area)->while_block = NULL;
    }

    heap_delete(area, GtNapArea);
}

/*---------------------------------------------------------------------------*/

static void i_destroy_gtobject(GtNapWindow *gtwin, const uint32_t index)
{
    GtNapObject *gtobj = NULL;
    cassert_no_null(gtwin);
    gtobj = arrpt_get(gtwin->objects, index, GtNapObject);

    _component_visible(gtobj->component, FALSE);

    if (gtwin->is_configured == TRUE)
    {
        if (gtobj->in_scroll == TRUE)
            _panel_destroy_component(gtwin->scrolled_panel, gtobj->component);
        else
            _panel_destroy_component(gtwin->panel, gtobj->component);
    }
    else
    {
        _component_destroy(&gtobj->component);
    }

    str_destopt(&gtobj->text);

    if (gtobj->text_block != NULL)
        hb_itemRelease(gtobj->text_block);

    if (gtobj->get_set_block != NULL)
        hb_itemRelease(gtobj->get_set_block);

    if (gtobj->is_editable_block != NULL)
        hb_itemRelease(gtobj->is_editable_block);

    if (gtobj->when_block != NULL)
        hb_itemRelease(gtobj->when_block);

    if (gtobj->valida_block != NULL)
        hb_itemRelease(gtobj->valida_block);

    if (gtobj->message_block != NULL)
        hb_itemRelease(gtobj->message_block);

    if (gtobj->keyfilter_block != NULL)
        hb_itemRelease(gtobj->keyfilter_block);

    if (gtobj->auto_block != NULL)
        hb_itemRelease(gtobj->auto_block);

    if (gtobj->wizard_block != NULL)
        hb_itemRelease(gtobj->wizard_block);

    arrst_destopt(&gtobj->columns, i_remove_column, GtNapColumn);

    heap_delete(&gtobj, GtNapObject);
    arrpt_delete(gtwin->objects, index, NULL, GtNapObject);
}

/*---------------------------------------------------------------------------*/

static void i_destroy_gtwin(GtNapWindow **dgtwin)
{
    GtNapWindow *gtwin = NULL;
    cassert_no_null(dgtwin);
    cassert_no_null(*dgtwin);
    gtwin = *dgtwin;

    {
        uint32_t i, n = arrpt_size(gtwin->objects, GtNapObject);
        for (i = 0; i < n; ++i)
            i_destroy_gtobject(gtwin, 0);
    }

    if (gtwin->scrolled_panel != NULL)
    {
        _component_visible((GuiComponent *)gtwin->scrolled_panel, FALSE);

        if (gtwin->is_configured == TRUE)
            _panel_destroy_component(gtwin->panel, (GuiComponent *)gtwin->scrolled_panel);
        else
            _component_destroy((GuiComponent **)&gtwin->scrolled_panel);
    }

    if (gtwin->toolbar != NULL)
    {
        i_remove_toolbar(gtwin->toolbar, gtwin->panel, gtwin->is_configured);
        heap_delete(&gtwin->toolbar, GtNapToolbar);
    }

    if (gtwin->gtarea != NULL)
        i_destroy_area(&gtwin->gtarea);

    if (gtwin->is_editable_block != NULL)
        hb_itemRelease(gtwin->is_editable_block);

    if (gtwin->confirm_block != NULL)
        hb_itemRelease(gtwin->confirm_block);

    if (gtwin->desist_block != NULL)
        hb_itemRelease(gtwin->desist_block);

    if (gtwin->error_date_block != NULL)
        hb_itemRelease(gtwin->error_date_block);

    str_destroy(&gtwin->nameid);
    cassert(arrpt_size(gtwin->objects, GtNapObject) == 0);
    arrpt_destroy(&gtwin->tabstops, NULL, GuiComponent);
    arrpt_destroy(&gtwin->objects, NULL, GtNapObject);
    arrpt_destroy(&gtwin->callbacks, i_destroy_callback, GtNapCallback);

    if (gtwin->parent_id == UINT32_MAX)
    {
        window_destroy(&gtwin->window);
    }
    else
    {
        cassert(gtwin->window == NULL);
        if (gtwin->panel != NULL)
            _component_destroy((GuiComponent **)&gtwin->panel);
    }

    heap_delete(dgtwin, GtNapWindow);
}

/*---------------------------------------------------------------------------*/

static void i_remove_property(GtNapProp *prop)
{
    cassert_no_null(prop);
    str_destroy(&prop->key);
    str_destroy(&prop->value);
}

/*---------------------------------------------------------------------------*/

static int i_prop_cmp(const GtNapProp *prop, const char_t *key)
{
    cassert_no_null(prop);
    return str_cmp(prop->key, key);
}

/*---------------------------------------------------------------------------*/

static void i_save_properties(const SetSt(GtNapProp) *properties)
{
    String *cfile = hfile_appdata("config.txt");
    Stream *stm = stm_to_file(tc(cfile), NULL);
    if (stm != NULL)
    {
        setst_foreach_const(prop, properties, GtNapProp)
            stm_writef(stm, tc(prop->key));
            stm_writef(stm, ":");
            stm_writef(stm, tc(prop->value));
            stm_writef(stm, "\n");
        setst_fornext_const(prop, properties, GtNapProp)
        stm_close(&stm);
    }

    str_destroy(&cfile);
}

/*---------------------------------------------------------------------------*/

static void i_load_properties(SetSt(GtNapProp) *properties)
{
    String *cfile = hfile_appdata("config.txt");
    Stream *stm = stm_from_file(tc(cfile), NULL);
    if (stm != NULL)
    {
        stm_lines(line, stm)
            String *key = NULL;
            String *value = NULL;
            GtNapProp *prop = NULL;
            str_split_trim(line, ":", &key, &value);
            prop = setst_insert(properties, tc(key), GtNapProp, char_t);
            if (prop != NULL)
            {
                prop->key = key;
                prop->value = value;
            }
            else
            {
                /* Duplicated property */
                str_destroy(&key);
                str_destroy(&value);
            }
        stm_next(line, stm)
        stm_close(&stm);
    }

    str_destroy(&cfile);
}

/*---------------------------------------------------------------------------*/

static void i_write_property(SetSt(GtNapProp) *properties, const char_t *wnameid, const char_t *propid, const char_t *value)
{
    String *propname = str_printf("%s-%s", wnameid, propid);
    GtNapProp *prop = setst_get(properties, tc(propname), GtNapProp, char_t);
    if (prop == NULL)
    {
        prop = setst_insert(properties, tc(propname), GtNapProp, char_t);
        prop->key = propname;
        prop->value = NULL;
        propname = NULL;
    }
    else
    {
        cassert_no_null(prop->value);
    }

    str_upd(&prop->value, value);
    str_destopt(&propname);
    i_save_properties(properties);
}

/*---------------------------------------------------------------------------*/

static const char_t *i_read_property(const SetSt(GtNapProp) *properties, const char_t *wnameid, const char_t *propid)
{
    String *propname = str_printf("%s-%s", wnameid, propid);
    const GtNapProp *prop = setst_get_const(properties, tc(propname), GtNapProp, char_t);
    const char_t *ret = NULL;
    if (prop != NULL)
        ret = tc(prop->value);
    str_destroy(&propname);
    return ret;
}

/*---------------------------------------------------------------------------*/

static void i_write_prop_r32(const char_t *wnameid, const char_t *propid, const real32_t value)
{
    char_t svalue[64];
    bstd_sprintf(svalue, sizeof(svalue), "%g", value);
    i_write_property(GTNAP_GLOBAL->properties, wnameid, propid, svalue);
}

/*---------------------------------------------------------------------------*/

static real32_t i_read_prop_r32(const char_t *wnameid, const char_t *propid)
{
    const char_t *value = i_read_property(GTNAP_GLOBAL->properties, wnameid, propid);
    if (value != NULL)
    {
        bool_t err = FALSE;
        real32_t val32 = str_to_r32(value, &err);
        if (err == FALSE)
            return val32;
    }

    return i_UNDEF_R32;
}

/*---------------------------------------------------------------------------*/

static void i_gtnap_destroy(GtNap **gtnap)
{
    cassert_no_null(gtnap);
    cassert_no_null(*gtnap);
    cassert(*gtnap == GTNAP_GLOBAL);
    cassert(arrpt_size((*gtnap)->menu_callbacks, GtNapCallback) == 0);
    arrpt_destroy(&(*gtnap)->windows, i_destroy_gtwin, GtNapWindow);
    arrpt_destroy(&(*gtnap)->menu_callbacks, i_destroy_callback, GtNapCallback);
    font_destroy(&(*gtnap)->global_font);
    font_destroy(&(*gtnap)->button_font);
    font_destroy(&(*gtnap)->edit_font);
    str_destroy(&(*gtnap)->title);
    str_destroy(&(*gtnap)->working_path);
    str_destroy(&(*gtnap)->debugger_path);

    if ((*gtnap)->debugger != NULL)
        nap_debugger_destroy(&(*gtnap)->debugger);

    setst_destroy(&(*gtnap)->properties, i_remove_property, GtNapProp);
    nforms_finish();
    heap_delete(&(*gtnap), GtNap);
}

/*---------------------------------------------------------------------------*/

static GtNapWindow *i_gtwin(GtNap *gtnap, const uint32_t wid)
{
    cassert_no_null(gtnap);
    arrpt_foreach(gtwin, gtnap->windows, GtNapWindow)
        if (gtwin->id == wid)
            return gtwin;
    arrpt_end()
    cassert_msg(FALSE, "Invalid window id");
    return NULL;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_gtwin_index(GtNap *gtnap, const uint32_t wid)
{
    cassert_no_null(gtnap);
    arrpt_foreach(gtwin, gtnap->windows, GtNapWindow)
        if (gtwin->id == wid)
            return gtwin_i;
    arrpt_end()
    cassert(FALSE);
    return UINT32_MAX;
}

/*---------------------------------------------------------------------------*/

static Window *i_effective_window(GtNapWindow *gtwin, GtNap *gtnap)
{
    cassert_no_null(gtwin);
    cassert_no_null(gtnap);
    if (gtwin->parent_id == UINT32_MAX)
    {
        cassert_no_null(gtwin->window);
        return gtwin->window;
    }
    else
    {
        GtNapWindow *gtparent = i_gtwin(gtnap, gtwin->parent_id);
        cassert_no_null(gtparent->window);
        return gtparent->window;
    }
}

/*---------------------------------------------------------------------------*/

bool_t i_gtwin_alive(GtNapWindow *gtwin, GtNap *gtnap)
{
    cassert_no_null(gtnap);
    arrpt_foreach(win, gtnap->windows, GtNapWindow)
        if (win == gtwin)
            return TRUE;
    arrpt_end();
    return FALSE;
}

/*---------------------------------------------------------------------------*/

static GtNapWindow *i_current_gtwin(GtNap *gtnap)
{
    uint32_t id = 0;
    cassert_no_null(gtnap);
    id = arrpt_size(gtnap->windows, GtNapWindow);
    if (id >= 1)
        return arrpt_get(gtnap->windows, id - 1, GtNapWindow);
    return NULL;
}

/*---------------------------------------------------------------------------*/

static GtNapWindow *i_current_main_gtwin(GtNap *gtnap)
{
    cassert_no_null(gtnap);
    arrpt_forback(gtwin, gtnap->windows, GtNapWindow)
        {
            if (gtwin->parent_id == UINT32_MAX)
                return gtwin;
        }
    arrpt_end()
    return NULL;
}

/*---------------------------------------------------------------------------*/

static GtNapObject *i_gtobj(GtNap *gtnap, const uint32_t wid, const uint32_t id)
{
    GtNapWindow *gtwin = i_gtwin(gtnap, wid);
    cassert_no_null(gtwin);
    return arrpt_get(gtwin->objects, id, GtNapObject);
}

/*---------------------------------------------------------------------------*/

static GtNapObject *i_get_button(GtNapWindow *gtwin, const uint32_t index)
{
    uint32_t i = 0;
    cassert_no_null(gtwin);
    arrpt_foreach(obj, gtwin->objects, GtNapObject)
        if (obj->type == ekOBJ_BUTTON)
        {
            if (i == index)
                return obj;
            i += 1;
        }
    arrpt_end();
    cassert(FALSE);
    return NULL;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_num_buttons(const GtNapWindow *gtwin)
{
    uint32_t n = 0;
    cassert_no_null(gtwin);
    arrpt_foreach_const(obj, gtwin->objects, GtNapObject)
        if (obj->type == ekOBJ_BUTTON)
            n += 1;
    arrpt_end();
    return n;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_num_images(const GtNapWindow *gtwin)
{
    uint32_t n = 0;
    cassert_no_null(gtwin);
    arrpt_foreach_const(obj, gtwin->objects, GtNapObject)
        if (obj->type == ekOBJ_IMAGE)
            n += 1;
    arrpt_end();
    return n;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_num_edits(const GtNapWindow *gtwin)
{
    uint32_t n = 0;
    cassert_no_null(gtwin);
    arrpt_foreach_const(obj, gtwin->objects, GtNapObject)
        if (obj->type == ekOBJ_EDIT)
            n += 1;
    arrpt_end();
    return n;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_num_texts(const GtNapWindow *gtwin)
{
    uint32_t n = 0;
    cassert_no_null(gtwin);
    arrpt_foreach_const(obj, gtwin->objects, GtNapObject)
        if (obj->type == ekOBJ_TEXTVIEW)
            n += 1;
    arrpt_end();
    return n;
}

/*---------------------------------------------------------------------------*/

static void i_set_defbutton(GtNapWindow *gtwin)
{
    GtNapObject *button = i_get_button(gtwin, gtwin->default_button);
    if (button != NULL)
        window_defbutton(gtwin->window, (Button *)button->component);
}

/*---------------------------------------------------------------------------*/

static Listener *i_gtnap_listener(HB_ITEM *block, const int32_t key, const uint32_t autoclose_id, GtNapWindow *gtwin, FPtr_gtnap_callback func_callback)
{
    GtNapCallback *callback = heap_new0(GtNapCallback);
    cassert_no_null(gtwin);
    callback->block = block ? hb_itemNew(block) : NULL;
    callback->gtwin = gtwin;
    callback->key = key;
    callback->autoclose_id = autoclose_id;
    arrpt_append(gtwin->callbacks, callback, GtNapCallback);
    return listener(callback, func_callback, GtNapCallback);
}

/*---------------------------------------------------------------------------*/

/* Change this value to make buttons higher */
static real32_t i_button_vpadding(void)
{
    return 0;
}

/*---------------------------------------------------------------------------*/

/* Change this value to make edits higher */
static real32_t i_edit_vpadding(void)
{
    return 0;
}

/*---------------------------------------------------------------------------*/

static real32_t i_button_size(const Font *font)
{
    /* Create an impostor window, only for measure the real height of buttons */
    real32_t bh = 0;
    Panel *panel = panel_create();
    Button *button = button_push();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_OFFSCREEN);
    Layout *layout = layout_create(1, 1);
    button_text(button, "DEMO");
    button_font(button, font);
    button_vpadding(button, i_button_vpadding());
    layout_button(layout, button, 0, 0);
    panel_layout(panel, layout);
    window_panel(window, panel);
    window_show(window);
    bh = button_get_height(button);
    window_destroy(&window);
    return bh;
}

/*---------------------------------------------------------------------------*/

static real32_t i_edit_size(const Font *font)
{
    /* Create an impostor window, only for measure the real height of editbox */
    real32_t eh = 0;
    Panel *panel = panel_create();
    Edit *edit = edit_create();
    Window *window = window_create(ekWINDOW_STD | ekWINDOW_OFFSCREEN);
    Layout *layout = layout_create(1, 1);
    edit_font(edit, font);
    edit_vpadding(edit, i_edit_vpadding());
    layout_edit(layout, edit, 0, 0);
    panel_layout(panel, layout);
    window_panel(window, panel);
    window_show(window);
    eh = edit_get_height(edit);
    window_destroy(&window);
    return eh;
}

/*---------------------------------------------------------------------------*/

static bool_t i_font_fit_height(GtNap *gtnap, const real32_t fsize, const real32_t height, const real32_t tolerance)
{
    real32_t w = 0, h = 0;
    cassert_no_null(gtnap);
    ptr_destopt(font_destroy, &gtnap->global_font, Font);
    gtnap->global_font = font_monospace(fsize, ekFCELL);

    /* Compute the real size of a cell, based on font */
    font_extents(gtnap->global_font, i_FONT_REF_TEXT, -1, &w, &h);
    if (h <= height + tolerance)
    {
        gtnap->label_y_sizef = h;
        gtnap->cell_y_sizef = h;
        gtnap->cell_x_sizef = w / (real32_t)unicode_nchars(i_FONT_REF_TEXT, ekUTF8);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*---------------------------------------------------------------------------*/

static bool_t i_button_fit_height(GtNap *gtnap, const real32_t fsize, const real32_t height, const real32_t tolerance)
{
    real32_t bsize = fsize;
    cassert_no_null(gtnap);
    while (bsize > i_MINIMAL_FONT_SIZE)
    {
        real32_t bheight = 0;
        ptr_destopt(font_destroy, &gtnap->button_font, Font);
        gtnap->button_font = font_monospace(bsize, ekFCELL);
        bheight = i_button_size(gtnap->button_font);
        if (bheight > height + tolerance)
        {
            bsize -= 1;
        }
        else
        {
            gtnap->button_y_sizef = bheight;
            break;
        }
    }

    return (bool_t)(bsize >= i_MINIMAL_FONT_SIZE);
}

/*---------------------------------------------------------------------------*/

static bool_t i_edit_fit_height(GtNap *gtnap, const real32_t fsize, const real32_t height, const real32_t tolerance)
{
    real32_t esize = fsize;
    cassert_no_null(gtnap);
    while (esize > i_MINIMAL_FONT_SIZE)
    {
        real32_t eheight = 0;
        ptr_destopt(font_destroy, &gtnap->edit_font, Font);
        gtnap->edit_font = font_monospace(esize, ekFCELL);
        eheight = i_edit_size(gtnap->edit_font);
        if (eheight > height + tolerance)
        {
            esize -= 1;
        }
        else
        {
            gtnap->edit_y_sizef = eheight;
            break;
        }
    }

    return (bool_t)(esize >= i_MINIMAL_FONT_SIZE);
}

/*---------------------------------------------------------------------------*/

static bool_t i_compute_font_size(const real32_t screen_width, const real32_t screen_height, GtNap *gtnap)
{
    bool_t ok = TRUE;
    const char_t *ffamily = NULL;
    real32_t cell_height = 0;
    real32_t fsize = 0;
    /*
     *    Common resolutions:
     *    Height          Width
     *    >= 1024         >= 1680
     *    >= 1024         >= 1440
     *    >= 1024         >= 1280
     *    >=  960         >= 1280
     *    >=  864         >= 1152
     *    >=  768         >= 1024
     *    >=  600         >= 1024   (NetBooks)
     *    >=  600         >=  800   (SuperVGA)
     *
     */
    if (screen_height <= 600)
    {
        /*
         * Practical tests have shown that, at low vertical resolution (height),
         * the "Courier New" font presents a jagged edge that makes the letters difficult to read.
         * In this situation, "Lucida Console" looks much better.
         *
         * "Lucida Console" isn't available on Linux or macOS. In these cases, we've left
         * the system default monospace font.
         */
#if defined(__WINDOWS__)
        ffamily = "Lucida Console";
#else
        ffamily = NULL;
#endif
    }
    else
    {
        /*
         * At high vertical resolution, "Courier New" no longer displays the aliasing
         * and looks better. The downside of "Lucida Console" in this case is that it
         * becomes too wide, looking heavy and ugly.
         *
         * "Courier New" is available on virtually all operating systems, including Linux
         * and macOS. If it isn't available, the default monospace font will be selected.
         */
        ffamily = "Courier New";
    }

    if (ffamily != NULL)
        draw2d_preferred_monospace(ffamily);

    cell_height = bmath_floorf(screen_height / (real32_t)INIT_ROWS);
    fsize = cell_height;

    /* Try to find a font size that fits the required screen height */
    while (fsize > i_MINIMAL_FONT_SIZE)
    {
        if (i_font_fit_height(gtnap, fsize, cell_height, 1) == TRUE)
            break;
        else
            fsize -= 1;
    }

    if (fsize <= i_MINIMAL_FONT_SIZE)
        ok = FALSE;

    /*
     * Try to find a button font size that fits the required screen height.
     * Button font size will be slightly small than cell font.
     */
    if (ok == TRUE)
        ok = i_button_fit_height(gtnap, fsize, cell_height, 1);

    /*
     * Try to find a edit font size that fits the required screen height.
     * Edit font size will be slightly small than cell font.
     */
    if (ok == TRUE)
        ok = i_edit_fit_height(gtnap, fsize, cell_height, 1);

    /* Fit the cell width */
    if (ok == TRUE)
    {
        real32_t current_width = INIT_COLS * gtnap->cell_x_sizef;
        if (bmath_absf(current_width - screen_width) > i_MAX_SCREEN_HEIGHT_TOLERANCE_PX)
        {
            real32_t scale = screen_width / current_width;
            Font *global_font = font_with_xscale(gtnap->global_font, scale);
            Font *button_font = font_with_xscale(gtnap->button_font, scale);
            Font *edit_font = font_with_xscale(gtnap->edit_font, scale);
            font_destroy(&gtnap->global_font);
            font_destroy(&gtnap->button_font);
            font_destroy(&gtnap->edit_font);
            gtnap->global_font = global_font;
            gtnap->button_font = button_font;
            gtnap->edit_font = edit_font;
            gtnap->cell_x_sizef = screen_width / (real32_t)INIT_COLS;
        }
    }

    /* Final cell sizes */
    if (ok == TRUE)
    {
        cassert(gtnap->cell_x_sizef > 0);
        cassert(gtnap->cell_y_sizef > 0);
        cassert(gtnap->label_y_sizef > 0 && gtnap->label_y_sizef <= gtnap->cell_y_sizef);
        /*cassert(gtnap->button_y_sizef > 0 && gtnap->button_y_sizef <= gtnap->cell_y_sizef);*/
        /*cassert(gtnap->edit_y_sizef > 0 && gtnap->edit_y_sizef <= gtnap->cell_y_sizef);*/
    }

    return ok;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_remove_utf8_CR(char_t *utf8)
{
    /* Remove the Carriage Return (CR) character (NAppGUI doesn't like) */
    uint32_t i = 0, j = 0;
    for (; utf8[i] != 0;)
    {
        if (utf8[i] != 13)
        {
            utf8[j] = utf8[i];
            j += 1;
        }

        i += 1;
    }

    utf8[j] = 0;
    return j;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_item_to_utf8(HB_ITEM *item, char_t *utf8, const uint32_t size)
{
    cassert(HB_ITEM_TYPE(item) == HB_IT_STRING);
    hb_itemCopyStrUTF8(item, (char *)utf8, (HB_SIZE)size);
    return i_remove_utf8_CR(utf8);
}

/*---------------------------------------------------------------------------*/

static String *i_item_to_utf8_string(HB_ITEM *item)
{
    HB_SIZE s1 = 0, s2 = 0;
    String *str = NULL;
    cassert(HB_ITEM_TYPE(item) == HB_IT_STRING);
    s1 = hb_itemCopyStrUTF8(item, NULL, (HB_SIZE)UINT32_MAX);
    str = str_reserve((uint32_t)s1);
    s2 = hb_itemCopyStrUTF8(item, tcc(str), s1 + 1);
    cassert_unref(s1 == s2, s2);
    i_remove_utf8_CR(tcc(str));
    return str;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_cp_to_utf8(const char_t *cp_str, char_t *utf8, const uint32_t size)
{
    HB_CODEPAGE *cp = hb_vmCDP();
    cassert_no_null(cp_str);
    cassert_no_null(utf8);
    hb_cdpStrToUTF8(cp, (const char *)cp_str, (HB_SIZE)str_len_c(cp_str), (char *)utf8, (HB_SIZE)size);
    return i_remove_utf8_CR(utf8);
}

/*---------------------------------------------------------------------------*/

static String *i_cp_to_utf8_string(const char_t *cp_str)
{
    /* TODO: Improve, make dynamic */
    char_t utf8[STATIC_TEXT_SIZE];
    String *str = NULL;
    i_cp_to_utf8(cp_str, utf8, sizeof32(utf8));
    str = str_c(utf8);
    return str;
}

/*---------------------------------------------------------------------------*/

static String *i_utf8_to_cp_string(const char_t *utf8)
{
    HB_CODEPAGE *cp = hb_vmCDP();
    HB_SIZE n = (HB_SIZE)str_len_c(utf8);
    HB_SIZE s1 = hb_cdpUTF8AsStrLen(cp, utf8, n, 0);
    String *str = str_reserve((uint32_t)s1);
    HB_SIZE s2 = hb_cdpUTF8ToStr(cp, utf8, n, tcc(str), s1 + 1);
    cassert_unref(s1 == s2, s2);
    return str;
}

/*---------------------------------------------------------------------------*/

static void i_utf8_to_cp(const char_t *utf8, char_t *buffer, const uint32_t size)
{
    HB_CODEPAGE *cp = hb_vmCDP();
    HB_SIZE n = (HB_SIZE)str_len_c(utf8);
    HB_SIZE s1 = hb_cdpUTF8AsStrLen(cp, utf8, n, 0);
    HB_SIZE s2 = hb_cdpUTF8ToStr(cp, utf8, n, buffer, size);
    cassert_unref(s1 == s2, s1);
    cassert_unref(s1 == s2, s2);
}

/*---------------------------------------------------------------------------*/

static uint8_t i_utf8_to_cp_char(const uint32_t codepoint)
{
    char_t utf8[16];
    char_t cpstr[16];
    HB_SIZE s1, s2;
    HB_CODEPAGE *cp = hb_vmCDP();
    uint32_t nb = unicode_to_char(codepoint, utf8, ekUTF8);
    utf8[nb] = 0;
    s1 = hb_cdpUTF8AsStrLen(cp, utf8, (HB_SIZE)nb, sizeof(utf8));
    s2 = hb_cdpUTF8ToStr(cp, utf8, (HB_SIZE)nb, cpstr, sizeof(cpstr));
    cassert_unref(s1 == s2, s1);
    cassert_unref(s1 == s2, s2);
    return (uint8_t)cpstr[0];
}

/*---------------------------------------------------------------------------*/

static S2Df i_resolution(void)
{
    S2Df screen = s2df(0, 0);
    const char_t *opt = "--res:";
    uint32_t i, argc = hb_cmdargARGC();
    const char_t **argv = dcast_const(hb_cmdargARGV(), char_t);

    for (i = 0; i < argc; ++i)
    {
        if (str_is_prefix(argv[i], opt) == TRUE)
        {
            String *width = NULL, *height = NULL;
            if (str_split(argv[i] + str_len_c(opt), "x", &width, &height) == TRUE)
            {
                screen.width = (real32_t)str_to_u32(tc(width), 10, NULL);
                screen.height = (real32_t)str_to_u32(tc(height), 10, NULL);
            }
            str_destroy(&width);
            str_destroy(&height);
            break;
        }
    }

    /* Minimum resolution accepted */
    if (screen.width < 800 || screen.height < 600)
    {
        R2Df warea = gui_workarea();
        screen = warea.size;
    }

    return screen;
}

/*---------------------------------------------------------------------------*/

static GtNap *i_gtnap_create(void)
{
    S2Df screen;
    const char_t *build_cfg = NULL;
    GTNAP_GLOBAL = heap_new0(GtNap);
    GTNAP_GLOBAL->title = i_cp_to_utf8_string(INIT_TITLE);
    GTNAP_GLOBAL->rows = INIT_ROWS;
    GTNAP_GLOBAL->cols = INIT_COLS;
    GTNAP_GLOBAL->windows = arrpt_create(GtNapWindow);
    GTNAP_GLOBAL->menu_callbacks = arrpt_create(GtNapCallback);
    GTNAP_GLOBAL->date_digits = (hb_setGetCentury() == (HB_BOOL)HB_TRUE) ? 8 : 6;
    GTNAP_GLOBAL->date_chars = GTNAP_GLOBAL->date_digits + 2;

    {
        char_t path[512];
        bfile_dir_work(path, sizeof(path));
        GTNAP_GLOBAL->working_path = str_c(path);
    }

#if defined(__DEBUG__)
    build_cfg = "Debug";
#else
    build_cfg = "Release";
#endif

    {
        const char_t *debpath = deblib_path();
#if defined(__MACOS__)
        GTNAP_GLOBAL->debugger_path = str_cpath("%s/%s/bin/gtnapdeb.app/Contents/MacOS/gtnapdeb", debpath, build_cfg);
#else
        GTNAP_GLOBAL->debugger_path = str_cpath("%s/%s/bin/gtnapdeb", debpath, build_cfg);
#endif
        GTNAP_GLOBAL->debugger_visible = FALSE;
        GTNAP_GLOBAL->debugger = NULL;
    }

    GTNAP_GLOBAL->properties = setst_create(i_prop_cmp, GtNapProp, char_t);
    i_load_properties(GTNAP_GLOBAL->properties);
    screen = i_resolution();
    if (i_compute_font_size(screen.width, screen.height, GTNAP_GLOBAL) == TRUE)
    {
        PHB_ITEM ritem = NULL;
        deblib_init_colors(i_COLORS);
        ritem = hb_itemDo(INIT_CODEBLOCK, 0);
        hb_itemRelease(ritem);
    }
    else
    {
        log_printf("Program can't init because invalid resolution %gx%g", screen.width, screen.height);
        osapp_finish();
    }

    hb_itemRelease(INIT_CODEBLOCK);
    INIT_TITLE[0] = 0;
    INIT_CODEBLOCK = NULL;
    return GTNAP_GLOBAL;
}

/*---------------------------------------------------------------------------*/

static bool_t i_with_scroll_panel(const GtNapWindow *gtwin)
{
    cassert_no_null(gtwin);
    if (gtwin->scroll_top >= 0)
        return TRUE;

    cassert(gtwin->scroll_top == INT32_MIN);
    cassert(gtwin->scroll_left == INT32_MIN);
    cassert(gtwin->scroll_bottom == INT32_MIN);
    cassert(gtwin->scroll_right == INT32_MIN);
    return FALSE;
}

/*---------------------------------------------------------------------------*/

static S2Df i_scroll_content_size(const ArrPt(GtNapObject) *objects)
{
    real32_t min_x = 1e10f;
    real32_t min_y = 1e10f;
    real32_t max_x = -1e10f;
    real32_t max_y = -1e10f;

    arrpt_foreach_const(object, objects, GtNapObject)
        if (object->in_scroll == TRUE)
        {
            real32_t x1 = object->pos.x;
            real32_t x2 = object->pos.x + object->size.width;
            real32_t y1 = object->pos.y;
            real32_t y2 = object->pos.y + object->size.height;
            if (x1 < min_x)
                min_x = x1;
            if (x2 > max_x)
                max_x = x2;
            if (y1 < min_y)
                min_y = y1;
            if (y2 > max_y)
                max_y = y2;
        }
    arrpt_end();

    return s2df((max_x - min_x) + GTNAP_GLOBAL->cell_x_sizef, (max_y - min_y) + GTNAP_GLOBAL->cell_y_sizef);
}

/*---------------------------------------------------------------------------*/

static void i_attach_to_panel(ArrPt(GtNapObject) *objects, Panel *main_panel, Panel *scroll_panel, const V2Df *scroll_offset, const objtype_t type, const GtNapToolbar *toolbar)
{
    cassert_no_null(scroll_offset);
    arrpt_foreach(object, objects, GtNapObject)
        if (object->type == type)
        {
            V2Df pos = object->pos;

            if (object->in_scroll == TRUE)
                _panel_attach_component(scroll_panel, object->component);
            else
                _panel_attach_component(main_panel, object->component);

            _component_visible(object->component, FALSE);

            if (toolbar != NULL)
            {
                switch (type)
                {
                case ekOBJ_LABEL:
                    pos.y += toolbar->heightf;
                    if (GTNAP_GLOBAL->cell_y_sizef > GTNAP_GLOBAL->label_y_sizef)
                        pos.y += (GTNAP_GLOBAL->cell_y_sizef - GTNAP_GLOBAL->label_y_sizef) / 2.f;
                    break;

                case ekOBJ_EDIT:
                    pos.y += toolbar->heightf;
                    if (GTNAP_GLOBAL->cell_y_sizef > GTNAP_GLOBAL->edit_y_sizef)
                        pos.y += (GTNAP_GLOBAL->cell_y_sizef - GTNAP_GLOBAL->edit_y_sizef) / 2.f;

                    if (object->in_scroll == TRUE)
                    {
                        object->size.width -= GTNAP_GLOBAL->cell_x_sizef;
                    }
                    break;

                case ekOBJ_IMAGE:
                case ekOBJ_MENU:
                    pos.y += toolbar->heightf;
                    break;

                case ekOBJ_TABLEVIEW:
                case ekOBJ_TEXTVIEW:
                    break;

                case ekOBJ_BUTTON:
                    if (object->editBoxIndexForButton != UINT32_MAX)
                    {
                        /* The same as related editbox */
                        pos.y += toolbar->heightf;
                    }

                    if (GTNAP_GLOBAL->cell_y_sizef > GTNAP_GLOBAL->button_y_sizef)
                        pos.y += (GTNAP_GLOBAL->cell_y_sizef - GTNAP_GLOBAL->button_y_sizef) / 2.f;
                    break;

                default:
                    cassert_default(type);
                }
            }

            if (object->in_scroll == TRUE)
            {
                pos.x += scroll_offset->x;
                pos.y += scroll_offset->y;
            }

            object->pos = pos;
            _component_set_frame(object->component, &pos, &object->size);
        }
    arrpt_end();
}

/*---------------------------------------------------------------------------*/

static void i_attach_toolbar_to_panel(const GtNapToolbar *toolbar, Panel *panel)
{
    if (toolbar != NULL)
    {
        V2Df p1, p2;
        S2Df s1, s2;
        real32_t bsize = toolbar->button_widthf + 4.f;
        p1.x = 0;
        p1.y = (toolbar->heightf - bsize) / 2.f;
        p2.x = 0;
        p2.y = p1.y;
        s1.width = bsize;
        s1.height = bsize;
        s2.width = 1;
        s2.height = bsize;

        arrpt_foreach(item, toolbar->items, GuiComponent)
            if (item != NULL)
            {
                const char_t *type = _component_type(item);
                _panel_attach_component(panel, item);
                _component_visible(item, FALSE);
                if (str_equ_c(type, "Button") == TRUE)
                {
                    _component_set_frame(item, &p1, &s1);
                    p1.x += s1.width;
                    p2.x = p1.x;
                }
                else
                {
                    cassert(str_equ_c(type, "View") == TRUE);
                    p2.x += 2;
                    _component_set_frame(item, &p2, &s2);
                    p2.x += 3;
                    p1.x = p2.x;
                }
            }
        arrpt_end();
    }
}

/*---------------------------------------------------------------------------*/

static void i_component_tabstop(ArrPt(GtNapObject) *objects, Window *window, ArrPt(GuiComponent) *tabstops, const objtype_t type)
{
    arrpt_foreach(object, objects, GtNapObject)
        if (object->type == type)
        {
            _component_visible(object->component, TRUE);

            switch (object->type)
            {
            case ekOBJ_LABEL:
            case ekOBJ_IMAGE:
                break;
            case ekOBJ_BUTTON:
                /* Buttons don't have tabstop
                   _component_taborder(object->component, window); */
                break;
            case ekOBJ_MENU:
            {
                View *view = nap_menuvert_view((Panel *)object->component);
                nap_menuvert_window((Panel *)object->component, window);
                arrpt_append(tabstops, (GuiComponent *)view, GuiComponent);
                break;
            }
            case ekOBJ_TABLEVIEW:
            case ekOBJ_TEXTVIEW:
            case ekOBJ_EDIT:
                arrpt_append(tabstops, object->component, GuiComponent);
                break;
            default:
                cassert_default(object->type);
            }
        }
    arrpt_end();
}

/*---------------------------------------------------------------------------*/

static void i_toolbar_tabstop(GtNapToolbar *toolbar, ArrPt(GuiComponent) *tabstops)
{
    if (toolbar != NULL)
    {
        /* At the moment, toolbar buttons not have tabstop */
        unref(tabstops);
        arrpt_foreach(item, toolbar->items, GuiComponent)
            if (item != NULL)
                _component_visible(item, TRUE);
        arrpt_end();
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnPreviousEdit(GtNapWindow *gtwin, Event *e)
{
    unref(e);
    window_previous_tabstop(gtwin->window);
}

/*---------------------------------------------------------------------------*/

static void i_OnNextTabstop(GtNapWindow *gtwin, Event *e)
{
    unref(e);
    cassert_no_null(gtwin);

    if (i_gtwin_alive(gtwin, GTNAP_GLOBAL) == FALSE)
        return;

    window_next_tabstop(gtwin->window);
}

/*---------------------------------------------------------------------------*/

static void i_OnLeftButton(GtNapWindow *gtwin, Event *e)
{
    unref(e);
    cassert_no_null(gtwin);
    if (gtwin->default_button > 0)
    {
        gtwin->default_button -= 1;
        i_set_defbutton(gtwin);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnRightButton(GtNapWindow *gtwin, Event *e)
{
    unref(e);
    cassert_no_null(gtwin);
    if (gtwin->default_button < i_num_buttons(gtwin) - 1)
    {
        gtwin->default_button += 1;
        i_set_defbutton(gtwin);
    }
}

/*---------------------------------------------------------------------------*/

/* Run the codeBlock that updates after a text entry in EditBox */
static void i_update_harbour_from_edit_text(const GtNapObject *gtobj)
{
    cassert_no_null(gtobj);
    if (gtobj->get_set_block != NULL)
    {
        PHB_ITEM pItem = NULL;
        const char_t *text = edit_get_text((Edit *)gtobj->component);

        if (gtobj->dtype == ekTYPE_CHARACTER)
        {
            char_t cp[STATIC_TEXT_SIZE];
            uint32_t i, len = 0;
            i_utf8_to_cp(text, cp, sizeof(cp));
            len = str_len_c(cp);
            for (i = len; i < gtobj->max_chars; ++i)
                cp[i] = ' ';
            cp[gtobj->max_chars] = 0;
            pItem = hb_itemPutC(NULL, cp);
        }
        else if (gtobj->dtype == ekTYPE_DATE)
        {
            pItem = hb_itemPutDS(NULL, text);
        }
        else
        {
            cassert_msg(FALSE, "Unknown data type in i_update_harbour_from_edit_text");
        }

        if (pItem != NULL)
        {
            PHB_ITEM ritem = hb_itemDo(gtobj->get_set_block, 1, pItem);
            hb_itemRelease(pItem);
            hb_itemRelease(ritem);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_set_label_text(GtNapObject *obj, const char_t *utf8_text)
{
    uint32_t nchars = UINT32_MAX;
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_LABEL);
    if (utf8_text != NULL)
    {
        nchars = unicode_nchars(utf8_text, ekUTF8);
        label_text((Label *)obj->component, utf8_text);
    }
    else if (obj->text != NULL)
    {
        nchars = str_nchars(obj->text);
        label_text((Label *)obj->component, tc(obj->text));
    }
    else if (obj->text_block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(obj->text_block, 0);
        HB_TYPE type = HB_ITEM_TYPE(ritem);

        if (type == HB_IT_STRING)
        {
            char_t buffer[STATIC_TEXT_SIZE];
            i_item_to_utf8(ritem, buffer, sizeof32(buffer));
            nchars = unicode_nchars(buffer, ekUTF8);
            label_text((Label *)obj->component, buffer);
        }

        hb_itemRelease(ritem);
    }

    /* Text has been updated */
    if (nchars != UINT32_MAX)
    {
        obj->size.width = (real32_t)nchars * GTNAP_GLOBAL->cell_x_sizef;
        if (obj->gtwin->is_configured == TRUE)
            _component_set_frame(obj->component, &obj->pos, &obj->size);
    }
}

/*---------------------------------------------------------------------------*/

static void i_stop_modal(GtNap *gtnap, GtNapWindow *gtwin, const uint32_t retcode)
{
    unref(gtnap);
    cassert_no_null(gtwin);
    gtwin->modal_window_alive = FALSE;
    window_stop_modal(gtwin->window, retcode);
}

/*---------------------------------------------------------------------------*/

static void i_OnEditChange(GtNapObject *gtobj, Event *e)
{
    const EvText *p = event_params(e, EvText);
    GtNapWindow *gtwin = NULL;
    FocusInfo finfo;

    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_EDIT);
    gtwin = gtobj->gtwin;
    cassert_no_null(gtwin);
    window_focus_info(gtwin->window, &finfo);

    /* Current window is in close process by 'window_stop_modal'. Validations should not be performed */
    if (gtwin->modal_window_alive == FALSE)
        return;

    /* Update Harbour with the content of the EditBox */
    i_update_harbour_from_edit_text(gtobj);

    /* The editbox has a validation code block */
    if (gtobj->valida_block != NULL)
    {
        bool_t valid = TRUE;
        PHB_ITEM ritem = hb_itemDo(gtobj->valida_block, 0);
        HB_TYPE type = HB_ITEM_TYPE(ritem);
        cassert_unref(type == HB_IT_LOGICAL, type);
        valid = (bool_t)hb_itemGetL(ritem);
        hb_itemRelease(ritem);

        /* If the input is not valid --> The editbox keep the focus and event finish here */
        if (valid == FALSE)
        {
            bool_t *r = event_result(e, bool_t);
            *r = FALSE;
            return;
        }
    }

    /* The window has a global function to process invalid date */
    if (gtobj->dtype == ekTYPE_DATE)
    {
        if (gtwin->error_date_block != NULL)
        {
            long date_ok = hb_dateUnformat(p->text, hb_setGetDateFormat());

            /* Date invalid --> The editbox keep the focus and event finish here */
            if (date_ok == 0)
            {
                PHB_ITEM ritem = NULL;
                bool_t *r = event_result(e, bool_t);
                ritem = hb_itemDo(gtwin->error_date_block, 0);
                hb_itemRelease(ritem);
                *r = FALSE;
                return;
            }
        }
    }

    /* Update possible labels associated with this input */
    arrpt_foreach(obj, gtwin->objects, GtNapObject)
        if (obj->type == ekOBJ_LABEL)
            i_set_label_text(obj, NULL);
    arrpt_end();

    /* If user have pressed the [ESC] key, we leave the stop for that event */
    if (gtwin->is_closed_by_esc == FALSE)
    {
        /* The last editbox has lost the focus --> Close the window */
        if (gtobj->is_last_edit == TRUE)
        {
            /* The user has explicity intro the editbox value */
            if (finfo.action == ekGUI_TAB_KEY || finfo.action == ekGUI_TAB_NEXT)
            {
                bool_t close = TRUE;

                /* We have asociated a confirmation block */
                if (gtwin->confirm_block != NULL)
                {
                    PHB_ITEM ritem = hb_itemDo(gtwin->confirm_block, 0);
                    HB_TYPE type = HB_ITEM_TYPE(ritem);
                    cassert_unref(type == HB_IT_LOGICAL, type);
                    close = (bool_t)hb_itemGetL(ritem);
                    hb_itemRelease(ritem);
                }

                if (close == TRUE)
                    i_stop_modal(GTNAP_GLOBAL, gtwin, NAP_MODAL_LAST_INPUT);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

static bool_t i_is_editable(GtNapWindow *gtwin, GtNapObject *gtobj)
{
    bool_t editable = TRUE;
    cassert_no_null(gtwin);
    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_EDIT || gtobj->type == ekOBJ_TEXTVIEW);
    if (editable == TRUE && gtwin->is_editable_block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(gtwin->is_editable_block, 0);
        cassert(HB_ITEM_TYPE(ritem) == HB_IT_LOGICAL);
        editable = (bool_t)hb_itemGetL(ritem);
        hb_itemRelease(ritem);
    }

    if (editable == TRUE && gtobj->is_editable_block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(gtobj->is_editable_block, 0);
        cassert(HB_ITEM_TYPE(ritem) == HB_IT_LOGICAL);
        editable = (bool_t)hb_itemGetL(ritem);
        hb_itemRelease(ritem);
    }

    return editable;
}

/*---------------------------------------------------------------------------*/

static void i_get_edit_text(const GtNapObject *obj, char_t *utf8, const uint32_t size)
{
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_EDIT);
    if (obj->get_set_block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(obj->get_set_block, 0);
        HB_TYPE type = HB_ITEM_TYPE(ritem);
        switch (type)
        {
        case HB_IT_STRING:
            cassert(obj->dtype == ekTYPE_CHARACTER);
            hb_itemCopyStrUTF8(ritem, (char *)utf8, (HB_SIZE)size);
            break;

        case HB_IT_DATE:
        {
            char date[16];
            char temp[16];
            cassert(obj->dtype == ekTYPE_DATE);
            hb_itemGetDS(ritem, date);
            hb_dateFormat(date, temp, hb_setGetDateFormat());
            str_copy_c(utf8, size, temp);
            break;
        }

        default:
            str_copy_c(utf8, size, "");
        }

        hb_itemRelease(ritem);
    }
    else
    {
        str_copy_c(utf8, size, "");
    }
}

/*---------------------------------------------------------------------------*/

static void i_jump_nchars(const char_t **src, const uint32_t nchars)
{
    uint32_t i = 0;
    cassert_no_null(src);
    cassert_no_null(*src);
    for (i = 0; i < nchars; ++i)
    {
        uint32_t nb;
        uint32_t c = unicode_to_u32b(*src, ekUTF8, &nb);
        if (c != 0)
        {
            *src += nb;
        }
        else
        {
            break;
        }
    }
}

/*---------------------------------------------------------------------------*/

static int32_t i_filter_number(const EvText *text, EvTextFilter *filter)
{
    int32_t len = text->len;

    if (len > 0)
    {
        const char_t *src2 = text->text;
        int32_t i = 0;
        i_jump_nchars(&src2, text->cpos - text->len);
        for (i = 0; i < text->len; ++i)
        {
            uint32_t nb;
            uint32_t c = unicode_to_u32b(src2, ekUTF8, &nb);
            if (c != 0)
            {
                if (c >= '0' && c <= '9')
                {
                }
                else
                {
                    cassert(len > 0);
                    len -= 1;
                }
            }
        }
    }

    {
        const char_t *src = text->text;
        char_t *dest = filter->text;
        uint32_t dsize = sizeof(filter->text);
        uint32_t i = 0, cpos = text->cpos;
        uint32_t back = 0;
        for (;;)
        {
            uint32_t nb;
            uint32_t c = unicode_to_u32b(src, ekUTF8, &nb);
            if (c != 0)
            {
                if (c >= '0' && c <= '9')
                {
                    if (dsize > nb)
                    {
                        unicode_to_char(c, dest, ekUTF8);
                        dest += nb;
                        dsize -= nb;
                    }
                }
                else
                {
                    if (cpos > i)
                        back += 1;
                }

                i += 1;
                src += nb;
            }
            /* End of input string */
            else
            {
                break;
            }
        }

        cassert(dsize > 0);
        *dest = '\0';
        filter->cpos = cpos - back;
        filter->apply = TRUE;
    }

    return len;
}

/*---------------------------------------------------------------------------*/

static void i_copy_nchars(const char_t **src, char_t **dest, uint32_t *dsize, const uint32_t nchars)
{
    uint32_t i = 0;
    cassert_no_null(src);
    cassert_no_null(*src);
    cassert_no_null(dest);
    cassert_no_null(*dest);
    cassert_no_null(dsize);
    for (i = 0; i < nchars; ++i)
    {
        uint32_t nb = 0;
        uint32_t c = unicode_to_u32b(*src, ekUTF8, &nb);
        if (c != 0 && *dsize > nb)
        {
            /* There is space in dest */
            unicode_to_char(c, *dest, ekUTF8);
            *src += nb;
            *dest += nb;
            *dsize -= nb;
        }
        else
        {
            break;
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_filter_overwrite(const EvText *text, EvTextFilter *filter, const uint32_t max_chars)
{
    bool_t updated = FALSE;
    /* Text has been inserted */
    cassert_no_null(text);
    cassert_no_null(filter);
    if (text->len > 0)
    {
        const char_t *src = text->text;
        char_t *dest = filter->text;
        uint32_t dsize = sizeof(filter->text);

        /* Copy all characters from init to caret position */
        i_copy_nchars(&src, &dest, &dsize, text->cpos);

        /* Jump 'len' chars in src */
        i_jump_nchars(&src, text->len);

        /* Copy the rest of chars */
        i_copy_nchars(&src, &dest, &dsize, UINT32_MAX);
        cassert(dsize > 0);
        *dest = '\0';
        updated = TRUE;
    }

    if (updated == FALSE)
        str_copy_c(filter->text, sizeof(filter->text), text->text);

    filter->cpos = text->cpos;
    filter->apply = TRUE;

    /* Trim to size*/
    {
        uint32_t nc = unicode_nchars(filter->text, ekUTF8);
        if (nc > max_chars)
        {
            const char_t *d = filter->text;
            i_jump_nchars(&d, max_chars);
            *((char_t *)d) = '\0';

            if (filter->cpos > max_chars)
                filter->cpos = max_chars;
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_filter_date(const EvText *text, EvTextFilter *filter, const char_t *format, bool_t insert)
{
    const char_t *src = text->text;
    char_t *dest = filter->text;
    uint32_t dsize = sizeof(filter->text);
    uint32_t i = 0;
    uint32_t cpos = text->cpos;
    for (;;)
    {
        uint32_t nbf;
        bool_t sep = FALSE;

        /* Current character of format string */
        uint32_t f = unicode_to_u32b(format, ekUTF8, &nbf);

        /* End of format string --> bye */
        if (f == 0)
            break;

        /* Digit position */
        if (f == 'd' || f == 'D' || f == 'm' || f == 'M' || f == 'y' || f == 'Y')
        {
            uint32_t nb;
            uint32_t d = unicode_to_u32b(src, ekUTF8, &nb);

            /* We have a digit into input text */
            if (d != 0)
            {
                /* Write the digit into dest */
                if (dsize > nb)
                {
                    unicode_to_char(d, dest, ekUTF8);
                    dest += nb;
                    dsize -= nb;
                }
                src = unicode_next(src, ekUTF8);
            }
            /* No more digits --> Write an space in dest */
            else
            {
                if (dsize > 1)
                {
                    unicode_to_char(' ', dest, ekUTF8);
                    dest += 1;
                    dsize -= 1;
                }
            }
        }
        /* We have a format separator character, just write it into dest */
        else
        {
            sep = TRUE;
            if (dsize > nbf)
            {
                unicode_to_char(f, dest, ekUTF8);
                dest += nbf;
                dsize -= nbf;
            }
        }

        /* Advance to next character of format string */
        format = unicode_next(format, ekUTF8);
        i += 1;

        /* Compute the new caret position */
        if (sep == TRUE)
        {
            if (insert == TRUE)
            {
                if (cpos >= i - 1)
                    cpos = cpos + 1;
            }
            else
            {
                if (cpos == i - 1)
                    cpos -= 1;
            }
        }
    }

    cassert(dsize > 0);
    *dest = '\0';
    filter->apply = TRUE;
    filter->cpos = cpos;
}

/*---------------------------------------------------------------------------*/

static void i_filter_tecla(const GtNapObject *gtobj, const EvText *text, EvTextFilter *filter)
{
    bool_t updated = FALSE;
    cassert_no_null(gtobj);
    cassert_no_null(text);
    cassert_no_null(filter);
    cassert(gtobj->type == ekOBJ_EDIT);
    /* Some text has been inserted */
    if (text->len > 0)
    {
        /* We have a filter */
        if (gtobj->keyfilter_block != NULL)
        {
            const char_t *src = text->text;
            char_t *dest = filter->text;
            uint32_t dsize = sizeof(filter->text);
            int32_t i, n = (int32_t)text->cpos - text->len;
            cassert(n >= 0);

            /* Copy the string prefix (old string init until new insertions) */
            i_copy_nchars(&src, &dest, &dsize, (uint32_t)n);

            /* Filter all characters inserted */
            for (i = 0; i < text->len; ++i)
            {
                uint32_t nb;
                uint32_t c = unicode_to_u32b(src, ekUTF8, &nb);
                if (c != 0)
                {
                    /* From Unicode (NappGUI) to code page */
                    uint8_t cp2 = i_utf8_to_cp_char(c);
                    uint32_t nb2, ncp;

                    /* Set character as lastKey */
                    hb_inkeySetLast(cp2);

                    /* Call to filter */
                    {
                        PHB_ITEM ritem = hb_itemDo(gtobj->keyfilter_block, 0);
                        HB_TYPE type = HB_ITEM_TYPE(ritem);
                        if (type == HB_IT_NIL)
                        {
                            ncp = c;
                            nb2 = nb;
                        }
                        else
                        {
                            char_t temp[32];
                            cassert(type == HB_IT_STRING);
                            hb_itemCopyStrUTF8(ritem, temp, sizeof(temp));
                            cassert(unicode_nchars(temp, ekUTF8) == 1);
                            ncp = unicode_to_u32b(temp, ekUTF8, &nb2);
                        }

                        hb_itemRelease(ritem);
                    }

                    /* There is space in dest */
                    if (dsize > nb2)
                    {
                        unicode_to_char(ncp, dest, ekUTF8);
                        dest += nb2;
                        dsize -= nb2;
                    }

                    src += nb;
                }
                else
                {
                    break;
                }
            }

            /* Copy the rest of the string */
            i_copy_nchars(&src, &dest, &dsize, UINT32_MAX);
            cassert(dsize > 0);

            *dest = '\0';
            updated = TRUE;
        }
    }

    /* No filter applied, just copy the input string */
    if (updated == FALSE)
        str_copy_c(filter->text, sizeof(filter->text), text->text);

    filter->apply = TRUE;
    filter->cpos = text->cpos;
}

/*---------------------------------------------------------------------------*/

static void i_filter_tecla_textview(const GtNapObject *gtobj, const EvText *text, EvTextFilter *filter)
{
    bool_t updated = FALSE;
    cassert_no_null(gtobj);
    cassert_no_null(text);
    cassert_no_null(filter);
    cassert(gtobj->type == ekOBJ_TEXTVIEW);
    /* Some text has been inserted */
    if (text->len > 0)
    {
        /* We have a filter */
        if (gtobj->keyfilter_block != NULL)
        {
            const char_t *src = text->text;
            char_t *dest = filter->text;
            uint32_t dsize = sizeof(filter->text);
            int32_t i;

            /* Filter all characters inserted */
            for (i = 0; i < text->len; ++i)
            {
                uint32_t nb;
                uint32_t c = unicode_to_u32b(src, ekUTF8, &nb);
                if (c != 0)
                {
                    /* From Unicode (NappGUI) to code page */
                    uint8_t cp2 = i_utf8_to_cp_char(c);
                    uint32_t nb2, ncp;

                    /* Set character as lastKey */
                    hb_inkeySetLast(cp2);

                    /* Call to filter */
                    {
                        PHB_ITEM ritem = hb_itemDo(gtobj->keyfilter_block, 0);
                        HB_TYPE type = HB_ITEM_TYPE(ritem);
                        if (type == HB_IT_NIL)
                        {
                            ncp = c;
                            nb2 = nb;
                        }
                        else
                        {
                            char_t temp[32];
                            cassert(type == HB_IT_STRING);
                            hb_itemCopyStrUTF8(ritem, temp, sizeof(temp));
                            cassert(unicode_nchars(temp, ekUTF8) == 1);
                            ncp = unicode_to_u32b(temp, ekUTF8, &nb2);
                        }

                        hb_itemRelease(ritem);
                    }

                    /* There is space in dest */
                    if (dsize > nb2)
                    {
                        unicode_to_char(ncp, dest, ekUTF8);
                        dest += nb2;
                        dsize -= nb2;
                    }

                    src += nb;
                }
                else
                {
                    break;
                }
            }

            *dest = '\0';
            updated = TRUE;
        }
    }

    filter->apply = updated;
}

/*---------------------------------------------------------------------------*/

static void i_OnEditFilter(GtNapObject *gtobj, Event *e)
{
    const EvText *p = event_params(e, EvText);
    EvTextFilter *res = event_result(e, EvTextFilter);
    GtNapWindow *gtwin = NULL;
    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_EDIT);
    gtwin = gtobj->gtwin;
    cassert_no_null(gtwin);

    if (i_is_editable(gtwin, gtobj) == FALSE)
    {
        /* If editBox is not editable --> Restore the original text */
        i_get_edit_text(gtobj, res->text, sizeof(res->text));
        if (p->cpos > 0)
        {
            if (p->len > 0)
                res->cpos = p->cpos - p->len;
            else
                res->cpos = p->cpos;
        }
        else
        {
            res->cpos = 0;
        }

        res->apply = TRUE;
    }
    else
    {
        if (gtobj->dtype == ekTYPE_DATE)
        {
            EvTextFilter fil1;
            EvTextFilter fil2;
            uint32_t len;
            fil1.apply = FALSE;
            fil2.apply = FALSE;

            len = i_filter_number(p, &fil1);
            cassert(fil1.apply == TRUE);

            {
                EvText tf;
                tf.text = fil1.text;
                tf.cpos = fil1.cpos;
                tf.len = len;
                i_filter_overwrite(&tf, &fil2, GTNAP_GLOBAL->date_digits);
            }

            cassert(fil2.apply == TRUE);

            {
                EvText tf;
                tf.text = fil2.text;
                tf.cpos = fil2.cpos;
                tf.len = 0;
                i_filter_date(&tf, res, hb_setGetDateFormat(), p->len >= 0);
            }

            cassert(res->apply == TRUE);

            if (res->cpos == GTNAP_GLOBAL->date_chars)
                gui_OnIdle(listener(gtwin, i_OnNextTabstop, GtNapWindow));
        }
        else
        {
            EvTextFilter filTec;
            filTec.apply = FALSE;
            i_filter_tecla(gtobj, p, &filTec);
            cassert(filTec.apply == TRUE);

            {
                EvText tf;
                cassert(filTec.cpos == p->cpos);
                tf.text = filTec.text;
                tf.cpos = filTec.cpos;
                tf.len = p->len;
                i_filter_overwrite(&tf, res, gtobj->max_chars);
            }

            cassert(res->apply == TRUE);

            /* End of editable string reached. */
            if (res->cpos >= gtobj->max_chars)
                gui_OnIdle(listener(gtwin, i_OnNextTabstop, GtNapWindow));
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_set_edit_message(GtNapObject *obj, GtNapObject *mes_obj)
{
    cassert_no_null(obj);
    cassert_no_null(mes_obj);
    cassert(obj->type == ekOBJ_EDIT);
    cassert(mes_obj->type == ekOBJ_LABEL);
    if (obj->message_block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(obj->message_block, 0);
        HB_TYPE type = HB_ITEM_TYPE(ritem);

        if (type == HB_IT_STRING)
        {
            char_t buffer[1024];
            uint32_t len;
            hb_itemCopyStrUTF8(ritem, (char *)buffer, (HB_SIZE)sizeof(buffer));
            len = unicode_nchars(buffer, ekUTF8);
            mes_obj->size.width = (real32_t)len * GTNAP_GLOBAL->cell_x_sizef;
            _component_set_frame(mes_obj->component, &mes_obj->pos, &mes_obj->size);
            label_text((Label *)mes_obj->component, buffer);
        }
        else
        {
            cassert_msg(FALSE, "Unkown type in i_set_edit_message");
        }

        hb_itemRelease(ritem);
    }
}

/*---------------------------------------------------------------------------*/

static void i_set_edit_text(const GtNapObject *obj)
{
    char_t buffer[STATIC_TEXT_SIZE];
    cassert_no_null(obj);
    i_get_edit_text(obj, buffer, sizeof(buffer));
    edit_text((Edit *)obj->component, buffer);
}

/*---------------------------------------------------------------------------*/

static void i_set_view_text(const GtNapObject *obj)
{
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TEXTVIEW);
    textview_clear((TextView *)obj->component);
    if (obj->get_set_block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(obj->get_set_block, 0);
        String *str = i_item_to_utf8_string(ritem);
        textview_writef((TextView *)obj->component, tc(str));
        hb_itemRelease(ritem);
        str_destroy(&str);
    }
}

/*---------------------------------------------------------------------------*/

static void i_launch_wizard(GtNapWindow *gtwin, GtNapObject *obj)
{
    char_t temp[1024];
    PHB_ITEM ritem = NULL;
    HB_TYPE type = HB_IT_NIL;
    cassert_no_null(gtwin);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_EDIT);
    cassert_no_null(obj->wizard_block);
    ritem = hb_itemDo(obj->wizard_block, 0);
    type = HB_ITEM_TYPE(ritem);

    if (type != HB_IT_NIL)
    {
        cassert(type == HB_IT_STRING);
        hb_itemCopyStrUTF8(ritem, temp, sizeof(temp));
        edit_text((Edit *)obj->component, temp);
    }

    hb_itemRelease(ritem);

    if (type != HB_IT_NIL)
        gui_OnIdle(listener(gtwin, i_OnNextTabstop, GtNapWindow));
}

/*---------------------------------------------------------------------------*/

static void i_OnAutoWizard(GtNapWindow *gtwin, Event *e)
{
    GtNapObject *gtobj = NULL;
    cassert_no_null(gtwin);

    if (i_gtwin_alive(gtwin, GTNAP_GLOBAL) == FALSE)
        return;

    gtobj = gtwin->wizard_obj;
    cassert_no_null(gtobj);
    unref(e);

    if (gtobj->can_auto_lista == TRUE && gtobj->auto_block != NULL && gtobj->wizard_block != NULL)
    {
        bool_t lista = FALSE;

        {
            PHB_ITEM ritem = hb_itemDo(gtobj->auto_block, 0);
            HB_TYPE type = HB_ITEM_TYPE(ritem);
            cassert_unref(type == HB_IT_LOGICAL, type);
            lista = (bool_t)hb_itemGetL(ritem);
            hb_itemRelease(ritem);
        }

        if (lista == TRUE)
        {
            gtobj->can_auto_lista = FALSE;
            i_launch_wizard(gtwin, gtobj);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnEditFocus(GtNapObject *gtobj, Event *e)
{
    const bool_t *p = event_params(e, bool_t);
    GtNapWindow *gtwin = NULL;
    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_EDIT);
    gtwin = gtobj->gtwin;
    cassert_no_null(gtwin);

    if (*p == TRUE)
    {
        gtobj->has_focus = TRUE;

        /* We sure only one control has the focus */
        arrpt_foreach(obj, gtwin->objects, GtNapObject)
            if (obj != gtobj)
            {
                obj->has_focus = FALSE;
                obj->can_auto_lista = TRUE;
            }
        arrpt_end();

        if (gtwin->message_label_id != UINT32_MAX)
        {
            GtNapObject *mes_obj = arrpt_get(gtwin->objects, gtwin->message_label_id, GtNapObject);
            i_set_edit_message(gtobj, mes_obj);
        }

        if (gtobj->when_block != NULL)
        {
            PHB_ITEM ritem = hb_itemDo(gtobj->when_block, 0);
            HB_TYPE type = HB_ITEM_TYPE(ritem);
            bool_t updated = FALSE;
            cassert_unref(type == HB_IT_LOGICAL, type);
            updated = (bool_t)hb_itemGetL(ritem);
            hb_itemRelease(ritem);

            if (updated == TRUE)
                i_set_edit_text(gtobj);
        }

        edit_select((Edit *)gtobj->component, 0, 0);

        if (gtobj->can_auto_lista == TRUE && gtobj->auto_block != NULL && gtobj->wizard_block != NULL)
        {
            gtwin->wizard_obj = gtobj;
            gui_OnIdle(listener(gtwin, i_OnAutoWizard, GtNapWindow));
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnTextFocus(GtNapObject *gtobj, Event *e)
{
    const bool_t *p = event_params(e, bool_t);
    GtNapWindow *gtwin = NULL;
    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_TEXTVIEW);
    gtwin = gtobj->gtwin;
    cassert_no_null(gtwin);

    if (*p == TRUE)
    {
        gtobj->has_focus = TRUE;

        /* We sure only one control has the focus */
        arrpt_foreach(obj, gtwin->objects, GtNapObject)
            if (obj != gtobj)
            {
                obj->has_focus = FALSE;
                obj->can_auto_lista = TRUE;
            }
        arrpt_end();
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnTextFilter(GtNapObject *gtobj, Event *e)
{
    const EvText *p = event_params(e, EvText);
    EvTextFilter *r = event_result(e, EvTextFilter);
    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_TEXTVIEW);
    i_filter_tecla_textview(gtobj, p, r);
}

/*---------------------------------------------------------------------------*/

static void i_gtwin_configure(GtNap *gtnap, GtNapWindow *gtwin, GtNapWindow *main_gtwin)
{
    Panel *scroll_panel = NULL;
    Layout *layout = layout_create(1, 1);
    V2Df offset = kV2D_ZEROf;
    cassert_no_null(gtnap);
    cassert_no_null(gtwin);
    cassert(gtwin->is_configured == FALSE);
    cassert(gtwin->panel == NULL);
    cassert_no_null(main_gtwin);
    cassert_no_null(main_gtwin->window);

    gtwin->panel = panel_custom(FALSE, FALSE, gtwin->border);

    /*if (gtwin->toolbar != NULL)
          gtwin->panel_size.height += (real32_t)GTNAP_GLOBAL->cell_y_size; */

    panel_size(gtwin->panel, gtwin->panel_size);
    panel_layout(gtwin->panel, layout);

    if (i_with_scroll_panel(gtwin) == TRUE)
    {
        /* We add a subpanel to window main panel to implement the scroll area */
        Panel *panel = panel_scroll(FALSE, TRUE);
        S2Df csize = i_scroll_content_size(gtwin->objects);
        int32_t cell_x = gtwin->scroll_left - gtwin->left;
        int32_t cell_y = gtwin->scroll_top - gtwin->top;
        real32_t pos_x = (real32_t)cell_x * GTNAP_GLOBAL->cell_x_sizef;
        real32_t pos_y = (real32_t)cell_y * GTNAP_GLOBAL->cell_y_sizef;
        real32_t width = (real32_t)(gtwin->scroll_right - gtwin->scroll_left + 3) * GTNAP_GLOBAL->cell_x_sizef;
        real32_t height = (real32_t)(gtwin->scroll_bottom - gtwin->scroll_top + 1) * GTNAP_GLOBAL->cell_y_sizef;
        V2Df pos = v2df(pos_x, pos_y);
        S2Df size = s2df(width, height);
        _panel_attach_component(gtwin->panel, (GuiComponent *)panel);
        _component_set_frame((GuiComponent *)panel, &pos, &size);
        _component_visible((GuiComponent *)panel, FALSE);
        _panel_content_size(panel, csize.width, csize.height);
        offset.x = -pos.x;
        offset.y = -pos.y;
        scroll_panel = panel;
        gtwin->scrolled_panel = panel;
    }

    /* Attach gui objects in certain Z-Order (from back to front) */
    i_attach_to_panel(gtwin->objects, gtwin->panel, scroll_panel, &offset, ekOBJ_MENU, gtwin->toolbar);
    i_attach_to_panel(gtwin->objects, gtwin->panel, scroll_panel, &offset, ekOBJ_TABLEVIEW, gtwin->toolbar);
    i_attach_to_panel(gtwin->objects, gtwin->panel, scroll_panel, &offset, ekOBJ_TEXTVIEW, gtwin->toolbar);
    i_attach_to_panel(gtwin->objects, gtwin->panel, scroll_panel, &offset, ekOBJ_LABEL, gtwin->toolbar);
    i_attach_to_panel(gtwin->objects, gtwin->panel, scroll_panel, &offset, ekOBJ_BUTTON, gtwin->toolbar);
    i_attach_to_panel(gtwin->objects, gtwin->panel, scroll_panel, &offset, ekOBJ_EDIT, gtwin->toolbar);
    i_attach_to_panel(gtwin->objects, gtwin->panel, scroll_panel, &offset, ekOBJ_IMAGE, gtwin->toolbar);
    i_attach_toolbar_to_panel(gtwin->toolbar, gtwin->panel);

    /* We are in a main (not embedded) window */
    if (gtwin->window != NULL)
    {
        cassert(gtwin == main_gtwin);
        cassert(gtwin->parent_id == UINT32_MAX);
        /* Add the window main panel */
        window_panel(gtwin->window, gtwin->panel);
        /* Clear the tabstop list */
        arrpt_clear(gtwin->tabstops, NULL, GuiComponent);
    }

    i_component_tabstop(gtwin->objects, main_gtwin->window, main_gtwin->tabstops, ekOBJ_MENU);
    i_component_tabstop(gtwin->objects, main_gtwin->window, main_gtwin->tabstops, ekOBJ_TABLEVIEW);
    i_component_tabstop(gtwin->objects, main_gtwin->window, main_gtwin->tabstops, ekOBJ_TEXTVIEW);
    i_component_tabstop(gtwin->objects, main_gtwin->window, main_gtwin->tabstops, ekOBJ_EDIT);
    i_component_tabstop(gtwin->objects, main_gtwin->window, main_gtwin->tabstops, ekOBJ_BUTTON);
    i_component_tabstop(gtwin->objects, main_gtwin->window, main_gtwin->tabstops, ekOBJ_LABEL);
    i_component_tabstop(gtwin->objects, main_gtwin->window, main_gtwin->tabstops, ekOBJ_IMAGE);

    if (scroll_panel != NULL)
        _component_visible((GuiComponent *)scroll_panel, TRUE);

    if (gtwin->window != NULL)
    {
        i_toolbar_tabstop(gtwin->toolbar, main_gtwin->tabstops);
    }
    else
    {
        /* Toolbar is not allowed in embedded windows */
        cassert(gtwin->toolbar == NULL);
    }

    /* We are in a main window */
    if (gtwin->parent_id == UINT32_MAX)
    {
        cassert(gtwin == main_gtwin);

        /* Configure the child (embedded) windows as subpanels */
        arrpt_forback(embgtwin, gtnap->windows, GtNapWindow)
            if (embgtwin->parent_id == gtwin->id)
            {
                V2Df pos;
                int32_t cell_x = embgtwin->left - gtwin->left;
                int32_t cell_y = embgtwin->top - gtwin->top;
                pos.x = (real32_t)cell_x * GTNAP_GLOBAL->cell_x_sizef;
                pos.y = (real32_t)cell_y * GTNAP_GLOBAL->cell_y_sizef;
                i_gtwin_configure(gtnap, embgtwin, gtwin);
                _panel_attach_component(gtwin->panel, (GuiComponent *)embgtwin->panel);
                _component_set_frame((GuiComponent *)embgtwin->panel, &pos, &embgtwin->panel_size);
                _component_visible((GuiComponent *)embgtwin->panel, TRUE);
            }
        arrpt_end();

        /* At this point the main window (with embedded windows) is complete.
         * We begin the tabstop configuration */
        _window_taborder(gtwin->window, NULL);
        arrpt_foreach(component, gtwin->tabstops, GuiComponent)
            _component_taborder(component, gtwin->window);
        arrpt_end()
    }

    /* Allow navigation between edit controls with arrows and return */
    if (i_num_edits(gtwin) > 0)
    {
        /* At the moment, embedded windows with edits is not allowed */
        GtNapObject *last_edit = NULL;

        arrpt_foreach(obj, gtwin->objects, GtNapObject)
            if (obj->type == ekOBJ_EDIT)
            {
                edit_OnChange((Edit *)obj->component, listener(obj, i_OnEditChange, GtNapObject));
                edit_OnFilter((Edit *)obj->component, listener(obj, i_OnEditFilter, GtNapObject));
                edit_OnFocus((Edit *)obj->component, listener(obj, i_OnEditFocus, GtNapObject));
                obj->is_last_edit = FALSE;
                last_edit = obj;
            }
        arrpt_end();

        cassert_no_null(last_edit);
        last_edit->is_last_edit = TRUE;
    }

    /* Allow TextView listeners */
    arrpt_foreach(obj, gtwin->objects, GtNapObject)
        if (obj->type == ekOBJ_TEXTVIEW)
        {
            textview_OnFocus((TextView *)obj->component, listener(obj, i_OnTextFocus, GtNapObject));
            if (obj->keyfilter_block != NULL)
                textview_OnFilter((TextView *)obj->component, listener(obj, i_OnTextFilter, GtNapObject));
        }
    arrpt_end();

    if (gtwin->buttons_navigation == TRUE)
    {
        if (i_num_buttons(gtwin) > 1)
        {
            /* At the moment, embedded windows with button navigation is not allowed */
            cassert(gtwin->window != NULL);
            window_hotkey(gtwin->window, ekKEY_LEFT, 0, listener(gtwin, i_OnLeftButton, GtNapWindow));
            window_hotkey(gtwin->window, ekKEY_RIGHT, 0, listener(gtwin, i_OnRightButton, GtNapWindow));
        }
    }

    gtwin->is_configured = TRUE;
}

/*---------------------------------------------------------------------------*/

static void i_gtnap_update(GtNap *gtnap, const real64_t prtime, const real64_t ctime)
{
    cassert(gtnap == NULL || gtnap == GTNAP_GLOBAL);
    gtnap = GTNAP_GLOBAL;
    cassert_no_null(gtnap);
    unref(prtime);
    unref(ctime);
    if (gtnap->modal_time_window != NULL)
    {
        GtNapWindow *gtwin = gtnap->modal_time_window;
        if (arrpt_find(gtnap->windows, gtwin, GtNapWindow) != UINT32_MAX)
        {
            if (gtnap->modal_delay_seconds > 0)
            {
                uint64_t now = btime_now();
                if ((now - gtnap->modal_timestamp) / 1000000 >= gtnap->modal_delay_seconds)
                {
                    gtnap->modal_timestamp = 0;
                    gtnap->modal_delay_seconds = 0;
                    gtnap->modal_time_window = NULL;
                    i_stop_modal(gtnap, gtwin, NAP_MODAL_TIMESTAMP);
                }
            }
        }
        else
        {
            gtnap->modal_timestamp = 0;
            gtnap->modal_delay_seconds = 0;
            gtnap->modal_time_window = NULL;
        }
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_init(const char_t *title, const uint32_t rows, const uint32_t cols, PHB_ITEM begin_block)
{
    void *hInstance = NULL;

#if defined(HB_OS_WIN)
    hb_winmainArgGet(&hInstance, NULL, NULL);
#endif

    nforms_start();
    str_copy_c(INIT_TITLE, sizeof32(INIT_TITLE), title);
    INIT_CODEBLOCK = hb_itemNew(begin_block);
    INIT_ROWS = rows;
    INIT_COLS = cols;

    osmain_imp(
        0, NULL, hInstance, 0.5f,
        (FPtr_app_create)i_gtnap_create,
        (FPtr_app_update)i_gtnap_update,
        (FPtr_destroy)i_gtnap_destroy,
        (char_t *)"");
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_log(const char_t *text)
{
    String *str = i_cp_to_utf8_string(text);
    log_printf("%s", tc(str));
    str_destroy(&str);
}

/*---------------------------------------------------------------------------*/

static void i_OnTerminalClose(GtNapWindow *gtwin, Event *e)
{
    unref(gtwin);
    unref(e);
    osapp_finish();
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_width(void)
{
    cassert_no_null(GTNAP_GLOBAL);
    return (uint32_t)(GTNAP_GLOBAL->cell_x_sizef * GTNAP_GLOBAL->cols);
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_height(void)
{
    cassert_no_null(GTNAP_GLOBAL);
    return (uint32_t)(GTNAP_GLOBAL->cell_y_sizef * GTNAP_GLOBAL->rows);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_terminal(void)
{
    GtNap *gtnap = GTNAP_GLOBAL;
    GtNapWindow *gtwin = NULL;
    cassert(arrpt_size(gtnap->windows, GtNapWindow) == 0);
    hb_gtnap_window(0, 0, gtnap->rows - 1, gtnap->cols - 1, "wid", tc(gtnap->title), FALSE, TRUE, TRUE, FALSE);
    gtwin = i_current_gtwin(gtnap);
    i_gtwin_configure(gtnap, gtwin, gtwin);
    window_OnClose(gtwin->window, listener(gtwin, i_OnTerminalClose, GtNapWindow));
    window_show(gtwin->window);
}

/*---------------------------------------------------------------------------*/

int32_t hb_gtnap_inkey(const vkey_t vkey)
{
    uint32_t i, n = sizeof(KEYMAPS) / sizeof(GtNapKey);
    for (i = 0; i < n; ++i)
    {
        if (KEYMAPS[i].vkey == vkey)
            return KEYMAPS[i].key;
    }

    return INT32_MAX;
}

/*---------------------------------------------------------------------------*/

static ___INLINE uint32_t i_window_flags(const bool_t close_return, const bool_t close_esc, const bool_t minimize_button)
{
    uint32_t flags = ekWINDOW_TITLE | ekWINDOW_CLOSE | ekWINDOW_MODAL_NOHIDE;

    if (close_return == TRUE)
        flags |= ekWINDOW_RETURN;

    if (close_esc == TRUE)
        flags |= ekWINDOW_ESC;

    if (minimize_button == TRUE)
        flags |= ekWINDOW_MIN;

    return flags;
}

/*---------------------------------------------------------------------------*/

static void i_OnWindowClose(GtNapWindow *gtwin, Event *e)
{
    const EvWinClose *p = event_params(e, EvWinClose);
    bool_t *res = event_result(e, bool_t);
    cassert(*res == TRUE);
    if (gtwin->desist_block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(gtwin->desist_block, 0);
        cassert(HB_ITEM_TYPE(ritem) == HB_IT_LOGICAL);
        *res = (bool_t)hb_itemGetL(ritem);
        hb_itemRelease(ritem);
    }

    if (*res == TRUE && p->origin == ekGUI_CLOSE_ESC)
        gtwin->is_closed_by_esc = TRUE;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_get_window_id(GtNap *gtnap)
{
    uint32_t id = NAP_WINDOW_FIST_ID;
    bool_t found = FALSE;
    while (!found)
    {
        bool_t valid_id = TRUE;
        arrpt_foreach_const(gtwin, gtnap->windows, GtNapWindow)
            if (gtwin->id == id)
            {
                id += 1;
                valid_id = FALSE;
                break;
            }
        arrpt_end();

        if (valid_id == TRUE)
            found = TRUE;
    }

    return id;
}

/*---------------------------------------------------------------------------*/

static GtNapWindow *i_new_window(GtNap *gtnap, uint32_t parent_id, const int32_t top, const int32_t left, const int32_t bottom, const int32_t right, const char_t *nameid, const bool_t border)
{
    GtNapWindow *gtwin = NULL;
    cassert_no_null(gtnap);
    gtwin = heap_new0(GtNapWindow);
    gtwin->id = i_get_window_id(gtnap);
    gtwin->parent_id = parent_id;
    gtwin->top = top;
    gtwin->left = left;
    gtwin->bottom = bottom;
    gtwin->right = right;
    gtwin->border = border;
    gtwin->scroll_top = INT32_MIN;
    gtwin->scroll_left = INT32_MIN;
    gtwin->scroll_bottom = INT32_MIN;
    gtwin->scroll_right = INT32_MIN;
    gtwin->nameid = str_c(nameid);
    gtwin->message_label_id = UINT32_MAX;
    gtwin->default_button = UINT32_MAX;
    gtwin->tabstops = arrpt_create(GuiComponent);
    gtwin->objects = arrpt_create(GtNapObject);
    gtwin->callbacks = arrpt_create(GtNapCallback);
    gtwin->panel_size.width = gtnap->cell_x_sizef * (real32_t)(gtwin->right - gtwin->left + 1);
    gtwin->panel_size.height = gtnap->cell_y_sizef * (real32_t)(gtwin->bottom - gtwin->top + 1);
    arrpt_append(gtnap->windows, gtwin, GtNapWindow);
    return gtwin;
}

/*---------------------------------------------------------------------------*/

const char_t *hb_gtnap_working_path(void)
{
    return tc(GTNAP_GLOBAL->working_path);
}

/*---------------------------------------------------------------------------*/

#define COL_GREEN 2
#define COL_CYAN 3
#define COL_RED 4
#define COL_MAGENTA 5
#define COL_BROWN 6
#define COL_WHITE 7
#define COL_LIGHT_GRAY 8
#define COL_BRIGHT_BLUE 9
#define COL_BRIGHT_GREEN 10
#define COL_BRIGHT_CYAN 11
#define COL_BRIGHT_RED 12
#define COL_BRIGHT_MAGENTA 13
#define COL_YELLOW 14
#define COL_BRIGHT_WHITE 15

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_black(void)
{
    return i_COLORS[COL_BLACK];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_blue(void)
{
    return i_COLORS[COL_BLUE];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_green(void)
{
    return i_COLORS[COL_GREEN];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_cyan(void)
{
    return i_COLORS[COL_CYAN];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_red(void)
{
    return i_COLORS[COL_RED];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_magenta(void)
{
    return i_COLORS[COL_MAGENTA];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_brown(void)
{
    return i_COLORS[COL_BROWN];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_white(void)
{
    return i_COLORS[COL_WHITE];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_light_gray(void)
{
    return i_COLORS[COL_LIGHT_GRAY];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_bright_blue(void)
{
    return i_COLORS[COL_BRIGHT_BLUE];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_bright_green(void)
{
    return i_COLORS[COL_BRIGHT_GREEN];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_bright_cyan(void)
{
    return i_COLORS[COL_BRIGHT_CYAN];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_bright_red(void)
{
    return i_COLORS[COL_BRIGHT_RED];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_bright_magenta(void)
{
    return i_COLORS[COL_BRIGHT_MAGENTA];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_yellow(void)
{
    return i_COLORS[COL_YELLOW];
}

/*---------------------------------------------------------------------------*/

color_t hb_gtnap_color_bright_white(void)
{
    return i_COLORS[COL_BRIGHT_WHITE];
}

/*---------------------------------------------------------------------------*/

static void i_OnWindowMoved(GtNapWindow *gtwin, Event *e)
{
    const EvPos *p = event_params(e, EvPos);
    cassert_no_null(gtwin);
    i_write_prop_r32(tc(gtwin->nameid), i_XPOS_PROP, p->x);
    i_write_prop_r32(tc(gtwin->nameid), i_YPOS_PROP, p->y);
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_window(const int32_t top, const int32_t left, const int32_t bottom, const int32_t right, const char_t *nameid, const char_t *title, const bool_t close_return, const bool_t close_esc, const bool_t minimize_button, const bool_t buttons_navigation)
{
    GtNapWindow *gtwin = i_new_window(GTNAP_GLOBAL, UINT32_MAX, top, left, bottom, right, nameid, FALSE);
    uint32_t flags = i_window_flags(close_return, close_esc, minimize_button);
    gtwin->window = window_create(flags);
    gtwin->buttons_navigation = buttons_navigation;

    if (str_empty_c(title) == FALSE)
    {
        char_t utf8[STATIC_TEXT_SIZE];
        i_cp_to_utf8(title, utf8, sizeof(utf8));
        window_title(gtwin->window, utf8);
    }
    else
    {
        window_title(gtwin->window, tc(GTNAP_GLOBAL->title));
    }

    window_cycle_tabstop(gtwin->window, FALSE);
    window_OnClose(gtwin->window, listener(gtwin, i_OnWindowClose, GtNapWindow));
    window_OnMoved(gtwin->window, listener(gtwin, i_OnWindowMoved, GtNapWindow));
    return gtwin->id;
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_window_embedded(const uint32_t wid, const int32_t top, const int32_t left, const int32_t bottom, const int32_t right, const char_t *nameid, const bool_t border)
{
    GtNapWindow *gtwin = i_new_window(GTNAP_GLOBAL, wid, top, left, bottom, right, nameid, border);
    return gtwin->id;
}

/*---------------------------------------------------------------------------*/

static void i_dettach_embedded(GtNap *gtnap, GtNapWindow *gtwin)
{
    cassert_no_null(gtnap);
    cassert_no_null(gtwin);
    if (gtwin->is_configured == TRUE)
    {
        /* We are in a main window --> Dettach all possible embedded windows */
        if (gtwin->parent_id == UINT32_MAX)
        {
            arrpt_foreach(embgtwin, GTNAP_GLOBAL->windows, GtNapWindow)
                if (embgtwin->parent_id == gtwin->id)
                {
                    cassert(embgtwin->is_configured == TRUE);
                    _component_visible((GuiComponent *)embgtwin->panel, FALSE);
                    _component_detach_from_panel((GuiComponent *)gtwin->panel, (GuiComponent *)embgtwin->panel);
                }
            arrpt_end()
        }
        /* We are in an embedded window --> Dettach from ONLY one parent */
        else
        {
            arrpt_foreach(maingtwin, GTNAP_GLOBAL->windows, GtNapWindow)
                if (gtwin->parent_id == maingtwin->id)
                {
                    cassert(maingtwin->is_configured == TRUE);
                    _panel_dettach_component(maingtwin->panel, (GuiComponent *)gtwin->panel);
                    break;
                }
            arrpt_end()
        }
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_destroy(const uint32_t wid)
{
    uint32_t id = i_gtwin_index(GTNAP_GLOBAL, wid);
    GtNapWindow *gtwin = arrpt_get(GTNAP_GLOBAL->windows, id, GtNapWindow);
    cassert_no_null(gtwin);
    /* Before destroy we have to dettach the possible parent-embedded connections */
    i_dettach_embedded(GTNAP_GLOBAL, gtwin);
    arrpt_delete(GTNAP_GLOBAL->windows, id, i_destroy_gtwin, GtNapWindow);
}

/*---------------------------------------------------------------------------*/

static const GtNapKey *i_convert_key(const int32_t key)
{
    uint32_t i, n = sizeof(KEYMAPS) / sizeof(GtNapKey);
    for (i = 0; i < n; ++i)
    {
        if (KEYMAPS[i].key == key)
            return &KEYMAPS[i];
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

static void i_OnWindowHotKey(GtNapCallback *callback, Event *e)
{
    cassert_no_null(callback);
    cassert_no_null(callback->gtwin);
    unref(e);

    if (callback->block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(callback->block, 0);
        hb_itemRelease(ritem);
    }

    if (callback->autoclose_id != UINT32_MAX)
        i_stop_modal(GTNAP_GLOBAL, callback->gtwin, NAP_MODAL_HOTKEY_AUTOCLOSE + callback->autoclose_id);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_hotkey(const uint32_t wid, const int32_t key, HB_ITEM *block, const bool_t autoclose)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    const GtNapKey *nkey = i_convert_key(key);
    cassert_no_null(gtwin);

    /* Exists a Harbour/NAppGUI key convertion */
    if (nkey != NULL)
    {
        {
            uint32_t pos = UINT32_MAX;

            /* Delete a previous callback on this hotkey */
            arrpt_foreach(callback, gtwin->callbacks, GtNapCallback)
                if (callback->key == key)
                {
                    pos = callback_i;
                    break;
                }
            arrpt_end();

            if (pos != UINT32_MAX)
                arrpt_delete(gtwin->callbacks, pos, i_destroy_callback, GtNapCallback);
        }

        {
            uint32_t autoclose_id = autoclose ? (uint32_t)nkey->vkey : UINT32_MAX;
            Listener *listener = i_gtnap_listener(block, key, autoclose_id, gtwin, i_OnWindowHotKey);
            window_hotkey(gtwin->window, nkey->vkey, nkey->modifiers, listener);
        }
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_editable(const uint32_t wid, HB_ITEM *is_editable_block)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    cassert_no_null(gtwin);
    cassert(gtwin->is_editable_block == NULL);
    gtwin->is_editable_block = hb_itemNew(is_editable_block);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_confirm(const uint32_t wid, HB_ITEM *confirm_block)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    cassert_no_null(gtwin);
    cassert(gtwin->confirm_block == NULL);
    gtwin->confirm_block = hb_itemNew(confirm_block);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_desist(const uint32_t wid, HB_ITEM *desist_block)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    cassert_no_null(gtwin);
    cassert(gtwin->desist_block == NULL);
    gtwin->desist_block = hb_itemNew(desist_block);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_errdate(const uint32_t wid, HB_ITEM *error_date_block)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    cassert_no_null(gtwin);
    cassert(gtwin->error_date_block == NULL);
    gtwin->error_date_block = hb_itemNew(error_date_block);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_scroll(const uint32_t wid, const int32_t top, const int32_t left, const int32_t bottom, const int32_t right)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    cassert(gtwin->scroll_top == INT32_MIN);
    cassert(gtwin->is_configured == FALSE);
    gtwin->scroll_top = top;
    gtwin->scroll_left = left;
    gtwin->scroll_bottom = bottom;
    gtwin->scroll_right = right;
}

/*---------------------------------------------------------------------------*/

static GtNapObject *i_focus_obj(GtNapWindow *gtwin)
{
    GtNapObject *focus = NULL;
    arrpt_foreach(gtobj, gtwin->objects, GtNapObject)
        if (gtobj->has_focus == TRUE)
        {
            cassert(focus == NULL);
            focus = gtobj;
        }
    arrpt_end();
    return focus;
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_copy(const uint32_t wid)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    GtNapObject *gtobj = i_focus_obj(gtwin);
    if (gtobj != NULL)
    {
        if (gtobj->type == ekOBJ_EDIT)
            edit_copy((Edit *)gtobj->component);
        else if (gtobj->type == ekOBJ_TEXTVIEW)
            textview_copy((TextView *)gtobj->component);
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_paste(const uint32_t wid)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    GtNapObject *gtobj = i_focus_obj(gtwin);
    if (gtobj != NULL)
    {
        if (gtobj->type == ekOBJ_EDIT)
            edit_paste((Edit *)gtobj->component);
        else if (gtobj->type == ekOBJ_TEXTVIEW)
            textview_paste((TextView *)gtobj->component);
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_cut(const uint32_t wid)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    GtNapObject *gtobj = i_focus_obj(gtwin);
    if (gtobj != NULL)
    {
        if (gtobj->type == ekOBJ_EDIT)
            edit_cut((Edit *)gtobj->component);
        else if (gtobj->type == ekOBJ_TEXTVIEW)
            textview_cut((TextView *)gtobj->component);
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_undo(const uint32_t wid)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    GtNapObject *gtobj = i_focus_obj(gtwin);
    if (gtobj != NULL)
    {
        if (gtobj->type == ekOBJ_EDIT)
            i_set_edit_text(gtobj);
        else if (gtobj->type == ekOBJ_TEXTVIEW)
            i_set_view_text(gtobj);
    }
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_window_modal(const uint32_t wid, const uint32_t pwid, const uint32_t delay_seconds)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    GtNapWindow *pgtwin = pwid > 0 ? i_gtwin(GTNAP_GLOBAL, pwid) : NULL;
    GtNapWindow *embgtwin = NULL;
    cassert_no_null(gtwin);

    /* An embedded window can't be launched as modal. We must launch the parent */
    if (gtwin->parent_id != UINT32_MAX)
    {
        embgtwin = gtwin;
        gtwin = i_gtwin(GTNAP_GLOBAL, gtwin->parent_id);
        cassert_no_null(gtwin);
    }

    {
        V2Df pos;
        uint32_t ret = 0;

        if (gtwin->is_configured == FALSE)
            i_gtwin_configure(GTNAP_GLOBAL, gtwin, gtwin);

        if (gtwin->buttons_navigation == TRUE)
        {
            uint32_t n = i_num_buttons(gtwin);
            if (n > 0)
            {
                if (gtwin->default_button == UINT32_MAX)
                    gtwin->default_button = 0;
                cassert(gtwin->default_button < n);
                i_set_defbutton(gtwin);
            }
        }

        pos.x = (real32_t)gtwin->left * GTNAP_GLOBAL->cell_x_sizef;
        pos.y = (real32_t)gtwin->top * GTNAP_GLOBAL->cell_y_sizef;

        if (arrpt_size(GTNAP_GLOBAL->windows, GtNapWindow) > 0)
        {
            GtNapWindow *base = arrpt_first(GTNAP_GLOBAL->windows, GtNapWindow);
            V2Df ppos;
            cassert_no_null(base);
            ppos = window_get_origin(base->window);
            pos.x += ppos.x;
            pos.y += ppos.y;
        }

        cassert(gtwin->window != NULL);
        /* Allow arrows/intro in TextView */
        if (embgtwin != NULL && i_num_texts(embgtwin) > 0)
        {
            window_hotkey(gtwin->window, ekKEY_UP, 0, NULL);
            window_hotkey(gtwin->window, ekKEY_DOWN, 0, NULL);
            window_hotkey(gtwin->window, ekKEY_RETURN, 0, NULL);
        }
        /* Allow arrows/intro navigation between editboxes */
        else if (i_num_edits(gtwin) > 0)
        {
            window_hotkey(gtwin->window, ekKEY_UP, 0, listener(gtwin, i_OnPreviousEdit, GtNapWindow));
            window_hotkey(gtwin->window, ekKEY_DOWN, 0, listener(gtwin, i_OnNextTabstop, GtNapWindow));
            window_hotkey(gtwin->window, ekKEY_RETURN, 0, listener(gtwin, i_OnNextTabstop, GtNapWindow));
            window_hotkey(gtwin->window, ekKEY_NUMRET, 0, listener(gtwin, i_OnNextTabstop, GtNapWindow));
        }

        /*
         * At this point, we have a precomputed window position, based on window (left, top)
         * and parent position. Its can be overwritten by a previous stored user position.
         */
        {
            real32_t x = i_read_prop_r32(tc(gtwin->nameid), i_XPOS_PROP);
            real32_t y = i_read_prop_r32(tc(gtwin->nameid), i_YPOS_PROP);
            if (x != i_UNDEF_R32 && y != i_UNDEF_R32)
            {
                pos.x = x;
                pos.y = y;
            }
        }

        window_origin(gtwin->window, pos);
        gtwin->is_closed_by_esc = FALSE;
        gtwin->modal_window_alive = TRUE;

        if (delay_seconds > 0)
        {
            GTNAP_GLOBAL->modal_timestamp = btime_now();
            GTNAP_GLOBAL->modal_delay_seconds = delay_seconds;
            GTNAP_GLOBAL->modal_time_window = gtwin;
        }
        else
        {
            GTNAP_GLOBAL->modal_timestamp = 0;
            GTNAP_GLOBAL->modal_delay_seconds = 0;
            GTNAP_GLOBAL->modal_time_window = NULL;
        }

        ret = window_modal(gtwin->window, pgtwin ? pgtwin->window : NULL);
        return ret;
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_window_stop_modal(const uint32_t wid, const uint32_t result)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    i_stop_modal(GTNAP_GLOBAL, gtwin, result);
}

/*---------------------------------------------------------------------------*/

static uint32_t i_add_object(const objtype_t type, const int32_t top, const int32_t left, const real32_t cell_x_sizef, const real32_t cell_y_sizef, const S2Df *size, const bool_t in_scroll, GuiComponent *component, GtNapWindow *gtwin)
{
    uint32_t id;
    GtNapObject *obj = NULL;
    cassert_no_null(gtwin);
    cassert_no_null(size);
    id = arrpt_size(gtwin->objects, GtNapObject);
    obj = heap_new0(GtNapObject);
    arrpt_append(gtwin->objects, obj, GtNapObject);
    obj->type = type;
    obj->top = top;
    obj->left = left;
    obj->component = component;
    obj->pos.x = (real32_t)left * cell_x_sizef;
    obj->pos.y = (real32_t)top * cell_y_sizef;
    obj->size = *size;
    obj->is_last_edit = FALSE;
    obj->in_scroll = in_scroll;
    obj->can_auto_lista = TRUE;
    obj->has_focus = FALSE;
    obj->editBoxIndexForButton = UINT32_MAX;
    obj->gtwin = gtwin;
    return id;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_add_label(const int32_t top, const int32_t left, const bool_t in_scroll, GtNapWindow *gtwin, GtNap *gtnap)
{
    Label *label = label_create();
    S2Df size;
    cassert_no_null(gtnap);
    label_font(label, gtnap->global_font);
    size.width = gtnap->cell_x_sizef;
    size.height = gtnap->label_y_sizef;
    return i_add_object(ekOBJ_LABEL, top, left, gtnap->cell_x_sizef, gtnap->cell_y_sizef, &size, in_scroll, (GuiComponent *)label, gtwin);
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_label(const uint32_t wid, const int32_t top, const int32_t left, HB_ITEM *text_block, const bool_t in_scroll)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    uint32_t id;
    GtNapObject *obj;
    cassert(gtwin->is_configured == FALSE);
    id = i_add_label(top - gtwin->top, left - gtwin->left, in_scroll, gtwin, GTNAP_GLOBAL);
    obj = arrpt_last(gtwin->objects, GtNapObject);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_LABEL);

    if (text_block != NULL)
    {
        if (HB_ITEM_TYPE(text_block) == HB_IT_STRING)
        {
            obj->text = i_item_to_utf8_string(text_block);
        }
        else
        {
            cassert(HB_ITEM_TYPE(text_block) == HB_IT_BLOCK);
            obj->text_block = hb_itemNew(text_block);
        }
    }

    i_set_label_text(obj, NULL);
    return id;
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_label_message(const uint32_t wid, const int32_t top, const int32_t left, const bool_t in_scroll)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    cassert(gtwin->is_configured == FALSE);
    cassert(gtwin->message_label_id == UINT32_MAX);
    gtwin->message_label_id = i_add_label(top - gtwin->top, left - gtwin->left, in_scroll, gtwin, GTNAP_GLOBAL);
    return gtwin->message_label_id;
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_label_update(const uint32_t wid, const uint32_t id, const int32_t top, const int32_t left, HB_ITEM *text_block)
{
    GtNapObject *gtobj = i_gtobj(GTNAP_GLOBAL, wid, id);
    GtNapWindow *gtwin = NULL;
    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_LABEL);
    gtwin = gtobj->gtwin;
    cassert_no_null(gtwin);

    str_destopt(&gtobj->text);

    if (gtobj->text_block != NULL)
    {
        hb_itemRelease(gtobj->text_block);
        gtobj->text_block = NULL;
    }

    if (text_block != NULL)
    {
        if (HB_ITEM_TYPE(text_block) == HB_IT_STRING)
        {
            gtobj->text = i_item_to_utf8_string(text_block);
        }
        else
        {
            cassert(HB_ITEM_TYPE(text_block) == HB_IT_BLOCK);
            gtobj->text_block = hb_itemNew(text_block);
        }
    }

    gtobj->pos.x = (real32_t)(left - gtwin->left) * GTNAP_GLOBAL->cell_x_sizef;
    gtobj->pos.y = (real32_t)(top - gtwin->top) * GTNAP_GLOBAL->cell_y_sizef;

    i_set_label_text(gtobj, NULL);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_label_fgcolor(const uint32_t wid, const uint32_t id, const color_t color)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_LABEL);
    label_color((Label *)obj->component, color);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_label_bgcolor(const uint32_t wid, const uint32_t id, const color_t color)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_LABEL);
    label_bgcolor((Label *)obj->component, color);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_label_color(const uint32_t wid, const uint32_t id, const char_t *hb_color)
{
    int attr = hb_gtColorToN(hb_color);
    int fore = attr & 0x000F;
    int back = (attr & 0x00F0) >> 4;
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_LABEL);
    cassert(fore < 16);
    cassert(back < 16);
    label_color((Label *)obj->component, (fore != COL_BLACK) ? i_COLORS[fore] : kCOLOR_DEFAULT);
    label_bgcolor((Label *)obj->component, (back != COL_WHITE) ? i_COLORS[back] : kCOLOR_DEFAULT);
}

/*---------------------------------------------------------------------------*/

static void i_OnButtonClick(GtNapCallback *callback, Event *e)
{
    cassert_no_null(callback);
    cassert_no_null(callback->gtwin);
    unref(e);
    if (callback->block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(callback->block, 0);
        hb_itemRelease(ritem);
    }

    if (callback->autoclose_id != UINT32_MAX)
        i_stop_modal(GTNAP_GLOBAL, callback->gtwin, NAP_MODAL_BUTTON_AUTOCLOSE + callback->autoclose_id);
}

/*---------------------------------------------------------------------------*/

static void i_set_button_text(GtNapObject *obj)
{
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_BUTTON);
    if (obj->text_block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(obj->text_block, 0);
        char_t utf8[STATIC_TEXT_SIZE];
        cassert(HB_ITEM_TYPE(ritem) == HB_IT_STRING);
        i_item_to_utf8(ritem, utf8, sizeof32(utf8));
        button_text((Button *)obj->component, utf8);
        hb_itemRelease(ritem);
    }
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_button(const uint32_t wid, const int32_t top, const int32_t left, const int32_t bottom, const int32_t right, HB_ITEM *text_block, HB_ITEM *click_block, const bool_t autoclose, const bool_t in_scroll)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    uint32_t autoclose_id = autoclose ? i_num_buttons(gtwin) + 1 : UINT32_MAX;
    Button *button = NULL;
    Listener *listener = NULL;
    S2Df size;
    uint32_t id = UINT32_MAX;
    cassert_no_null(gtwin);
    button = button_push();
    listener = i_gtnap_listener(click_block, INT32_MAX, autoclose_id, gtwin, i_OnButtonClick);
    button_OnClick(button, listener);
    button_font(button, GTNAP_GLOBAL->button_font);
    button_vpadding(button, i_button_vpadding());
    cassert(bottom == top);
    size.width = (real32_t)(right - left + 1) * GTNAP_GLOBAL->cell_x_sizef;
    size.height = (real32_t)(bottom - top + 1) * GTNAP_GLOBAL->button_y_sizef;
    id = i_add_object(ekOBJ_BUTTON, top - gtwin->top, left - gtwin->left, GTNAP_GLOBAL->cell_x_sizef, GTNAP_GLOBAL->cell_y_sizef, &size, in_scroll, (GuiComponent *)button, gtwin);

    if (text_block != NULL)
    {
        GtNapObject *obj = arrpt_last(gtwin->objects, GtNapObject);
        cassert_no_null(obj);
        cassert(obj->type == ekOBJ_BUTTON);
        obj->text_block = hb_itemNew(text_block);
        i_set_button_text(obj);
    }

    return id;
}

/*---------------------------------------------------------------------------*/

static void i_OnImageClick(GtNapCallback *callback, Event *e)
{
    cassert_no_null(callback);
    cassert_no_null(callback->gtwin);
    unref(e);
    if (callback->block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(callback->block, 0);
        hb_itemRelease(ritem);
    }

    if (callback->autoclose_id != UINT32_MAX)
        i_stop_modal(GTNAP_GLOBAL, callback->gtwin, NAP_MODAL_IMAGE_AUTOCLOSE + callback->autoclose_id);
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_image(const uint32_t wid, const int32_t top, const int32_t left, const int32_t bottom, const int32_t right, const char_t *pathname, HB_ITEM *click_block, const bool_t autoclose, const bool_t in_scroll)
{
    Image *image = NULL;
    char_t utf8[STATIC_TEXT_SIZE];
    i_cp_to_utf8(pathname, utf8, sizeof32(utf8));
    image = image_from_file(utf8, NULL);

    if (image != NULL)
    {
        GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
        uint32_t autoclose_id = autoclose ? i_num_images(gtwin) + 1 : UINT32_MAX;
        ImageView *view;
        Listener *listener;
        S2Df size;
        cassert_no_null(gtwin);
        view = imageview_create();
        listener = i_gtnap_listener(click_block, INT32_MAX, autoclose_id, gtwin, i_OnImageClick);
        imageview_OnClick(view, listener);
        imageview_scale(view, ekGUI_SCALE_AUTO);
        size.width = (real32_t)(right - left + 1) * GTNAP_GLOBAL->cell_x_sizef;
        size.height = (real32_t)(bottom - top + 1) * GTNAP_GLOBAL->cell_y_sizef;
        imageview_image(view, image);
        image_destroy(&image);
        return i_add_object(ekOBJ_IMAGE, top - gtwin->top, left - gtwin->left, GTNAP_GLOBAL->cell_x_sizef, GTNAP_GLOBAL->cell_y_sizef, &size, in_scroll, (GuiComponent *)view, gtwin);
    }

    return UINT32_MAX;
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_edit(const uint32_t wid, const int32_t top, const int32_t left, const uint32_t width, const char_t type, HB_ITEM *get_set_block, HB_ITEM *is_editable_block, HB_ITEM *when_block, HB_ITEM *valida_block, HB_ITEM *message_block, HB_ITEM *keyfilter_block, const bool_t in_scroll)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    Edit *edit = edit_create();
    S2Df size;
    uint32_t id = UINT32_MAX;
    GtNapObject *obj = NULL;
    cassert_no_null(gtwin);
    edit_font(edit, GTNAP_GLOBAL->edit_font);
    edit_vpadding(edit, i_edit_vpadding());
    size.width = (real32_t)(width + 1) * GTNAP_GLOBAL->cell_x_sizef;
    size.height = GTNAP_GLOBAL->edit_y_sizef;
    id = i_add_object(ekOBJ_EDIT, top - gtwin->top, left - gtwin->left, GTNAP_GLOBAL->cell_x_sizef, GTNAP_GLOBAL->cell_y_sizef, &size, in_scroll, (GuiComponent *)edit, gtwin);
    obj = arrpt_last(gtwin->objects, GtNapObject);
    cassert_no_null(obj);
    cassert(obj->type = ekOBJ_EDIT);

    obj->max_chars = width;

    if (type == 'C')
    {
        obj->dtype = ekTYPE_CHARACTER;
    }
    else if (type == 'D')
    {
        obj->dtype = ekTYPE_DATE;
    }
    else
    {
        obj->dtype = ENUM_MAX(datatype_t);
        cassert(FALSE);
    }

    if (get_set_block != NULL)
        obj->get_set_block = hb_itemNew(get_set_block);

    if (is_editable_block != NULL)
        obj->is_editable_block = hb_itemNew(is_editable_block);

    if (when_block != NULL)
        obj->when_block = hb_itemNew(when_block);

    if (valida_block != NULL)
        obj->valida_block = hb_itemNew(valida_block);

    if (message_block != NULL)
        obj->message_block = hb_itemNew(message_block);

    if (keyfilter_block != NULL)
        obj->keyfilter_block = hb_itemNew(keyfilter_block);

    i_set_edit_text(obj);

    return id;
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_edit_color(const uint32_t wid, const uint32_t id, const char_t *hb_color)
{
    ArrPt(String) *hbcols = str_splits(hb_color, ",", TRUE, FALSE);
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_EDIT);

    if (arrpt_size(hbcols, String) > 0)
    {
        const String *c = arrpt_get_const(hbcols, 0, String);
        int attr = hb_gtColorToN(tc(c));
        int fore = attr & 0x000F;
        int back = (attr & 0x00F0) >> 4;
        cassert(fore < 16);
        cassert(back < 16);
        edit_color((Edit *)obj->component, (fore != COL_BLACK) ? i_COLORS[fore] : kCOLOR_DEFAULT);
        edit_bgcolor((Edit *)obj->component, (back != COL_BRIGHT_WHITE) ? i_COLORS[back] : kCOLOR_DEFAULT);
    }

    if (arrpt_size(hbcols, String) > 1)
    {
        const String *c = arrpt_get_const(hbcols, 1, String);
        int attr = hb_gtColorToN(tc(c));
        int fore = attr & 0x000F;
        int back = (attr & 0x00F0) >> 4;
        cassert(fore < 16);
        cassert(back < 16);
        edit_color_focus((Edit *)obj->component, (fore != COL_BLACK) ? i_COLORS[fore] : kCOLOR_DEFAULT);
        edit_bgcolor_focus((Edit *)obj->component, (back != COL_BRIGHT_WHITE) ? i_COLORS[back] : kCOLOR_DEFAULT);
    }

    arrpt_destroy(&hbcols, str_destroy, String);
}

/*---------------------------------------------------------------------------*/

static void i_OnWizardButton(GtNapCallback *callback, Event *e)
{
    GtNapObject *obj = NULL;
    cassert_no_null(callback);
    cassert_no_null(callback->gtwin);
    cassert(callback->autoclose_id == UINT32_MAX);
    unref(e);
    obj = arrpt_get(callback->gtwin->objects, callback->key, GtNapObject);
    i_launch_wizard(callback->gtwin, obj);
}

/*---------------------------------------------------------------------------*/

static void i_OnKeyWizard(GtNapWindow *gtwin, Event *e)
{
    cassert_no_null(gtwin);
    unref(e);
    arrpt_foreach(obj, gtwin->objects, GtNapObject)
        if (obj->type == ekOBJ_EDIT)
        {
            if (obj->has_focus == TRUE)
            {
                if (obj->wizard_block != NULL)
                    i_launch_wizard(gtwin, obj);
                return;
            }
        }
    arrpt_end();
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_edit_wizard(const uint32_t wid, const uint32_t id, const uint32_t bid, int32_t key, HB_ITEM *auto_block, HB_ITEM *wizard_block)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    const GtNapKey *nkey = i_convert_key(key);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_EDIT);
    cassert(obj->auto_block == NULL);
    cassert(obj->wizard_block == NULL);
    unref(key);
    if (auto_block != NULL)
        obj->auto_block = hb_itemNew(auto_block);

    if (wizard_block != NULL)
        obj->wizard_block = hb_itemNew(wizard_block);

    if (bid != UINT32_MAX)
    {
        GtNapObject *bobj = i_gtobj(GTNAP_GLOBAL, wid, bid);
        Listener *listener = NULL;
        cassert_no_null(bobj);
        cassert(bobj->type == ekOBJ_BUTTON);
        listener = i_gtnap_listener(NULL, id, UINT32_MAX, bobj->gtwin, i_OnWizardButton);
        bobj->editBoxIndexForButton = id;
        button_OnClick((Button *)bobj->component, listener);

        {
            char_t text[8];
            uint32_t b = unicode_to_char(0x25BE, text, ekUTF8);
            text[b] = '\0';
            button_text((Button *)bobj->component, text);
        }
    }

    if (nkey != NULL)
        window_hotkey(obj->gtwin->window, nkey->vkey, 0, listener(obj->gtwin, i_OnKeyWizard, GtNapWindow));
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_textview(const uint32_t wid, const int32_t top, const int32_t left, const int32_t bottom, const int32_t right, HB_ITEM *get_set_block, HB_ITEM *valida_block, HB_ITEM *keyfilter_block, const bool_t in_scroll)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    TextView *view = textview_create();
    S2Df size;
    uint32_t id = UINT32_MAX;
    GtNapObject *obj = NULL;
    bool_t is_editable = FALSE;
    cassert_no_null(gtwin);
    textview_family(view, font_family(GTNAP_GLOBAL->global_font));
    textview_fsize(view, font_size(GTNAP_GLOBAL->global_font));
    textview_units(view, font_units(GTNAP_GLOBAL->global_font));
    textview_apply_all(view);
    size.width = (real32_t)(right - left + 1) * GTNAP_GLOBAL->cell_x_sizef;
    size.height = (real32_t)(bottom - top + 1) * GTNAP_GLOBAL->cell_y_sizef;
    id = i_add_object(ekOBJ_TEXTVIEW, top - gtwin->top, left - gtwin->left, GTNAP_GLOBAL->cell_x_sizef, GTNAP_GLOBAL->cell_y_sizef, &size, in_scroll, (GuiComponent *)view, gtwin);
    obj = arrpt_last(gtwin->objects, GtNapObject);
    cassert_no_null(obj);
    cassert(obj->type = ekOBJ_TEXTVIEW);

    if (get_set_block != NULL)
        obj->get_set_block = hb_itemNew(get_set_block);

    if (valida_block != NULL)
        obj->valida_block = hb_itemNew(valida_block);

    if (keyfilter_block != NULL)
        obj->keyfilter_block = hb_itemNew(keyfilter_block);

    i_set_view_text(obj);
    is_editable = i_is_editable(gtwin, obj);
    textview_editable(view, is_editable);
    return id;
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_textview_scroll(const uint32_t wid, const uint32_t id, const bool_t horizontal, const bool_t vertical)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TEXTVIEW);
    textview_scroll_visible((TextView *)obj->component, horizontal, vertical);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_textview_caret(const uint32_t wid, const uint32_t id, const int64_t pos)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TEXTVIEW);
    textview_select((TextView *)obj->component, (int32_t)pos, (int32_t)pos);
    textview_scroll_caret((TextView *)obj->component);
}

/*---------------------------------------------------------------------------*/

static void i_OnTextConfirm(GtNapObject *gtobj, Event *e)
{
    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_TEXTVIEW);
    cassert_no_null(gtobj->gtwin);
    unref(e);
    if (gtobj->get_set_block != NULL)
    {
        bool_t valid = TRUE;
        bool_t confirm = FALSE;
        HB_ITEM *citem = hb_itemDo(gtobj->get_set_block, 0);
        cassert_no_null(citem);
        cassert(HB_ITEM_TYPE(citem) == HB_IT_STRING);

        /* Overwrite the Harbour variable with the current TextView text (for validation) */
        {
            const char_t *utf8 = textview_get_text((const TextView *)gtobj->component);
            String *cpstr = i_utf8_to_cp_string(utf8);
            HB_ITEM *item = hb_itemPutC(NULL, tc(cpstr));

            if (item != NULL)
            {
                HB_ITEM *ritem = hb_itemDo(gtobj->get_set_block, 1, item);
                hb_itemRelease(item);
                hb_itemRelease(ritem);
            }

            str_destroy(&cpstr);
        }

        /* Validate the text */
        if (gtobj->valida_block != NULL)
        {
            HB_ITEM *ritem = hb_itemDo(gtobj->valida_block, 0);
            cassert(HB_ITEM_TYPE(ritem) == HB_IT_LOGICAL);
            valid = (bool_t)hb_itemGetL(ritem);
            hb_itemRelease(ritem);
        }

        /* Confirma the text */
        if (valid == TRUE)
        {
            if (gtobj->gtwin->confirm_block != NULL)
            {
                HB_ITEM *ritem = hb_itemDo(gtobj->gtwin->confirm_block, 0);
                cassert(HB_ITEM_TYPE(ritem) == HB_IT_LOGICAL);
                confirm = (bool_t)hb_itemGetL(ritem);
                hb_itemRelease(ritem);
            }
        }

        /* Restore the Harbour variable with the original text */
        if (confirm == FALSE)
        {
            HB_ITEM *ritem = hb_itemDo(gtobj->get_set_block, 1, citem);
            hb_itemRelease(ritem);
        }

        hb_itemRelease(citem);

        if (confirm == TRUE)
        {
            GtNapWindow *gtwin = gtobj->gtwin;
            if (gtwin->parent_id != UINT32_MAX)
                gtwin = i_gtwin(GTNAP_GLOBAL, gtwin->parent_id);
            i_stop_modal(GTNAP_GLOBAL, gtwin, NAP_MODAL_TEXT_CONFIRM);
        }
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_textview_button(const uint32_t wid, const uint32_t id, const uint32_t bid)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    GtNapObject *bobj = i_gtobj(GTNAP_GLOBAL, wid, bid);
    cassert_no_null(obj);
    cassert_no_null(bobj);
    cassert(obj->type == ekOBJ_TEXTVIEW);
    cassert(bobj->type == ekOBJ_BUTTON);
    button_OnClick((Button *)bobj->component, listener(obj, i_OnTextConfirm, GtNapObject));
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_textview_hotkey(uint32_t wid, uint32_t id, int32_t key)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    const GtNapKey *nkey = i_convert_key(key);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TEXTVIEW);
    if (nkey != NULL)
    {
        Window *window = i_effective_window(obj->gtwin, GTNAP_GLOBAL);
        window_hotkey(window, nkey->vkey, nkey->modifiers, listener(obj, i_OnTextConfirm, GtNapObject));
    }
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_menu(const uint32_t wid, const int32_t top, const int32_t left, const int32_t bottom, const int32_t right, const bool_t autoclose, const bool_t in_scroll)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    Panel *panel = nap_menu_create(autoclose, GTNAP_GLOBAL->global_font, GTNAP_GLOBAL->cell_x_sizef, GTNAP_GLOBAL->cell_y_sizef);
    S2Df size, final_size;
    uint32_t id = UINT32_MAX;
    cassert_no_null(gtwin);
    size.width = (real32_t)(right - left + 1) * GTNAP_GLOBAL->cell_x_sizef;
    size.height = (real32_t)(bottom - top + 1) * GTNAP_GLOBAL->cell_y_sizef;
    _panel_compose(panel, &size, &final_size);
    _panel_locate(panel);
    id = i_add_object(ekOBJ_MENU, top - gtwin->top, left - gtwin->left, GTNAP_GLOBAL->cell_x_sizef, GTNAP_GLOBAL->cell_y_sizef, &size, in_scroll, (GuiComponent *)panel, gtwin);
    return id;
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_menu_add(const uint32_t wid, uint32_t id, HB_ITEM *text_block, HB_ITEM *click_block, uint32_t kpos)
{
    GtNapObject *gtobj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(gtobj);
    cassert_no_null(gtobj->gtwin);
    cassert(gtobj->type == ekOBJ_MENU);
    nap_menu_add((Panel *)gtobj->component, gtobj->gtwin->window, text_block, click_block, kpos);
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_menu_selected(const uint32_t wid, uint32_t id)
{
    GtNapObject *gtobj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_MENU);
    return nap_menu_selected((Panel *)gtobj->component);
}

/*---------------------------------------------------------------------------*/

static bool_t i_in_vect(const ArrSt(uint32_t) *sel, const uint32_t i)
{
    arrst_foreach_const(id, sel, uint32_t)
        if (*id == i)
            return TRUE;
    arrst_end();
    return FALSE;
}

/*---------------------------------------------------------------------------*/

static void i_toogle_sel(TableView *view, const ArrSt(uint32_t) *sel, const uint32_t row)
{
    if (i_in_vect(sel, row) == TRUE)
        tableview_deselect(view, &row, 1);
    else
        tableview_select(view, &row, 1);
}

/*---------------------------------------------------------------------------*/

static void i_OnTableRowClick(GtNapObject *gtobj, Event *e)
{
    const EvTbRow *p = event_params(e, EvTbRow);
    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_TABLEVIEW);
    if (gtobj->multisel == TRUE)
    {
        /* The row has not been selected in click --> We force the selection */
        if (p->sel == FALSE)
        {
            const ArrSt(uint32_t) *sel = tableview_selected((TableView *)gtobj->component);
            i_toogle_sel((TableView *)gtobj->component, sel, p->row);
            tableview_update((TableView *)gtobj->component);
        }
    }
    else if (gtobj->autoclose == TRUE)
    {
        uint32_t ret_value = NAP_MODAL_ROW_CLICK;
        GtNapWindow *gtwin = gtobj->gtwin;
        GtNapArea *gtarea = NULL;
        cassert_no_null(gtwin);
        gtarea = gtwin->gtarea;
        if (gtarea != NULL)
        {
            uint32_t *recno = arrst_get(gtarea->records, p->row, uint32_t);
            cassert_no_null(recno);
            ret_value += *recno;
        }
        else
        {
            ret_value += p->row + 1;
        }

        i_stop_modal(GTNAP_GLOBAL, gtwin, ret_value);
    }
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_tableview(const uint32_t wid, const int32_t top, const int32_t left, const int32_t bottom, const int32_t right, const bool_t multisel, const bool_t autoclose, const bool_t in_scroll)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    TableView *view = tableview_create();
    S2Df size;
    uint32_t id = UINT32_MAX;
    GtNapObject *obj = NULL;
    cassert_no_null(gtwin);
    cassert(gtwin->gtarea == NULL);
    tableview_font(view, GTNAP_GLOBAL->global_font);
    tableview_multisel(view, multisel, multisel);
    size.width = (real32_t)(right - left + 1) * GTNAP_GLOBAL->cell_x_sizef;
    size.height = (real32_t)(bottom - top + 1) * GTNAP_GLOBAL->cell_y_sizef;
    id = i_add_object(ekOBJ_TABLEVIEW, top - gtwin->top, left - gtwin->left, GTNAP_GLOBAL->cell_x_sizef, GTNAP_GLOBAL->cell_y_sizef, &size, in_scroll, (GuiComponent *)view, gtwin);
    obj = arrpt_last(gtwin->objects, GtNapObject);
    cassert_no_null(obj);
    cassert(obj->type = ekOBJ_TABLEVIEW);
    obj->multisel = multisel;
    obj->autoclose = autoclose;
    tableview_OnRowClick(view, listener(obj, i_OnTableRowClick, GtNapObject));
    tableview_row_height(view, GTNAP_GLOBAL->cell_y_sizef);
    tableview_hkey_scroll(view, TRUE, 0.f);
    return id;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_header_char_width(const char_t *title, uint32_t *nlines)
{
    uint32_t nchars = 0;
    ArrPt(String) *strs = str_splits(title, "\n", TRUE, FALSE);
    cassert_no_null(nlines);
    *nlines = arrpt_size(strs, String);
    arrpt_foreach_const(str, strs, String)
        uint32_t n = unicode_nchars(tc(str), ekUTF8);
        if (n > nchars)
            nchars = n;
    arrpt_end();

    arrpt_destroy(&strs, str_destroy, String);
    return nchars;
}

/*---------------------------------------------------------------------------*/

static real32_t i_col_widthf(const uint32_t fixed_chars, const uint32_t str_chars, const GtNap *gtnap)
{
    uint32_t cwidth = fixed_chars;
    cassert_no_null(gtnap);

    if (cwidth == 0)
        cwidth = str_chars;

    return (real32_t)(cwidth + 1) * gtnap->cell_x_sizef;
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_column(const uint32_t wid, const uint32_t id, const uint32_t width, HB_ITEM *head_block, HB_ITEM *eval_block)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    GtNapColumn *col = NULL;
    uint32_t cid = UINT32_MAX;
    uint32_t hnchars = 0;
    uint32_t nlines = 0;
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);

    if (obj->columns == NULL)
        obj->columns = arrst_create(GtNapColumn);

    cid = tableview_add_column_text((TableView *)obj->component);
    cassert(cid == arrst_size(obj->columns, GtNapColumn));

    col = arrst_new(obj->columns, GtNapColumn);

    if (head_block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(head_block, 0);
        col->title = i_item_to_utf8_string(ritem);
        hb_itemRelease(ritem);
    }
    else
    {
        col->title = str_c("");
    }

    str_repl_c(tcc(col->title), ";", "\n");
    hnchars = i_header_char_width(tc(col->title), &col->header_lines);
    col->fixed_chars = width;
    col->widthf = i_col_widthf(col->fixed_chars, hnchars, GTNAP_GLOBAL);
    col->align = ekJUSTIFY;
    col->block = hb_itemNew(eval_block);

    arrst_foreach(c, obj->columns, GtNapColumn)
        if (c->header_lines > nlines)
            nlines = c->header_lines;
    arrst_end();

    tableview_header_title((TableView *)obj->component, cid, tc(col->title));
    tableview_column_width((TableView *)obj->component, cid, col->widthf);
    tableview_column_align((TableView *)obj->component, cid, col->align);
    tableview_header_align((TableView *)obj->component, cid, col->align);
    tableview_header_height((TableView *)obj->component, (real32_t)nlines * GTNAP_GLOBAL->cell_y_sizef);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_scroll(const uint32_t wid, const uint32_t id, const bool_t horizontal, const bool_t vertical)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    tableview_scroll_visible((TableView *)obj->component, horizontal, vertical);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_grid(const uint32_t wid, const uint32_t id, const bool_t hlines, const bool_t vlines)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    tableview_grid((TableView *)obj->component, hlines, vlines);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_header(const uint32_t wid, const uint32_t id, const bool_t visible)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    tableview_header_visible((TableView *)obj->component, visible);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_freeze(const uint32_t wid, const uint32_t id, const uint32_t col_id)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    cassert(col_id > 0);
    tableview_column_freeze((TableView *)obj->component, col_id - 1);
}

/*---------------------------------------------------------------------------*/

static GtNapArea *i_create_area(void)
{
    GtNapArea *area = heap_new0(GtNapArea);
    area->records = arrst_create(uint32_t);
    area->cache_recno = UINT32_MAX;
    area->while_block = NULL;
    return area;
}

/*---------------------------------------------------------------------------*/

static void i_rtrim(char_t *buffer)
{
    uint32_t n = str_len_c(buffer);
    cassert_no_null(buffer);
    while (n > 0)
    {
        if (buffer[n - 1] == ' ')
            buffer[n - 1] = '\0';
        else
            return;

        n -= 1;
    }
}

/*---------------------------------------------------------------------------*/

static void i_hbitem_to_char(HB_ITEM *item, char_t *buffer, const uint32_t size, const bool_t utf8)
{
    HB_TYPE type = HB_ITEM_TYPE(item);
    buffer[0] = '\0';

    switch (type)
    {
    case HB_IT_STRING:
        if (utf8 == TRUE)
        {
            const char_t *text = hb_itemGetCPtr(item);
            str_copy_c(buffer, size, text);
        }
        else
        {
            hb_itemCopyStrUTF8(item, buffer, size);
        }

        i_rtrim(buffer);
        break;

    case HB_IT_DATE:
    {
        char date[16];
        hb_itemGetDS(item, date);
        hb_dateFormat(date, buffer, hb_setGetDateFormat());
        break;
    }

    case HB_IT_DOUBLE:
    {
        double value = hb_itemGetND(item);
        bstd_sprintf(buffer, size, "%12.4f", value);
        break;
    }

    case HB_IT_LONG:
    case HB_IT_INTEGER:
    {
        HB_MAXINT value = hb_itemGetNInt(item);
        bstd_sprintf(buffer, size, "%d", (int)value);
        break;
    }

    case HB_IT_LOGICAL:
    {
        HB_BOOL value = hb_itemGetL(item);
        bstd_sprintf(buffer, size, "%s", value ? "true" : "false");
        break;
    }

    default:
        buffer[0] = '\0';
    }
}

/*---------------------------------------------------------------------------*/

static const char_t *i_area_eval_field(GtNapArea *gtarea, const uint32_t field_id, const uint32_t row_id, align_t *align)
{
    uint32_t recno = 0;
    const GtNapColumn *column = NULL;
    HB_ITEM *ritem = NULL;

    cassert_no_null(gtarea);
    cassert_no_null(gtarea->gtobj);
    cassert(gtarea->gtobj->type == ekOBJ_TABLEVIEW);
    cassert(field_id > 0);

    /* Go to DB record */
    recno = *arrst_get_const(gtarea->records, row_id, uint32_t);
    SELF_GOTO(gtarea->area, recno);

    /* Get the table column */
    column = arrst_get_const(gtarea->gtobj->columns, field_id - 1, GtNapColumn);

    /* CodeBlock that computes the cell content */
    ritem = hb_itemDo(column->block, 0);

    /* Fill the temporal cell buffer with cell result */
    i_hbitem_to_char(ritem, TEMP_BUFFER, sizeof(TEMP_BUFFER), FALSE);

    hb_itemRelease(ritem);

    if (align != NULL)
        *align = column->align;

    return TEMP_BUFFER;
}

/*---------------------------------------------------------------------------*/

static void i_area_column_width(GtNapArea *gtarea, const uint32_t col, const char_t *text)
{
    GtNapColumn *column = NULL;
    uint32_t nchars = 0;
    real32_t width = 0;
    cassert_no_null(gtarea);
    cassert_no_null(gtarea->gtobj);
    cassert(gtarea->gtobj->type == ekOBJ_TABLEVIEW);
    column = arrst_get(gtarea->gtobj->columns, col, GtNapColumn);
    nchars = unicode_nchars(text, ekUTF8);
    width = i_col_widthf(column->fixed_chars, nchars, GTNAP_GLOBAL);

    if (width > column->widthf)
    {
        column->widthf = width;
        tableview_column_width((TableView *)gtarea->gtobj->component, col, column->widthf);
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnTableAreaData(GtNapArea *gtarea, Event *e)
{
    uint32_t etype = event_type(e);
    cassert_no_null(gtarea);

    switch (etype)
    {
    case ekGUI_EVENT_TBL_BEGIN:
        SELF_RECNO(gtarea->area, &gtarea->cache_recno);
        break;

    case ekGUI_EVENT_TBL_END:
        SELF_GOTO(gtarea->area, gtarea->cache_recno);
        gtarea->cache_recno = UINT32_MAX;
        break;

    case ekGUI_EVENT_TBL_NROWS:
    {
        uint32_t *n = event_result(e, uint32_t);
        *n = arrst_size(gtarea->records, uint32_t);
        break;
    }

    case ekGUI_EVENT_TBL_CELL:
    {
        EvTbCell *cell = event_result(e, EvTbCell);
        const EvTbPos *pos = event_params(e, EvTbPos);
        cell->text = i_area_eval_field(gtarea, pos->col + 1, pos->row, &cell->align);
        /* Table column automatic width based on cell content */
        i_area_column_width(gtarea, pos->col, cell->text);
        break;
    }

    default:
        break;
    }
}

/*---------------------------------------------------------------------------*/

static void i_OnTableAreaRowSelect(GtNapArea *gtarea, Event *e)
{
    cassert_no_null(gtarea);
    cassert_no_null(gtarea->gtobj);
    cassert(gtarea->gtobj->type == ekOBJ_TABLEVIEW);
    if (gtarea->gtobj->multisel == FALSE)
    {
        const EvTbSel *sel = event_params(e, EvTbSel);
        uint32_t first = 0;
        uint32_t recno = 0;
        cassert(arrst_size(sel->sel, uint32_t) == 1);
        /* The row selected in table */
        first = *arrst_first_const(sel->sel, uint32_t);
        /* The DB RECNO in this row selected in table */
        recno = *arrst_get_const(gtarea->records, first, uint32_t);
        /* Just GOTO */
        SELF_GOTO(gtarea->area, recno);
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_bind_area(const uint32_t wid, const uint32_t id, HB_ITEM *while_block)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    GtNapWindow *gtwin = NULL;
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    gtwin = obj->gtwin;
    cassert_no_null(gtwin);

    if (gtwin->gtarea != NULL)
        i_destroy_area(&gtwin->gtarea);

    gtwin->gtarea = i_create_area();
    gtwin->gtarea->area = (AREA *)hb_rddGetCurrentWorkAreaPointer();
    gtwin->gtarea->gtobj = obj;

    if (while_block != NULL)
        gtwin->gtarea->while_block = hb_itemNew(while_block);
    else
        gtwin->gtarea->while_block = NULL;

    tableview_OnData((TableView *)obj->component, listener(gtwin->gtarea, i_OnTableAreaData, GtNapArea));
    tableview_OnSelect((TableView *)obj->component, listener(gtwin->gtarea, i_OnTableAreaRowSelect, GtNapArea));
}

/*---------------------------------------------------------------------------*/

static const char_t *i_data_eval_field(GtNapObject *gtobj, const uint32_t col_id, const uint32_t row_id, align_t *align)
{
    const GtNapColumn *column = NULL;
    HB_ITEM *pitem = NULL;
    HB_ITEM *ritem = NULL;

    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_TABLEVIEW);

    /* Get the table column */
    column = arrst_get_const(gtobj->columns, col_id, GtNapColumn);

    /* TODO: CACHE THIS PARAM */
    pitem = hb_itemPutNI(NULL, row_id + 1);

    /* CodeBlock that computes the cell content */
    ritem = hb_itemDo(column->block, 1, pitem);

    /* Fill the temporal cell buffer with cell result */
    i_hbitem_to_char(ritem, TEMP_BUFFER, sizeof(TEMP_BUFFER), FALSE);

    hb_itemRelease(pitem);
    hb_itemRelease(ritem);

    if (align != NULL)
        *align = column->align;

    return TEMP_BUFFER;
}

/*---------------------------------------------------------------------------*/

static void i_OnTableData(GtNapObject *gtobj, Event *e)
{
    uint32_t etype = event_type(e);
    cassert_no_null(gtobj);
    cassert(gtobj->type == ekOBJ_TABLEVIEW);
    cassert_no_null(gtobj->gtwin);

    switch (etype)
    {
    case ekGUI_EVENT_TBL_BEGIN:
        break;

    case ekGUI_EVENT_TBL_END:
        break;

    case ekGUI_EVENT_TBL_NROWS:
    {
        uint32_t *n = event_result(e, uint32_t);
        *n = gtobj->gtwin->num_rows;
        break;
    }

    case ekGUI_EVENT_TBL_CELL:
    {
        EvTbCell *cell = event_result(e, EvTbCell);
        const EvTbPos *pos = event_params(e, EvTbPos);
        cell->text = i_data_eval_field(gtobj, pos->col, pos->row, &cell->align);
        break;
    }

    default:
        break;
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_bind_data(const uint32_t wid, const uint32_t id, const uint32_t num_rows)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    GtNapWindow *gtwin = NULL;
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    gtwin = obj->gtwin;
    cassert_no_null(gtwin);
    gtwin->num_rows = num_rows;
    tableview_OnData((TableView *)obj->component, listener(obj, i_OnTableData, GtNapObject));
    tableview_OnSelect((TableView *)obj->component, NULL);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_deselect_all(const uint32_t wid, const uint32_t id)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    tableview_deselect_all((TableView *)obj->component);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_select_row(const uint32_t wid, const uint32_t id, const uint32_t row_id)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    tableview_select((TableView *)obj->component, &row_id, 1);
    tableview_update((TableView *)obj->component);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_toggle_row(const uint32_t wid, const uint32_t id, const uint32_t row_id)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    const ArrSt(uint32_t) *sel = NULL;
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    sel = tableview_selected((TableView *)obj->component);
    i_toogle_sel((TableView *)obj->component, sel, row_id);
    tableview_update((TableView *)obj->component);
}

/*---------------------------------------------------------------------------*/

const ArrSt(uint32_t) *hb_gtnap_tableview_selected_rows(const uint32_t wid, const uint32_t id)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    return tableview_selected((TableView *)obj->component);
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_tableview_focus_row(const uint32_t wid, const uint32_t id)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    uint32_t focused;
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    focused = tableview_get_focus_row((TableView *)obj->component);
    return focused;
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_tableview_recno_from_row(const uint32_t wid, const uint32_t id, const uint32_t row_id)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    GtNapWindow *gtwin = NULL;
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    gtwin = obj->gtwin;
    cassert_no_null(gtwin);
    if (gtwin->gtarea != NULL)
    {
        uint32_t recno = *arrst_get_const(gtwin->gtarea->records, row_id, uint32_t);
        return recno;
    }

    return UINT32_MAX;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_row_from_recno(GtNapArea *area, const uint32_t recno)
{
    cassert_no_null(area);
    cassert(recno > 0);
    arrst_foreach_const(rec, area->records, uint32_t)
        if (*rec == recno)
            return rec_i;
    arrst_end();
    return UINT32_MAX;
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_tableview_row_from_recno(const uint32_t wid, const uint32_t id, const uint32_t recno)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    GtNapWindow *gtwin = NULL;
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    gtwin = obj->gtwin;
    cassert_no_null(gtwin);
    if (gtwin->gtarea != NULL)
    {
        uint32_t row = i_row_from_recno(gtwin->gtarea, recno);
        return row;
    }

    return UINT32_MAX;
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_refresh_current(const uint32_t wid, const uint32_t id)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    tableview_update((TableView *)obj->component);
}

/*---------------------------------------------------------------------------*/

static void i_area_refresh(GtNapArea *area)
{
    HB_ULONG ulCurRec;

    cassert_no_null(area);

    /* Database current RECNO() */
    SELF_RECNO(area->area, &ulCurRec);

    /* Clear the current record index */
    arrst_clear(area->records, NULL, uint32_t);

    /* Generate the record index for TableView */
    if (area->while_block == NULL)
    {
        HB_BOOL fEof;
        SELF_GOTOP(area->area);
        SELF_EOF(area->area, &fEof);
        while (fEof == HB_FALSE)
        {
            HB_ULONG uiRecNo = 0;
            SELF_RECNO(area->area, &uiRecNo);
            arrst_append(area->records, (uint32_t)uiRecNo, uint32_t);
            SELF_SKIP(area->area, 1);
            SELF_EOF(area->area, &fEof);
        }
    }
    else
    {
        HB_BOOL fEof;
        SELF_GOTOP(area->area);
        SELF_EOF(area->area, &fEof);
        while (fEof == HB_FALSE)
        {
            HB_ULONG uiRecNo = 0;
            SELF_RECNO(area->area, &uiRecNo);

            {
                PHB_ITEM ritem = hb_itemDo(area->while_block, 0);
                HB_TYPE type = HB_ITEM_TYPE(ritem);
                bool_t add = FALSE;
                cassert_unref(type == HB_IT_LOGICAL, type);
                add = (bool_t)hb_itemGetL(ritem);
                hb_itemRelease(ritem);

                if (add == TRUE)
                    arrst_append(area->records, (uint32_t)uiRecNo, uint32_t);
            }

            SELF_SKIP(area->area, 1);
            SELF_EOF(area->area, &fEof);
        }
    }

    /* Restore database RECNO() */
    SELF_GOTO(area->area, ulCurRec);
}

/*---------------------------------------------------------------------------*/

static void i_area_select_row(GtNapArea *gtarea)
{
    HB_ULONG ulCurRec;
    uint32_t sel_row;
    TableView *view;

    cassert_no_null(gtarea);
    cassert_no_null(gtarea->gtobj);
    cassert(gtarea->gtobj->type == ekOBJ_TABLEVIEW);
    view = (TableView *)gtarea->gtobj->component;

    /* Current selected */
    SELF_RECNO(gtarea->area, &ulCurRec);

    sel_row = i_row_from_recno(gtarea, (uint32_t)ulCurRec);

    /* In multisel table, the selected rows comes from  VN_Selecio */
    if (gtarea->gtobj->multisel == TRUE)
    {
        if (tableview_get_focus_row(view) == UINT32_MAX)
        {
            /* We use RECNO for focused row */
            if (sel_row != UINT32_MAX)
            {
                tableview_focus_row(view, sel_row, ekTOP);
            }
            else
            {
                uint32_t nrecs = arrst_size(gtarea->records, uint32_t);
                sel_row = tableview_get_focus_row(view);
                /* We move recno to current focused row */
                if (sel_row >= nrecs)
                {
                    sel_row = 0;
                }

                if (sel_row < nrecs)
                {
                    uint32_t recno = *arrst_get_const(gtarea->records, sel_row, uint32_t);
                    tableview_select(view, &sel_row, 1);
                    SELF_GOTO(gtarea->area, recno);
                }
            }
        }
    }
    else
    {
        tableview_deselect_all(view);

        if (sel_row != UINT32_MAX)
        {
            tableview_select(view, &sel_row, 1);
            tableview_focus_row(view, sel_row, ekTOP);
        }
        /* RECNO() doesn't exists in view (perhaps is deleted) */
        else
        {
            uint32_t nrecs = arrst_size(gtarea->records, uint32_t);
            sel_row = tableview_get_focus_row(view);
            /* We move recno to current focused row */
            if (sel_row < nrecs)
            {
                uint32_t recno = *arrst_get_const(gtarea->records, sel_row, uint32_t);
                tableview_select(view, &sel_row, 1);
                SELF_GOTO(gtarea->area, recno);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_tableview_refresh_all(const uint32_t wid, const uint32_t id)
{
    GtNapObject *obj = i_gtobj(GTNAP_GLOBAL, wid, id);
    GtNapWindow *gtwin = NULL;
    TableView *view = NULL;
    cassert_no_null(obj);
    cassert(obj->type == ekOBJ_TABLEVIEW);
    gtwin = obj->gtwin;
    view = (TableView *)obj->component;
    cassert_no_null(gtwin);
    if (gtwin->gtarea != NULL && view != NULL)
    {
        i_area_refresh(gtwin->gtarea);
        tableview_update(view);
        i_area_select_row(gtwin->gtarea);
    }
    else
    {
        tableview_update(view);
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_toolbar(const uint32_t wid, const uint32_t image_pixels)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    cassert_no_null(gtwin);
    cassert(gtwin->toolbar == NULL);
    gtwin->toolbar = heap_new0(GtNapToolbar);
    gtwin->toolbar->items = arrpt_create(GuiComponent);
    gtwin->toolbar->button_widthf = (real32_t)image_pixels;
    gtwin->toolbar->heightf = GTNAP_GLOBAL->cell_y_sizef * 2.f;
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_toolbar_button(const uint32_t wid, const char_t *pathname, const char_t *tooltip, HB_ITEM *click_block)
{
    Image *image = NULL;
    char_t utf8[STATIC_TEXT_SIZE];
    i_cp_to_utf8(pathname, utf8, sizeof(utf8));
    image = image_from_file(utf8, NULL);

    if (image != NULL)
    {
        GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
        Button *button = button_flat();
        Listener *listener = i_gtnap_listener(click_block, INT32_MAX, UINT32_MAX, gtwin, i_OnButtonClick);
        uint32_t image_size = UINT32_MAX;
        cassert_no_null(gtwin);
        cassert_no_null(gtwin->toolbar);
        image_size = (uint32_t)gtwin->toolbar->button_widthf;
        if (image_width(image) != image_size || image_height(image) != image_size)
        {
            Image *scaled = image_scale(image, image_size, image_size);
            image_destroy(&image);
            image = scaled;
        }

        button_image(button, image);
        button_OnClick(button, listener);
        i_cp_to_utf8(tooltip, utf8, sizeof(utf8));
        button_tooltip(button, utf8);
        arrpt_append(gtwin->toolbar->items, (GuiComponent *)button, GuiComponent);
        image_destroy(&image);
    }
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_toolbar_separator(const uint32_t wid)
{
    GtNapWindow *gtwin = i_gtwin(GTNAP_GLOBAL, wid);
    View *separator = _view_create(ekVIEW_BORDER | ekVIEW_CONTROL);
    cassert_no_null(gtwin);
    cassert_no_null(gtwin->toolbar);
    arrpt_append(gtwin->toolbar->items, (GuiComponent *)separator, GuiComponent);
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_cualib_init_log(void)
{
    osbs_start();
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_cualib_window_f4_lista(void)
{
    GtNapWindow *gtwin = i_current_gtwin(GTNAP_GLOBAL);
    window_hotkey(gtwin->window, ekKEY_F4, 0, listener(gtwin, i_OnKeyWizard, GtNapWindow));
}

/*---------------------------------------------------------------------------*/

uint32_t hb_gtnap_cualib_window_current_edit(void)
{
    uint32_t id = 0;
    GtNapWindow *gtwin = i_current_main_gtwin(GTNAP_GLOBAL);
    arrpt_foreach(obj, gtwin->objects, GtNapObject)
        if (obj->type == ekOBJ_EDIT)
        {
            if (obj->has_focus == TRUE)
                return id;
            id += 1;
        }
    arrpt_end();
    return UINT32_MAX;
}

/*---------------------------------------------------------------------------*/

void hb_gtnap_cualib_default_button(const uint32_t nDefault)
{
    GtNapWindow *gtwin = i_current_gtwin(GTNAP_GLOBAL);
    cassert(gtwin->default_button == UINT32_MAX);
    cassert(nDefault > 0);
    gtwin->default_button = nDefault - 1;
}

/*---------------------------------------------------------------------------*/

static GtNap *i_gtnap_forms_create(void)
{
    const char_t *build_cfg = NULL;
    GTNAP_GLOBAL = heap_new0(GtNap);
    GTNAP_GLOBAL->date_digits = (hb_setGetCentury() == (HB_BOOL)HB_TRUE) ? 8 : 6;
    GTNAP_GLOBAL->date_chars = GTNAP_GLOBAL->date_digits + 2;

    {
        char_t path[512];
        bfile_dir_work(path, sizeof(path));
        GTNAP_GLOBAL->working_path = str_c(path);
    }

#if defined(__DEBUG__)
    build_cfg = "Debug";
#else
    build_cfg = "Release";
#endif

    {
        const char_t *debpath = deblib_path();
#if defined(__MACOS__)
        GTNAP_GLOBAL->debugger_path = str_cpath("%s/%s/bin/gtnapdeb.app/Contents/MacOS/gtnapdeb", debpath, build_cfg);
#else
        GTNAP_GLOBAL->debugger_path = str_cpath("%s/%s/bin/gtnapdeb", debpath, build_cfg);
#endif
        GTNAP_GLOBAL->debugger_visible = FALSE;
        GTNAP_GLOBAL->debugger = NULL;
    }

    {
        PHB_ITEM ritem = NULL;
        deblib_init_colors(i_COLORS);
        ritem = hb_itemDo(INIT_CODEBLOCK, 0);
        hb_itemRelease(ritem);
    }

    hb_itemRelease(INIT_CODEBLOCK);
    INIT_TITLE[0] = 0;
    INIT_CODEBLOCK = NULL;
    return GTNAP_GLOBAL;
}

/*---------------------------------------------------------------------------*/

static void i_gtnap_forms_update(GtNap *gtnap, const real64_t prtime, const real64_t ctime)
{
    cassert(gtnap == NULL || gtnap == GTNAP_GLOBAL);
    gtnap = GTNAP_GLOBAL;
    cassert_no_null(gtnap);
    unref(prtime);
    unref(ctime);
}

/*---------------------------------------------------------------------------*/

static void i_gtnap_forms_destroy(GtNap **gtnap)
{
    cassert_no_null(gtnap);
    cassert_no_null(*gtnap);
    cassert(*gtnap == GTNAP_GLOBAL);
    str_destroy(&(*gtnap)->working_path);
    str_destroy(&(*gtnap)->debugger_path);

    if ((*gtnap)->debugger != NULL)
        nap_debugger_destroy(&(*gtnap)->debugger);

    nforms_finish();
    heap_delete(&(*gtnap), GtNap);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_init_app(HB_ITEM *main_block)
{
    void *hInstance = NULL;

#if defined(HB_OS_WIN)
    hb_winmainArgGet(&hInstance, NULL, NULL);
#endif

    nforms_start();
    INIT_CODEBLOCK = hb_itemNew(main_block);

    osmain_imp(
        0, NULL, hInstance, 0.5f,
        (FPtr_app_create)i_gtnap_forms_create,
        (FPtr_app_update)i_gtnap_forms_update,
        (FPtr_destroy)i_gtnap_forms_destroy,
        (char_t *)"");
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_exit_app(void)
{
    osapp_finish();
}

/*---------------------------------------------------------------------------*/

static void i_OnFormMoved(GtNapForm *form, Event *e)
{
    const EvPos *p = event_params(e, EvPos);
    cassert_no_null(form);
    i_write_prop_r32(tc(form->nameid), i_XPOS_PROP, p->x);
    i_write_prop_r32(tc(form->nameid), i_YPOS_PROP, p->y);
}

/*---------------------------------------------------------------------------*/

static void i_OnFormResize(GtNapForm *form, Event *e)
{
    const EvSize *p = event_params(e, EvSize);
    Window *window = event_sender(e, Window);
    cassert_no_null(form);
    if (window_get_maximize(window) == TRUE)
    {
        i_write_prop_r32(tc(form->nameid), i_WIDTH_PROP, i_MAXIMIZED_SIZE);
        i_write_prop_r32(tc(form->nameid), i_HEIGHT_PROP, i_MAXIMIZED_SIZE);
    }
    else
    {
        i_write_prop_r32(tc(form->nameid), i_WIDTH_PROP, p->width);
        i_write_prop_r32(tc(form->nameid), i_HEIGHT_PROP, p->height);
    }
}

/*---------------------------------------------------------------------------*/

GtNapForm *hbnap_forms_load(const char_t *pathname, const char_t *resource_path, const uint32_t flags)
{
    NForm *form = nform_from_file(pathname, NULL);
    if (form != NULL)
    {
        GtNapForm *gtform = heap_new0(GtNapForm);
        str_split_pathext(pathname, NULL, &gtform->nameid, NULL);
        gtform->form = form;
        gtform->respath = str_c(resource_path);
        gtform->binds = arrst_create(GtNapBind);
        gtform->callbacks = arrpt_create(GtNapCallback);

        if (!(flags & HBNAP_FORMS_EMBEDDED_PANEL))
        {
            uint32_t nflags = 0;
            if (flags & HBNAP_FORMS_RESIZABLE)
            {
                nflags |= ekWINDOW_STDRES;
                gtform->is_resizable = TRUE;
            }
            else
            {
                nflags |= ekWINDOW_STD;
            }

            if (flags & HBNAP_FORMS_CLOSE_ON_ESC)
                nflags |= ekWINDOW_ESC;

            if (flags & HBNAP_FORMS_CLOSE_ON_RETURN)
                nflags |= ekWINDOW_RETURN;

            gtform->window = nform_window(gtform->form, nflags, resource_path);
            if (gtform->window != NULL)
            {
                window_OnMoved(gtform->window, listener(gtform, i_OnFormMoved, GtNapForm));
                window_OnResize(gtform->window, listener(gtform, i_OnFormResize, GtNapForm));
            }
            else
            {
                hbnap_forms_destroy(&gtform);
            }
        }

        return gtform;
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

static void i_remove_fcolumn(GtNapFColumn *column)
{
    cassert_no_null(column);
    if (column->block != NULL)
        hb_itemRelease(column->block);
}

/*---------------------------------------------------------------------------*/

static void i_destroy_farea(GtNapFArea **area)
{
    cassert_no_null(area);
    (*area)->form = NULL;
    (*area)->table = NULL;
    (*area)->area = NULL;
    str_destroy(&(*area)->cellname);
    arrst_destroy(&(*area)->records, NULL, uint32_t);
    arrst_destroy(&(*area)->columns, i_remove_fcolumn, GtNapFColumn);
    heap_delete(area, GtNapFArea);
}

/*---------------------------------------------------------------------------*/

static void i_remove_fnode(GtNapFNode *node)
{
    cassert_no_null(node);
    node->area = NULL;
}

/*---------------------------------------------------------------------------*/

static void i_remove_farea2(GtNapFArea2 *area)
{
    cassert_no_null(area);
    area->area = NULL; /* The life cycle of the area is managed exclusively by Harbour */

    arrst_destroy(&area->columns, i_remove_fcolumn, GtNapFColumn);

    if (area->relfrom != NULL)
        hb_itemRelease(area->relfrom);

    if (area->relto != NULL)
        hb_itemRelease(area->relto);
}

/*---------------------------------------------------------------------------*/

static void i_destroy_fdbconn(GtNapFDBConn **dbconn)
{
    cassert_no_null(dbconn);
    (*dbconn)->form = NULL;
    (*dbconn)->table = NULL;
    str_destroy(&(*dbconn)->cellname);
    treest_destroy(&(*dbconn)->tdata, i_remove_fnode, GtNapFNode);
    arrst_destroy(&(*dbconn)->areas, i_remove_farea2, GtNapFArea2);
    heap_delete(dbconn, GtNapFDBConn);
}

/*---------------------------------------------------------------------------*/

static void i_remove_bind(GtNapBind *bind)
{
    cassert_no_null(bind);
    str_destroy(&bind->gui_id);
    if (bind->value != NULL)
        hb_itemRelease(bind->value);
    if (bind->listener != NULL)
        listener_destroy(&bind->listener);
    bind->value = NULL;
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_destroy(GtNapForm **form)
{
    cassert_no_null(form);
    cassert_no_null(*form);
    str_destroy(&(*form)->nameid);
    str_destroy(&(*form)->respath);
    ptr_destopt(window_destroy, &(*form)->window, Window);

    if ((*form)->OnClose_block != NULL)
    {
        hb_itemRelease((*form)->OnClose_block);
        (*form)->OnClose_block = NULL;
    }

    arrst_destroy(&(*form)->binds, i_remove_bind, GtNapBind);
    arrpt_destroy(&(*form)->callbacks, i_destroy_callback, GtNapCallback);
    ptr_destopt(i_destroy_farea, &(*form)->area, GtNapFArea);
    ptr_destopt(i_destroy_fdbconn, &(*form)->dbconn, GtNapFDBConn);
    nform_destroy(&(*form)->form);
    heap_delete(form, GtNapForm);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_title(GtNapForm *form, const char_t *text)
{
    cassert_no_null(form);
    window_title(form->window, text);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_set_text(GtNapForm *form, const char_t *cell, const char_t *text)
{
    cassert_no_null(form);
    nform_set_control_str(form->form, cell, text);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_set_int(GtNapForm *form, const char_t *cell, const int32_t value)
{
    cassert_no_null(form);
    nform_set_control_int(form->form, cell, value);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_insert_text(GtNapForm *form, const char_t *cell, const char_t *text)
{
    cassert_no_null(form);
    nform_add_control_str(form->form, cell, text);
}

/*---------------------------------------------------------------------------*/

int32_t hbnap_forms_get_int(GtNapForm *form, const char_t *cell)
{
    int32_t value = 0;
    cassert_no_null(form);
    if (nform_get_control_int(form->form, cell, &value) == TRUE)
        return value;
    return -1;
}

/*---------------------------------------------------------------------------*/

bool_t hbnap_forms_embed(GtNapForm *form, GtNapForm *embedded_form, const char_t *cell)
{
    bool_t ok = FALSE;
    cassert_no_null(form);
    cassert_no_null(embedded_form);
    cassert_no_null(form->window);
    cassert(embedded_form->window == NULL);
    ok = nform_change_embedded_panel(form->form, embedded_form->form, tc(embedded_form->respath), cell);
    if (ok == TRUE)
        window_update(form->window);
    return ok;
}

/*---------------------------------------------------------------------------*/

static const char_t *i_farea_eval_field(GtNapFArea *area, const uint32_t field_id, const uint32_t row_id)
{
    uint32_t recno = 0;
    const GtNapFColumn *column = NULL;
    HB_ITEM *ritem = NULL;

    cassert_no_null(area);
    cassert(field_id > 0);

    /* Go to DB record */
    recno = *arrst_get_const(area->records, row_id, uint32_t);
    SELF_GOTO(area->area, recno);

    /* Get the table column */
    column = arrst_get_const(area->columns, field_id - 1, GtNapFColumn);

    /* CodeBlock that computes the cell content */
    ritem = hb_itemDo(column->block, 0);

    /* Fill the temporal cell buffer with cell result */
    i_hbitem_to_char(ritem, TEMP_BUFFER, sizeof(TEMP_BUFFER), TRUE);

    hb_itemRelease(ritem);
    return TEMP_BUFFER;
}

/*---------------------------------------------------------------------------*/

static void i_OnTableFAreaData(GtNapFArea *area, Event *e)
{
    uint32_t etype = event_type(e);
    cassert_no_null(area);

    switch (etype)
    {
    case ekGUI_EVENT_TBL_BEGIN:
        SELF_RECNO(area->area, &area->cache_recno);
        break;

    case ekGUI_EVENT_TBL_END:
        SELF_GOTO(area->area, area->cache_recno);
        area->cache_recno = UINT32_MAX;
        break;

    case ekGUI_EVENT_TBL_NROWS:
    {
        uint32_t *n = event_result(e, uint32_t);
        *n = arrst_size(area->records, uint32_t);
        break;
    }

    case ekGUI_EVENT_TBL_CELL:
    {
        EvTbCell *cell = event_result(e, EvTbCell);
        const EvTbPos *pos = event_params(e, EvTbPos);
        cell->text = i_farea_eval_field(area, pos->col + 1, pos->row);
        break;
    }

    default:
        break;
    }
}

/*---------------------------------------------------------------------------*/

static void i_farea_refresh(GtNapFArea *area)
{
    HB_ULONG ulCurRec;

    cassert_no_null(area);

    /* Database current RECNO() */
    SELF_RECNO(area->area, &ulCurRec);

    /* Clear the current record index */
    arrst_clear(area->records, NULL, uint32_t);

    /* Generate the record index for TableView */
    // if (area->while_block == NULL)
    {
        HB_BOOL fEof;
        SELF_GOTOP(area->area);
        SELF_EOF(area->area, &fEof);
        while (fEof == HB_FALSE)
        {
            HB_ULONG uiRecNo = 0;
            SELF_RECNO(area->area, &uiRecNo);
            arrst_append(area->records, (uint32_t)uiRecNo, uint32_t);
            SELF_SKIP(area->area, 1);
            SELF_EOF(area->area, &fEof);
        }
    }
    // else
    //{
    //     HB_BOOL fEof;
    //     SELF_GOTOP(area->area);
    //     SELF_EOF(area->area, &fEof);
    //     while (fEof == HB_FALSE)
    //     {
    //         HB_ULONG uiRecNo = 0;
    //         SELF_RECNO(area->area, &uiRecNo);

    //        {
    //            PHB_ITEM ritem = hb_itemDo(area->while_block, 0);
    //            HB_TYPE type = HB_ITEM_TYPE(ritem);
    //            bool_t add = FALSE;
    //            cassert_unref(type == HB_IT_LOGICAL, type);
    //            add = (bool_t)hb_itemGetL(ritem);
    //            hb_itemRelease(ritem);

    //            if (add == TRUE)
    //                arrst_append(area->records, (uint32_t)uiRecNo, uint32_t);
    //        }

    //        SELF_SKIP(area->area, 1);
    //        SELF_EOF(area->area, &fEof);
    //    }
    //}

    /* Restore database RECNO() */
    SELF_GOTO(area->area, ulCurRec);
}

/*---------------------------------------------------------------------------*/

static uint32_t i_frow_from_recno(GtNapFArea *area, const uint32_t recno)
{
    cassert_no_null(area);
    cassert(recno > 0);
    arrst_foreach_const(rec, area->records, uint32_t)
        if (*rec == recno)
            return rec_i;
    arrst_end();
    return UINT32_MAX;
}

/*---------------------------------------------------------------------------*/

static void i_farea_select_row(GtNapFArea *area)
{
    HB_ULONG ulCurRec;
    uint32_t sel_row;
    // TableView *view;

    cassert_no_null(area);
    // cassert_no_null(gtarea->gtobj);
    // cassert(gtarea->gtobj->type == ekOBJ_TABLEVIEW);
    // view = (TableView *)gtarea->gtobj->component;

    /* Current selected */
    SELF_RECNO(area->area, &ulCurRec);

    sel_row = i_frow_from_recno(area, (uint32_t)ulCurRec);

    /* In multisel table, the selected rows comes from  VN_Selecio */
    // if (gtarea->gtobj->multisel == TRUE)
    //{
    //     if (tableview_get_focus_row(view) == UINT32_MAX)
    //     {
    //         /* We use RECNO for focused row */
    //         if (sel_row != UINT32_MAX)
    //         {
    //             tableview_focus_row(view, sel_row, ekTOP);
    //         }
    //         else
    //         {
    //             uint32_t nrecs = arrst_size(gtarea->records, uint32_t);
    //             sel_row = tableview_get_focus_row(view);
    //             /* We move recno to current focused row */
    //             if (sel_row >= nrecs)
    //             {
    //                 sel_row = 0;
    //             }

    //            if (sel_row < nrecs)
    //            {
    //                uint32_t recno = *arrst_get_const(gtarea->records, sel_row, uint32_t);
    //                tableview_select(view, &sel_row, 1);
    //                SELF_GOTO(gtarea->area, recno);
    //            }
    //        }
    //    }
    //}
    // else
    {
        tableview_deselect_all(area->table);

        if (sel_row != UINT32_MAX)
        {
            tableview_select(area->table, &sel_row, 1);
            tableview_focus_row(area->table, sel_row, ekTOP);
        }
        /* RECNO() doesn't exists in view (perhaps is deleted) */
        else
        {
            uint32_t nrecs = arrst_size(area->records, uint32_t);
            sel_row = tableview_get_focus_row(area->table);
            /* We move recno to current focused row */
            if (sel_row < nrecs)
            {
                uint32_t recno = *arrst_get_const(area->records, sel_row, uint32_t);
                tableview_select(area->table, &sel_row, 1);
                SELF_GOTO(area->area, recno);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_map_bind_area_to_form(GtNapFArea *area)
{
    cassert_no_null(area);
    cassert(area->table == NULL);
    cassert_no_null(area->form);
    area->table = nform_get_tableview(area->form->form, tc(area->cellname));
    if (area->table != NULL)
    {
        tableview_OnData(area->table, listener(area, i_OnTableFAreaData, GtNapFArea));
        i_farea_refresh(area);
        tableview_update(area->table);
        i_farea_select_row(area);
    }
}

/*---------------------------------------------------------------------------*/

static GtNapFArea *i_create_farea(GtNapForm *form, AREA *area)
{
    GtNapFArea *farea = heap_new0(GtNapFArea);
    farea->form = form;
    farea->cellname = NULL;
    farea->table = NULL;
    farea->area = area;
    farea->cache_recno = UINT32_MAX;
    farea->records = arrst_create(uint32_t);
    farea->columns = arrst_create(GtNapFColumn);
    return farea;
}

/*---------------------------------------------------------------------------*/

static GtNapFDBConn *i_create_dbconn(GtNapForm *form, const char_t *cell)
{
    GtNapFDBConn *dbconn = heap_new0(GtNapFDBConn);
    dbconn->form = form;
    dbconn->cellname = str_c(cell);
    dbconn->table = NULL;
    dbconn->tdata = treest_create(GtNapFNode);
    dbconn->areas = arrst_create(GtNapFArea2);
    return dbconn;
}

/*---------------------------------------------------------------------------*/

static void i_map_bind_to_form(NForm *form, ArrSt(GtNapBind) *binds)
{
    arrst_foreach(bind, binds, GtNapBind)
        if (bind->value != NULL)
        {
            PHB_ITEM base = bind->value;

            if (HB_ITEM_TYPE(base) == HB_IT_BYREF)
                base = hb_itemUnRef(base);

            if (HB_ITEM_TYPE(base) == HB_IT_STRING)
            {
                const char_t *text = hb_itemGetCPtr(base);
                String *str = str_c(text);
                i_rtrim(tcc(str));
                nform_set_control_str(form, tc(bind->gui_id), tc(str));
                str_destroy(&str);
            }
            else if (HB_ITEM_TYPE(base) == HB_IT_LOGICAL)
            {
                HB_BOOL value = hb_itemGetL(base);
                nform_set_control_bool(form, tc(bind->gui_id), (bool_t)value);
            }
            else if (HB_ITEM_TYPE(base) == HB_IT_INTEGER)
            {
                long value = hb_itemGetNL(base);
                nform_set_control_int(form, tc(bind->gui_id), (int32_t)value);
            }
            else if (HB_ITEM_TYPE(base) == HB_IT_DOUBLE)
            {
                double value = hb_itemGetND(base);
                nform_set_control_real(form, tc(bind->gui_id), (real32_t)value);
            }
        }
        else if (bind->listener != NULL)
        {
            if (nform_set_listener(form, tc(bind->gui_id), bind->listener) == TRUE)
                bind->listener = NULL;
        }
    arrst_end()
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_bind(GtNapForm *form, HB_ITEM *cell_bind)
{
    HB_SIZE i, n = UINT32_MAX;
    cassert_no_null(form);
    cassert(HB_ITEM_TYPE(cell_bind) == HB_IT_ARRAY);
    n = hb_arrayLen(cell_bind);
    for (i = 1; i <= n; ++i)
    {
        PHB_ITEM bind_item = hb_arrayGetItemPtr(cell_bind, i);
        PHB_ITEM name_item = NULL;
        PHB_ITEM var_item = NULL;
        const char *gui_id = NULL;
        GtNapBind *bind = arrst_new0(form->binds, GtNapBind);
        cassert(HB_ITEM_TYPE(bind_item) == HB_IT_ARRAY);
        cassert(hb_arrayLen(bind_item) == 2);
        name_item = hb_arrayGetItemPtr(bind_item, 1);
        var_item = hb_arrayGetItemPtr(bind_item, 2);
        cassert(HB_ITEM_TYPE(name_item) == HB_IT_STRING);
        gui_id = hb_itemGetCPtr(name_item);
        bind->gui_id = str_c(cast_const(gui_id, char_t));
        bind->value = hb_itemNew(var_item);
    }

    i_map_bind_to_form(form->form, form->binds);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_bind_store(GtNapForm *form)
{
    cassert_no_null(form);
    arrst_foreach(bind, form->binds, GtNapBind)
        if (bind->value != NULL)
        {
            /* We only can save a value in HB_IT_BYREF types */
            if (HB_ITEM_TYPE(bind->value) == HB_IT_BYREF)
            {
                PHB_ITEM base = hb_itemUnRef(bind->value);
                if (HB_ITEM_TYPE(base) == HB_IT_STRING)
                {
                    const char_t *text = NULL;
                    if (nform_get_control_str(form->form, tc(bind->gui_id), &text) == TRUE)
                        hb_itemPutC(base, text);
                }
                else if (HB_ITEM_TYPE(base) == HB_IT_LOGICAL)
                {
                    bool_t value = FALSE;
                    if (nform_get_control_bool(form->form, tc(bind->gui_id), &value) == TRUE)
                        hb_itemPutL(base, value ? HB_TRUE : HB_FALSE);
                }
                else if (HB_ITEM_TYPE(base) == HB_IT_INTEGER)
                {
                    int32_t value = 0;
                    if (nform_get_control_int(form->form, tc(bind->gui_id), &value) == TRUE)
                        hb_itemPutNL(base, (long)value);
                }
                else if (HB_ITEM_TYPE(base) == HB_IT_DOUBLE)
                {
                    real32_t value = 0;
                    if (nform_get_control_real(form->form, tc(bind->gui_id), &value) == TRUE)
                        hb_itemPutND(base, (double)value);
                }
            }
        }
    arrst_end()
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_area_bind(GtNapForm *form, HB_ITEM *column_bind)
{
    AREA *area = NULL;
    HB_SIZE n = UINT32_MAX;
    cassert_no_null(form);
    cassert(form->area == NULL);
    cassert(HB_ITEM_TYPE(column_bind) == HB_IT_ARRAY);
    n = hb_arrayLen(column_bind);
    cassert(n > 1);

    area = cast(hb_rddGetCurrentWorkAreaPointer(), AREA);
    if (area != NULL)
    {
        HB_SIZE i;
        form->area = i_create_farea(form, area);

        for (i = 2; i <= n; ++i)
        {
            PHB_ITEM bind_item = hb_arrayGetItemPtr(column_bind, i);
            PHB_ITEM block_item = NULL;
            GtNapFColumn *column = NULL;
            /* At the moment, the column-bind item only has one member. The column block */
            cassert(HB_ITEM_TYPE(bind_item) == HB_IT_ARRAY);
            cassert(hb_arrayLen(bind_item) == 1);
            block_item = hb_arrayGetItemPtr(bind_item, 1);
            cassert(HB_ITEM_TYPE(block_item) == HB_IT_BLOCK);
            column = arrst_new0(form->area->columns, GtNapFColumn);
            column->block = block_item ? hb_itemNew(block_item) : NULL;
        }

        /* The first element in bind array is the tableView cell name */
        {
            PHB_ITEM name_item = hb_arrayGetItemPtr(column_bind, 1);
            const char *gui_id = NULL;
            cassert(HB_ITEM_TYPE(name_item) == HB_IT_STRING);
            gui_id = hb_itemGetCPtr(name_item);
            form->area->cellname = str_c(cast_const(gui_id, char_t));
            i_map_bind_area_to_form(form->area);
        }
    }
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_area_refresh(GtNapForm *form)
{
    cassert_no_null(form);
    cassert_no_null(form->area);
    i_farea_refresh(form->area);
    tableview_update(form->area->table);
    i_farea_select_row(form->area);
}

/*---------------------------------------------------------------------------*/

uint32_t hbnap_forms_area_recno(GtNapForm *form)
{
    const ArrSt(uint32_t) *sel = 0;
    cassert_no_null(form);
    cassert_no_null(form->area);
    sel = tableview_selected(form->area->table);
    if (arrst_size(sel, uint32_t) == 1)
    {
        uint32_t index = *arrst_first_const(sel, uint32_t);
        return *arrst_get(form->area->records, index, uint32_t);
    }
    else
    {
        return UINT32_MAX;
    }
}

/*---------------------------------------------------------------------------*/

static align_t i_hbalign(int hbalign)
{
    switch (hbalign)
    {
    case HBNAP_LEFT:
        return ekLEFT;
    case HBNAP_CENTER:
        return ekCENTER;
    case HBNAP_RIGHT:
        return ekRIGHT;
    default:
        cassert_default(hbalign);
    }

    return ekLEFT;
}

/*---------------------------------------------------------------------------*/

static void i_OnTreeFAreaData(GtNapFDBConn *dbconn, Event *e)
{
    uint32_t etype = event_type(e);
    cassert_no_null(dbconn);

    switch (etype)
    {

    case ekGUI_EVENT_TBL_NROOTS:
    {
        uint32_t *nroots = event_result(e, uint32_t);
        NodeSt(GtNapFNode) *root = treest_root_get(dbconn->tdata, GtNapFNode);
        *nroots = root ? treest_node_size(root, GtNapFNode) : 0;
        break;
    }

    case ekGUI_EVENT_TBL_NODEINFO:
    {
        const EvTbNode *node = event_params(e, EvTbNode);
        EvTbNodeInfo *info = event_result(e, EvTbNodeInfo);
        NodeSt(GtNapFNode) *parent = cast(node->parent, NodeSt(GtNapFNode));
        NodeSt(GtNapFNode) *child = NULL;
        GtNapFNode *data = NULL;

        if (parent == NULL)
            parent = treest_root_get(dbconn->tdata, GtNapFNode);

        child = treest_node_get(parent, node->child, GtNapFNode);
        data = treest_node_data(child, GtNapFNode);
        info->node = child;
        info->nchildren = treest_node_size(child, GtNapFNode);
        info->expanded = data->expanded;
        break;
    }

    case ekGUI_EVENT_TBL_EXPAND:
    {
        const EvTbExpand *p = event_params(e, EvTbExpand);
        NodeSt(GtNapFNode) *node = cast(p->node, NodeSt(GtNapFNode));
        GtNapFNode *data = treest_node_data(node, GtNapFNode);
        data->expanded = p->expanded;
        break;
    }

    case ekGUI_EVENT_TBL_CELL:
    {
        EvTbCell *cell = event_result(e, EvTbCell);
        const EvTbPos *pos = event_params(e, EvTbPos);
        bstd_sprintf(TEMP_BUFFER, sizeof(TEMP_BUFFER), "Data (%d, %d)", pos->col, pos->row);
        cell->text = TEMP_BUFFER;
        //cell->text = i_farea_eval_field(area, pos->col + 1, pos->row);
        break;
    }

    default:
        break;
    }
}

/*---------------------------------------------------------------------------*/

static bool_t i_items_equal(PHB_ITEM a, PHB_ITEM b)
{
    HB_TYPE t = HB_ITEM_TYPE(a);
    cassert(t == HB_ITEM_TYPE(b));

    if (t & HB_IT_NUMERIC)
        return (bool_t)(hb_itemGetND(a) == hb_itemGetND(b));
    if (t & HB_IT_STRING)
        return (bool_t)(hb_stricmp(hb_itemGetCPtr(a), hb_itemGetCPtr(b)) == 0);
    if (t & HB_IT_LOGICAL)
        return (bool_t)(hb_itemGetL(a) == hb_itemGetL(b));

    cassert(FALSE);
    return FALSE;
}

/*---------------------------------------------------------------------------*/

/*
static void i_dump_area_order(AREA *area, const char_t *label)
{
    DBORDERINFO info;
    memset(&info, 0, sizeof(info));
    info.itmResult = hb_itemNew(NULL);

    SELF_ORDINFO(area, DBOI_NUMBER, &info);
    int nOrder = hb_itemGetNI(info.itmResult);

    SELF_ORDINFO(area, DBOI_NAME, &info);
    const char *szName = hb_itemGetCPtr(info.itmResult);
    bstd_printf("%s → order=%d  tag='%s'\n", label, nOrder, szName);
    hb_itemRelease(info.itmResult);
} 
*/

/*---------------------------------------------------------------------------*/

/*
static void i_dump_area_tags(AREAP area, const char *label)
{
    DBORDERINFO info;
    int i, nTotal;

    memset(&info, 0, sizeof(info));
    info.itmResult = hb_itemNew(NULL);
    info.itmOrder  = hb_itemNew(NULL);

    SELF_ORDINFO(area, DBOI_ORDERCOUNT, &info);
    nTotal = hb_itemGetNI(info.itmResult);
    bstd_printf("%s: %d tag(s)\n", label, nTotal);

    for (i = 1; i <= nTotal; i++)
    {
        hb_itemPutNI(info.itmOrder, i);
        SELF_ORDINFO(area, DBOI_NAME, &info);
        bstd_printf("  [%d] '%s'\n", i, hb_itemGetCPtr(info.itmResult));
    }

    hb_itemRelease(info.itmResult);
    hb_itemRelease(info.itmOrder);
}
*/

/*---------------------------------------------------------------------------*/

static void i_build_children(GtNapFDBConn *dbconn, NodeSt(GtNapFNode) *parent, uint32_t level)
{
    GtNapFArea2 *parea = NULL;
    GtNapFArea2 *carea = NULL;
    GtNapFNode *pdata = NULL;    
    PHB_ITEM key = NULL;
    HB_BOOL found = HB_FALSE;

    cassert_no_null(dbconn);
    parea = arrst_get(dbconn->areas, level, GtNapFArea2);
    carea = arrst_get(dbconn->areas, level + 1, GtNapFArea2);
    pdata = treest_node_data(parent, GtNapFNode);

    /* Position parent at its record so relfrom block reads the right value */
    SELF_GOTO(parea->area, pdata->recno);

    /* Evaluate parent relfrom block → seek key for child area */
    key = hb_itemDo(parea->relfrom, 0);

    /* 
     * SEEK in child area (order already set by Harbour SET ORDER TO TAG). 
     * Hard seek (HB_FALSE remains in EOF if it not found) 
     * Find last (HB_FALSE find the first record match)
     */
    /*
    i_dump_area_tags(carea->area, "INVOICES");
    i_dump_area_order(carea->area, "Label");
    */
    SELF_SEEK(carea->area, HB_FALSE, key, HB_FALSE);

    SELF_FOUND(carea->area, &found);
    if (found == HB_TRUE)
    {
        HB_BOOL eof = HB_FALSE;
        uint32_t nareas = arrst_size(dbconn->areas, GtNapFArea2);
        SELF_EOF(carea->area, &eof);
        while (eof == HB_FALSE)
        {
            /* Child key must still match parent key */
            PHB_ITEM ckey = hb_itemDo(parea->relto, 0);
            bool_t equ = i_items_equal(key, ckey);
            hb_itemRelease(ckey);

            if (equ == TRUE)
            {
                /* Add child node to tree */
                NodeSt(GtNapFNode) *child = treest_node_insert(parent, UINT32_MAX, GtNapFNode);
                GtNapFNode *cdata = treest_node_data(child, GtNapFNode);
                HB_ULONG crecno = 0;

                SELF_RECNO(carea->area, &crecno);
                cdata->area = carea;
                cdata->recno = crecno;
                cdata->expanded = FALSE;

                /* Recurse for grandchildren if more levels exist */
                if (level + 2 < nareas)
                    i_build_children(dbconn, child, level + 1);

                /* Restore child cursor after recursion, then advance */
                SELF_GOTO(carea->area, crecno);
                SELF_SKIP(carea->area, 1);
                SELF_EOF(carea->area, &eof);
            }
            else
            {
                break;
            }
        }
    }

    hb_itemRelease(key);
}

/*---------------------------------------------------------------------------*/

static void i_dbconn_refresh(GtNapFDBConn *dbconn)
{
    uint32_t nareas = 0;
    NodeSt(GtNapFNode) *root = NULL;
    GtNapFNode *vdata = NULL;
    GtNapFArea2 *area = NULL;
    HB_BOOL eof = HB_FALSE;

    cassert_no_null(dbconn);
    treest_clear(dbconn->tdata, i_remove_fnode, GtNapFNode);
    nareas = arrst_size(dbconn->areas, GtNapFArea2);
    cassert(nareas > 0);

    /* Virtual root: invisible container for all top-level nodes */
    root = treest_root_new(dbconn->tdata, GtNapFNode);
    vdata = treest_node_data(root, GtNapFNode);
    vdata->area = NULL;
    vdata->recno = UINT32_MAX;
    vdata->expanded = TRUE;

    /* Navigate root area and build one top-level node per record */
    area = arrst_get(dbconn->areas, 0, GtNapFArea2);
    SELF_GOTOP(area->area);
    SELF_EOF(area->area, &eof);

    while (eof == HB_FALSE)
    {
        HB_ULONG recno = 0;
        NodeSt(GtNapFNode) *node = NULL;
        GtNapFNode *data = NULL;

        SELF_RECNO(area->area, &recno);
        node = treest_node_insert(root, UINT32_MAX, GtNapFNode);
        data = treest_node_data(node, GtNapFNode);
        data->area = area;
        data->recno = recno;
        data->expanded = FALSE;

        if (nareas > 1)
            i_build_children(dbconn, node, 0);

        /* Restore root cursor after recursion, then advance */
        SELF_GOTO(area->area, recno);
        SELF_SKIP(area->area, 1);
        SELF_EOF(area->area, &eof);
    }
}

/*---------------------------------------------------------------------------*/

static void i_map_dbconn_to_form(GtNapFDBConn *dbconn)
{
    cassert_no_null(dbconn);
    cassert(dbconn->table == NULL);
    cassert_no_null(dbconn->form);
    dbconn->table = nform_get_tableview(dbconn->form->form, tc(dbconn->cellname));
    if (dbconn->table != NULL)
    {
        tableview_tree(dbconn->table, 0);
        tableview_OnData(dbconn->table, listener(dbconn, i_OnTreeFAreaData, GtNapFDBConn));
        i_dbconn_refresh(dbconn);
        tableview_update(dbconn->table);
    }
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_tree_bind(GtNapForm *form, const char_t *cell, HB_ITEM *areas, HB_ITEM *relations, HB_ITEM *columns)
{
    HB_SIZE nA = UINT32_MAX;
    HB_SIZE nR = UINT32_MAX;
    HB_SIZE nB = UINT32_MAX;
    HB_SIZE i = 0;
    cassert_no_null(form);
    cassert(form->area == NULL);
    cassert(form->dbconn == NULL);
    cassert(HB_ITEM_TYPE(areas) == HB_IT_ARRAY);
    cassert(HB_ITEM_TYPE(relations) == HB_IT_ARRAY);
    cassert(HB_ITEM_TYPE(columns) == HB_IT_ARRAY);
    form->dbconn = i_create_dbconn(form, cell);
    nA = hb_arrayLen(areas);
    nR = hb_arrayLen(relations);
    nB = hb_arrayLen(columns);
    cassert(nA > 1);
    cassert(nR == nA - 1);
    cassert(nB == nA);

    for (i = 1; i <= nA; ++i)
    {
        GtNapFArea2 *area = arrst_new0(form->dbconn->areas, GtNapFArea2);

        /* Database area */
        {
            PHB_ITEM area_item = hb_arrayGetItemPtr(areas, i);
            const char *area_id = NULL;
            int iArea = 0;
            HB_ERRCODE hbres;
            cassert(HB_ITEM_TYPE(area_item) == HB_IT_STRING);
            area_id = hb_itemGetCPtr(area_item);
            hbres = hb_rddGetAliasNumber(area_id, &iArea);
            cassert_unref(hbres == HB_SUCCESS, hbres);
            area->area = cast(hb_rddGetWorkAreaPointer(iArea), AREA);
            cassert_no_null(area->area);
        }

        /* Area->Columns data mappings */
        {
            PHB_ITEM bind_item = hb_arrayGetItemPtr(columns, i);
            HB_SIZE j, nCols = UINT32_MAX;
            area->columns = arrst_create(GtNapFColumn);
            cassert(HB_ITEM_TYPE(bind_item) == HB_IT_ARRAY);
            nCols = hb_arrayLen(bind_item);
            for (j = 1; j <= nCols; ++j)
            {
                GtNapFColumn *col = arrst_new0(area->columns, GtNapFColumn);
                PHB_ITEM col_item = hb_arrayGetItemPtr(bind_item, j);
                PHB_ITEM align_item = NULL;
                PHB_ITEM block_item = NULL;                
                cassert(HB_ITEM_TYPE(col_item) == HB_IT_ARRAY);
                cassert(hb_arrayLen(col_item) == 2);
                align_item = hb_arrayGetItemPtr(col_item, 1);
                block_item = hb_arrayGetItemPtr(col_item, 2);                
                cassert(HB_ITEM_TYPE(align_item) == HB_IT_INTEGER);
                cassert(HB_ITEM_TYPE(block_item) == HB_IT_BLOCK);
                col->align = i_hbalign(hb_itemGetNI(align_item));
                col->block = block_item ? hb_itemNew(block_item) : NULL;
            }
        }

        /* Relation with next area */
        if (i < nA)
        {
            PHB_ITEM rel_item = hb_arrayGetItemPtr(relations, i);
            PHB_ITEM from_item = NULL;
            PHB_ITEM to_item = NULL;                
            cassert(HB_ITEM_TYPE(rel_item) == HB_IT_ARRAY);
            cassert(hb_arrayLen(rel_item) == 2);
            from_item = hb_arrayGetItemPtr(rel_item, 1);
            to_item = hb_arrayGetItemPtr(rel_item, 2);                
            cassert_no_null(from_item);
            cassert_no_null(to_item);
            cassert(HB_ITEM_TYPE(from_item) == HB_IT_BLOCK);
            cassert(HB_ITEM_TYPE(to_item) == HB_IT_BLOCK);
            area->relfrom = hb_itemNew(from_item);
            area->relto = hb_itemNew(to_item);
        }
        else
        {
            area->relfrom = NULL;
            area->relto = NULL;
        }        
    }

    i_map_dbconn_to_form(form->dbconn);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_item_list(GtNapForm *form, const char_t *cell, HB_ITEM *items)
{
    HB_SIZE i, n = UINT32_MAX;
    cassert_no_null(form);
    cassert(HB_ITEM_TYPE(items) == HB_IT_ARRAY);
    nform_clear_control_list(form->form, cell);
    n = hb_arrayLen(items);
    for (i = 1; i <= n; ++i)
    {
        PHB_ITEM hitem = hb_arrayGetItemPtr(items, i);
        const char_t *text = NULL;
        cassert(HB_ITEM_TYPE(hitem) == HB_IT_STRING);
        text = hb_itemGetCPtr(hitem);
        nform_add_control_item(form->form, cell, text);
    }
}

/*---------------------------------------------------------------------------*/

static Listener *i_gtnap_form_listener(HB_ITEM *block, GtNapForm *form, FPtr_gtnap_callback func_callback)
{
    GtNapCallback *callback = heap_new0(GtNapCallback);
    cassert_no_null(form);
    callback->block = block ? hb_itemNew(block) : NULL;
    callback->form = form;
    callback->key = INT32_MAX;
    callback->autoclose_id = UINT32_MAX;
    arrpt_append(form->callbacks, callback, GtNapCallback);
    return listener(callback, func_callback, GtNapCallback);
}

/*---------------------------------------------------------------------------*/

static void i_OnFormButtonClick(GtNapCallback *callback, Event *e)
{
    cassert_no_null(callback);
    cassert_no_null(callback->form);
    unref(e);
    if (callback->block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(callback->block, 0);
        hb_itemRelease(ritem);
    }
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_OnClick(GtNapForm *form, const char_t *cell, HB_ITEM *click_block)
{
    Listener *listener = i_gtnap_form_listener(click_block, form, i_OnFormButtonClick);
    cassert_no_null(form);
    nform_set_listener(form->form, cell, listener);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_maximize(GtNapForm *form)
{
    cassert_no_null(form);
    window_maximize(form->window);
}

/*---------------------------------------------------------------------------*/

static void i_OnFormClose(GtNapForm *form, Event *e)
{
    cassert_no_null(form);
    if (form->OnClose_block != NULL)
    {
        PHB_ITEM ritem = hb_itemDo(form->OnClose_block, 0);
        if (HB_ITEM_TYPE(ritem) == HB_IT_LOGICAL)
        {
            bool_t *r = event_result(e, bool_t);
            *r = (bool_t)hb_itemGetL(ritem);
        }
        hb_itemRelease(ritem);
    }
}

/*---------------------------------------------------------------------------*/

static V2Df i_center_window(const Window *parent, Window *window)
{
    V2Df p1 = window_get_origin(parent);
    S2Df s1 = window_get_size(parent);
    S2Df s2 = window_get_size(window);
    V2Df p2;
    p2.x = p1.x + (s1.width - s2.width) / 2;
    p2.y = p1.y + (s1.height - s2.height) / 2;
    return p2;
}

/*---------------------------------------------------------------------------*/

static void i_form_frame(GtNapForm *form, Window *parent)
{
    S2Df size;
    cassert_no_null(form);
    size.width = i_read_prop_r32(tc(form->nameid), i_WIDTH_PROP);
    size.height = i_read_prop_r32(tc(form->nameid), i_HEIGHT_PROP);
    if ((size.width == i_MAXIMIZED_SIZE || size.height == i_MAXIMIZED_SIZE) && form->is_resizable == TRUE)
    {
        window_maximize(form->window);
    }
    else
    {
        V2Df pos;
        pos.x = i_read_prop_r32(tc(form->nameid), i_XPOS_PROP);
        pos.y = i_read_prop_r32(tc(form->nameid), i_YPOS_PROP);

        if ((pos.x == i_UNDEF_R32 || pos.y == i_UNDEF_R32) && parent != NULL)
            pos = i_center_window(parent, form->window);

        if ((size.width != i_UNDEF_R32 && size.height != i_UNDEF_R32) && form->is_resizable == TRUE)
            window_client_size(form->window, size);

        window_origin(form->window, pos);
    }
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_show(GtNapForm *form, HB_ITEM *onclose_block)
{
    cassert_no_null(form);
    if (form->OnClose_block != NULL)
        hb_itemRelease(form->OnClose_block);

    form->OnClose_block = hb_itemNew(onclose_block);
    window_OnClose(form->window, listener(form, i_OnFormClose, GtNapForm));
    window_update(form->window);
    i_form_frame(form, NULL);
    window_show(form->window);
}

/*---------------------------------------------------------------------------*/

static uint32_t i_forms_modal(GtNapForm *form, Window *parent)
{
    cassert_no_null(form);
    window_update(form->window);
    i_form_frame(form, parent);
    form->modal_ret = window_modal(form->window, parent);
    return form->modal_ret;
}

/*---------------------------------------------------------------------------*/

uint32_t hbnap_forms_modal(GtNapForm *form, GtNapForm *parent)
{
    cassert_no_null(parent);
    return i_forms_modal(form, parent->window);
}

/*---------------------------------------------------------------------------*/

uint32_t hbnap_forms_modal_gtnap(GtNapForm *form)
{
    GtNapWindow *gtwin = i_current_gtwin(GTNAP_GLOBAL);
    cassert_no_null(gtwin);
    return i_forms_modal(form, gtwin->window);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_stop_modal(GtNapForm *form, const uint32_t value)
{
    cassert_no_null(form);
    window_stop_modal(form->window, value);
}

/*---------------------------------------------------------------------------*/

R2Df hbnap_forms_control_frame(GtNapForm *form, const char_t *cell)
{
    cassert_no_null(form);
    return nform_get_control_frame(form->form, cell, form->window);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_update(GtNapForm *form)
{
    cassert_no_null(form);
    window_update(form->window);
}

/*---------------------------------------------------------------------------*/

typedef struct i_mainitem_t MainItem;
typedef struct i_maindata_t MainData;

struct i_mainitem_t
{
    String *title;
    String *more;
    color_t back;
    Image *icon;
    bool_t isnew;
    HB_ITEM *OnClick;
    real32_t pos_x;
    real32_t pos_y;
    real32_t icon_scale;
};

struct i_maindata_t
{
    String *title;
    Image *logo;
    real32_t title_width;
    real32_t title_height;
    real32_t logo_width;
    real32_t logo_height;
    Font *title_font;
    Font *item_font;
    Font *item_sfont;
    uint32_t hover_item;
    ArrSt(MainItem) *items;
};

/*---------------------------------------------------------------------------*/

DeclSt(MainItem);
static const real32_t i_ITEM_WIDTH = 260;
static const real32_t i_ITEM_HEIGHT = 160;
static const real32_t i_ITEM_SEP = 16;
static const real32_t i_ITEM_CIRCLE_RADIX = 50;
static const real32_t i_ITEM_CIRCLE_OFFSET = 14;
static const real32_t i_ITEM_ICON_SIZE = 64;
static const real32_t i_ITEM_TEXT_PADDING_BOTTOM = 12;
static const real32_t i_IMAGE_PADDING_TOP = 30;
static const real32_t i_IMAGE_PADDING_BOTTOM = 36;
static const real32_t i_IMAGE_PADDING_RIGHT = 70;
static const real32_t i_TITLE_PADDING_LEFT = 80;

/*---------------------------------------------------------------------------*/

static void i_remove_mainitem(MainItem *item)
{
    str_destroy(&item->title);
    str_destroy(&item->more);
    ptr_destopt(image_destroy, &item->icon, Image);
    if (item->OnClick != NULL)
    {
        hb_itemRelease(item->OnClick);
        item->OnClick = NULL;
    }
}

/*---------------------------------------------------------------------------*/

static void i_destroy_maindata(MainData **data)
{
    cassert_no_null(data);
    cassert_no_null(*data);
    str_destopt(&(*data)->title);
    font_destroy(&(*data)->title_font);
    font_destroy(&(*data)->item_font);
    font_destroy(&(*data)->item_sfont);
    ptr_destopt(image_destroy, &(*data)->logo, Image);
    arrst_destroy(&(*data)->items, i_remove_mainitem, MainItem);
    heap_delete(data, MainData);
}

/*---------------------------------------------------------------------------*/

static void i_draw_mainitem(DCtx *ctx, const MainItem *item, const Font *font, const Font *sfont, const T2Df *t2d)
{
    color_t color = kCOLOR_WHITE;
    cassert_no_null(item);
    draw_matrixf(ctx, t2d);
    draw_fill_color(ctx, item->back);
    draw_line_color(ctx, color);
    draw_text_color(ctx, color);
    draw_line_width(ctx, 3);
    draw_rect(ctx, ekFILL, 0, 0, i_ITEM_WIDTH, i_ITEM_HEIGHT);
    draw_circle(ctx, ekSTROKE, i_ITEM_WIDTH / 2, (i_ITEM_HEIGHT / 2) - i_ITEM_CIRCLE_OFFSET, i_ITEM_CIRCLE_RADIX);

    if (item->icon != NULL)
    {
        T2Df scale;
        t2d_movef(&scale, t2d, (i_ITEM_WIDTH - i_ITEM_ICON_SIZE) / 2, (i_ITEM_HEIGHT - i_ITEM_ICON_SIZE) / 2 - i_ITEM_CIRCLE_OFFSET);
        t2d_scalef(&scale, &scale, item->icon_scale, item->icon_scale);
        draw_matrixf(ctx, &scale);
        draw_image(ctx, item->icon, 0, 0);
        draw_matrixf(ctx, t2d);
    }

    if (str_empty(item->title) == FALSE)
    {
        draw_font(ctx, font);
        draw_text_align(ctx, ekCENTER, ekBOTTOM);
        draw_text(ctx, tc(item->title), i_ITEM_WIDTH / 2, i_ITEM_HEIGHT - i_ITEM_TEXT_PADDING_BOTTOM);
    }

    if (str_empty(item->more) == FALSE)
    {
        draw_font(ctx, sfont);
        draw_text_align(ctx, ekRIGHT, ekTOP);
        draw_text(ctx, tc(item->more), i_ITEM_WIDTH, 0);
    }
}

/*---------------------------------------------------------------------------*/

static T2Df i_item_transform(const MainItem *item, const bool_t hover)
{
    T2Df t2d;
    t2d_movef(&t2d, kT2D_IDENTf, item->pos_x, item->pos_y);

    if (hover == TRUE)
    {
        t2d_movef(&t2d, &t2d, i_ITEM_WIDTH / 2, i_ITEM_HEIGHT / 2);
        t2d_scalef(&t2d, &t2d, 1.05f, 1.05f);
        t2d_movef(&t2d, &t2d, - i_ITEM_WIDTH / 2, - i_ITEM_HEIGHT / 2);
    }

    return t2d;
}

/*---------------------------------------------------------------------------*/

static void i_OnDrawMainView(GtNapForm *form, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    View *view = event_sender(e, View);
    MainData *data = view_get_data(view, MainData);
    real32_t logo_x = 0;
    bool_t draw_title = FALSE;
    color_t text_color;
    cassert_no_null(data);
    unref(form);

    if (gui_dark_mode() == TRUE)
        text_color = color_rgb(255, 255, 255);
    else
        text_color = color_rgb(99, 99, 99);

    logo_x = p->width - data->logo_width - i_IMAGE_PADDING_RIGHT;

    if (str_empty(data->title) == FALSE)
    {
        draw_text_color(p->ctx, text_color);
        draw_line_color(p->ctx, text_color);
        draw_font(p->ctx, data->title_font);

        if (data->title_width == 0)
            draw_text_extents(p->ctx, tc(data->title), -1, &data->title_width, &data->title_height);

        /* Avoid title and logo overlap */
        if (data->title_width + i_TITLE_PADDING_LEFT < logo_x)
            draw_title = TRUE;

        if (draw_title == TRUE)
        {
            real32_t line_y = i_IMAGE_PADDING_TOP + data->logo_height;
            draw_text(p->ctx, tc(data->title), i_TITLE_PADDING_LEFT, i_IMAGE_PADDING_TOP + data->logo_height - data->title_height);
            draw_line(p->ctx, i_TITLE_PADDING_LEFT, line_y, i_TITLE_PADDING_LEFT + data->title_width + data->logo_width, line_y);
        }
    }

    if (data->logo != NULL)
    {
        /* If no title is drawn, the logo is centered */
        if (draw_title == FALSE)
            logo_x = (p->width - data->logo_width) / 2;
        draw_image(p->ctx, data->logo, logo_x, i_IMAGE_PADDING_TOP);
    }

    arrst_foreach_const(item, data->items, MainItem)
        T2Df t2d = i_item_transform(item, (bool_t)(item_i == data->hover_item));
        i_draw_mainitem(p->ctx, item, data->item_font, data->item_sfont, &t2d);
    arrst_end()
}

/*---------------------------------------------------------------------------*/

static void i_mainitems_locations(View *view, const real32_t width)
{
    MainData *data = view_get_data(view, MainData);
    real32_t content_width = 0;
    real32_t content_height = 0;
    uint32_t n = 0, ncols, nrows;
    cassert_no_null(data);
    n = arrst_size(data->items, MainItem);

    /* Compute the number of columns */
    for (ncols = 4; ncols >= 1; --ncols)
    {
        content_width = i_ITEM_WIDTH * ncols + i_ITEM_SEP * (ncols - 1);
        if (content_width < width || ncols == 1)
            break;
    }

    cassert(ncols > 0);
    nrows = (n / ncols) + ((n % ncols) ? 1 : 0);
    content_height = i_IMAGE_PADDING_TOP + i_ITEM_HEIGHT * nrows + i_ITEM_SEP * (nrows - 1);
    content_height += data->logo_height;

    if (data->logo_height > 0)
        content_height += i_IMAGE_PADDING_BOTTOM;

    content_height += i_IMAGE_PADDING_BOTTOM;

    /* Compute the items position */
    arrst_foreach(item, data->items, MainItem)
        uint32_t i = item_i % ncols;
        uint32_t j = item_i / ncols;
        item->pos_x = ((width - content_width) / 2) + i * (i_ITEM_WIDTH + i_ITEM_SEP);
        item->pos_y = i_IMAGE_PADDING_TOP + data->logo_height + j * (i_ITEM_HEIGHT + i_ITEM_SEP);
        if (data->logo_height > 0)
            item->pos_y += i_IMAGE_PADDING_BOTTOM;
    arrst_end()

    view_content_size(view, s2df(content_width, content_height), s2df(10, 10));
}

/*---------------------------------------------------------------------------*/

static void i_OnSizeMainView(GtNapForm *form, Event *e)
{
    const EvSize *p = event_params(e, EvSize);
    View *view = event_sender(e, View);
    i_mainitems_locations(view, p->width);
    unref(form);
}

/*---------------------------------------------------------------------------*/

static void i_OnMoveMainView(GtNapForm *form, Event *e)
{
    const EvMouse *p = event_params(e, EvMouse);
    View *view = event_sender(e, View);
    MainData *data = view_get_data(view, MainData);
    cassert_no_null(data);
    data->hover_item = UINT32_MAX;
    arrst_foreach_const(item, data->items, MainItem)
        R2Df r2d = r2df(item->pos_x, item->pos_y, i_ITEM_WIDTH, i_ITEM_HEIGHT);
        if (r2d_containsf(&r2d, p->x, p->y) == TRUE)
        {
            data->hover_item = item_i;
            break;
        }
    arrst_end()
    view_update(view);
    unref(form);
}

/*---------------------------------------------------------------------------*/

static void i_OnClickMainView(GtNapForm *form, Event *e)
{
    View *view = event_sender(e, View);
    MainData *data = view_get_data(view, MainData);
    cassert_no_null(data);
    if (data->hover_item != UINT32_MAX)
    {
        const MainItem *item = arrst_get_const(data->items, data->hover_item, MainItem);
        PHB_ITEM ritem = hb_itemDo(item->OnClick, 0);
        hb_itemRelease(ritem);
    }
    unref(form);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_main_cover(GtNapForm *form, const char_t *canvas_cell, const char_t *title, const char_t *logo_path, HB_ITEM *cover_items)
{
    View *view = NULL;
    S2Df view_size;
    MainData *data = heap_new0(MainData);
    HB_SIZE i, n;
    cassert_no_null(form);
    view = nform_get_view(form->form, canvas_cell);
    cassert_no_null(view);
    cassert_no_null(cover_items);
    cassert(HB_ITEM_TYPE(cover_items) == HB_IT_ARRAY);
    data->title = str_c(title);
    data->title_font = font_system(42, 0);
    data->item_font = font_system(20, ekFBOLD);
    data->item_sfont = font_system(14, ekFBOLD);
    data->logo = image_from_file(logo_path, NULL);

    if (data->logo != NULL)
    {
        data->logo_width = (real32_t)image_width(data->logo);
        data->logo_height = (real32_t)image_height(data->logo);
    }

    data->items = arrst_create(MainItem);
    data->hover_item = UINT32_MAX;

    n = hb_arrayLen(cover_items);
    for (i = 1; i <= n; ++i)
    {
        MainItem *item = arrst_new0(data->items, MainItem);
        HB_ITEM *citem = hb_arrayGetItemPtr(cover_items, i);
        HB_ITEM *ititle = NULL;
        HB_ITEM *icon = NULL;
        HB_ITEM *color = NULL;
        HB_ITEM *more = NULL;
        HB_ITEM *isnew = NULL;
        HB_ITEM *block = NULL;
        cassert_no_null(citem);
        cassert(HB_ITEM_TYPE(citem) == HB_IT_ARRAY);
        cassert(hb_arrayLen(citem) == 6);
        ititle = hb_arrayGetItemPtr(citem, 1);
        icon = hb_arrayGetItemPtr(citem, 2);
        color = hb_arrayGetItemPtr(citem, 3);
        more = hb_arrayGetItemPtr(citem, 4);
        isnew = hb_arrayGetItemPtr(citem, 5);
        block = hb_arrayGetItemPtr(citem, 6);
        cassert(HB_ITEM_TYPE(ititle) == HB_IT_STRING);
        cassert(HB_ITEM_TYPE(icon) == HB_IT_STRING);
        cassert(HB_ITEM_TYPE(color) == HB_IT_STRING);
        cassert(HB_ITEM_TYPE(more) == HB_IT_STRING);
        cassert(HB_ITEM_TYPE(isnew) == HB_IT_LOGICAL);
        cassert(HB_ITEM_TYPE(block) == HB_IT_BLOCK);
        item->title = str_c(hb_itemGetCPtr(ititle));
        item->icon = image_from_file(hb_itemGetCPtr(icon), NULL);
        item->back = color_html(hb_itemGetCPtr(color));
        item->more = str_c(hb_itemGetCPtr(more));
        item->isnew = (bool_t)hb_itemGetL(isnew);
        item->OnClick = hb_itemNew(block);

        if (item->icon != NULL)
        {
            real32_t width = (real32_t)image_width(item->icon);
            item->icon_scale = i_ITEM_ICON_SIZE / width;
        }
    }

    view_data(view, &data, i_destroy_maindata, MainData);
    view_OnDraw(view, listener(form, i_OnDrawMainView, GtNapForm));
    view_OnSize(view, listener(form, i_OnSizeMainView, GtNapForm));
    view_OnMove(view, listener(form, i_OnMoveMainView, GtNapForm));
    view_OnClick(view, listener(form, i_OnClickMainView, GtNapForm));
    view_get_size(view, &view_size);
    i_mainitems_locations(view, view_size.width);
}

/*---------------------------------------------------------------------------*/

GtNapMenu *hbnap_menu_create(void)
{
    Menu *menu = menu_create();
    return cast(menu, GtNapMenu);
}

/*---------------------------------------------------------------------------*/

static void i_remove_menu_callbacks(Menu *menu);

/*---------------------------------------------------------------------------*/

static void i_remove_item_callbacks(MenuItem *item)
{
    Menu *submenu = menuitem_get_submenu(item);
    uint32_t callback_id = UINT32_MAX;

    arrpt_foreach(callback, GTNAP_GLOBAL->menu_callbacks, GtNapCallback)
        if (callback->menuitem == cast(item, GtNapMenuItem))
        {
            callback_id = callback_i;
            break;
        }
    arrpt_end()

    if (callback_id != UINT32_MAX)
        arrpt_delete(GTNAP_GLOBAL->menu_callbacks, callback_id, i_destroy_callback, GtNapCallback);

    if (submenu != NULL)
        i_remove_menu_callbacks(submenu);
}

/*---------------------------------------------------------------------------*/

static void i_remove_menu_callbacks(Menu *menu)
{
    uint32_t i, n = menu_count(menu);
    for (i = 0; i < n; ++i)
    {
        MenuItem *item = menu_get_item(menu, i);
        i_remove_item_callbacks(item);
    }
}

/*---------------------------------------------------------------------------*/

void hbnap_menu_destroy(GtNapMenu *menu)
{
    i_remove_menu_callbacks(cast(menu, Menu));
    menu_destroy(dcast(&menu, Menu));
}

/*---------------------------------------------------------------------------*/

void hbnap_menu_add_item(GtNapMenu *menu, GtNapMenuItem *item)
{
    menu_add_item(cast(menu, Menu), cast(item, MenuItem));
}

/*---------------------------------------------------------------------------*/

void hbnap_menu_ins_item(GtNapMenu *menu, const uint32_t pos, GtNapMenuItem *item)
{
    menu_ins_item(cast(menu, Menu), pos, cast(item, MenuItem));
}

/*---------------------------------------------------------------------------*/

void hbnap_menu_del_item(GtNapMenu *menu, const uint32_t pos)
{
    MenuItem *item = menu_get_item(cast(menu, Menu), pos);
    i_remove_item_callbacks(item);
    menu_del_item(cast(menu, Menu), pos);
}

/*---------------------------------------------------------------------------*/

uint32_t hbnap_menu_count(const GtNapMenu *menu)
{
    return menu_count(cast(menu, Menu));
}

/*---------------------------------------------------------------------------*/

GtNapMenuItem *hbnap_menu_get_item(GtNapMenu *menu, const uint32_t index)
{
    MenuItem *item = menu_get_item(cast(menu, Menu), index);
    return cast(item, GtNapMenuItem);
}

/*---------------------------------------------------------------------------*/

void hbnap_menubar(GtNapMenu *menu, GtNapForm *form)
{
    cassert_no_null(form);
    osapp_menubar(cast(menu, Menu), form->window);
}

/*---------------------------------------------------------------------------*/

bool_t hbnap_is_menubar(const GtNapMenu *menu)
{
    return menu_is_menubar(cast_const(menu, Menu));
}

/*---------------------------------------------------------------------------*/

void hbnap_menu_popup(GtNapMenu *menu, GtNapForm *form, const int32_t x, const int32_t y)
{
    cassert_no_null(form);
    menu_launch(cast(menu, Menu), form->window, v2df((real32_t)x, (real32_t)y));
}

/*---------------------------------------------------------------------------*/

static void i_OnMenuClick(GtNapCallback *callback, Event *e)
{
    cassert_no_null(callback);
    unref(e);
    if (callback->block != NULL)
    {
        /* The menuitem itself will be allways the first param in click callback */
        PHB_ITEM pItem = hb_itemPutPtr(NULL, callback->menuitem);
        PHB_ITEM ritem = hb_itemDo(callback->block, 1, pItem);
        hb_itemRelease(pItem);
        hb_itemRelease(ritem);
    }
}

/*---------------------------------------------------------------------------*/

static Listener *i_gtnap_menu_listener(HB_ITEM *block, GtNapMenuItem *item)
{
    GtNapCallback *callback = heap_new0(GtNapCallback);
    cassert_no_null(item);
    callback->block = block ? hb_itemNew(block) : NULL;
    callback->menuitem = item;
    callback->key = INT32_MAX;
    callback->autoclose_id = UINT32_MAX;
    arrpt_append(GTNAP_GLOBAL->menu_callbacks, callback, GtNapCallback);
    return listener(callback, i_OnMenuClick, GtNapCallback);
}

/*---------------------------------------------------------------------------*/

GtNapMenuItem *hbnap_menuitem_create(const char_t *text, const char_t *icon_path, HB_ITEM *click_block)
{
    MenuItem *item = menuitem_create();
    menuitem_text(item, text);

    if (str_empty_c(icon_path) == FALSE)
    {
        Image *image = image_from_file(icon_path, NULL);
        if (image != NULL)
        {
            menuitem_image(item, image);
            image_destroy(&image);
        }
    }

    {
        Listener *listener = i_gtnap_menu_listener(click_block, cast(item, GtNapMenuItem));
        menuitem_OnClick(item, listener);
    }

    return cast(item, GtNapMenuItem);
}

/*---------------------------------------------------------------------------*/

GtNapMenuItem *hbnap_menuitem_separator(void)
{
    MenuItem *item = menuitem_separator();
    return cast(item, GtNapMenuItem);
}

/*---------------------------------------------------------------------------*/

void hbnap_menuitem_submenu(GtNapMenuItem *item, GtNapMenu *submenu)
{
    menuitem_submenu(cast(item, MenuItem), dcast(&submenu, Menu));
}

/*---------------------------------------------------------------------------*/

const char_t *hbnap_menuitem_get_text(const GtNapMenuItem *item)
{
    return menuitem_get_text(cast_const(item, MenuItem));
}

/*---------------------------------------------------------------------------*/

GtNapMenu *hbnap_menuitem_get_submenu(GtNapMenuItem *item)
{
    Menu *menu = menuitem_get_submenu(cast(item, MenuItem));
    return cast(menu, GtNapMenu);
}

/*---------------------------------------------------------------------------*/

String *hb_block_to_utf8(HB_ITEM *item)
{
    String *str = NULL;

    if (HB_ITEM_TYPE(item) == HB_IT_STRING)
    {
        str = i_item_to_utf8_string(item);
    }
    else if (HB_ITEM_TYPE(item) == HB_IT_BLOCK)
    {
        PHB_ITEM ritem = hb_itemDo(item, 0);
        str = i_item_to_utf8_string(ritem);
        hb_itemRelease(ritem);
    }
    else
    {
        cassert_msg(FALSE, "Unknown block type");
        str = str_c("");
    }

    return str;
}

/*---------------------------------------------------------------------------*/

static int s_GtId;
static HB_GT_FUNCS SuperTable;
#define HB_GTSUPER (&SuperTable)
#define HB_GTID_PTR (&s_GtId)

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_Lock(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Lock");
    return TRUE;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Unlock(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Unlock");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Init(PHB_GT pGT, HB_FHANDLE hFilenoStdin, HB_FHANDLE hFilenoStdout, HB_FHANDLE hFilenoStderr)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(hFilenoStdin);
    HB_SYMBOL_UNUSED(hFilenoStdout);
    HB_SYMBOL_UNUSED(hFilenoStderr);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Init");

    /*
   //HB_GTSUPER_INIT( pGT, hFilenoStdin, hFilenoStdout, hFilenoStderr );
   //HB_GTSELF_RESIZE( pGT, 35, 110);
   //HB_GTSELF_SETFLAG( pGT, HB_GTI_REDRAWMAX, 1 );
   //HB_GTSELF_SEMICOLD( pGT );
   */
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Exit(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Exit");
}

/*---------------------------------------------------------------------------*/

static void *hb_gtnap_New(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_New");
    return HB_GTSUPER_NEW(pGT);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Free(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Free");
    HB_GTSUPER_FREE(pGT);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Mark(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Mark");
    HB_GTSUPER_MARK(pGT);
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_Resize(PHB_GT pGT, int iRow, int iCol)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Resize: %d, %d", iRow, iCol);
    return TRUE;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_SetMode(PHB_GT pGT, int iRow, int iCol)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetMode: %d, %d", iRow, iCol);
    return TRUE;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_GetSize(PHB_GT pGT, int *piRows, int *piCols)
{
    HB_SYMBOL_UNUSED(pGT);
    if (GTNAP_GLOBAL != NULL)
    {
        *piRows = (int)GTNAP_GLOBAL->rows;
        *piCols = (int)GTNAP_GLOBAL->cols;
    }
    else
    {
        *piRows = 0;
        *piCols = 0;
    }

    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetSize: %d, %d", *piRows, *piCols);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_SemiCold(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SemiCold");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_ColdArea(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_ColdArea %d, %d, %d, %d", iTop, iLeft, iBottom, iRight);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_ExposeArea(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_ExposeArea %d, %d, %d, %d", iTop, iLeft, iBottom, iRight);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_ScrollArea(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight, int iColor, HB_USHORT usChar, int iRows, int iCols)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_ScrollArea %d, %d, %d, %d, %d, %d, %d, %d", iTop, iLeft, iBottom, iRight, iColor, usChar, iRows, iCols);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_TouchLine(PHB_GT pGT, int iRow)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_TouchLine %d", iRow);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_TouchCell(PHB_GT pGT, int iRow, int iCol)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_TouchCell %d %d", iRow, iCol);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Redraw(PHB_GT pGT, int iRow, int iCol, int iSize)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Redraw %d %d %d", iRow, iCol, iSize);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_RedrawDiff(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_RedrawDiff");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Refresh(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Refresh");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Flush(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Flush");
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_MaxCol(PHB_GT pGT)
{
    int maxcol = 0;
    HB_SYMBOL_UNUSED(pGT);
    if (GTNAP_GLOBAL != NULL)
        maxcol = GTNAP_GLOBAL->cols - 1;

    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MaxCol %d", maxcol);

    return maxcol;
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_MaxRow(PHB_GT pGT)
{
    int maxrow = 0;
    HB_SYMBOL_UNUSED(pGT);
    if (GTNAP_GLOBAL != NULL)
        maxrow = GTNAP_GLOBAL->rows - 1;

    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MaxRow %d", maxrow);

    return maxrow;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_CheckPos(PHB_GT pGT, int iRow, int iCol, long *plIndex)
{
    HB_SYMBOL_UNUSED(pGT);
    *plIndex = 0;
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_CheckPos %d %d %d", iRow, iCol, (int)*plIndex);
    return TRUE;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_SetPos(PHB_GT pGT, int iRow, int iCol)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetPos %d %d", iRow, iCol);

    if (GTNAP_GLOBAL != NULL)
    {
        if (GTNAP_GLOBAL->debugger != NULL)
        {
            nap_debugger_set_pos(GTNAP_GLOBAL->debugger, (uint32_t)iRow, (uint32_t)iCol);
        }
        else
        {
            GtNapWindow *gtwin = i_current_gtwin(GTNAP_GLOBAL);
            if (gtwin != NULL)
            {
                gtwin->cursor_row = (int32_t)iRow;
                gtwin->cursor_col = (int32_t)iCol;
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_GetPos(PHB_GT pGT, int *piRow, int *piCol)
{
    HB_SYMBOL_UNUSED(pGT);
    if (GTNAP_GLOBAL != NULL)
    {
        if (GTNAP_GLOBAL->debugger != NULL)
        {
            uint32_t row, col;
            nap_debugger_get_pos(GTNAP_GLOBAL->debugger, &row, &col);
            *piRow = (int)row;
            *piCol = (int)col;
        }
        else
        {
            GtNapWindow *gtwin = i_current_gtwin(GTNAP_GLOBAL);
            if (gtwin != NULL)
            {
                *piRow = (int)gtwin->cursor_row;
                *piCol = (int)gtwin->cursor_col;
            }
            else
            {
                *piRow = 0;
                *piCol = 0;
            }
        }
    }

    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetPos %d %d", *piRow, *piCol);
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_IsColor(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_IsColor");
    return TRUE;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_GetColorStr(PHB_GT pGT, char *pszColorString)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(pszColorString);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetColorStr");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_SetColorStr(PHB_GT pGT, const char *szColorString)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetColorStr %s", szColorString);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_ColorSelect(PHB_GT pGT, int iColorIndex)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_ColorSelect %d", iColorIndex);
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_GetColor(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetColor");
    return 0;
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_ColorNum(PHB_GT pGT, const char *szColorString)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_ColorNum %s", szColorString);
    return 0;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_ColorsToString(PHB_GT pGT, int *pColors, int iColorCount, char *pszColorString, int iBufSize)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(pColors);
    HB_SYMBOL_UNUSED(iColorCount);
    HB_SYMBOL_UNUSED(pszColorString);
    HB_SYMBOL_UNUSED(iBufSize);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_ColorsToString");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_StringToColors(PHB_GT pGT, const char *szColorString, int **pColorsPtr, int *piColorCount)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(szColorString);
    HB_SYMBOL_UNUSED(pColorsPtr);
    HB_SYMBOL_UNUSED(piColorCount);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_StringToColors");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_GetColorData(PHB_GT pGT, int **pColorsPtr, int *piColorCount, int *piColorIndex)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(pColorsPtr);
    HB_SYMBOL_UNUSED(piColorCount);
    HB_SYMBOL_UNUSED(piColorIndex);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetColorData");
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_GetClearColor(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetClearColor");
    return 0;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_SetClearColor(PHB_GT pGT, int iColor)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetClearColor %d", iColor);
}

/*---------------------------------------------------------------------------*/

static HB_USHORT hb_gtnap_GetClearChar(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetClearChar");
    return 0;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_SetClearChar(PHB_GT pGT, HB_USHORT usChar)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetClearChar %d", usChar);
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_GetCursorStyle(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetCursorStyle");
    return SC_NONE;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_SetCursorStyle(PHB_GT pGT, int iStyle)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetCursorStyle %d", iStyle);

    if (GTNAP_GLOBAL != NULL && GTNAP_GLOBAL->debugger != NULL)
        nap_debugger_cursor(GTNAP_GLOBAL->debugger, (uint32_t)iStyle);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_GetScrCursor(PHB_GT pGT, int *piRow, int *piCol, int *piStyle)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(piRow);
    HB_SYMBOL_UNUSED(piCol);
    HB_SYMBOL_UNUSED(piStyle);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetScrCursor");
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_GetScrChar(PHB_GT pGT, int iRow, int iCol, int *piColor, HB_BYTE *pbAttr, HB_USHORT *pusChar)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(piColor);
    HB_SYMBOL_UNUSED(pbAttr);
    HB_SYMBOL_UNUSED(pusChar);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetScrChar %d, %d", iRow, iCol);
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_PutScrChar(PHB_GT pGT, int iRow, int iCol, int iColor, HB_BYTE bAttr, HB_USHORT usChar)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_PutScrChar %d, %d, %d, %d, %d", iRow, iCol, iColor, bAttr, usChar);
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_GetScrUC(PHB_GT pGT, int iRow, int iCol, int *piColor, HB_BYTE *pbAttr, HB_UCHAR *puChar, HB_BOOL fTerm)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(piColor);
    HB_SYMBOL_UNUSED(pbAttr);
    HB_SYMBOL_UNUSED(puChar);
    HB_SYMBOL_UNUSED(fTerm);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetScrUC %d, %d", iRow, iCol);
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_DispBegin(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_DispBegin");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_DispEnd(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_DispEnd");
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_DispCount(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_DispCount");
    return 0;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_GetChar(PHB_GT pGT, int iRow, int iCol, int *pbColor, HB_BYTE *pbAttr, HB_USHORT *pusChar)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(iCol);
    HB_SYMBOL_UNUSED(iRow);
    *pbColor = 0;
    *pbAttr = 0;
    *pusChar = 65;
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetChar");
    return TRUE;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_hb_codepoint(HB_USHORT usChar)
{
    return (uint32_t)usChar;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_PutChar(PHB_GT pGT, int iRow, int iCol, int bColor, HB_BYTE bAttr, HB_USHORT usChar)
{
    HB_SYMBOL_UNUSED(pGT);
    if (GTNAP_GLOBAL != NULL && GTNAP_GLOBAL->debugger != NULL)
    {
        uint32_t codepoint = i_hb_codepoint(usChar);
        nap_debugger_putchar(GTNAP_GLOBAL->debugger, (uint32_t)iRow, (uint32_t)iCol, codepoint, (byte_t)bColor, (byte_t)bAttr);
    }

    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_PutChar %d %d %d %d %d", iRow, iCol, bColor, bAttr, usChar);

    return TRUE;
}

/*---------------------------------------------------------------------------*/

static long hb_gtnap_RectSize(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_RectSize %d %d %d %d", iTop, iLeft, iBottom, iRight);
    return 0;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Save(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight, void *pBuffer)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Save %d %d %d %d", iTop, iLeft, iBottom, iRight);

    if (GTNAP_GLOBAL != NULL && GTNAP_GLOBAL->debugger != NULL)
        nap_debugger_save(GTNAP_GLOBAL->debugger, (uint32_t)iTop, (uint32_t)iLeft, (uint32_t)iBottom, (uint32_t)iRight, (byte_t *)pBuffer);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Rest(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight, const void *pBuffer)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Rest %d %d %d %d", iTop, iLeft, iBottom, iRight);

    if (GTNAP_GLOBAL != NULL && GTNAP_GLOBAL->debugger != NULL)
        nap_debugger_rest(GTNAP_GLOBAL->debugger, (uint32_t)iTop, (uint32_t)iLeft, (uint32_t)iBottom, (uint32_t)iRight, (const byte_t *)pBuffer);
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_PutText(PHB_GT pGT, int iRow, int iCol, int bColor, const char *pText, HB_SIZE ulLen)
{
    char_t utf8[STATIC_TEXT_SIZE];
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(iRow);
    HB_SYMBOL_UNUSED(iCol);
    HB_SYMBOL_UNUSED(bColor);
    HB_SYMBOL_UNUSED(pText);
    HB_SYMBOL_UNUSED(ulLen);
    i_cp_to_utf8(pText, utf8, sizeof32(utf8));

    if (GTNAP_GLOBAL != NULL && GTNAP_GLOBAL->debugger != NULL)
        nap_debugger_puttext(GTNAP_GLOBAL->debugger, (uint32_t)iRow, (uint32_t)iCol, (byte_t)bColor, utf8);

    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_PutText %d %d", iRow, iCol);

    return 0;
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_PutTextW(PHB_GT pGT, int iRow, int iCol, int bColor, const HB_WCHAR *pText, HB_SIZE ulLen)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(bColor);
    HB_SYMBOL_UNUSED(pText);
    HB_SYMBOL_UNUSED(ulLen);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_PutTextW %d %d", iRow, iCol);
    return 0;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Replicate(PHB_GT pGT, int iRow, int iCol, int bColor, HB_BYTE bAttr, HB_USHORT usChar, HB_SIZE ulLen)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(bColor);
    HB_SYMBOL_UNUSED(bAttr);
    HB_SYMBOL_UNUSED(usChar);
    HB_SYMBOL_UNUSED(ulLen);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Replicate %d %d", iRow, iCol);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_WriteAt(PHB_GT pGT, int iRow, int iCol, const char *pText, HB_SIZE ulLength)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(ulLength);

    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_WriteAt %d %d", iRow, iCol);

    if (GTNAP_GLOBAL != NULL)
    {
        GtNapWindow *gtwin = i_current_gtwin(GTNAP_GLOBAL);
        if (gtwin != NULL)
        {
            i_add_label(iRow - gtwin->top, iCol - gtwin->left, FALSE, gtwin, GTNAP_GLOBAL);
            if (pText != NULL)
            {
                GtNapObject *obj = arrpt_last(gtwin->objects, GtNapObject);
                char_t utf8[STATIC_TEXT_SIZE];
                i_cp_to_utf8(pText, utf8, sizeof32(utf8));
                i_set_label_text(obj, utf8);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_WriteAtW(PHB_GT pGT, int iRow, int iCol, const HB_WCHAR *szText, HB_SIZE nLength)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(szText);
    HB_SYMBOL_UNUSED(nLength);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_WriteAtW %d %d", iRow, iCol);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Write(PHB_GT pGT, const char *szText, HB_SIZE nLength)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(szText);
    HB_SYMBOL_UNUSED(nLength);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Write");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_WriteW(PHB_GT pGT, const HB_WCHAR *szText, HB_SIZE nLength)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(szText);
    HB_SYMBOL_UNUSED(nLength);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_WriteW");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_WriteCon(PHB_GT pGT, const char *szText, HB_SIZE nLength)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(szText);
    HB_SYMBOL_UNUSED(nLength);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_WriteCon");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_WriteConW(PHB_GT pGT, const HB_WCHAR *szText, HB_SIZE nLength)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(szText);
    HB_SYMBOL_UNUSED(nLength);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_WriteConW");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_SetAttribute(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight, int bColor)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(bColor);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetAttribute %d %d %d %d", iTop, iLeft, iBottom, iRight);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_DrawShadow(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight, int iColor)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(iColor);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_DrawShadow %d %d %d %d", iTop, iLeft, iBottom, iRight);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Scroll(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight, int bColor, HB_USHORT bChar, int iRows, int iCols)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Scroll %d %d %d %d", iTop, iLeft, iBottom, iRight);

    if (GTNAP_GLOBAL != NULL)
    {
        if (GTNAP_GLOBAL->debugger != NULL)
        {
            uint32_t codepoint = i_hb_codepoint(bChar);
            nap_debugger_scroll(GTNAP_GLOBAL->debugger, (uint32_t)iTop, (uint32_t)iLeft, (uint32_t)iBottom, (uint32_t)iRight, (uint32_t)iRows, (uint32_t)iCols, codepoint, (byte_t)bColor);
        }
        else
        {
            GtNapWindow *gtwin = i_current_gtwin(GTNAP_GLOBAL);

            if (gtwin != NULL)
            {
                uint32_t i, n;
                /*
                   FRAN: The scroll, at the moment, delete all texts
                   Improve taking into account the input rectangle
                   Take into account if a real scroll exists (iRows > 0 || iCols > 0)
                */
                n = arrpt_size(gtwin->objects, GtNapObject);
                for (i = 0; i < n;)
                {
                    GtNapObject *object = arrpt_get(gtwin->objects, i, GtNapObject);
                    const char_t *type = _component_type(object->component);
                    if (str_equ_c(type, "Label") == TRUE)
                    {
                        i_destroy_gtobject(gtwin, i);
                        n -= 1;
                    }
                    else
                    {
                        i += 1;
                    }
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_ScrollUp(PHB_GT pGT, int iRows, int iColor, HB_USHORT usChar)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(iColor);
    HB_SYMBOL_UNUSED(usChar);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_ScrollUp %d", iRows);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Box(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight, const char *pbyFrame, int bColor)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(pbyFrame);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Box %d %d %d %d (%s)", iTop, iLeft, iBottom, iRight, pbyFrame);

    if (GTNAP_GLOBAL != NULL && GTNAP_GLOBAL->debugger != NULL)
        nap_debugger_box(GTNAP_GLOBAL->debugger, (uint32_t)iTop, (uint32_t)iLeft, (uint32_t)iBottom, (uint32_t)iRight, (byte_t)bColor);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_BoxW(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight, const HB_WCHAR *szFrame, int iColor)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(szFrame);
    HB_SYMBOL_UNUSED(iColor);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_BoxW %d %d %d %d", iTop, iLeft, iBottom, iRight);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_BoxD(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight, const char *szFrame, int iColor)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(szFrame);
    HB_SYMBOL_UNUSED(iColor);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_BoxD %d %d %d %d", iTop, iLeft, iBottom, iRight);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_BoxS(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight, const char *szFrame, int iColor)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(szFrame);
    HB_SYMBOL_UNUSED(iColor);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_BoxS %d %d %d %d", iTop, iLeft, iBottom, iRight);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_HorizLine(PHB_GT pGT, int iRow, int iLeft, int iRight, HB_USHORT bChar, int bColor)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(bChar);
    HB_SYMBOL_UNUSED(bColor);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_HorizLine %d %d %d", iRow, iLeft, iRight);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_VertLine(PHB_GT pGT, int iCol, int iTop, int iBottom, HB_USHORT bChar, int bColor)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(bChar);
    HB_SYMBOL_UNUSED(bColor);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_VertLine %d %d %d", iCol, iTop, iBottom);
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_GetBlink(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_GetBlink");
    return TRUE;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_SetBlink(PHB_GT pGT, HB_BOOL bBlink)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetBlink %d", bBlink);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_SetSnowFlag(PHB_GT pGT, HB_BOOL fNoSnow)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(fNoSnow);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetSnowFlag");
}

/*---------------------------------------------------------------------------*/

static const char *hb_gtnap_Version(PHB_GT pGT, int iType)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Version %d", iType);

    if (iType == 0)
        return HB_GT_DRVNAME(HB_GT_NAME);

    return "Harbour Terminal: GTNAP";
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_Suspend(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Suspend");
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_Resume(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Resume");
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_PreExt(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_PreExt");
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_PostExt(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_PostExt");
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_OutStd(PHB_GT pGT, const char *pbyStr, HB_SIZE ulLen)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(pbyStr);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_OutStd %d", (int)ulLen);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_OutErr(PHB_GT pGT, const char *pbyStr, HB_SIZE ulLen)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_OutErr '%s' (%d)", pbyStr, (int)ulLen);

    if (pbyStr != NULL && ulLen > 0)
    {
        char_t utf8[STATIC_TEXT_SIZE];
        i_cp_to_utf8(pbyStr, utf8, sizeof32(utf8));
    }
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Tone(PHB_GT pGT, double dFrequency, double dDuration)
{
    /* dDuration is in 'Ticks' (18.2 per second) */
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Tone %f %f", dFrequency, dDuration);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_Bell(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Bell");
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_Info(PHB_GT pGT, int iType, PHB_GT_INFO pInfo)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Info %d", iType);

    switch (iType)
    {
    case HB_GTI_GETWIN:
        /* Its setting a new window */
        if (pInfo->pNewVal != NULL)
        {
            HB_TYPE type = HB_ITEM_TYPE(pInfo->pNewVal);
            if (type == HB_IT_ARRAY)
            {
                HB_SIZE n = hb_arrayLen(pInfo->pNewVal);
                if (n == 5)
                {
                    PHB_ITEM wname = hb_arrayGetItemPtr(pInfo->pNewVal, 1);
                    if (HB_ITEM_TYPE(wname) == HB_IT_STRING)
                    {
                        char_t utf8[STATIC_TEXT_SIZE];
                        hb_itemCopyStrUTF8(wname, utf8, sizeof(utf8));
                        if (str_equ_c(utf8, "Debugger") == TRUE)
                        {
                            PHB_ITEM wtop = hb_arrayGetItemPtr(pInfo->pNewVal, 2);
                            PHB_ITEM wleft = hb_arrayGetItemPtr(pInfo->pNewVal, 3);
                            PHB_ITEM wbottom = hb_arrayGetItemPtr(pInfo->pNewVal, 4);
                            PHB_ITEM wright = hb_arrayGetItemPtr(pInfo->pNewVal, 5);
                            uint32_t ncols = 0, nrows = 0;
                            cassert(HB_ITEM_TYPE(wtop) == HB_IT_INTEGER);
                            cassert(HB_ITEM_TYPE(wleft) == HB_IT_INTEGER);
                            cassert(HB_ITEM_TYPE(wbottom) == HB_IT_INTEGER);
                            cassert(HB_ITEM_TYPE(wright) == HB_IT_INTEGER);
                            ncols = hb_itemGetNI(wright) - hb_itemGetNI(wleft) + 1;
                            nrows = hb_itemGetNI(wbottom) - hb_itemGetNI(wtop) + 1;
                            cassert(GTNAP_GLOBAL->debugger == NULL);
                            unref(ncols);
                            unref(nrows);
                            GTNAP_GLOBAL->debugger = nap_debugger_create(tc(GTNAP_GLOBAL->debugger_path), GTNAP_GLOBAL->rows, GTNAP_GLOBAL->cols);
                            GTNAP_GLOBAL->debugger_visible = TRUE;
                            pInfo->pResult = hb_itemPutNI(pInfo->pResult, i_DEBUGGER_WINDOW_HASH);
                        }
                        else
                        {
                            /* TODO: Review this window class */
                            cassert(FALSE);
                        }
                    }
                    else
                    {
                        /* TODO: Review this window class */
                        cassert(FALSE);
                    }
                }
                else
                {
                    /* TODO: Review this window class */
                    cassert(FALSE);
                }
            }
        }
        /* Its asking for the current window */
        else
        {
            cassert(pInfo->pResult == NULL);
            if (GTNAP_GLOBAL->debugger != NULL && GTNAP_GLOBAL->debugger_visible == TRUE)
                pInfo->pResult = hb_itemPutNI(pInfo->pResult, i_DEBUGGER_WINDOW_HASH);
            else
                pInfo->pResult = hb_itemPutNI(pInfo->pResult, i_MAIN_WINDOW_HASH);
        }
        break;

    case HB_GTI_SETWIN:
        /* Recovering current window --> Destroy the debugger */
        if (pInfo->pNewVal != NULL)
        {
            HB_TYPE type = HB_ITEM_TYPE(pInfo->pNewVal);
            switch (type)
            {
            case HB_IT_INTEGER:
            case HB_IT_LONG:
            {
                uint32_t wid = hb_itemGetNI(pInfo->pNewVal);
                /* Close the debugger process */
                if (wid == i_MAIN_WINDOW_HASH)
                {
                    GtNapWindow *gtwin = i_current_gtwin(GTNAP_GLOBAL);
                    if (gtwin != NULL)
                        window_show(gtwin->window);

                    if (GTNAP_GLOBAL->debugger != NULL)
                    {
                        nap_debugger_show(GTNAP_GLOBAL->debugger, FALSE);
                        GTNAP_GLOBAL->debugger_visible = FALSE;
                    }
                }
                else if (wid == i_DEBUGGER_WINDOW_HASH)
                {
                    if (GTNAP_GLOBAL->debugger != NULL)
                    {
                        nap_debugger_show(GTNAP_GLOBAL->debugger, TRUE);
                        GTNAP_GLOBAL->debugger_visible = TRUE;
                    }
                }
                else
                {
                    /* TODO: Review this window ID */
                    cassert(FALSE);
                }

                break;
            }

            default:
                cassert_default(type);
            }
        }
        else
        {
            /* TODO: Review parameter value */
            cassert(FALSE);
        }
        break;

    case HB_GTI_COMPATBUFFER:
        pInfo->pResult = hb_itemPutL(pInfo->pResult, TRUE);
        break;

    case HB_GTI_BOXCP:
        pInfo->pResult = hb_itemPutC(pInfo->pResult,
                                     pGT->cdpBox ? pGT->cdpBox->id : NULL);
        if (hb_itemType(pInfo->pNewVal) & HB_IT_STRING)
        {
            if (hb_itemGetCLen(pInfo->pNewVal) > 0)
            {
                PHB_CODEPAGE cdpBox = hb_cdpFind(hb_itemGetCPtr(pInfo->pNewVal));
                if (cdpBox)
                    pGT->cdpBox = cdpBox;
            }
            else
                pGT->cdpBox = NULL;
        }
        break;

    default:
        cassert_default(iType);
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_Alert(PHB_GT pGT, PHB_ITEM message, PHB_ITEM options, int a, int b, double c)
{
    String *msg = NULL;
    ArrPt(String) *opts = NULL;
    uint32_t ret = 0;

    HB_SYMBOL_UNUSED(pGT);

    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_Alert %d %d %f", a, b, c);

    if (HB_ITEM_TYPE(message) == HB_IT_STRING)
    {
        char_t utf8[STATIC_TEXT_SIZE];
        hb_itemCopyStrUTF8(message, utf8, sizeof(utf8));
        msg = str_c(utf8);
    }
    else
    {
        msg = str_c("Unknown alert message");
    }

    opts = arrpt_create(String);
    if (HB_ITEM_TYPE(options) == HB_IT_ARRAY)
    {
        HB_SIZE i, n = hb_arrayLen(options);
        for (i = 0; i < n; ++i)
        {
            PHB_ITEM elem = hb_arrayGetItemPtr(options, i + 1);
            String *opt = NULL;
            if (HB_ITEM_TYPE(elem) == HB_IT_STRING)
            {
                char_t utf8[STATIC_TEXT_SIZE];
                hb_itemCopyStrUTF8(elem, utf8, sizeof(utf8));
                opt = str_c(utf8);
            }
            else
            {
                opt = str_c("Unknown opt");
            }

            arrpt_append(opts, opt, String);
        }
    }

    ret = gui_info_window(TRUE, tc(msg), "Harbour alert", "Please contact ASPEC technical support", "--", 0, opts, 0);
    str_destroy(&msg);
    arrpt_destroy(&opts, str_destroy, String);

    return ret + 1;
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_SetFlag(PHB_GT pGT, int iType, int iNewValue)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(iNewValue);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetFlag %d", iType);
    return 0;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_SetDispCP(PHB_GT pGT, const char *pszTermCDP, const char *pszHostCDP, HB_BOOL fBox)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(pszTermCDP);
    HB_SYMBOL_UNUSED(pszHostCDP);
    HB_SYMBOL_UNUSED(fBox);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetDispCP");
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_SetKeyCP(PHB_GT pGT, const char *pszTermCDP, const char *pszHostCDP)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(pszTermCDP);
    HB_SYMBOL_UNUSED(pszHostCDP);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_SetKeyCP");
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_ReadKey(PHB_GT pGT, int iEventMask)
{
    int iKey = 0;
    HB_SYMBOL_UNUSED(pGT);

    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_ReadKey %d", iEventMask);

    if (GTNAP_GLOBAL != NULL && GTNAP_GLOBAL->debugger != NULL)
        iKey = (int)nap_debugger_read_key(GTNAP_GLOBAL->debugger);

    return iKey;
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_InkeyGet(PHB_GT pGT, HB_BOOL fWait, double dSeconds, int iEventMask)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(fWait);
    HB_SYMBOL_UNUSED(dSeconds);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_InkeyGet %d", iEventMask);
    return 0;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_InkeyPut(PHB_GT pGT, int iKey)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_InkeyPut %d", iKey);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_InkeyIns(PHB_GT pGT, int iKey)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_InkeyIns %d", iKey);
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_InkeyLast(PHB_GT pGT, int iEventMask)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_InkeyLast %d", iEventMask);
    return 0;
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_InkeyNext(PHB_GT pGT, int iEventMask)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_InkeyNext %d", iEventMask);
    return 0;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_InkeyPoll(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_InkeyPoll");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_InkeySetText(PHB_GT pGT, const char *szText, HB_SIZE nLen, HB_BOOL fEol)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(szText);
    HB_SYMBOL_UNUSED(fEol);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_InkeySetText %d", (int)nLen);
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_InkeySetLast(PHB_GT pGT, int iKey)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_InkeySetLast %d", iKey);
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_InkeyReset(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_InkeyReset");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_InkeyExit(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_InkeyExit");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseInit(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseInit");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseExit(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseExit");
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_MouseIsPresent(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseIsPresent");
    return TRUE;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseShow(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseShow");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseHide(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseHide");
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_MouseGetCursor(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseGetCursor");
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseSetCursor(PHB_GT pGT, HB_BOOL fVisible)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(fVisible);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseSetCursor");
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_MouseCol(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseCol");
    return 1;
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_MouseRow(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseRow");
    return 1;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseGetPos(PHB_GT pGT, int *piRow, int *piCol)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(piRow);
    HB_SYMBOL_UNUSED(piCol);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseGetPos");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseSetPos(PHB_GT pGT, int iRow, int iCol)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseSetPos %d %d", iRow, iCol);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseSetBounds(PHB_GT pGT, int iTop, int iLeft, int iBottom, int iRight)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseSetBounds %d %d %d %d", iTop, iLeft, iBottom, iRight);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseGetBounds(PHB_GT pGT, int *piTop, int *piLeft, int *piBottom, int *piRight)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(piTop);
    HB_SYMBOL_UNUSED(piLeft);
    HB_SYMBOL_UNUSED(piBottom);
    HB_SYMBOL_UNUSED(piRight);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseGetBounds");
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_MouseStorageSize(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseStorageSize");
    return 1;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseSaveState(PHB_GT pGT, void *pBuffer)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(pBuffer);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseSaveState");
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseRestoreState(PHB_GT pGT, const void *pBuffer)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(pBuffer);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseRestoreState");
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_MouseGetDoubleClickSpeed(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseGetDoubleClickSpeed");
    return 1;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_MouseSetDoubleClickSpeed(PHB_GT pGT, int iSpeed)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(iSpeed);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseSetDoubleClickSpeed");
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_MouseCountButton(PHB_GT pGT)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseCountButton");
    return 3;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_MouseButtonState(PHB_GT pGT, int iButton)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseButtonState %d", iButton);
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_MouseButtonPressed(PHB_GT pGT, int iButton, int *piRow, int *piCol)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(piRow);
    HB_SYMBOL_UNUSED(piCol);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseButtonPressed %d", iButton);
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gtnap_MouseButtonReleased(PHB_GT pGT, int iButton, int *piRow, int *piCol)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(piRow);
    HB_SYMBOL_UNUSED(piCol);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseButtonReleased %d", iButton);
    return HB_TRUE;
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_MouseReadKey(PHB_GT pGT, int iEventMask)
{
    HB_SYMBOL_UNUSED(pGT);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_MouseReadKey %d", iEventMask);
    return 0;
}

/*---------------------------------------------------------------------------*/

static int hb_gtnap_gfxPrimitive(PHB_GT pGT, int iType, int iTop, int iLeft, int iBottom, int iRight, int iColor)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(iColor);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_gfxPrimitive (%d) %d %d %d %d", iType, iTop, iLeft, iBottom, iRight);

    return 1;
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_gfxText(PHB_GT pGT, int iTop, int iLeft, const char *cBuf, int iColor, int iSize, int iWidth)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(cBuf);
    HB_SYMBOL_UNUSED(iColor);
    HB_SYMBOL_UNUSED(iSize);
    HB_SYMBOL_UNUSED(iWidth);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_gfxText %d %d", iTop, iLeft);
}

/*---------------------------------------------------------------------------*/

static void hb_gtnap_WhoCares(PHB_GT pGT, void *pCargo)
{
    HB_SYMBOL_UNUSED(pGT);
    HB_SYMBOL_UNUSED(pCargo);
    if (i_LOG_HBFUNCS == TRUE)
        log_printf("hb_gtnap_WhoCares");
}

/*---------------------------------------------------------------------------*/

static HB_BOOL hb_gt_FuncInit(PHB_GT_FUNCS pFuncTable)
{
    pFuncTable->Lock = hb_gtnap_Lock;
    pFuncTable->Unlock = hb_gtnap_Unlock;
    pFuncTable->Init = hb_gtnap_Init;
    pFuncTable->Exit = hb_gtnap_Exit;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->New = hb_gtnap_New;
        pFuncTable->Free = hb_gtnap_Free;
        pFuncTable->Mark = hb_gtnap_Mark;
    }

    pFuncTable->Resize = hb_gtnap_Resize;
    pFuncTable->SetMode = hb_gtnap_SetMode;
    pFuncTable->GetSize = hb_gtnap_GetSize;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->SemiCold = hb_gtnap_SemiCold;
        pFuncTable->ColdArea = hb_gtnap_ColdArea;
    }

    pFuncTable->ExposeArea = hb_gtnap_ExposeArea;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->ScrollArea = hb_gtnap_ScrollArea;
        pFuncTable->TouchLine = hb_gtnap_TouchLine;
        pFuncTable->TouchCell = hb_gtnap_TouchCell;
        pFuncTable->Redraw = hb_gtnap_Redraw;
        pFuncTable->RedrawDiff = hb_gtnap_RedrawDiff;
        pFuncTable->Refresh = hb_gtnap_Refresh;
        pFuncTable->Flush = hb_gtnap_Flush;
    }

    pFuncTable->MaxCol = hb_gtnap_MaxCol;
    pFuncTable->MaxRow = hb_gtnap_MaxRow;
    pFuncTable->CheckPos = hb_gtnap_CheckPos;
    pFuncTable->SetPos = hb_gtnap_SetPos;
    pFuncTable->GetPos = hb_gtnap_GetPos;
    pFuncTable->IsColor = hb_gtnap_IsColor;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->GetColorStr = hb_gtnap_GetColorStr;
        pFuncTable->SetColorStr = hb_gtnap_SetColorStr;
        pFuncTable->ColorSelect = hb_gtnap_ColorSelect;
        pFuncTable->GetColor = hb_gtnap_GetColor;
        pFuncTable->ColorNum = hb_gtnap_ColorNum;
        pFuncTable->ColorsToString = hb_gtnap_ColorsToString;
        pFuncTable->StringToColors = hb_gtnap_StringToColors;
        pFuncTable->GetColorData = hb_gtnap_GetColorData;
        pFuncTable->GetClearColor = hb_gtnap_GetClearColor;
        pFuncTable->SetClearColor = hb_gtnap_SetClearColor;
        pFuncTable->GetClearChar = hb_gtnap_GetClearChar;
        pFuncTable->SetClearChar = hb_gtnap_SetClearChar;
    }

    pFuncTable->GetCursorStyle = hb_gtnap_GetCursorStyle;
    pFuncTable->SetCursorStyle = hb_gtnap_SetCursorStyle;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->GetScrCursor = hb_gtnap_GetScrCursor;
        pFuncTable->GetScrChar = hb_gtnap_GetScrChar;
        pFuncTable->PutScrChar = hb_gtnap_PutScrChar;
        pFuncTable->GetScrUC = hb_gtnap_GetScrUC;
    }

    pFuncTable->DispBegin = hb_gtnap_DispBegin;
    pFuncTable->DispEnd = hb_gtnap_DispEnd;
    pFuncTable->DispCount = hb_gtnap_DispCount;
    pFuncTable->GetChar = hb_gtnap_GetChar;
    pFuncTable->PutChar = hb_gtnap_PutChar;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->RectSize = hb_gtnap_RectSize;
    }

    pFuncTable->Save = hb_gtnap_Save;
    pFuncTable->Rest = hb_gtnap_Rest;
    pFuncTable->PutText = hb_gtnap_PutText;
    pFuncTable->PutTextW = hb_gtnap_PutTextW;
    pFuncTable->Replicate = hb_gtnap_Replicate;
    pFuncTable->WriteAt = hb_gtnap_WriteAt;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->WriteAtW = hb_gtnap_WriteAtW;
        pFuncTable->Write = hb_gtnap_Write;
        pFuncTable->WriteW = hb_gtnap_WriteW;
        pFuncTable->WriteCon = hb_gtnap_WriteCon;
        pFuncTable->WriteConW = hb_gtnap_WriteConW;
        pFuncTable->DrawShadow = hb_gtnap_DrawShadow;
        pFuncTable->ScrollUp = hb_gtnap_ScrollUp;
    }

    pFuncTable->SetAttribute = hb_gtnap_SetAttribute;
    pFuncTable->Scroll = hb_gtnap_Scroll;
    pFuncTable->Box = hb_gtnap_Box;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->BoxW = hb_gtnap_BoxW;
        pFuncTable->BoxD = hb_gtnap_BoxD;
        pFuncTable->BoxS = hb_gtnap_BoxS;
    }

    pFuncTable->HorizLine = hb_gtnap_HorizLine;
    pFuncTable->VertLine = hb_gtnap_VertLine;
    pFuncTable->GetBlink = hb_gtnap_GetBlink;
    pFuncTable->SetBlink = hb_gtnap_SetBlink;
    pFuncTable->Version = hb_gtnap_Version;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->SetSnowFlag = hb_gtnap_SetSnowFlag;
        pFuncTable->Suspend = hb_gtnap_Suspend;
        pFuncTable->Resume = hb_gtnap_Resume;
        pFuncTable->PreExt = hb_gtnap_PreExt;
        pFuncTable->PostExt = hb_gtnap_PostExt;
    }

    pFuncTable->OutStd = hb_gtnap_OutStd;
    pFuncTable->OutErr = hb_gtnap_OutErr;
    pFuncTable->Tone = hb_gtnap_Tone;
    pFuncTable->Info = hb_gtnap_Info;
    pFuncTable->Alert = hb_gtnap_Alert;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->Bell = hb_gtnap_Bell;
        pFuncTable->SetFlag = hb_gtnap_SetFlag;
    }

    /* internationalization */
    if (i_FULL_HBFUNCS)
    {
        pFuncTable->SetDispCP = hb_gtnap_SetDispCP;
        pFuncTable->SetKeyCP = hb_gtnap_SetKeyCP;
    }

    /* keyboard */
    pFuncTable->ReadKey = hb_gtnap_ReadKey;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->InkeyGet = hb_gtnap_InkeyGet;
        pFuncTable->InkeyPut = hb_gtnap_InkeyPut;
        pFuncTable->InkeyIns = hb_gtnap_InkeyIns;
        pFuncTable->InkeyLast = hb_gtnap_InkeyLast;
        pFuncTable->InkeyNext = hb_gtnap_InkeyNext;
        pFuncTable->InkeyPoll = hb_gtnap_InkeyPoll;
        pFuncTable->InkeySetText = hb_gtnap_InkeySetText;
        pFuncTable->InkeySetLast = hb_gtnap_InkeySetLast;
        pFuncTable->InkeyReset = hb_gtnap_InkeyReset;
        pFuncTable->InkeyExit = hb_gtnap_InkeyExit;
    }

    /* mouse */
    pFuncTable->MouseInit = hb_gtnap_MouseInit;
    pFuncTable->MouseExit = hb_gtnap_MouseExit;
    pFuncTable->MouseIsPresent = hb_gtnap_MouseIsPresent;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->MouseShow = hb_gtnap_MouseShow;
        pFuncTable->MouseHide = hb_gtnap_MouseHide;
        pFuncTable->MouseGetCursor = hb_gtnap_MouseGetCursor;
        pFuncTable->MouseSetCursor = hb_gtnap_MouseSetCursor;
    }

    pFuncTable->MouseCol = hb_gtnap_MouseCol;
    pFuncTable->MouseRow = hb_gtnap_MouseRow;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->MouseGetPos = hb_gtnap_MouseGetPos;
        pFuncTable->MouseSetPos = hb_gtnap_MouseSetPos;
        pFuncTable->MouseSetBounds = hb_gtnap_MouseSetBounds;
        pFuncTable->MouseGetBounds = hb_gtnap_MouseGetBounds;
        pFuncTable->MouseStorageSize = hb_gtnap_MouseStorageSize;
        pFuncTable->MouseSaveState = hb_gtnap_MouseSaveState;
        pFuncTable->MouseRestoreState = hb_gtnap_MouseRestoreState;
        pFuncTable->MouseGetDoubleClickSpeed = hb_gtnap_MouseGetDoubleClickSpeed;
        pFuncTable->MouseSetDoubleClickSpeed = hb_gtnap_MouseSetDoubleClickSpeed;
    }

    pFuncTable->MouseCountButton = hb_gtnap_MouseCountButton;
    pFuncTable->MouseButtonState = hb_gtnap_MouseButtonState;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->MouseButtonPressed = hb_gtnap_MouseButtonPressed;
        pFuncTable->MouseButtonReleased = hb_gtnap_MouseButtonReleased;
        pFuncTable->MouseReadKey = hb_gtnap_MouseReadKey;
    }

    /* Graphics API */
    pFuncTable->GfxPrimitive = hb_gtnap_gfxPrimitive;
    pFuncTable->GfxText = hb_gtnap_gfxText;

    if (i_FULL_HBFUNCS)
    {
        pFuncTable->WhoCares = hb_gtnap_WhoCares;
    }

    return TRUE;
}

/*---------------------------------------------------------------------------*/

#include "hbgtreg.h"
