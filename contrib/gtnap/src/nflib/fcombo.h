/* Form combobox */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FCombo *fcombo_create(void);

_nflib_api void fcombo_destroy(FCombo **fcombo);

_nflib_api void fcombo_synchro(const FCombo *fcombo, Combo *combo);

__END_C
