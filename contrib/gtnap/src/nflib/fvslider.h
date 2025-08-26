/* Form vertical slider */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FVSlider *fvslider_create(void);

_nflib_api void fvslider_destroy(FVSlider **slider);

_nflib_api void fvslider_synchro(const FVSlider *fslider, Slider *slider);

__END_C
