/* Form popup */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FPopUp *fpopup_create(void);

_nflib_api void fpopup_destroy(FPopUp **fpopup);

_nflib_api void fpopup_synchro(const FPopUp *fpopup, PopUp *popup, const char_t *resource_path);

__END_C
