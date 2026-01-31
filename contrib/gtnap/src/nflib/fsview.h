/* Form scroll view */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FSView *fsview_create(void);

_nflib_api void fsview_destroy(FSView **view);

_nflib_api void fsview_synchro(const FSView *fsview, View *view);

__END_C
