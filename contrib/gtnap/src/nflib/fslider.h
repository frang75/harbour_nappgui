/* Form slider */

#include "nflib.hxx"

__EXTERN_C

_nflib_api FSlider *fslider_create(void);

_nflib_api void fslider_destroy(FSlider **slider);

_nflib_api void fslider_synchro(const FSlider *fslider, Slider *slider);

__END_C
