/*
    This is part of gtnap
    HBNAP/Forms API implementation. For full graphic applications.
*/

#include "hbnap.h"
#include "gtnap.inl"
#include "hbnap.ch"
#include "nap_debugger.inl"

#include <nforms/nforms.h>
#include <nforms/nform.h>
#include <deblib/deblib.h>
#include <osapp/osmain.h>
#include <osapp/osapp.h>
#include <gui/drawctrl.inl>
#include <gui/gui.h>
#include <gui/imageview.h>
#include <gui/menu.h>
#include <gui/menuitem.h>
#include <gui/panel.h>
#include <gui/tableview.h>
#include <gui/tableviewh.h>
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
#include <sewer/bstd.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>

#include "hbapi.h"
#include "hbapiitm.h"
#include "hbapirdd.h"
#include "hbapistr.h"
#include "hbset.h"

/*---------------------------------------------------------------------------*/

typedef struct _hbnap_callback_t HbNapCallback;
typedef struct _hbnap_bind_t HbNapBind;
typedef struct _hbnap_fcolumn_t HbNapFColumn;
typedef struct _hbnap_fnode_t HbNapFNode;
typedef struct _hbnap_fbdconn_t HbNapFDBConn;
typedef struct _hbnap_farea2_t HbNapFArea2;
typedef struct _hbnap_prop_t HbNapProp;
typedef struct _hbnap_t HbNap;
typedef void (*FPtr_hbnap_callback)(HbNapCallback *callback, Event *event);

struct _hbnap_callback_t
{
    HbNapForm *form;
    HbNapMenuItem *menuitem;
    HB_ITEM *block;
    int32_t key;
    uint32_t autoclose_id;
};

struct _hbnap_bind_t
{
    String *gui_id;
    PHB_ITEM value;
    Listener *listener;
};

struct _hbnap_fcolumn_t
{
    align_t align;
    HB_ITEM *block;
};

struct _hbnap_fnode_t
{
    HbNapFArea2 *area;
    HB_ULONG recno;
    bool_t expanded;
    uint32_t nchildren; /* Child count forward. Stops recursion for collapsed nodes. */
};

struct _hbnap_fbdconn_t
{
    HbNapForm *form;
    String *cellname;
    TableView *table;
    ArrSt(HbNapFArea2) *areas;
    ArrSt(uint32_t) *records;
    TreeSt(HbNapFNode) *tdata;
};

struct _hbnap_farea2_t
{
    AREA *area;
    ArrSt(HbNapFColumn) *columns;
    HB_ITEM *relkey;
    HB_USHORT fexpanded;
    HB_ULONG cache_recno;
};

struct _hbnap_form_t
{
    NForm *form;
    Window *window;
    String *nameid;
    String *respath;
    uint32_t modal_ret;
    HbNapFDBConn *dbconn;
    HB_ITEM *OnClose_block;
    bool_t is_resizable;
    ArrSt(HbNapBind) *binds;
    ArrPt(HbNapCallback) *callbacks;
};

struct _hbnap_prop_t
{
    String *key;
    String *value;
};

struct _hbnap_t
{
    String *working_path;
    uint8_t date_digits;
    uint8_t date_chars;
    String *debugger_path;
    bool_t debugger_visible;
    GtNapDebugger *debugger;
    ArrPt(HbNapCallback) *menu_callbacks;
    SetSt(HbNapProp) *properties;
};

DeclPt(HbNapCallback);
DeclSt(HbNapProp);
DeclSt(HbNapBind);
DeclSt(HbNapFColumn);
DeclSt(HbNapFArea2);
DeclSt(HbNapFNode);

/*---------------------------------------------------------------------------*/

static HbNap *HBNAP_GLOBAL = NULL;
static PHB_ITEM INIT_CODEBLOCK = NULL;
static color_t i_COLORS[16];

static const real32_t i_UNDEF_R32 = REAL32_MAX;
static const real32_t i_MAXIMIZED_SIZE = 1e10f;
static const char_t *i_XPOS_PROP = "XPOS";
static const char_t *i_YPOS_PROP = "YPOS";
static const char_t *i_WIDTH_PROP = "WIDTH";
static const char_t *i_HEIGHT_PROP = "HEIGHT";

#define HBNAP_TEXT_SIZE 1024
static char_t HBNAP_TEMP_BUFFER[HBNAP_TEXT_SIZE];

/*---------------------------------------------------------------------------*/

static void i_remove_property(HbNapProp *prop)
{
    cassert_no_null(prop);
    str_destroy(&prop->key);
    str_destroy(&prop->value);
}

/*---------------------------------------------------------------------------*/

static int i_prop_cmp(const HbNapProp *prop, const char_t *key)
{
    cassert_no_null(prop);
    return str_cmp(prop->key, key);
}

/*---------------------------------------------------------------------------*/

static void i_save_properties(const SetSt(HbNapProp) *properties)
{
    String *cfile = hfile_appdata("config.txt");
    Stream *stm = stm_to_file(tc(cfile), NULL);
    if (stm != NULL)
    {
        setst_foreach_const(prop, properties, HbNapProp)
            stm_writef(stm, tc(prop->key));
            stm_writef(stm, ":");
            stm_writef(stm, tc(prop->value));
            stm_writef(stm, "\n");
        setst_fornext_const(prop, properties, HbNapProp)
        stm_close(&stm);
    }

    str_destroy(&cfile);
}

/*---------------------------------------------------------------------------*/

