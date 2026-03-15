/* Form horizontal separator */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FHline *fhline_create(void);

_nflib_api void fhline_destroy(FHline **fhline);

_nflib_api void fhline_synchro(const FHline *fhline, Line *line);

__END_C
