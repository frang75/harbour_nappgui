/*
    This is part of gtnap
*/

#include "gtnap.h"
#include "nappgui.h"

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_CREATE )
{
    Edit *edit = edit_create();
    edit_font(edit, hb_gtnap_global_font());
    hb_retptr(edit);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_MULTILINE )
{
    Edit *edit = edit_multiline();
    hb_retptr(edit);
}

/*---------------------------------------------------------------------------*/

static void i_OnEditFilter(GtNapCallback *idp, Event *e)
{
    hb_gtnap_callback(idp, e);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_ONFILTER )
{
    Edit *edit = (Edit*)hb_parptr(1);
    Listener *listener = hb_gtnap_comp_listener(2, (GuiComponent*)edit, i_OnEditFilter);
    edit_OnFilter(edit, listener);
}

/*---------------------------------------------------------------------------*/

static void i_OnEditChange(GtNapCallback *idp, Event *e)
{
    hb_gtnap_callback(idp, e);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_ONCHANGE )
{
    Edit *edit = (Edit*)hb_parptr(1);
    Listener *listener = hb_gtnap_comp_listener(2, (GuiComponent*)edit, i_OnEditChange);
    edit_OnChange(edit, listener);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_TEXT )
{
    Edit *edit = (Edit*)hb_parptr(1);
    const char_t *text = hb_gtnap_parText(2);
    edit_text(edit, text);
}

/*---------------------------------------------------------------------------*/

// Images & Fonts not supported yet!

//void edit_font(Edit *edit, const Font *font);

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_ALIGN )
{
    Edit *edit = (Edit*)hb_parptr(1);
    align_t align = (align_t)hb_parni(2);
    edit_align(edit, align);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_PASSMODE )
{
    Edit *edit = (Edit*)hb_parptr(1);
    bool_t passmode = (bool_t)hb_parl(2);
    edit_passmode(edit, passmode);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_EDITABLE )
{
    Edit *edit = (Edit*)hb_parptr(1);
    bool_t is_editable = (bool_t)hb_parl(2);
    edit_editable(edit, is_editable);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_AUTOSELECT )
{
    Edit *edit = (Edit*)hb_parptr(1);
    bool_t autoselect = (bool_t)hb_parl(2);
    edit_autoselect(edit, autoselect);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_TOOLTIP )
{
    Edit *edit = (Edit*)hb_parptr(1);
    const char_t *text = hb_gtnap_parText(2);
    edit_tooltip(edit, text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_COLOR )
{
    Edit *edit = (Edit*)hb_parptr(1);
    color_t color = (color_t)hb_parni(2);
    edit_color(edit, color);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_COLOR_FOCUS )
{
    Edit *edit = (Edit*)hb_parptr(1);
    color_t color = (color_t)hb_parni(2);
    edit_color_focus(edit, color);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_BGCOLOR )
{
    Edit *edit = (Edit*)hb_parptr(1);
    color_t color = (color_t)hb_parni(2);
    edit_bgcolor(edit, color);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_BGCOLOR_FOCUS )
{
    Edit *edit = (Edit*)hb_parptr(1);
    color_t color = (color_t)hb_parni(2);
    edit_bgcolor_focus(edit, color);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_PHTEXT )
{
    Edit *edit = (Edit*)hb_parptr(1);
    const char_t *text = hb_gtnap_parText(2);
    edit_phtext(edit, text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_PHCOLOR )
{
    Edit *edit = (Edit*)hb_parptr(1);
    color_t color = (color_t)hb_parni(2);
    edit_phcolor(edit, color);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_PHSTYLE )
{
    Edit *edit = (Edit*)hb_parptr(1);
    uint32_t fstyle = hb_parni(2);
    edit_phstyle(edit, fstyle);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_EDIT_GET_TEXT )
{
    Edit *edit = (Edit*)hb_parptr(1);
    const char_t *text = edit_get_text(edit);
    hb_retc(text);
}
