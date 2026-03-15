/* Form vertical separator */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FVline *fvline_create(void);

_nflib_api void fvline_destroy(FVline **fvline);

_nflib_api void fvline_synchro(const FVline *fvline, Line *line);

__END_C