static void i_load_properties(SetSt(HbNapProp) *properties)
{
    String *cfile = hfile_appdata("config.txt");
    Stream *stm = stm_from_file(tc(cfile), NULL);
    if (stm != NULL)
    {
        stm_lines(line, stm)
            String *key = NULL;
            String *value = NULL;
            HbNapProp *prop = NULL;
            str_split_trim(line, ":", &key, &value);
            prop = setst_insert(properties, tc(key), HbNapProp, char_t);
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

static void i_write_property(SetSt(HbNapProp) *properties, const char_t *wnameid, const char_t *propid, const char_t *value)
{
    String *propname = str_printf("%s-%s", wnameid, propid);
    HbNapProp *prop = setst_get(properties, tc(propname), HbNapProp, char_t);
    if (prop == NULL)
    {
        prop = setst_insert(properties, tc(propname), HbNapProp, char_t);
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

static const char_t *i_read_property(const SetSt(HbNapProp) *properties, const char_t *wnameid, const char_t *propid)
{
    String *propname = str_printf("%s-%s", wnameid, propid);
    const HbNapProp *prop = setst_get_const(properties, tc(propname), HbNapProp, char_t);
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
    i_write_property(HBNAP_GLOBAL->properties, wnameid, propid, svalue);
}

/*---------------------------------------------------------------------------*/

static real32_t i_read_prop_r32(const char_t *wnameid, const char_t *propid)
{
    const char_t *value = i_read_property(HBNAP_GLOBAL->properties, wnameid, propid);
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

static void i_destroy_hbnap_callback(HbNapCallback **callback)
{
    cassert_no_null(callback);
    cassert_no_null(*callback);
    if ((*callback)->block != NULL)
        hb_itemRelease((*callback)->block);
    heap_delete(callback, HbNapCallback);
}

/*---------------------------------------------------------------------------*/

static HbNap *i_hbnap_state_create(void)
{
    const char_t *build_cfg = NULL;
    cassert(HBNAP_GLOBAL == NULL);
    HBNAP_GLOBAL = heap_new0(HbNap);
    HBNAP_GLOBAL->menu_callbacks = arrpt_create(HbNapCallback);
    HBNAP_GLOBAL->date_digits = (hb_setGetCentury() == (HB_BOOL)HB_TRUE) ? 8 : 6;
    HBNAP_GLOBAL->date_chars = HBNAP_GLOBAL->date_digits + 2;

    {
        char_t path[512];
        bfile_dir_work(path, sizeof(path));
        HBNAP_GLOBAL->working_path = str_c(path);
    }

#if defined(__DEBUG__)
    build_cfg = "Debug";
#else
    build_cfg = "Release";
#endif

    {
        const char_t *debpath = deblib_path();
#if defined(__MACOS__)
        HBNAP_GLOBAL->debugger_path = str_cpath("%s/%s/bin/gtnapdeb.app/Contents/MacOS/gtnapdeb", debpath, build_cfg);
#else
        HBNAP_GLOBAL->debugger_path = str_cpath("%s/%s/bin/gtnapdeb", debpath, build_cfg);
#endif
        HBNAP_GLOBAL->debugger_visible = FALSE;
        HBNAP_GLOBAL->debugger = NULL;
    }

    HBNAP_GLOBAL->properties = setst_create(i_prop_cmp, HbNapProp, char_t);
    i_load_properties(HBNAP_GLOBAL->properties);
    deblib_init_colors(i_COLORS);
    return HBNAP_GLOBAL;
}

/*---------------------------------------------------------------------------*/

static void i_hbnap_state_destroy(HbNap **gtnap)
{
    cassert_no_null(gtnap);
    cassert_no_null(*gtnap);
    cassert(*gtnap == HBNAP_GLOBAL);
    cassert(arrpt_size((*gtnap)->menu_callbacks, HbNapCallback) == 0);
    arrpt_destroy(&(*gtnap)->menu_callbacks, i_destroy_hbnap_callback, HbNapCallback);
    str_destroy(&(*gtnap)->working_path);
    str_destroy(&(*gtnap)->debugger_path);

    if ((*gtnap)->debugger != NULL)
        nap_debugger_destroy(&(*gtnap)->debugger);

    setst_destroy(&(*gtnap)->properties, i_remove_property, HbNapProp);
    heap_delete(gtnap, HbNap);
}

/*---------------------------------------------------------------------------*/

static HbNap *i_hbnap_create(void)
{
    i_hbnap_state_create();

    {
        PHB_ITEM ritem = hb_itemDo(INIT_CODEBLOCK, 0);
        hb_itemRelease(ritem);
    }

    hb_itemRelease(INIT_CODEBLOCK);
    INIT_CODEBLOCK = NULL;
    return HBNAP_GLOBAL;
}

/*---------------------------------------------------------------------------*/

static void i_hbnap_update(HbNap *gtnap, const real64_t prtime, const real64_t ctime)
{
    cassert(gtnap == NULL || gtnap == HBNAP_GLOBAL);
    gtnap = HBNAP_GLOBAL;
    cassert_no_null(gtnap);
    unref(prtime);
    unref(ctime);
}

/*---------------------------------------------------------------------------*/

static void i_hbnap_destroy(HbNap **gtnap)
{
    i_hbnap_state_destroy(gtnap);
    nforms_finish();
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
        (FPtr_app_create)i_hbnap_create,
        (FPtr_app_update)i_hbnap_update,
        (FPtr_destroy)i_hbnap_destroy,
        (char_t *)"");
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_exit_app(void)
{
    osapp_finish();
}

/*---------------------------------------------------------------------------*/

void hbnap_init_indirect(void)
{
    cassert(HBNAP_GLOBAL == NULL);
    nforms_start();
    i_hbnap_state_create();
}

/*---------------------------------------------------------------------------*/

void hbnap_exit_indirect(void)
{
    if (HBNAP_GLOBAL != NULL)
        i_hbnap_state_destroy(&HBNAP_GLOBAL);
    nforms_finish();
}

/*---------------------------------------------------------------------------*/

static void i_OnFormMoved(HbNapForm *form, Event *e)
{
    const EvPos *p = event_params(e, EvPos);
    cassert_no_null(form);
    i_write_prop_r32(tc(form->nameid), i_XPOS_PROP, p->x);
    i_write_prop_r32(tc(form->nameid), i_YPOS_PROP, p->y);
}

/*---------------------------------------------------------------------------*/

static void i_OnFormResize(HbNapForm *form, Event *e)
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

HbNapForm *hbnap_forms_load(const char_t *pathname, const char_t *resource_path, const uint32_t flags)
{
    NForm *form = nform_from_file(pathname, NULL);
    if (form != NULL)
    {
        HbNapForm *gtform = heap_new0(HbNapForm);
        str_split_pathext(pathname, NULL, &gtform->nameid, NULL);
        gtform->form = form;
        gtform->respath = str_c(resource_path);
        gtform->binds = arrst_create(HbNapBind);
        gtform->callbacks = arrpt_create(HbNapCallback);

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
                window_OnMoved(gtform->window, listener(gtform, i_OnFormMoved, HbNapForm));
                window_OnResize(gtform->window, listener(gtform, i_OnFormResize, HbNapForm));
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

static void i_remove_fcolumn(HbNapFColumn *column)
{
    cassert_no_null(column);
    if (column->block != NULL)
        hb_itemRelease(column->block);
}

/*---------------------------------------------------------------------------*/

static void i_remove_fnode(HbNapFNode *node)
{
    cassert_no_null(node);
    node->area = NULL;
}

/*---------------------------------------------------------------------------*/

static void i_remove_farea2(HbNapFArea2 *area)
{
    cassert_no_null(area);
    area->area = NULL; /* The life cycle of the area is managed exclusively by Harbour */

    arrst_destroy(&area->columns, i_remove_fcolumn, HbNapFColumn);

    if (area->relkey != NULL)
        hb_itemRelease(area->relkey);
}

/*---------------------------------------------------------------------------*/

static void i_destroy_fdbconn(HbNapFDBConn **dbconn)
{
    cassert_no_null(dbconn);
    (*dbconn)->form = NULL;
    (*dbconn)->table = NULL;
    str_destroy(&(*dbconn)->cellname);

    if ((*dbconn)->tdata != NULL)
        treest_destroy(&(*dbconn)->tdata, i_remove_fnode, HbNapFNode);

    if ((*dbconn)->records != NULL)
        arrst_destroy(&(*dbconn)->records, NULL, uint32_t);

    arrst_destroy(&(*dbconn)->areas, i_remove_farea2, HbNapFArea2);
    heap_delete(dbconn, HbNapFDBConn);
}

/*---------------------------------------------------------------------------*/

static void i_remove_bind(HbNapBind *bind)
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

void hbnap_forms_destroy(HbNapForm **form)
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

    arrst_destroy(&(*form)->binds, i_remove_bind, HbNapBind);
    arrpt_destroy(&(*form)->callbacks, i_destroy_hbnap_callback, HbNapCallback);
    ptr_destopt(i_destroy_fdbconn, &(*form)->dbconn, HbNapFDBConn);
    nform_destroy(&(*form)->form);
    heap_delete(form, HbNapForm);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_title(HbNapForm *form, const char_t *text)
{
    cassert_no_null(form);
    window_title(form->window, text);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_set_text(HbNapForm *form, const char_t *cell, const char_t *text)
{
    cassert_no_null(form);
    nform_set_control_str(form->form, cell, text);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_set_int(HbNapForm *form, const char_t *cell, const int32_t value)
{
    cassert_no_null(form);
    nform_set_control_int(form->form, cell, value);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_insert_text(HbNapForm *form, const char_t *cell, const char_t *text)
{
    cassert_no_null(form);
    nform_add_control_str(form->form, cell, text);
}

/*---------------------------------------------------------------------------*/

int32_t hbnap_forms_get_int(HbNapForm *form, const char_t *cell)
{
    int32_t value = 0;
    cassert_no_null(form);
    if (nform_get_control_int(form->form, cell, &value) == TRUE)
        return value;
    return -1;
}

/*---------------------------------------------------------------------------*/

bool_t hbnap_forms_embed(HbNapForm *form, HbNapForm *embedded_form, const char_t *cell)
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

static const char_t *i_farea_eval_field(HbNapFDBConn *dbconn, const uint32_t field_id, const uint32_t row_id)
{
    uint32_t recno = 0;
    HbNapFArea2 *area = NULL;
    const HbNapFColumn *column = NULL;
    HB_ITEM *ritem = NULL;

    cassert_no_null(dbconn);
    cassert(field_id > 0);
    area = arrst_get(dbconn->areas, 0, HbNapFArea2);

    /* Go to DB record */
    recno = *arrst_get_const(dbconn->records, row_id, uint32_t);
    SELF_GOTO(area->area, recno);

    /* Get the table column */
    column = arrst_get_const(area->columns, field_id - 1, HbNapFColumn);

    /* CodeBlock that computes the cell content */
    ritem = hb_itemDo(column->block, 0);

    /* Fill the temporal cell buffer with cell result */
    hb_item_to_char(ritem, HBNAP_TEMP_BUFFER, sizeof(HBNAP_TEMP_BUFFER), TRUE);

    hb_itemRelease(ritem);
    return HBNAP_TEMP_BUFFER;
}

/*---------------------------------------------------------------------------*/

static void i_OnTableFAreaData(HbNapFDBConn *dbconn, Event *e)
{
    uint32_t etype = event_type(e);
    HbNapFArea2 *area = NULL;
    cassert_no_null(dbconn);
    area = arrst_get(dbconn->areas, 0, HbNapFArea2);

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
        *n = arrst_size(dbconn->records, uint32_t);
        break;
    }

    case ekGUI_EVENT_TBL_CELL:
    {
        EvTbCell *cell = event_result(e, EvTbCell);
        const EvTbPos *pos = event_params(e, EvTbPos);
        cell->text = i_farea_eval_field(dbconn, pos->col + 1, pos->row);
        break;
    }

    default:
        break;
    }
}

/*---------------------------------------------------------------------------*/

static void i_farea_refresh(HbNapFDBConn *dbconn)
{
    HB_ULONG ulCurRec;
    HbNapFArea2 *area = NULL;

    cassert_no_null(dbconn);
    area = arrst_get(dbconn->areas, 0, HbNapFArea2);

    /* Database current RECNO() */
    SELF_RECNO(area->area, &ulCurRec);

    /* Clear the current record index */
    arrst_clear(dbconn->records, NULL, uint32_t);

    /* Generate the record index for TableView */
    {
        HB_BOOL fEof;
        SELF_GOTOP(area->area);
        SELF_EOF(area->area, &fEof);
        while (fEof == HB_FALSE)
        {
            HB_ULONG uiRecNo = 0;
            SELF_RECNO(area->area, &uiRecNo);
            arrst_append(dbconn->records, (uint32_t)uiRecNo, uint32_t);
            SELF_SKIP(area->area, 1);
            SELF_EOF(area->area, &fEof);
        }
    }

    /* Restore database RECNO() */
    SELF_GOTO(area->area, ulCurRec);
}

/*---------------------------------------------------------------------------*/

static uint32_t i_frow_from_recno(HbNapFDBConn *dbconn, const uint32_t recno)
{
    cassert_no_null(dbconn);
    cassert(recno > 0);
    arrst_foreach_const(rec, dbconn->records, uint32_t)
        if (*rec == recno)
            return rec_i;
    arrst_end();
    return UINT32_MAX;
}

/*---------------------------------------------------------------------------*/

static void i_farea_select_row(HbNapFDBConn *dbconn)
{
    HB_ULONG ulCurRec;
    uint32_t sel_row;
    HbNapFArea2 *area = NULL;

    cassert_no_null(dbconn);
    area = arrst_get(dbconn->areas, 0, HbNapFArea2);

    /* Current selected */
    SELF_RECNO(area->area, &ulCurRec);

    sel_row = i_frow_from_recno(dbconn, (uint32_t)ulCurRec);

    tableview_deselect_all(dbconn->table);

    if (sel_row != UINT32_MAX)
    {
        tableview_select(dbconn->table, &sel_row, 1);
        tableview_focus_row(dbconn->table, sel_row, ekTOP);
    }
    /* RECNO() doesn't exists in view (perhaps is deleted) */
    else
    {
        uint32_t nrecs = arrst_size(dbconn->records, uint32_t);
        sel_row = tableview_get_focus_row(dbconn->table);
        /* We move recno to current focused row */
        if (sel_row < nrecs)
        {
            uint32_t recno = *arrst_get_const(dbconn->records, sel_row, uint32_t);
            tableview_select(dbconn->table, &sel_row, 1);
            SELF_GOTO(area->area, recno);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_map_bind_area_to_form(HbNapFDBConn *dbconn)
{
    cassert_no_null(dbconn);
    cassert(dbconn->table == NULL);
    cassert_no_null(dbconn->form);
    dbconn->table = nform_get_tableview(dbconn->form->form, tc(dbconn->cellname));
    if (dbconn->table != NULL)
    {
        tableview_OnData(dbconn->table, listener(dbconn, i_OnTableFAreaData, HbNapFDBConn));
        i_farea_refresh(dbconn);
        tableview_update(dbconn->table);
        i_farea_select_row(dbconn);
    }
}

/*---------------------------------------------------------------------------*/

static HbNapFDBConn *i_create_farea(HbNapForm *form, AREA *area)
{
    HbNapFDBConn *farea = heap_new0(HbNapFDBConn);
    HbNapFArea2 *area2 = NULL;
    farea->form = form;
    farea->cellname = NULL;
    farea->table = NULL;
    farea->tdata = NULL;
    farea->records = arrst_create(uint32_t);
    farea->areas = arrst_create(HbNapFArea2);
    area2 = arrst_new0(farea->areas, HbNapFArea2);
    area2->area = area;
    area2->columns = arrst_create(HbNapFColumn);
    area2->relkey = NULL;
    area2->fexpanded = 0;
    area2->cache_recno = UINT32_MAX;
    return farea;
}

/*---------------------------------------------------------------------------*/

static HbNapFDBConn *i_create_dbconn(HbNapForm *form, const char_t *cell)
{
    HbNapFDBConn *dbconn = heap_new0(HbNapFDBConn);
    dbconn->form = form;
    dbconn->cellname = str_c(cell);
    dbconn->table = NULL;
    dbconn->tdata = treest_create(HbNapFNode);
    dbconn->areas = arrst_create(HbNapFArea2);
    return dbconn;
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

static void i_map_bind_to_form(NForm *form, ArrSt(HbNapBind) *binds)
{
    arrst_foreach(bind, binds, HbNapBind)
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

void hbnap_forms_bind(HbNapForm *form, HB_ITEM *cell_bind)
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
        HbNapBind *bind = arrst_new0(form->binds, HbNapBind);
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

void hbnap_forms_bind_store(HbNapForm *form)
{
    cassert_no_null(form);
    arrst_foreach(bind, form->binds, HbNapBind)
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

void hbnap_forms_area_bind(HbNapForm *form, HB_ITEM *column_bind)
{
    AREA *area = NULL;
    HB_SIZE n = UINT32_MAX;
    cassert_no_null(form);
    cassert(form->dbconn == NULL);
    cassert(HB_ITEM_TYPE(column_bind) == HB_IT_ARRAY);
    n = hb_arrayLen(column_bind);
    cassert(n > 1);

    area = cast(hb_rddGetCurrentWorkAreaPointer(), AREA);
    if (area != NULL)
    {
        HB_SIZE i;
        HbNapFArea2 *area2 = NULL;
        form->dbconn = i_create_farea(form, area);
        area2 = arrst_get(form->dbconn->areas, 0, HbNapFArea2);

        for (i = 2; i <= n; ++i)
        {
            PHB_ITEM bind_item = hb_arrayGetItemPtr(column_bind, i);
            PHB_ITEM block_item = NULL;
            HbNapFColumn *column = NULL;
            /* At the moment, the column-bind item only has one member. The column block */
            cassert(HB_ITEM_TYPE(bind_item) == HB_IT_ARRAY);
            cassert(hb_arrayLen(bind_item) == 1);
            block_item = hb_arrayGetItemPtr(bind_item, 1);
            cassert(HB_ITEM_TYPE(block_item) == HB_IT_BLOCK);
            column = arrst_new0(area2->columns, HbNapFColumn);
            column->block = block_item ? hb_itemNew(block_item) : NULL;
        }

        /* The first element in bind array is the tableView cell name */
        {
            PHB_ITEM name_item = hb_arrayGetItemPtr(column_bind, 1);
            const char *gui_id = NULL;
            cassert(HB_ITEM_TYPE(name_item) == HB_IT_STRING);
            gui_id = hb_itemGetCPtr(name_item);
            form->dbconn->cellname = str_c(cast_const(gui_id, char_t));
            i_map_bind_area_to_form(form->dbconn);
        }
    }
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_area_refresh(HbNapForm *form)
{
    cassert_no_null(form);
    cassert_no_null(form->dbconn);
    i_farea_refresh(form->dbconn);
    tableview_update(form->dbconn->table);
    i_farea_select_row(form->dbconn);
}

/*---------------------------------------------------------------------------*/

uint32_t hbnap_forms_area_recno(HbNapForm *form)
{
    const ArrSt(uint32_t) *sel = 0;
    cassert_no_null(form);
    cassert_no_null(form->dbconn);
    sel = tableview_selected(form->dbconn->table);
    if (arrst_size(sel, uint32_t) == 1)
    {
        uint32_t index = *arrst_first_const(sel, uint32_t);
        return *arrst_get(form->dbconn->records, index, uint32_t);
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

static bool_t i_items_equal(PHB_ITEM a, PHB_ITEM b)
{
    HB_TYPE ta = HB_ITEM_TYPE(a);
    HB_TYPE tb = HB_ITEM_TYPE(b);

    /* Numeric subtypes (INTEGER, LONG, DOUBLE) are interchangeable:
     * DBOI_KEYVAL always returns DOUBLE while code blocks may return INTEGER */
    if ((ta & HB_IT_NUMERIC) && (tb & HB_IT_NUMERIC))
        return (bool_t)(hb_itemGetND(a) == hb_itemGetND(b));

    cassert(ta == tb);

    if (ta & HB_IT_STRING)
        return (bool_t)(hb_stricmp(hb_itemGetCPtr(a), hb_itemGetCPtr(b)) == 0);
    if (ta & HB_IT_LOGICAL)
        return (bool_t)(hb_itemGetL(a) == hb_itemGetL(b));

    cassert(FALSE);
    return FALSE;
}

/*---------------------------------------------------------------------------*/

static uint32_t i_count_db_children(HbNapFArea2 *parea, HB_ULONG precno, HbNapFArea2 *carea)
{
    PHB_ITEM key = NULL;
    HB_BOOL found = HB_FALSE;
    uint32_t count = 0;
    SELF_GOTO(parea->area, precno);
    key = hb_itemDo(parea->relkey, 0);
    SELF_SEEK(carea->area, HB_FALSE, key, HB_FALSE);
    SELF_FOUND(carea->area, &found);
    if (found == HB_TRUE)
    {
        HB_BOOL eof = HB_FALSE;
        SELF_EOF(carea->area, &eof);
        while (eof == HB_FALSE)
        {
            DBORDERINFO kinfo;
            bool_t equ = FALSE;
            memset(&kinfo, 0, sizeof(kinfo));
            kinfo.itmResult = hb_itemNew(NULL);
            SELF_ORDINFO(carea->area, DBOI_KEYVAL, &kinfo);
            equ = i_items_equal(key, kinfo.itmResult);
            hb_itemRelease(kinfo.itmResult);
            if (equ == TRUE)
            {
                SELF_SKIP(carea->area, 1);
                SELF_EOF(carea->area, &eof);
                count++;
            }
            else
            {
                break;
            }
        }
    }

    hb_itemRelease(key);
    return count;
}

/*---------------------------------------------------------------------------*/

static void i_build_children(HbNapFDBConn *dbconn, NodeSt(HbNapFNode) *parent, uint32_t level)
{
    HbNapFArea2 *parea = NULL;
    HbNapFArea2 *carea = NULL;
    HbNapFNode *pdata = NULL;
    PHB_ITEM key = NULL;
    HB_BOOL found = HB_FALSE;

    cassert_no_null(dbconn);
    parea = arrst_get(dbconn->areas, level, HbNapFArea2);
    carea = arrst_get(dbconn->areas, level + 1, HbNapFArea2);
    pdata = treest_node_data(parent, HbNapFNode);

    /* Position parent at its record so relkey block reads the right value */
    SELF_GOTO(parea->area, pdata->recno);

    /* Evaluate parent relkey block, seek key for child area */
    key = hb_itemDo(parea->relkey, 0);

    /*
     * SEEK in child area (order already set).
     * Hard seek (HB_FALSE remains in EOF if it not found)
     * Find last (HB_FALSE find the first record match)
     */
    SELF_SEEK(carea->area, HB_FALSE, key, HB_FALSE);

    SELF_FOUND(carea->area, &found);
    if (found == HB_TRUE)
    {
        HB_BOOL eof = HB_FALSE;
        uint32_t nareas = arrst_size(dbconn->areas, HbNapFArea2);
        SELF_EOF(carea->area, &eof);
        while (eof == HB_FALSE)
        {
            /* Child key must still match parent key (read from active index) */
            DBORDERINFO kinfo;
            bool_t equ = FALSE;
            memset(&kinfo, 0, sizeof(kinfo));
            kinfo.itmResult = hb_itemNew(NULL);
            SELF_ORDINFO(carea->area, DBOI_KEYVAL, &kinfo);
            equ = i_items_equal(key, kinfo.itmResult);
            hb_itemRelease(kinfo.itmResult);

            if (equ == TRUE)
            {
                /* Add child node to tree */
                NodeSt(HbNapFNode) *child = treest_node_insert(parent, UINT32_MAX, HbNapFNode);
                HbNapFNode *cdata = treest_node_data(child, HbNapFNode);
                HB_ULONG crecno = 0;

                SELF_RECNO(carea->area, &crecno);
                cdata->area = carea;
                cdata->recno = crecno;
                if (carea->fexpanded != 0)
                {
                    PHB_ITEM pExp = hb_itemNew(NULL);
                    SELF_GETVALUE(carea->area, carea->fexpanded, pExp);
                    cdata->expanded = (bool_t)hb_itemGetL(pExp);
                    hb_itemRelease(pExp);
                }
                else
                {
                    cdata->expanded = FALSE;
                }

                cdata->nchildren = 0;
                /* Exists grandchildren */
                if (level + 2 < nareas)
                {
                    /* Recurse for grandchildren only if expanded, collapsed nodes get a count without building */
                    if (cdata->expanded == TRUE)
                    {
                        i_build_children(dbconn, child, level + 1);
                    }
                    else
                    {
                        HbNapFArea2 *gcarea = arrst_get(dbconn->areas, level + 2, HbNapFArea2);
                        cdata->nchildren = i_count_db_children(carea, crecno, gcarea);
                    }
                }

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

static uint32_t i_area_level(const HbNapFDBConn *dbconn, const HbNapFArea2 *area)
{
    cassert_no_null(dbconn);
    arrst_foreach_const(larea, dbconn->areas, HbNapFArea2)
        if (larea == area)
            return larea_i;
    arrst_end();
    cassert(FALSE);
    return UINT32_MAX;
}

/*---------------------------------------------------------------------------*/

static void i_OnTreeFAreaData(HbNapFDBConn *dbconn, Event *e)
{
    uint32_t etype = event_type(e);
    cassert_no_null(dbconn);

    switch (etype)
    {

    case ekGUI_EVENT_TBL_NROOTS:
    {
        uint32_t *nroots = event_result(e, uint32_t);
        NodeSt(HbNapFNode) *root = treest_root_get(dbconn->tdata, HbNapFNode);
        *nroots = root ? treest_node_size(root, HbNapFNode) : 0;
        break;
    }

    case ekGUI_EVENT_TBL_NODEINFO:
    {
        const EvTbNode *node = event_params(e, EvTbNode);
        EvTbNodeInfo *info = event_result(e, EvTbNodeInfo);
        NodeSt(HbNapFNode) *parent = cast(node->parent, NodeSt(HbNapFNode));
        NodeSt(HbNapFNode) *child = NULL;
        HbNapFNode *data = NULL;

        if (parent == NULL)
            parent = treest_root_get(dbconn->tdata, HbNapFNode);

        child = treest_node_get(parent, node->child, HbNapFNode);
        data = treest_node_data(child, HbNapFNode);
        info->node = child;

        {
            /* If children are built, use tree count, otherwise use the pre-counted DB children */
            uint32_t nc = treest_node_size(child, HbNapFNode);
            cassert((nc > 0 && data->nchildren == 0) || nc == 0);
            info->nchildren = (nc > 0) ? nc : data->nchildren;
        }

        info->expanded = data->expanded;
        break;
    }

    case ekGUI_EVENT_TBL_EXPAND:
    {
        const EvTbExpand *p = event_params(e, EvTbExpand);
        NodeSt(HbNapFNode) *node = cast(p->node, NodeSt(HbNapFNode));
        HbNapFNode *data = treest_node_data(node, HbNapFNode);
        cassert_no_null(data);
        cassert_no_null(data->area);
        data->expanded = p->expanded;

        /* Expanding a node whose subtree was counted but not built */
        if (p->expanded == TRUE && data->nchildren > 0 && treest_node_size(node, HbNapFNode) == 0)
        {
            uint32_t level = i_area_level(dbconn, data->area);
            i_build_children(dbconn, node, level);
        }

        /* Mark expanded/collapsed in DB */
        if (data->area->fexpanded != 0)
        {
            DBLOCKINFO linfo;
            HB_ERRCODE hbres;
            PHB_ITEM pVal = hb_itemPutL(NULL, (HB_BOOL)p->expanded);
            SELF_GOTO(data->area->area, data->recno);
            memset(&linfo, 0, sizeof(linfo));
            linfo.uiMethod = REC_LOCK;
            hbres = SELF_LOCK(data->area->area, &linfo);
            cassert_unref(hbres == HB_SUCCESS, hbres);

            if (linfo.fResult)
            {
                hbres = SELF_PUTVALUE(data->area->area, data->area->fexpanded, pVal);
                cassert_unref(hbres == HB_SUCCESS, hbres);
                linfo.uiMethod = REC_UNLOCK;
                hbres = SELF_LOCK(data->area->area, &linfo);
                cassert_unref(hbres == HB_SUCCESS, hbres);
            }

            hb_itemRelease(pVal);
        }
        break;
    }

    case ekGUI_EVENT_TBL_CELL:
    {
        const EvTbPos *pos = event_params(e, EvTbPos);
        NodeSt(HbNapFNode) *node = cast(pos->node, NodeSt(HbNapFNode));
        HbNapFNode *data = treest_node_data(node, HbNapFNode);
        const HbNapFColumn *col = arrst_get_const(data->area->columns, pos->col, HbNapFColumn);

        /* Position current node cursor */
        SELF_GOTO(data->area->area, data->recno);

        /* Position all ancestor cursors (bottom-up walk, skip virtual root) */
        {
            NodeSt(HbNapFNode) *anode = treest_node_parent(node, HbNapFNode);
            while (anode != NULL)
            {
                HbNapFNode *adata = treest_node_data(anode, HbNapFNode);
                if (adata->area != NULL)
                    SELF_GOTO(adata->area->area, adata->recno);
                anode = treest_node_parent(anode, HbNapFNode);
            }
        }

        {
            EvTbCell *cell = event_result(e, EvTbCell);
            PHB_ITEM ritem = hb_itemDo(col->block, 0);
            hb_item_to_char(ritem, HBNAP_TEMP_BUFFER, sizeof(HBNAP_TEMP_BUFFER), TRUE);
            hb_itemRelease(ritem);
            cell->text = HBNAP_TEMP_BUFFER;
            cell->align = col->align;
        }

        break;
    }

    default:
        break;
    }
}

/*---------------------------------------------------------------------------*/

static HB_USHORT i_area_expanded_field(AREA *area)
{
    HB_USHORT n = 0;
    HB_USHORT i;
    char szName[64];
    SELF_FIELDCOUNT(area, &n);
    for (i = 1; i <= n; ++i)
    {
        SELF_FIELDNAME(area, i, szName);
        if (hb_stricmp(szName, "EXPANDED") == 0)
        {
            PHB_ITEM pType = hb_itemNew(NULL);
            bool_t islogic = FALSE;
            SELF_FIELDINFO(area, i, DBS_TYPE, pType);
            islogic = (bool_t)(hb_itemGetCPtr(pType)[0] == 'L');
            hb_itemRelease(pType);
            return islogic ? i : 0;
        }
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

static void i_dbconn_refresh(HbNapFDBConn *dbconn)
{
    uint32_t nareas = 0;
    NodeSt(HbNapFNode) *root = NULL;
    HbNapFNode *vdata = NULL;
    HbNapFArea2 *area = NULL;
    HB_BOOL eof = HB_FALSE;

    cassert_no_null(dbconn);
    treest_clear(dbconn->tdata, i_remove_fnode, HbNapFNode);
    nareas = arrst_size(dbconn->areas, HbNapFArea2);
    cassert(nareas > 0);

    /* Virtual root: invisible container for all top-level nodes */
    root = treest_root_new(dbconn->tdata, HbNapFNode);
    vdata = treest_node_data(root, HbNapFNode);
    vdata->area = NULL;
    vdata->recno = UINT32_MAX;
    vdata->expanded = TRUE;

    /* Navigate root area and build one top-level node per record */
    area = arrst_get(dbconn->areas, 0, HbNapFArea2);
    SELF_GOTOP(area->area);
    SELF_EOF(area->area, &eof);

    while (eof == HB_FALSE)
    {
        HB_ULONG recno = 0;
        NodeSt(HbNapFNode) *node = NULL;
        HbNapFNode *data = NULL;

        SELF_RECNO(area->area, &recno);
        node = treest_node_insert(root, UINT32_MAX, HbNapFNode);
        data = treest_node_data(node, HbNapFNode);
        data->area = area;
        data->recno = recno;
        if (area->fexpanded != 0)
        {
            PHB_ITEM pExp = hb_itemNew(NULL);
            SELF_GETVALUE(area->area, area->fexpanded, pExp);
            data->expanded = (bool_t)hb_itemGetL(pExp);
            hb_itemRelease(pExp);
        }
        else
        {
            data->expanded = FALSE;
        }

        /* Exists node hierarchy */
        data->nchildren = 0;
        if (nareas > 1)
        {
            /* Only recurse into children if this node is expanded */
            if (data->expanded == TRUE)
            {
                i_build_children(dbconn, node, 0);
            }
            else
            {
                HbNapFArea2 *carea = arrst_get(dbconn->areas, 1, HbNapFArea2);
                data->nchildren = i_count_db_children(area, recno, carea);
            }
        }

        /* Restore root cursor after recursion, then advance */
        SELF_GOTO(area->area, recno);
        SELF_SKIP(area->area, 1);
        SELF_EOF(area->area, &eof);
    }
}

/*---------------------------------------------------------------------------*/

static void i_map_dbconn_to_form(HbNapFDBConn *dbconn)
{
    cassert_no_null(dbconn);
    cassert(dbconn->table == NULL);
    cassert_no_null(dbconn->form);
    dbconn->table = nform_get_tableview(dbconn->form->form, tc(dbconn->cellname));
    if (dbconn->table != NULL)
    {
        tableview_tree(dbconn->table, 0);
        tableview_OnData(dbconn->table, listener(dbconn, i_OnTreeFAreaData, HbNapFDBConn));
        i_dbconn_refresh(dbconn);
        tableview_update(dbconn->table);
    }
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_tree_bind(HbNapForm *form, const char_t *cell, HB_ITEM *areas, HB_ITEM *relations, HB_ITEM *columns)
{
    HB_SIZE nA = UINT32_MAX;
    HB_SIZE nR = UINT32_MAX;
    HB_SIZE nB = UINT32_MAX;
    HB_SIZE i = 0;
    cassert_no_null(form);
    cassert(form->dbconn == NULL);
    cassert(HB_ITEM_TYPE(areas) == HB_IT_ARRAY);
    cassert(HB_ITEM_TYPE(relations) == HB_IT_ARRAY);
    cassert(HB_ITEM_TYPE(columns) == HB_IT_ARRAY);
    form->dbconn = i_create_dbconn(form, cell);
    nA = hb_arrayLen(areas);
    nR = hb_arrayLen(relations);
    nB = hb_arrayLen(columns);
    cassert(nA > 1);
    cassert_unref(nR == nA - 1, nR);
    cassert_unref(nB == nA, nB);

    for (i = 1; i <= nA; ++i)
    {
        HbNapFArea2 *area = arrst_new0(form->dbconn->areas, HbNapFArea2);

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
            area->fexpanded = i_area_expanded_field(area->area);
        }

        /* Area->Columns data mappings */
        {
            PHB_ITEM bind_item = hb_arrayGetItemPtr(columns, i);
            HB_SIZE j, nCols = UINT32_MAX;
            area->columns = arrst_create(HbNapFColumn);
            cassert(HB_ITEM_TYPE(bind_item) == HB_IT_ARRAY);
            nCols = hb_arrayLen(bind_item);
            for (j = 1; j <= nCols; ++j)
            {
                HbNapFColumn *col = arrst_new0(area->columns, HbNapFColumn);
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

        /* Relation with next area: { parent_key_block, "cdx_path", "CHILD_TAG" } */
        if (i < nA)
        {
            PHB_ITEM rel_item = hb_arrayGetItemPtr(relations, i);
            PHB_ITEM key_item = NULL;
            PHB_ITEM cdx_item = NULL;
            PHB_ITEM tag_item = NULL;
            const char *cdx_path = NULL;
            const char *tag_name = NULL;
            cassert(HB_ITEM_TYPE(rel_item) == HB_IT_ARRAY);
            cassert(hb_arrayLen(rel_item) == 3);
            key_item = hb_arrayGetItemPtr(rel_item, 1);
            cdx_item = hb_arrayGetItemPtr(rel_item, 2);
            tag_item = hb_arrayGetItemPtr(rel_item, 3);
            cassert(HB_ITEM_TYPE(key_item) == HB_IT_BLOCK);
            cassert(HB_ITEM_TYPE(cdx_item) == HB_IT_STRING);
            cassert(HB_ITEM_TYPE(tag_item) == HB_IT_STRING);
            area->relkey = hb_itemNew(key_item);
            cdx_path = hb_itemGetCPtr(cdx_item);
            tag_name = hb_itemGetCPtr(tag_item);

            /* Open CDX and activate tag on the child area */
            {
                HB_ERRCODE hbres;
                int iCArea = 0;
                AREA *carea = NULL;

                {
                    PHB_ITEM carea_item = hb_arrayGetItemPtr(areas, i + 1);
                    cassert(HB_ITEM_TYPE(carea_item) == HB_IT_STRING);
                    hbres = hb_rddGetAliasNumber(hb_itemGetCPtr(carea_item), &iCArea);
                    cassert_unref(hbres == HB_SUCCESS, hbres);
                    carea = cast(hb_rddGetWorkAreaPointer(iCArea), AREA);
                    cassert_no_null(carea);
                }

                /*
                 * Open CDX and activate the specified tag.
                 * SELF_ORDLSTADD evaluates the key expression via the Harbour VM
                 * macro compiler, which resolves fields against the CURRENTLY
                 * SELECTED work area — not the area pointer argument. We must
                 * select the child area first so field names resolve correctly.
                 */
                {
                    DBORDERINFO oinfo;
                    int iSavedArea = hb_rddGetCurrentWorkAreaNumber();
                    hbres = hb_rddSelectWorkAreaNumber(iCArea);
                    cassert_unref(hbres == HB_SUCCESS, hbres);
                    hbres = SELF_ORDLSTCLEAR(carea);
                    cassert_unref(hbres == HB_SUCCESS, hbres);
                    memset(&oinfo, 0, sizeof(oinfo));
                    oinfo.atomBagName = hb_itemPutC(NULL, cdx_path);
                    oinfo.itmResult = hb_itemNew(NULL);
                    hbres = SELF_ORDLSTADD(carea, &oinfo);
                    cassert_unref(hbres == HB_SUCCESS, hbres);
                    hb_itemRelease(oinfo.atomBagName);
                    oinfo.atomBagName = NULL;
                    oinfo.itmOrder = hb_itemPutC(NULL, tag_name);
                    hbres = SELF_ORDLSTFOCUS(carea, &oinfo);
                    cassert_unref(hbres == HB_SUCCESS, hbres);
                    hb_itemRelease(oinfo.itmOrder);
                    hb_itemRelease(oinfo.itmResult);

                    /*
                     * ORDLSTFOCUS returns HB_SUCCESS even if tag name does not exist
                     * verify by checking the active order number
                     */
                    {
                        DBORDERINFO vinfo;
                        int pos = 0;
                        memset(&vinfo, 0, sizeof(vinfo));
                        vinfo.itmResult = hb_itemNew(NULL);
                        hbres = SELF_ORDINFO(carea, DBOI_NUMBER, &vinfo);
                        cassert_unref(hbres == HB_SUCCESS, hbres);
                        pos = hb_itemGetNI(vinfo.itmResult);
                        cassert_unref(pos != 0, pos);
                        hb_itemRelease(vinfo.itmResult);
                    }

                    hbres = hb_rddSelectWorkAreaNumber(iSavedArea);
                    cassert_unref(hbres == HB_SUCCESS, hbres);
                }
            }
        }
        else
        {
            area->relkey = NULL;
        }
    }

    i_map_dbconn_to_form(form->dbconn);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_item_list(HbNapForm *form, const char_t *cell, HB_ITEM *items)
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

static HbNapCallback *i_create_hbnap_callback(HB_ITEM *block, ArrPt(HbNapCallback) *callbacks)
{
    HbNapCallback *callback = heap_new0(HbNapCallback);
    callback->block = block ? hb_itemNew(block) : NULL;
    callback->key = INT32_MAX;
    callback->autoclose_id = UINT32_MAX;
    arrpt_append(callbacks, callback, HbNapCallback);
    return callback;
}

/*---------------------------------------------------------------------------*/

static Listener *i_hbnap_form_listener(HB_ITEM *block, HbNapForm *form, FPtr_hbnap_callback func_callback)
{
    HbNapCallback *callback;
    cassert_no_null(form);
    callback = i_create_hbnap_callback(block, form->callbacks);
    callback->form = form;
    return listener(callback, func_callback, HbNapCallback);
}

/*---------------------------------------------------------------------------*/

static void i_OnFormButtonClick(HbNapCallback *callback, Event *e)
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

void hbnap_forms_OnClick(HbNapForm *form, const char_t *cell, HB_ITEM *click_block)
{
    Listener *listener = i_hbnap_form_listener(click_block, form, i_OnFormButtonClick);
    cassert_no_null(form);
    nform_set_listener(form->form, cell, listener);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_maximize(HbNapForm *form)
{
    cassert_no_null(form);
    window_maximize(form->window);
}

/*---------------------------------------------------------------------------*/

static void i_OnFormClose(HbNapForm *form, Event *e)
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

static void i_form_frame(HbNapForm *form, Window *parent)
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

void hbnap_forms_show(HbNapForm *form, HB_ITEM *onclose_block)
{
    cassert_no_null(form);
    if (form->OnClose_block != NULL)
        hb_itemRelease(form->OnClose_block);

    form->OnClose_block = hb_itemNew(onclose_block);
    window_OnClose(form->window, listener(form, i_OnFormClose, HbNapForm));
    window_update(form->window);
    i_form_frame(form, NULL);
    window_show(form->window);
}

/*---------------------------------------------------------------------------*/

static uint32_t i_forms_modal(HbNapForm *form, Window *parent)
{
    cassert_no_null(form);
    window_update(form->window);
    i_form_frame(form, parent);
    form->modal_ret = window_modal(form->window, parent);
    return form->modal_ret;
}

/*---------------------------------------------------------------------------*/

uint32_t hbnap_forms_modal(HbNapForm *form, HbNapForm *parent)
{
    cassert_no_null(parent);
    return i_forms_modal(form, parent->window);
}

/*---------------------------------------------------------------------------*/

uint32_t hbnap_forms_modal_gtnap(HbNapForm *form)
{
    Window *parent = gtnap_current_window();
    cassert_no_null(parent);
    return i_forms_modal(form, parent);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_stop_modal(HbNapForm *form, const uint32_t value)
{
    cassert_no_null(form);
    window_stop_modal(form->window, value);
}

/*---------------------------------------------------------------------------*/

R2Df hbnap_forms_control_frame(HbNapForm *form, const char_t *cell)
{
    cassert_no_null(form);
    return nform_get_control_frame(form->form, cell, form->window);
}

/*---------------------------------------------------------------------------*/

void hbnap_forms_update(HbNapForm *form)
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

static void i_OnDrawMainView(HbNapForm *form, Event *e)
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

static void i_OnSizeMainView(HbNapForm *form, Event *e)
{
    const EvSize *p = event_params(e, EvSize);
    View *view = event_sender(e, View);
    i_mainitems_locations(view, p->width);
    unref(form);
}

/*---------------------------------------------------------------------------*/

static void i_OnMoveMainView(HbNapForm *form, Event *e)
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

static void i_OnClickMainView(HbNapForm *form, Event *e)
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

void hbnap_forms_main_cover(HbNapForm *form, const char_t *canvas_cell, const char_t *title, const char_t *logo_path, HB_ITEM *cover_items)
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
    view_OnDraw(view, listener(form, i_OnDrawMainView, HbNapForm));
    view_OnSize(view, listener(form, i_OnSizeMainView, HbNapForm));
    view_OnMove(view, listener(form, i_OnMoveMainView, HbNapForm));
    view_OnClick(view, listener(form, i_OnClickMainView, HbNapForm));
    view_get_size(view, &view_size);
    i_mainitems_locations(view, view_size.width);
}

/*---------------------------------------------------------------------------*/

HbNapMenu *hbnap_menu_create(void)
{
    Menu *menu = menu_create();
    return cast(menu, HbNapMenu);
}

/*---------------------------------------------------------------------------*/

static void i_remove_menu_callbacks(Menu *menu);

/*---------------------------------------------------------------------------*/

static void i_remove_item_callbacks(MenuItem *item)
{
    Menu *submenu = menuitem_get_submenu(item);
    uint32_t callback_id = UINT32_MAX;

    arrpt_foreach(callback, HBNAP_GLOBAL->menu_callbacks, HbNapCallback)
        if (callback->menuitem == cast(item, HbNapMenuItem))
        {
            callback_id = callback_i;
            break;
        }
    arrpt_end()

    if (callback_id != UINT32_MAX)
        arrpt_delete(HBNAP_GLOBAL->menu_callbacks, callback_id, i_destroy_hbnap_callback, HbNapCallback);

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

void hbnap_menu_destroy(HbNapMenu *menu)
{
    i_remove_menu_callbacks(cast(menu, Menu));
    menu_destroy(dcast(&menu, Menu));
}

/*---------------------------------------------------------------------------*/

void hbnap_menu_add_item(HbNapMenu *menu, HbNapMenuItem *item)
{
    menu_add_item(cast(menu, Menu), cast(item, MenuItem));
}

/*---------------------------------------------------------------------------*/

void hbnap_menu_ins_item(HbNapMenu *menu, const uint32_t pos, HbNapMenuItem *item)
{
    menu_ins_item(cast(menu, Menu), pos, cast(item, MenuItem));
}

/*---------------------------------------------------------------------------*/

void hbnap_menu_del_item(HbNapMenu *menu, const uint32_t pos)
{
    MenuItem *item = menu_get_item(cast(menu, Menu), pos);
    i_remove_item_callbacks(item);
    menu_del_item(cast(menu, Menu), pos);
}

/*---------------------------------------------------------------------------*/

uint32_t hbnap_menu_count(const HbNapMenu *menu)
{
    return menu_count(cast(menu, Menu));
}

/*---------------------------------------------------------------------------*/

HbNapMenuItem *hbnap_menu_get_item(HbNapMenu *menu, const uint32_t index)
{
    MenuItem *item = menu_get_item(cast(menu, Menu), index);
    return cast(item, HbNapMenuItem);
}

/*---------------------------------------------------------------------------*/

void hbnap_menubar(HbNapMenu *menu, HbNapForm *form)
{
    cassert_no_null(form);
    osapp_menubar(cast(menu, Menu), form->window);
}

/*---------------------------------------------------------------------------*/

bool_t hbnap_is_menubar(const HbNapMenu *menu)
{
    return menu_is_menubar(cast_const(menu, Menu));
}

/*---------------------------------------------------------------------------*/

void hbnap_menu_popup(HbNapMenu *menu, HbNapForm *form, const int32_t x, const int32_t y)
{
    cassert_no_null(form);
    menu_launch(cast(menu, Menu), form->window, v2df((real32_t)x, (real32_t)y));
}

/*---------------------------------------------------------------------------*/

static void i_OnMenuClick(HbNapCallback *callback, Event *e)
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

static Listener *i_hbnap_menu_listener(HB_ITEM *block, HbNapMenuItem *item)
{
    HbNapCallback *callback;
    cassert_no_null(item);
    callback = i_create_hbnap_callback(block, HBNAP_GLOBAL->menu_callbacks);
    callback->menuitem = item;
    return listener(callback, i_OnMenuClick, HbNapCallback);
}

/*---------------------------------------------------------------------------*/

HbNapMenuItem *hbnap_menuitem_create(const char_t *text, const char_t *icon_path, HB_ITEM *click_block)
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
        Listener *listener = i_hbnap_menu_listener(click_block, cast(item, HbNapMenuItem));
        menuitem_OnClick(item, listener);
    }

    return cast(item, HbNapMenuItem);
}

/*---------------------------------------------------------------------------*/

HbNapMenuItem *hbnap_menuitem_separator(void)
{
    MenuItem *item = menuitem_separator();
    return cast(item, HbNapMenuItem);
}

/*---------------------------------------------------------------------------*/

void hbnap_menuitem_submenu(HbNapMenuItem *item, HbNapMenu *submenu)
{
    menuitem_submenu(cast(item, MenuItem), dcast(&submenu, Menu));
}

/*---------------------------------------------------------------------------*/

const char_t *hbnap_menuitem_get_text(const HbNapMenuItem *item)
{
    return menuitem_get_text(cast_const(item, MenuItem));
}

/*---------------------------------------------------------------------------*/

HbNapMenu *hbnap_menuitem_get_submenu(HbNapMenuItem *item)
{
    Menu *menu = menuitem_get_submenu(cast(item, MenuItem));
    return cast(menu, HbNapMenu);
}
