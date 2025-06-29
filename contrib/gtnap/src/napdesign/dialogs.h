/* Dialog boxes */

#include "designer.hxx"

void dialog_dbind(void);

String *dialog_form_name(Window *parent, const char_t *name);

FLabel *dialog_new_label(Window *parent, const Font *font, const DSelect *sel);

FButton *dialog_new_button(Window *parent, const DSelect *sel);

FCheck *dialog_new_check(Window *parent, const DSelect *sel);

FEdit *dialog_new_edit(Window *parent, const DSelect *sel);

FText *dialog_new_text(Window *parent, const DSelect *sel);

FImage *dialog_new_image(Window *parent, const DSelect *sel, const char_t *folder_path);

FSlider *dialog_new_slider(Window *parent, const DSelect *sel);

FProgress *dialog_new_progress(Window *parent, const DSelect *sel);

FPopUp *dialog_new_popup(Window *parent, const DSelect *sel);

FListBox *dialog_new_listbox(Window *parent, const DSelect *sel);

FElem *dialog_new_elem(Window *parent, const char_t *folder_path);

FTable *dialog_new_table(Window *parent, const DSelect *sel);

FLayout *dialog_vertical_layout(Window *parent, const Font *font, const DSelect *sel);

FLayout *dialog_horizontal_layout(Window *parent, const Font *font, const DSelect *sel);

FLayout *dialog_grid_layout(Window *parent, const Font *font, const DSelect *sel);

uint8_t dialog_unsaved_changes(Window *parent);

bool_t dialog_remove_form(Window *parent, const char_t *name);

void dialog_name_already_exists(Window *parent, const char_t *name);
