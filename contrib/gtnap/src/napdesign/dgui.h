/* Design gui elements */

#include "designer.hxx"

void dgui_init(void);

void dgui_finish(void);

View *dgui_panel_header(const char_t *title, const Font *font, Listener *OnClose);

Panel *dgui_drawer(const char_t *title, const Font *font, Panel *child, const bool_t open, Listener *OnChange);
