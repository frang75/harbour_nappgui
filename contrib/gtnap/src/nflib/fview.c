/* Form view */

#include "fview.h"
#include <gui/view.h>
#include <geom2d/s2d.h>
#include <core/dbind.h>
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

void fview_synchro(const FView *fview, View *view)
{
    cassert_no_null(view);
    view_size(view, s2df(fview->min_width, fview->min_height));
}

