/* Form tabs */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FTabs *ftabs_create(void);

_nflib_api void ftabs_destroy(FTabs **ftabs);

_nflib_api void ftabs_synchro(const FTabs *ftabs, Tabs *tabs, const char_t *resource_path);

__END_C
