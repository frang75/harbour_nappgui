/* Form view */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FView *fview_create(void);

_nflib_api void fview_destroy(FView **view);

_nflib_api void fview_synchro(const FView *fview, View *view);

__END_C
