/* Form layout */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FLayout *flayout_create(const uint32_t ncols, const uint32_t nrows);

_nflib_api FLayout *flayout_read(Stream *stm);

_nflib_api FLayout *flayout_read_with_vers(Stream *stm, const uint16_t vers);

_nflib_api void flayout_destroy(FLayout **layout);

_nflib_api void flayout_write(Stream *stm, const FLayout *layout);

_nflib_api void flayout_margin_left(FLayout *layout, const real32_t margin);

_nflib_api void flayout_margin_top(FLayout *layout, const real32_t margin);

_nflib_api void flayout_margin_right(FLayout *layout, const real32_t margin);

_nflib_api void flayout_margin_bottom(FLayout *layout, const real32_t margin);

_nflib_api void flayout_margin_col(FLayout *layout, const uint32_t col, const real32_t margin);

_nflib_api void flayout_margin_row(FLayout *layout, const uint32_t row, const real32_t margin);

_nflib_api void flayout_insert_col(FLayout *layout, const uint32_t col);

_nflib_api void flayout_remove_col(FLayout *layout, const uint32_t col);

_nflib_api void flayout_insert_row(FLayout *layout, const uint32_t row);

_nflib_api void flayout_remove_row(FLayout *layout, const uint32_t row);

_nflib_api void flayout_remove_cell(FLayout *layout, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_label(FLayout *layout, FLabel *label, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_button(FLayout *layout, FButton *button, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_check(FLayout *layout, FCheck *check, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_radio(FLayout *layout, FRadio *radio, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_tool(FLayout *layout, FTool *tool, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_popup(FLayout *layout, FPopUp *popup, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_edit(FLayout *layout, FEdit *edit, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_combo(FLayout *layout, FCombo *combo, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_listbox(FLayout *layout, FListBox *listbox, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_slider(FLayout *layout, FSlider *slider, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_vslider(FLayout *layout, FVSlider *vslider, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_progress(FLayout *layout, FProgress *progress, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_text(FLayout *layout, FText *text, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_image(FLayout *layout, FImage *image, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_table(FLayout *layout, FTable *table, const uint32_t col, const uint32_t row);

_nflib_api void flayout_add_layout(FLayout *layout, FLayout *sublayout, const uint32_t col, const uint32_t row);

_nflib_api uint32_t flayout_ncols(const FLayout *layout);

_nflib_api uint32_t flayout_nrows(const FLayout *layout);

_nflib_api FColumn *flayout_column(FLayout *layout, const uint32_t col);

_nflib_api FRow *flayout_row(FLayout *layout, const uint32_t row);

_nflib_api FCell *flayout_cell(FLayout *layout, const uint32_t col, const uint32_t row);

_nflib_api const FCell *flayout_ccell(const FLayout *layout, const uint32_t col, const uint32_t row);

_nflib_api void flayout_col_expand(const FLayout *layout, Layout *glayout);

_nflib_api void flayout_row_expand(const FLayout *layout, Layout *glayout);

_nflib_api Layout *flayout_to_gui(const FLayout *layout, const char_t *resource_path, const real32_t empty_width, const real32_t empty_height);

_nflib_api GuiControl *flayout_search_gui_control(const FLayout *layout, Layout *gui_layout, const char_t *cell_name);

__END_C
