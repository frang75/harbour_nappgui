/* Design layout (editable parameters) */

#include "designer.hxx"

DLayout *dlayout_create(const uint32_t ncols, const uint32_t nrows);

void dlayout_destroy(DLayout **layout);

void dlayout_insert_col(DLayout *layout, const uint32_t col);

void dlayout_remove_col(DLayout *layout, const uint32_t col);

void dlayout_insert_row(DLayout *layout, const uint32_t row);

void dlayout_remove_row(DLayout *layout, const uint32_t row);

void dlayout_add_layout(DLayout *layout, DLayout *sublayout, const uint32_t col, const uint32_t row);

/* Create a real GUI layout from editable layout properties */
Layout *dlayout_gui_layout(const DLayout *layout);