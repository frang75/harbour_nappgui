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
    HB_ITEM *text_block = hb_param(2, HB_IT_BLOCK | HB_IT_STRING);
    hbnap_forms_title(form, text_block);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_SHOW)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    HB_ITEM *onclose_block = hb_param(2, HB_IT_BLOCK);
    hbnap_forms_show(form, onclose_block);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_FORMS_MAIN_COVER)
{
    GtNapForm *form = cast(hb_parptr(1), GtNapForm);
    const char_t *canvas_cell = hb_parcx(2);
    const char_t *title = hb_parcx(3);
    const char_t *logo_path = hb_parcx(4);
    hbnap_forms_main_cover(form, canvas_cell, title, logo_path);
}

