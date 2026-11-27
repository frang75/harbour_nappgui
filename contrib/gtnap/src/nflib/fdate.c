/* Form dateview */

#include "fdate.h"
#include "ffont.h"
#include "nflib.inl"
#include <gui/dateview.h>
#include <draw2d/font.h>
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FDate *fdate_create(void)
{
    return dbind_create(FDate);
}

/*---------------------------------------------------------------------------*/

void fdate_destroy(FDate **date)
{
    dbind_destroy(date, FDate);
}

/*---------------------------------------------------------------------------*/

void fdate_synchro(const FDate *fdate, DateView *view)
{
    Font *font = NULL;
    cassert_no_null(fdate);
    font = ffont_font(&fdate->font);
    dateview_font(view, font);
    dateview_first_wday(view, _nflib_wday(fdate->first_wday));
    font_destroy(&font);
}
