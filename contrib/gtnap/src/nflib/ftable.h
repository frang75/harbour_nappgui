/* Form tableview */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FTable *ftable_create(void);

_nflib_api void ftable_destroy(FTable **table);

_nflib_api void ftable_synchro(const FTable *table, TableView *view);

__END_C
