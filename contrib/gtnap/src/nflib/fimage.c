/* Form imageview */

#include "fimage.h"
#include "nflib.h"
#include "nflib.inl"
#include <gui/imageview.h>
#include <draw2d/image.h>
#include <geom2d/s2d.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FImage *fimage_create(void)
{
    return dbind_create(FImage);
}

/*---------------------------------------------------------------------------*/

void fimage_destroy(FImage **fimage)
{
    dbind_destroy(fimage, FImage);
}

/*---------------------------------------------------------------------------*/

void fimage_synchro(const FImage *fimage, ImageView *view, const char_t *resource_path)
{
    cassert_no_null(fimage);
    imageview_scale(view, _nflib_scale(fimage->scale));
    imageview_size(view, s2df(fimage->min_width, fimage->min_height));

    if (resource_path != NULL)
    {
        String *path = str_printf("%s%s", resource_path, tc(fimage->path));
        Image *image = image_from_file(tc(path), NULL);
        if (image != NULL)
        {
            imageview_image(view, image);
            image_destroy(&image);
        }
        else
        {
            const Image *rimage = nflib_default_image();
            imageview_image(view, rimage);
        }

        str_destroy(&path);
    }
}

