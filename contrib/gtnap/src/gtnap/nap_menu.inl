
#include "gui.hxx"

__EXTERN_C

Panel *nap_menu_create(const bool_t autoclose);

void nap_menu_add(Panel *panel, Window *window, HB_ITEM *text_block, HB_ITEM *click_block, const uint32_t hotkey_pos);

void nap_menuvert_taborder(Panel *panel, Window *window);

uint32_t nap_menu_selected(Panel *panel);

__END_C