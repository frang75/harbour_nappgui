/*
    This is part of gtnap
    TODO: More info
*/

// NAppGUI-Button wrapper for Harbour
// https://nappgui.com/en/gui/button.html
#include "gtnap.h"
#include "nappgui.h"

/*---------------------------------------------------------------------------*/

__EXTERN_C

void _component_set_tag(GuiComponent *component, const uint32_t tag);
uint32_t _component_get_tag(const GuiComponent *component);

__END_C

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_PUSH )
{
    Button *button = button_push();
    hb_retptr(button);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_CHECK )
{
    Button *button = button_check();
    hb_retptr(button);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_CHECK3 )
{
    Button *button = button_check3();
    hb_retptr(button);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_RADIO )
{
    Button *button = button_radio();
    hb_retptr(button);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_FLAT )
{
    Button *button = button_flat();
    hb_retptr(button);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_FLATGLE )
{
    Button *button = button_flatgle();
    hb_retptr(button);
}

/*---------------------------------------------------------------------------*/

static void i_OnButtonClick(GtNapCallback *callback, Event *e)
{
    hb_gtnap_callback(callback, e);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_ONCLICK )
{
    Button *button = (Button*)hb_parptr(1);
    Listener *listener = hb_gtnap_comp_listener(2, (GuiComponent*)button, i_OnButtonClick);
    button_OnClick(button, listener);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_TEXT )
{
    Button *button = (Button*)hb_parptr(1);
    const char_t *text = hb_gtnap_parText(2);
    button_text(button, text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_TEXT_ALT )
{
    Button *button = (Button*)hb_parptr(1);
    const char_t *text = hb_gtnap_parText(2);
    button_text_alt(button, text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_TOOLTIP )
{
    Button *button = (Button*)hb_parptr(1);
    const char_t *text = hb_gtnap_parText(2);
    button_tooltip(button, text);
}

/*---------------------------------------------------------------------------*/

// void button_font(Button *button, const Font *font);

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_IMAGE )
{
    Button *button = (Button*)hb_parptr(1);
    Image *image = hb_gtnap_parImage(2);
    button_image(button, image);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_IMAGE_ALT )
{
    Button *button = (Button*)hb_parptr(1);
    Image *image = hb_gtnap_parImage(2);
    button_image_alt(button, image);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_ID )
{
    Button *button = (Button*)hb_parptr(1);
    uint32_t id = hb_parni(2);
    _component_set_tag((GuiComponent*)button, id);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_GET_ID )
{
    Button *button = (Button*)hb_parptr(1);
    uint32_t id = _component_get_tag((GuiComponent*)button);
    hb_retni((int)id);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_STATE )
{
    Button *button = (Button*)hb_parptr(1);
    state_t state = (state_t)hb_parni(2);
    button_state(button, state);
}

/*---------------------------------------------------------------------------*/

HB_FUNC( NAP_BUTTON_GET_STATE )
{
    Button *button = (Button*)hb_parptr(1);
    state_t state = button_get_state(button);
    hb_retni((int)state);
}

/*---------------------------------------------------------------------------*/