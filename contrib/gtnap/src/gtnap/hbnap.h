/*
    This is part of gtnap
    HBNAP/Forms API. For full graphic applications.
*/

#ifndef HB_HBNAP_H_
#define HB_HBNAP_H_

#include "hbvmint.h"
#include "hbgtcore.h"
#include <gui/gui.hxx>

typedef struct _gtnap_form_t GtNapForm;
typedef struct _gtnap_menuitem_t GtNapMenuItem;
typedef struct _gtnap_menu_t GtNapMenu;

HB_EXTERN_BEGIN

extern void hbnap_forms_init_app(HB_ITEM *main_block);

extern void hbnap_forms_exit_app(void);

extern GtNapForm *hbnap_forms_load(const char_t *pathname, const char_t *resource_path, const uint32_t flags);

extern void hbnap_forms_destroy(GtNapForm **form);

extern void hbnap_forms_title(GtNapForm *form, const char_t *text);

extern void hbnap_forms_set_text(GtNapForm *form, const char_t *cell, const char_t *text);

extern void hbnap_forms_set_int(GtNapForm *form, const char_t *cell, const int32_t value);

extern void hbnap_forms_insert_text(GtNapForm *form, const char_t *cell, const char_t *text);

extern int32_t hbnap_forms_get_int(GtNapForm *form, const char_t *cell);

extern bool_t hbnap_forms_embed(GtNapForm *form, GtNapForm *embedded_form, const char_t *cell);

extern void hbnap_forms_bind(GtNapForm *form, HB_ITEM *cell_bind);

extern void hbnap_forms_bind_store(GtNapForm *form);

extern void hbnap_forms_area_bind(GtNapForm *form, HB_ITEM *column_bind);

extern void hbnap_forms_area_refresh(GtNapForm *form);

extern uint32_t hbnap_forms_area_recno(GtNapForm *form);

extern void hbnap_forms_tree_bind(GtNapForm *form, const char_t *cell, HB_ITEM *areas, HB_ITEM *relations, HB_ITEM *columns);

extern void hbnap_forms_item_list(GtNapForm *form, const char_t *cell, HB_ITEM *items);

extern void hbnap_forms_OnClick(GtNapForm *form, const char_t *cell, HB_ITEM *click_block);

extern void hbnap_forms_maximize(GtNapForm *form);

extern void hbnap_forms_show(GtNapForm *form, HB_ITEM *onclose_block);

extern uint32_t hbnap_forms_modal(GtNapForm *form, GtNapForm *parent);

extern uint32_t hbnap_forms_modal_gtnap(GtNapForm *form);

extern void hbnap_forms_stop_modal(GtNapForm *form, const uint32_t value);

R2Df hbnap_forms_control_frame(GtNapForm *form, const char_t *cell);

extern void hbnap_forms_update(GtNapForm *form);

extern void hbnap_forms_main_cover(GtNapForm *form, const char_t *canvas_cell, const char_t *title, const char_t *logo_path, HB_ITEM *cover_items);

/* --------------------------- */
/* HBNAP Dynamic menus support */
/* --------------------------- */
extern GtNapMenu *hbnap_menu_create(void);

extern void hbnap_menu_destroy(GtNapMenu *menu);

extern void hbnap_menu_add_item(GtNapMenu *menu, GtNapMenuItem *item);

extern void hbnap_menu_ins_item(GtNapMenu *menu, const uint32_t pos, GtNapMenuItem *item);

extern void hbnap_menu_del_item(GtNapMenu *menu, const uint32_t pos);

extern uint32_t hbnap_menu_count(const GtNapMenu *menu);

extern GtNapMenuItem *hbnap_menu_get_item(GtNapMenu *menu, const uint32_t index);

extern void hbnap_menubar(GtNapMenu *menu, GtNapForm *form);

extern bool_t hbnap_is_menubar(const GtNapMenu *menu);

extern void hbnap_menu_popup(GtNapMenu *menu, GtNapForm *form, const int32_t x, const int32_t y);

extern GtNapMenuItem *hbnap_menuitem_create(const char_t *text, const char_t *icon_path, HB_ITEM *click_block);

extern GtNapMenuItem *hbnap_menuitem_separator(void);

extern void hbnap_menuitem_submenu(GtNapMenuItem *item, GtNapMenu *submenu);

extern const char_t *hbnap_menuitem_get_text(const GtNapMenuItem *item);

extern GtNapMenu *hbnap_menuitem_get_submenu(GtNapMenuItem *item);

HB_EXTERN_END

#endif
