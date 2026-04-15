/* NAppGUI forms serialization */

#include "nflib.hxx"

__EXTERN_C

_nflib_api void nflib_start(void);

_nflib_api void nflib_finish(void);

_nflib_api const Image *nflib_default_image(void);

_nflib_api const Image *nflib_default_view(void);

_nflib_api const Image *nflib_default_icon(void);

_nflib_api extern color_t kCOLOR_LAYOUT_BDLT;
_nflib_api extern color_t kCOLOR_LAYOUT_BDDK;
_nflib_api extern color_t kCOLOR_LAYOUT_BGLT;
_nflib_api extern color_t kCOLOR_LAYOUT_BGDK;
_nflib_api extern color_t kCOLOR_LABEL_LT;
_nflib_api extern color_t kCOLOR_LABEL_DK;
_nflib_api extern color_t kCOLOR_LABEL_BGLT;
_nflib_api extern color_t kCOLOR_LABEL_BGDK;

__END_C
