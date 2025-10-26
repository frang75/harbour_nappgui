/* Design form */

#include "designer.hxx"

DForm *dform_empty(Designer *app);

DForm *dform_read(Stream *stm, Designer *app);

void dform_destroy(DForm **form);

void dform_write(Stream *stm, DForm *form);

void dform_compose(DForm *form);

void dform_description(DForm *form, const char_t *desc);

const char_t* dform_get_description(const DForm *form);

void dform_set(DForm *form, Panel *inspect, Panel *propedit);

bool_t dform_need_save(const DForm *form);

bool_t dform_OnMove(DForm *form, const real32_t mouse_x, const real32_t mouse_y);

bool_t dform_OnClick(DForm *form, Window *window, Panel *inspect, Panel *propedit, const Font *font, const widget_t widget, const real32_t mouse_x, const real32_t mouse_y, const gui_mouse_t button);

bool_t dform_OnExit(DForm *form);

bool_t dform_OnSupr(DForm *form, Panel *inspect, Panel *propedit);

V2Df dform_get_origin(const DForm *form);

void dform_origin(DForm *form, const V2Df origin);

void dform_synchro_cell_image(DForm *form, const DSelect *sel, const Image *image, const char_t *imgname);

void dform_synchro_label(DForm *form, const DSelect *sel);

void dform_synchro_button(DForm *form, const DSelect *sel);

void dform_synchro_check(DForm *form, const DSelect *sel);

void dform_synchro_radio(DForm *form, const DSelect *sel);

void dform_synchro_tool(DForm *form, const DSelect *sel);

void dform_synchro_popup(DForm *form, const DSelect *sel, const char_t *resource_path);

void dform_synchro_edit(DForm *form, const DSelect *sel);

void dform_synchro_combo(DForm *form, const DSelect *sel);

void dform_synchro_listbox(DForm *form, const DSelect *sel, const char_t *resource_path);

void dform_synchro_slider(DForm *form, const DSelect *sel);

void dform_synchro_vslider(DForm *form, const DSelect *sel);

void dform_synchro_progress(DForm *form, const DSelect *sel);

void dform_synchro_textview(DForm *form, const DSelect *sel);

void dform_synchro_imageview(DForm *form, const DSelect *sel);

void dform_synchro_table(DForm *form, const DSelect *sel);

void dform_synchro_layout_margin(DForm *form, const DSelect *sel);

void dform_synchro_column_margin(DForm *form, const DSelect *sel, const FColumn *fcol, const uint32_t col);

void dform_synchro_column_width(DForm *form, const DSelect *sel, const FColumn *fcol, const uint32_t col);

void dform_synchro_row_margin(DForm *form, const DSelect *sel, const FRow *frow, const uint32_t row);

void dform_synchro_row_height(DForm *form, const DSelect *sel, const FRow *frow, const uint32_t row);

void dform_synchro_cell_halign(DForm *form, const DSelect *sel, const FCell *fcell, const uint32_t col, const uint32_t row);

void dform_synchro_cell_valign(DForm *form, const DSelect *sel, const FCell *fcell, const uint32_t col, const uint32_t row);

FCell *dform_sel_fcell(const DSelect *sel);

void dform_draw(const DForm *form, const widget_t swidget, const Font *default_font, const Font *bold_font, const cmode_t cmode, const DColors *colors, const char_t *form_name, DCtx *ctx);

uint32_t dform_selpath_size(const DForm *form);

const char_t *dform_cell_type(const celltype_t type);

const Image *dform_cell_icon(const celltype_t type);

const char_t *dform_selpath_caption(const DForm *form, const uint32_t col, const uint32_t row);

const Image *dform_selpath_icon(const DForm *form, const uint32_t col, const uint32_t row);

void dform_inspect_select(DForm *form, Panel *propedit, const uint32_t row);

void dform_set_need_save(DForm *form);

void dform_simulate(DForm *form, Window *window);
