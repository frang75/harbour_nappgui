/* Form progressbar */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FProgress *fprogress_create(void);

_nflib_api void fprogress_destroy(FProgress **fprogress);

_nflib_api void fprogress_synchro(const FProgress *fprogress, Progress *progress);

__END_C
