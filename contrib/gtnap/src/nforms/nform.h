/* NAppGUI form */

#include "nforms.hxx"

__EXTERN_C

_nforms_api NForm *nform_from_data(const byte_t *data, const uint32_t size);

_nforms_api NForm *nform_from_file(const char_t *pathname, ferror_t *error);

_nforms_api void nform_destroy(NForm **form);

_nforms_api Window *nform_window(const NForm *form, const uint32_t flags, const char_t *resource_path);

_nforms_api void nform_set_control_str(NForm *form, const char_t *cell_name, const char_t *value);

_nforms_api void nform_add_control_str(NForm *form, const char_t *cell_name, const char_t *value);

_nforms_api void nform_set_control_bool(NForm *form, const char_t *cell_name, const bool_t value);

_nforms_api void nform_set_control_int(NForm *form, const char_t *cell_name, const int32_t value);

_nforms_api void nform_set_control_real(NForm *form, const char_t *cell_name, const real32_t value);

_nforms_api bool_t nform_get_control_str(const NForm *form, const char_t *cell_name, const char_t **value);

_nforms_api bool_t nform_get_control_bool(const NForm *form, const char_t *cell_name, bool_t *value);

_nforms_api bool_t nform_get_control_int(const NForm *form, const char_t *cell_name, int32_t *value);

_nforms_api bool_t nform_get_control_real(const NForm *form, const char_t *cell_name, real32_t *value);

_nforms_api bool_t nform_set_listener(NForm *form, const char_t *cell_name, Listener *listener);

_nforms_api R2Df nform_get_control_frame(NForm *form, const char_t *cell_name, Window *window);

_nforms_api TableView *nform_get_tableview(NForm *form, const char_t *cell_name);

__END_C
