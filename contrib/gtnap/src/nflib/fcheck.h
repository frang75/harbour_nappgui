/* Form checkbox */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FCheck *fcheck_create(void);

_nflib_api void fcheck_destroy(FCheck **fcheck);

_nflib_api void fcheck_synchro(const FCheck *fcheck, Button *button);

__END_C
