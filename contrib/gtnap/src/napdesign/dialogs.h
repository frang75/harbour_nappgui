/* Dialog boxes */

#include "designer.hxx"

void dialog_dbind(void);

void dialog_form_name_exists(Window *parent, const Font *font, const char_t *name);

void dialog_no_remove_col(Window *parent, const Font *font, const char_t *layout_name);

void dialog_no_remove_row(Window *parent, const Font *font, const char_t *layout_name);

bool_t dialog_new_form(Window *parent, const Font *font, String **filename, String **desc);

bool_t dialog_props_form(Window *parent, const Font *font, String **filename, String **desc);

bool_t dialog_remove_form(Window *parent, const Font *font, const char_t *name);

bool_t dialog_remove_col(Window *parent, const Font *font, const char_t *layout_name, const uint32_t col);

bool_t dialog_remove_row(Window *parent, const Font *font, const char_t *layout_name, const uint32_t row);

uint8_t dialog_unsaved_changes(Window *parent, const Font *font, const char_t *text);

FLabel *dialog_new_label(Window *parent, const Font *font, const DSelect *sel);

FButton *dialog_new_button(Window *parent, const Font *font, const DSelect *sel);

FCheck *dialog_new_check(Window *parent, const Font *font, const DSelect *sel);

FRadio *dialog_new_radio(Window *parent, const Font *font, const DSelect *sel);

FTool *dialog_new_tool(Window *parent, const Font *font, const DSelect *sel, const char_t *folder_path);

FElem *dialog_new_elem(Window *parent, const Font *font, const char_t *caption, const ResId iconId, const ResId headerId, const char_t *folder_path);

FPopUp *dialog_new_popup(Window *parent, const Font *font, const DSelect *sel);

FEdit *dialog_new_edit(Window *parent, const Font *font, const DSelect *sel);

FCombo *dialog_new_combo(Window *parent, const Font *font, const DSelect *sel);

FListBox *dialog_new_listbox(Window *parent, const Font *font, const DSelect *sel);

FSlider *dialog_new_slider(Window *parent, const Font *font, const DSelect *sel);

FVSlider *dialog_new_vslider(Window *parent, const Font *font, const DSelect *sel);

FProgress *dialog_new_progress(Window *parent, const Font *font, const DSelect *sel);

FView *dialog_new_view(Window *parent, const Font *font, const DSelect *sel);

FSView *dialog_new_sview(Window *parent, const Font *font, const DSelect *sel);

FText *dialog_new_text(Window *parent, const Font *font, const DSelect *sel);

FImage *dialog_new_image(Window *parent, const Font *font, const DSelect *sel, const char_t *folder_path);

FTable *dialog_new_table(Window *parent, const Font *font, const DSelect *sel);

FHline *dialog_new_hline(Window *parent, const Font *font, const DSelect *sel);

FVline *dialog_new_vline(Window *parent, const Font *font, const DSelect *sel);

FLayout *dialog_vertical_layout(Window *parent, const Font *font, const DSelect *sel);

FLayout *dialog_horizontal_layout(Window *parent, const Font *font, const DSelect *sel);

FLayout *dialog_grid_layout(Window *parent, const Font *font, const DSelect *sel);
