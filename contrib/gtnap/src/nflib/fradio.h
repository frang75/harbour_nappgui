/* Form radio button */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FRadio *fradio_create(void);

_nflib_api void fradio_destroy(FRadio **fradio);

_nflib_api void fradio_synchro(const FRadio *fradio, Button *button);

__END_C
