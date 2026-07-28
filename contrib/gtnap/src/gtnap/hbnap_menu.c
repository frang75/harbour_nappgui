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
    HbNapMenu *menu = hbnap_menu_create();
    hb_retptr(menu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_DESTROY)
{
    HbNapMenu *menu = cast(hb_parptr(1), HbNapMenu);
    hbnap_menu_destroy(menu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_ADD_ITEM)
{
    HbNapMenu *menu = cast(hb_parptr(1), HbNapMenu);
    HbNapMenuItem *item = cast(hb_parptr(2), HbNapMenuItem);
    hbnap_menu_add_item(menu, item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_INS_ITEM)
{
    HbNapMenu *menu = cast(hb_parptr(1), HbNapMenu);
    uint32_t pos = (uint32_t)hb_parni(2);
    HbNapMenuItem *item = cast(hb_parptr(3), HbNapMenuItem);
    hbnap_menu_ins_item(menu, pos, item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_DEL_ITEM)
{
    HbNapMenu *menu = cast(hb_parptr(1), HbNapMenu);
    uint32_t pos = (uint32_t)hb_parni(2);
    hbnap_menu_del_item(menu, pos);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_COUNT)
{
    HbNapMenu *menu = cast(hb_parptr(1), HbNapMenu);
    uint32_t n = hbnap_menu_count(menu);
    hb_retni(n);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_GET_ITEM)
{
    HbNapMenu *menu = cast(hb_parptr(1), HbNapMenu);
    uint32_t index = (uint32_t)hb_parni(2);
    HbNapMenuItem *item = hbnap_menu_get_item(menu, index);
    hb_retptr(item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_BAR)
{
    HbNapMenu *menu = cast(hb_parptr(1), HbNapMenu);
    HbNapForm *form = cast(hb_parptr(2), HbNapForm);
    hbnap_menubar(menu, form);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_IS_MENUBAR)
{
    HbNapMenu *menu = cast(hb_parptr(1), HbNapMenu);
    bool_t isbar = hbnap_is_menubar(menu);
    hb_retl((int)isbar);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENU_POPUP)
{
    HbNapMenu *menu = cast(hb_parptr(1), HbNapMenu);
    HbNapForm *form = cast(hb_parptr(2), HbNapForm);
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
    HbNapMenuItem *item = hbnap_menuitem_create(text, icon_path, click_block);
    hb_retptr(item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENUITEM_SEPARATOR)
{
    HbNapMenuItem *item = hbnap_menuitem_separator();
    hb_retptr(item);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENUITEM_SUBMENU)
{
    HbNapMenuItem *item = cast(hb_parptr(1), HbNapMenuItem);
    HbNapMenu *submenu = cast(hb_parptr(2), HbNapMenu);
    hbnap_menuitem_submenu(item, submenu);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENUITEM_GET_TEXT)
{
    HbNapMenuItem *item = cast(hb_parptr(1), HbNapMenuItem);
    const char_t *text = hbnap_menuitem_get_text(item);
    hb_retc(text);
}

/*---------------------------------------------------------------------------*/

HB_FUNC(HBNAP_MENUITEM_GET_SUBMENU)
{
    HbNapMenuItem *item = cast(hb_parptr(1), HbNapMenuItem);
    HbNapMenu *menu = hbnap_menuitem_get_submenu(item);
    hb_retptr(menu);
}
