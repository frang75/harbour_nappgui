/* Form panel */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FPanel *fpanel_create(void);

_nflib_api void fpanel_destroy(FPanel **panel);

_nflib_api void fpanel_synchro(const FPanel *fpanel, Panel *panel);

__END_C
