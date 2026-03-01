/*
    This is part of gtnap
    TODO: More info
*/
/* Dynamic menus support for GTNAP-Forms */

#include "gtnap.h"
#include "gtnap.inl"
#include <gui/menuitem.h>
#include <gui/menu.h>
#include <core/strings.h>

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_CREATE)
{
    GtNapMenu *menu = hbnap_menu_create();
    hb_retptr(menu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_DESTROY)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    hbnap_menu_destroy(menu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_ADD_ITEM)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    GtNapMenuItem *item = cast(hb_parptr(2), GtNapMenuItem);
    hbnap_menu_add_item(menu, item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_INS_ITEM)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    uint32_t pos = (uint32_t)hb_parni(2);
    GtNapMenuItem *item = cast(hb_parptr(3), GtNapMenuItem);
    hbnap_menu_ins_item(menu, pos, item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_DEL_ITEM)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    uint32_t pos = (uint32_t)hb_parni(2);
    hbnap_menu_del_item(menu, pos);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_COUNT)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    uint32_t n = hbnap_menu_count(menu);
    hb_retni(n);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_GET_ITEM)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    uint32_t index = (uint32_t)hb_parni(2);
    GtNapMenuItem *item = hbnap_menu_get_item(menu, index);
    hb_retptr(item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_BAR)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    GtNapForm *form = cast(hb_parptr(2), GtNapForm);
    hbnap_menubar(menu, form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_IS_MENUBAR)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    bool_t isbar = hbnap_is_menubar(menu);
    hb_retl((int)isbar);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENU_POPUP)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    GtNapForm *form = cast(hb_parptr(2), GtNapForm);
    int32_t x = (int32_t)hb_parni(3);
    int32_t y = (int32_t)hb_parni(4);
    hbnap_menu_popup(menu, form, x, y);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENUITEM_CREATE)
{
    HB_ITEM *text_block = hb_param(1, HB_IT_BLOCK | HB_IT_STRING);
    const char_t *icon_path = hb_parcx(2);
    HB_ITEM *click_block = hb_param(3, HB_IT_BLOCK);
    GtNapMenuItem *item = hbnap_menuitem_create(text_block, icon_path, click_block);
    hb_retptr(item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENUITEM_SEPARATOR)
{
    GtNapMenuItem *item = hbnap_menuitem_separator();
    hb_retptr(item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENUITEM_SUBMENU)
{
    GtNapMenuItem *item = cast(hb_parptr(1), GtNapMenuItem);
    GtNapMenu *submenu = cast(hb_parptr(2), GtNapMenu);
    hbnap_menuitem_submenu(item, submenu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENUITEM_GET_TEXT)
{
    GtNapMenuItem *item = cast(hb_parptr(1), GtNapMenuItem);
    String *text = hbnap_menuitem_get_text(item);
    hb_retc(tc(text));
    str_destroy(&text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(NAP_DMENUITEM_GET_SUBMENU)
{
    GtNapMenuItem *item = cast(hb_parptr(1), GtNapMenuItem);
    GtNapMenu *menu = hbnap_menuitem_get_submenu(item);
    hb_retptr(menu);
}
