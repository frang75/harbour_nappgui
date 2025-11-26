/* NAppGUI Designer Application */

#include "designer.hxx"

void designer_need_save(Designer *app);

void designer_canvas_update(Designer *app);

void designer_inspect_update(Designer *app);

void designer_inspect_select(Designer *app, const uint32_t row);

void designer_clipboard_controls(Designer *app, const bool_t can_copy, const bool_t can_paste);

void designer_undo_controls(Designer *app, const bool_t can_undo, const bool_t can_redo);

void designer_undo_stack(Designer *app, const uint32_t size);

void designer_promote_left(Designer *app, const DSelect *sel);

void designer_promote_right(Designer *app, const DSelect *sel);

void designer_promote_top(Designer *app, const DSelect *sel);

void designer_promote_bottom(Designer *app, const DSelect *sel);

const char_t *designer_folder_path(const Designer *app);

const Font *designer_default_font(const Designer *app);

const DColors *designer_colors(const Designer *app);

Window *designer_main_window(const Designer *app);

Panel *designer_drawer(Designer *app, Panel *child, const drawer_t drawer);
