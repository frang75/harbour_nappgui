/* Form label */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FLabel *flabel_create(void);

_nflib_api void flabel_destroy(FLabel **flabel);

_nflib_api void flabel_synchro(const FLabel *flabel, Label *label);

__END_C
