/* Form editbox */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FEdit *fedit_create(void);

_nflib_api void fedit_destroy(FEdit **fedit);

_nflib_api void fedit_synchro(const FEdit *fedit, Edit *edit);

__END_C
