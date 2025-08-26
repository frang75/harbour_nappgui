/* Form listbox */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FListBox *flistbox_create(void);

_nflib_api void flistbox_destroy(FListBox **flistbox);

_nflib_api void flistbox_synchro(const FListBox *flistbox, ListBox *listbox, const char_t *resource_path);

__END_C
