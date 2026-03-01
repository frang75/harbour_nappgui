/* Designer layout */

#include "designer.hxx"

void dlayout_global_init(void);

DLayout *dlayout_from_flayout(const FLayout *flayout, const char_t *resource_path, const DColors *colors);

DCell *dlayout_cell(DLayout *layout, const uint32_t col, const uint32_t row);

const DCell *dlayout_ccell(const DLayout *layout, const uint32_t col, const uint32_t row);

void dlayout_destroy(DLayout **layout);

uint32_t dlayout_ncols(const DLayout *layout);

uint32_t dlayout_nrows(const DLayout *layout);

void dlayout_insert_col(DLayout *layout, const uint32_t col);

void dlayout_remove_col(DLayout *layout, const uint32_t col);

void dlayout_insert_row(DLayout *layout, const uint32_t row);

void dlayout_remove_row(DLayout *layout, const uint32_t row);

void dlayout_remove_cell(DLayout *layout, const uint32_t col, const uint32_t row);

void dlayout_add_layout(DLayout *layout, DLayout *sublayout, const uint32_t col, const uint32_t row);

void dlayout_set_image(DLayout *layout, const Image *image, const uint32_t col, const uint32_t row, const DColors *colors);

void dlayout_add_image(DLayout *layout, const Image *image, const uint32_t col, const uint32_t row, const DColors *colors);

void dlayout_del_image(DLayout *layout, const uint32_t index, const uint32_t col, const uint32_t row);

void dlayout_clear_images(DLayout *layout, const uint32_t col, const uint32_t row);

void dlayout_synchro_elems(DLayout *layout, const uint32_t col, const uint32_t row, const ArrSt(FElem) *elems, const char_t *resource_path, const DColors *colors);

void dlayout_synchro_visual(DLayout *dlayout, const Layout *glayout, const V2Df gorigin, const V2Df lorigin);

void dlayout_path_at_pos(const DLayout *dlayout, const FLayout *flayout, const Layout *glayout, const real32_t x, const real32_t y, ArrSt(DSelect) *path);

void dlayout_draw(const DLayout *dlayout, const FLayout *flayout, const Layout *glayout, const DSelect *hover, const DSelect *sel, const widget_t swidget, const Font *default_font, const Font *bold_font, const cmode_t cmode, const DColors *colors, const char_t *form_name, const bool_t focus, DCtx *ctx);

R2Df dlayout_sel_rect(const DSelect *sel);

R2Df dlayout_flayout_rect(const DLayout *dlayout, const FLayout *flayout, const FLayout *ref_flayout);
