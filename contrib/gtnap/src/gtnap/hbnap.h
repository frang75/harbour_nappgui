/*
    This is part of gtnap
    HBNAP/Forms API. For full graphic applications.
*/

#ifndef HB_HBNAP_H_
#define HB_HBNAP_H_

#include "hbvmint.h"
#include "hbgtcore.h"
#include <gui/gui.hxx>

typedef struct _hbnap_form_t HbNapForm;
typedef struct _hbnap_menuitem_t HbNapMenuItem;
typedef struct _hbnap_menu_t HbNapMenu;

HB_EXTERN_BEGIN

extern void hbnap_forms_init_app(HB_ITEM *main_block);

extern void hbnap_forms_exit_app(void);

extern HbNapForm *hbnap_forms_load(const char_t *pathname, const char_t *resource_path, const uint32_t flags);

extern void hbnap_forms_destroy(HbNapForm **form);

extern void hbnap_forms_title(HbNapForm *form, const char_t *text);

extern void hbnap_forms_set_text(HbNapForm *form, const char_t *cell, const char_t *text);

extern void hbnap_forms_set_int(HbNapForm *form, const char_t *cell, const int32_t value);

extern void hbnap_forms_insert_text(HbNapForm *form, const char_t *cell, const char_t *text);

extern int32_t hbnap_forms_get_int(HbNapForm *form, const char_t *cell);

extern bool_t hbnap_forms_embed(HbNapForm *form, HbNapForm *embedded_form, const char_t *cell);

extern void hbnap_forms_bind(HbNapForm *form, HB_ITEM *cell_bind);

extern void hbnap_forms_bind_store(HbNapForm *form);

extern void hbnap_forms_table_bind(HbNapForm *form, const char_t *cell, HB_ITEM *area, HB_ITEM *columns);

extern void hbnap_forms_area_refresh(HbNapForm *form);

extern uint32_t hbnap_forms_area_recno(HbNapForm *form);

extern void hbnap_forms_tree_bind(HbNapForm *form, const char_t *cell, HB_ITEM *areas, HB_ITEM *relations, HB_ITEM *columns);

extern void hbnap_forms_item_list(HbNapForm *form, const char_t *cell, HB_ITEM *items);

extern void hbnap_forms_OnClick(HbNapForm *form, const char_t *cell, HB_ITEM *click_block);

extern void hbnap_forms_maximize(HbNapForm *form);

extern void hbnap_forms_show(HbNapForm *form, HB_ITEM *onclose_block);

extern uint32_t hbnap_forms_modal(HbNapForm *form, HbNapForm *parent);

extern uint32_t hbnap_forms_modal_gtnap(HbNapForm *form);

extern void hbnap_forms_stop_modal(HbNapForm *form, const uint32_t value);

R2Df hbnap_forms_control_frame(HbNapForm *form, const char_t *cell);

extern void hbnap_forms_update(HbNapForm *form);

extern void hbnap_forms_main_cover(HbNapForm *form, const char_t *canvas_cell, const char_t *title, const char_t *logo_path, HB_ITEM *cover_items);


extern HbNapMenu *hbnap_menu_create(void);

extern void hbnap_menu_destroy(HbNapMenu *menu);

extern void hbnap_menu_add_item(HbNapMenu *menu, HbNapMenuItem *item);

extern void hbnap_menu_ins_item(HbNapMenu *menu, const uint32_t pos, HbNapMenuItem *item);

extern void hbnap_menu_del_item(HbNapMenu *menu, const uint32_t pos);

extern uint32_t hbnap_menu_count(const HbNapMenu *menu);

extern HbNapMenuItem *hbnap_menu_get_item(HbNapMenu *menu, const uint32_t index);

extern void hbnap_menubar(HbNapMenu *menu, HbNapForm *form);

extern bool_t hbnap_is_menubar(const HbNapMenu *menu);

extern void hbnap_menu_popup(HbNapMenu *menu, HbNapForm *form, const int32_t x, const int32_t y);

extern HbNapMenuItem *hbnap_menuitem_create(const char_t *text, const char_t *icon_path, HB_ITEM *click_block);

extern HbNapMenuItem *hbnap_menuitem_separator(void);

extern void hbnap_menuitem_submenu(HbNapMenuItem *item, HbNapMenu *submenu);

extern const char_t *hbnap_menuitem_get_text(const HbNapMenuItem *item);

extern HbNapMenu *hbnap_menuitem_get_submenu(HbNapMenuItem *item);

HB_EXTERN_END

#endif
