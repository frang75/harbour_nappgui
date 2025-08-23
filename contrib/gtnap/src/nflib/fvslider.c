/* Form vertical slider */

#include "fvslider.h"
#include <gui/slider.h>
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FVSlider *fvslider_create(void)
{
    return dbind_create(FVSlider);
}

/*---------------------------------------------------------------------------*/

void fvslider_destroy(FVSlider **slider)
{
    dbind_destroy(slider, FVSlider);
}

/*---------------------------------------------------------------------------*/

void fvslider_synchro(const FVSlider *fslider, Slider *slider)
{
    cassert_no_null(slider);
    slider_min_width(slider, fslider->min_height);
    slider_value(slider, .5f);
}
