/* Form imageview */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FImage *fimage_create(void);

_nflib_api void fimage_destroy(FImage **fimage);

_nflib_api void fimage_synchro(const FImage *fimage, ImageView *view, const char_t *resource_path);

__END_C
