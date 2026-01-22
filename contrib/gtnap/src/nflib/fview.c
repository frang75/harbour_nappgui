/* Form view */

#include "fview.h"
#include "nflib.h"
#include <gui/view.h>
#include <draw2d/dctx.h>
#include <draw2d/draw.h>
#include <draw2d/image.h>
#include <geom2d/s2d.h>
#include <geom2d/t2d.h>
#include <core/dbind.h>
#include <core/event.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FView *fview_create(void)
{
    return dbind_create(FView);
}

/*---------------------------------------------------------------------------*/

void fview_destroy(FView **view)
{
    dbind_destroy(view, FView);
}

/*---------------------------------------------------------------------------*/

static void i_OnDraw(void *nonused, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    const Image *image = nflib_default_view();
    real32_t imgwidth = (real32_t)image_width(image);
    real32_t imgheight = (real32_t)image_height(image);
    real32_t scalex = p->width /imgwidth;
    real32_t scaley = p->height / imgheight;
    T2Df t2d;
    unref(nonused);
    t2d_scalef(&t2d, kT2D_IDENTf, scalex, scaley);
    draw_matrixf(p->ctx, &t2d);
    draw_image(p->ctx, image, 0, 0);    
}
    
/*---------------------------------------------------------------------------*/

void fview_synchro(const FView *fview, View *view)
{
    cassert_no_null(view);
    view_size(view, s2df(fview->min_width, fview->min_height));
    view_OnDraw(view, listener(NULL, i_OnDraw, void));
}

