/*
 * HBNAP Forms API. Dynamic menus support
 */

#include "hbnap.h"
#include "gtnap.inl"
#include <gui/menuitem.h>
#include <gui/menu.h>
#include <core/strings.h>

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_CREATE)
{
    GtNapMenu *menu = hbnap_menu_create();
    hb_retptr(menu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_DESTROY)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    hbnap_menu_destroy(menu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_ADD_ITEM)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    GtNapMenuItem *item = cast(hb_parptr(2), GtNapMenuItem);
    hbnap_menu_add_item(menu, item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_INS_ITEM)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    uint32_t pos = (uint32_t)hb_parni(2);
    GtNapMenuItem *item = cast(hb_parptr(3), GtNapMenuItem);
    hbnap_menu_ins_item(menu, pos, item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_DEL_ITEM)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    uint32_t pos = (uint32_t)hb_parni(2);
    hbnap_menu_del_item(menu, pos);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_COUNT)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    uint32_t n = hbnap_menu_count(menu);
    hb_retni(n);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_GET_ITEM)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    uint32_t index = (uint32_t)hb_parni(2);
    GtNapMenuItem *item = hbnap_menu_get_item(menu, index);
    hb_retptr(item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_BAR)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    GtNapForm *form = cast(hb_parptr(2), GtNapForm);
    hbnap_menubar(menu, form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_IS_MENUBAR)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    bool_t isbar = hbnap_is_menubar(menu);
    hb_retl((int)isbar);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_POPUP)
{
    GtNapMenu *menu = cast(hb_parptr(1), GtNapMenu);
    GtNapForm *form = cast(hb_parptr(2), GtNapForm);
    int32_t x = (int32_t)hb_parni(3);
    int32_t y = (int32_t)hb_parni(4);
    hbnap_menu_popup(menu, form, x, y);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENUITEM_CREATE)
{
    const char_t *text = hb_parcx(1);
    const char_t *icon_path = hb_parcx(2);
    HB_ITEM *click_block = hb_param(3, HB_IT_BLOCK);
    GtNapMenuItem *item = hbnap_menuitem_create(text, icon_path, click_block);
    hb_retptr(item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENUITEM_SEPARATOR)
{
    GtNapMenuItem *item = hbnap_menuitem_separator();
    hb_retptr(item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENUITEM_SUBMENU)
{
    GtNapMenuItem *item = cast(hb_parptr(1), GtNapMenuItem);
    GtNapMenu *submenu = cast(hb_parptr(2), GtNapMenu);
    hbnap_menuitem_submenu(item, submenu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENUITEM_GET_TEXT)
{
    GtNapMenuItem *item = cast(hb_parptr(1), GtNapMenuItem);
    const char_t *text = hbnap_menuitem_get_text(item);
    hb_retc(text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENUITEM_GET_SUBMENU)
{
    GtNapMenuItem *item = cast(hb_parptr(1), GtNapMenuItem);
    GtNapMenu *menu = hbnap_menuitem_get_submenu(item);
    hb_retptr(menu);
}
