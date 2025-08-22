/* Form slider */

#include "fslider.h"
#include <gui/slider.h>
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FSlider *fslider_create(void)
{
    return dbind_create(FSlider);
}

/*---------------------------------------------------------------------------*/

void fslider_destroy(FSlider **slider)
{
    dbind_destroy(slider, FSlider);
}

/*---------------------------------------------------------------------------*/

void fslider_synchro(const FSlider *fslider, Slider *slider)
{
    cassert_no_null(slider);
    slider_min_width(slider, fslider->min_width);
    slider_value(slider, .5f);
}

