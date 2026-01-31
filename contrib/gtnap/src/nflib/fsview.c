/* Form scroll view */

#include "fsview.h"
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

FSView *fsview_create(void)
{
    return dbind_create(FSView);
}

/*---------------------------------------------------------------------------*/

void fsview_destroy(FSView **view)
{
    dbind_destroy(view, FSView);
}

/*---------------------------------------------------------------------------*/

static void i_draw(DCtx *ctx, const real32_t x, const real32_t y, const real32_t width, const real32_t height)
{
    const Image *image = nflib_default_view();
    real32_t imgwidth = (real32_t)image_width(image);
    real32_t imgheight = (real32_t)image_height(image);
    real32_t scalex = width /imgwidth;
    real32_t scaley = height / imgheight;
    T2Df t2d;
    t2d_movef(&t2d, kT2D_IDENTf, x, y);
    t2d_scalef(&t2d, &t2d, scalex, scaley);
    draw_matrixf(ctx, &t2d);
    draw_image(ctx, image, 0, 0);
}
    
/*---------------------------------------------------------------------------*/

static void i_OnDraw(void *nonused, Event *e)
{
    const EvDraw *p = event_params(e, EvDraw);
    unref(nonused);
    i_draw(p->ctx, 0, 0, p->width, p->height);
    i_draw(p->ctx, p->width, 0, p->width, p->height);
    i_draw(p->ctx, 0, p->height, p->width, p->height);
    i_draw(p->ctx, p->width, p->height, p->width, p->height);
}

/*---------------------------------------------------------------------------*/

static void i_OnSize(void *nonused, Event *e)
{
    EvSize *p = event_params(e, EvSize);
    View *view = event_sender(e, View);
    unref(nonused);
    view_content_size(view, s2df(p->width * 2, p->height * 2), s2df(10, 10));
}

/*---------------------------------------------------------------------------*/

void fsview_synchro(const FSView *fsview, View *view)
{
    cassert_no_null(view);
    view_size(view, s2df(fsview->min_width, fsview->min_height));
    view_OnDraw(view, listener(NULL, i_OnDraw, void));
    view_OnSize(view, listener(NULL, i_OnSize, void));
}

