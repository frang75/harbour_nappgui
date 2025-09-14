/* NAppGUI Designer Application */

#include "designer.hxx"

void designer_need_save(Designer *app);

void designer_canvas_update(Designer *app);

void designer_inspect_update(Designer *app);

void designer_inspect_select(Designer *app, const uint32_t row);

const char_t *designer_folder_path(const Designer *app);

const Font *designer_default_font(const Designer *app);

const DColors *designer_colors(const Designer *app);

Window *designer_main_window(const Designer *app);

Panel *designer_drawer(Designer *app, Panel *child, const drawer_t drawer);
