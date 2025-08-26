/* Form textview */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FText *ftext_create(void);

_nflib_api void ftext_destroy(FText **text);

_nflib_api void ftext_synchro(const FText *ftext, TextView *view);

__END_C
