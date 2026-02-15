/*
* GTNAP Forms API. Support for new full graphics applications.
* New API, not valid for semi-graphic applications
*/

#include "gtnap.h"
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
    GtNapForm *form = hbnap_forms_load(pathname, resource_path, flags);
    hb_retptr(form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_DESTROY)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    hbnap_forms_destroy(&form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_TITLE)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    const char_t *text = hb_parcx(2);
    hbnap_forms_title(form, text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_SET_TEXT)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    const char_t *cell = hb_parcx(2);
    const char_t *text = hb_parcx(3);
    hbnap_forms_set_text(form, cell, text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_BIND_AREA)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    HB_ITEM *column_bind = hb_param(2, HB_IT_ARRAY);
    hbnap_forms_bind_area(form, column_bind);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_ONCLICK)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    const char_t *cell = hb_parcx(2);
    HB_ITEM *block = hb_param(3, HB_IT_BLOCK);
    hbnap_forms_OnClick(form, cell, block);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_MAXIMIZE)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    hbnap_forms_maximize(form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_SHOW)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    HB_ITEM *onclose_block = hb_param(2, HB_IT_BLOCK);
    hbnap_forms_show(form, onclose_block);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_MODAL)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    GtNapForm *parent = cast(hb_parptr(2), GtNapForm);
    uint32_t ret = hbnap_forms_modal(form, parent);
    hb_retni(ret);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_MODAL_GTNAP)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    uint32_t ret = hbnap_forms_modal_gtnap(form);
    hb_retni(ret);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_STOP_MODAL)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    uint32_t value = hb_parni(2);
    hbnap_forms_stop_modal(form, value);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_MAIN_COVER)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    const char_t *canvas_cell = hb_parcx(2);
    const char_t *title = hb_parcx(3);
    const char_t *logo_path = hb_parcx(4);
    HB_ITEM *cover_items = hb_param(5, HB_IT_ARRAY);
    hbnap_forms_main_cover(form, canvas_cell, title, logo_path, cover_items);
}

