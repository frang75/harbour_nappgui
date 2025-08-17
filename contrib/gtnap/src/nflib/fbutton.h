/* Form button */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FButton *fbutton_create(void);

_nflib_api void fbutton_destroy(FButton **fbutton);

_nflib_api void fbutton_synchro(const FButton *fbutton, Button *button);

__END_C
