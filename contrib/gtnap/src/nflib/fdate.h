/* Form dateview */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FDate *fdate_create(void);

_nflib_api void fdate_destroy(FDate **date);

_nflib_api void fdate_synchro(const FDate *fdate, DateView *view);

__END_C
