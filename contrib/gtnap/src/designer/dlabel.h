/* Design label */

#include "designer.hxx"

DLabel *dlabel_create(void);

void dlabel_destroy(DLabel **label);

void dlabel_text(DLabel *label, const char_t *text);