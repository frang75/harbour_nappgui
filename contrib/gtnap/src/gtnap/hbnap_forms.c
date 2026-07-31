/*
 * HBNAP Forms API. Support for new full graphics applications.
 */

#include "hbnap.h"
#include "gtnap.inl"

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_INIT_APP)
{
    PHB_ITEM main_block = hb_param(1, HB_IT_BLOCK);
    hbnap_forms_init_app(main_block);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_EXIT_APP)
{
    hbnap_forms_exit_app();
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_LOAD)
{
    const char_t *pathname = hb_parcx(1);
    const char_t *resource_path = hb_parcx(2);
    uint32_t flags = hb_parni(3);
    HbNapForm *form = hbnap_forms_load(pathname, resource_path, flags);
    hb_retptr(form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_DESTROY)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    hbnap_forms_destroy(&form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_TITLE)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    const char_t *text = hb_parcx(2);
    hbnap_forms_title(form, text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_SET_TEXT)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    const char_t *cell = hb_parcx(2);
    const char_t *text = hb_parcx(3);
    hbnap_forms_set_text(form, cell, text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_SET_INT)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    const char_t *cell = hb_parcx(2);
    int32_t value = (int32_t)hb_parni(3);
    hbnap_forms_set_int(form, cell, value);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_INSERT_TEXT)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    const char_t *cell = hb_parcx(2);
    const char_t *text = hb_parcx(3);
    hbnap_forms_insert_text(form, cell, text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_GET_INT)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    const char_t *cell = hb_parcx(2);
    int32_t value = hbnap_forms_get_int(form, cell);
    hb_retni(value);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_EMBED)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    HbNapForm *embedded_form = cast(hb_parptr(2), HbNapForm);
    const char_t *cell = hb_parcx(3);
    bool_t ok = hbnap_forms_embed(form, embedded_form, cell);
    hb_retl(ok);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_BIND)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    HB_ITEM *cell_bind = hb_param(2, HB_IT_ARRAY);
    hbnap_forms_bind(form, cell_bind);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_BIND_STORE)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    hbnap_forms_bind_store(form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_TABLE_BIND)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    const char_t *cell = hb_parcx(2);
    HB_ITEM *area = hb_param(3, HB_IT_STRING);
    HB_ITEM *columns = hb_param(4, HB_IT_ARRAY);
    hbnap_forms_table_bind(form, cell, area, columns);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_AREA_REFRESH)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    hbnap_forms_area_refresh(form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_AREA_RECNO)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    uint32_t recno = hbnap_forms_area_recno(form);
    hb_retni(recno);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_TREE_BIND)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    const char_t *cell = hb_parcx(2);
    HB_ITEM *areas = hb_param(3, HB_IT_ARRAY);
    HB_ITEM *relations = hb_param(4, HB_IT_ARRAY);
    HB_ITEM *columns = hb_param(5, HB_IT_ARRAY);
    hbnap_forms_tree_bind(form, cell, areas, relations, columns);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_ITEM_LIST)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    const char_t *cell = hb_parcx(2);
    HB_ITEM *items = hb_param(3, HB_IT_ARRAY);
    hbnap_forms_item_list(form, cell, items);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_ONCLICK)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    const char_t *cell = hb_parcx(2);
    HB_ITEM *block = hb_param(3, HB_IT_BLOCK);
    hbnap_forms_OnClick(form, cell, block);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_MAXIMIZE)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    hbnap_forms_maximize(form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_SHOW)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    HB_ITEM *onclose_block = hb_param(2, HB_IT_BLOCK);
    hbnap_forms_show(form, onclose_block);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_MODAL)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    HbNapForm *parent = cast(hb_parptr(2), HbNapForm);
    uint32_t ret = hbnap_forms_modal(form, parent);
    hb_retni(ret);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_MODAL_GTNAP)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    uint32_t ret = hbnap_forms_modal_gtnap(form);
    hb_retni(ret);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_STOP_MODAL)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    uint32_t value = hb_parni(2);
    hbnap_forms_stop_modal(form, value);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_CONTROL_FRAME)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    const char_t *cell_name = hb_parcx(2);
    R2Df frame = hbnap_forms_control_frame(form, cell_name);
    hb_reta(4);
    hb_storvnl((int32_t)frame.pos.x, -1, 1);
    hb_storvnl((int32_t)frame.pos.y, -1, 2);
    hb_storvnl((int32_t)frame.size.width, -1, 3);
    hb_storvnl((int32_t)frame.size.height, -1, 4);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_UPDATE)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    hbnap_forms_update(form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_MAIN_COVER)
{
    HbNapForm *form = cast(hb_parptr(1), HbNapForm);
    const char_t *canvas_cell = hb_parcx(2);
    const char_t *title = hb_parcx(3);
    const char_t *logo_path = hb_parcx(4);
    HB_ITEM *cover_items = hb_param(5, HB_IT_ARRAY);
    hbnap_forms_main_cover(form, canvas_cell, title, logo_path, cover_items);
}

