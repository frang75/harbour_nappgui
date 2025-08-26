/* Form textview */

#include "ftext.h"
#include <gui/textview.h>
#include <geom2d/s2d.h>
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FText *ftext_create(void)
{
    return dbind_create(FText);
}

/*---------------------------------------------------------------------------*/

void ftext_destroy(FText **text)
{
    dbind_destroy(text, FText);
}

/*---------------------------------------------------------------------------*/

void ftext_synchro(const FText *ftext, TextView *view)
{
    cassert_no_null(ftext);
    textview_editable(view, !ftext->read_only);
    textview_size(view, s2df(ftext->min_width, ftext->min_height));
}
