/* Form horizontal separator */

#include "fhline.h"
#include "nflib.inl"
#include <gui/line.h>
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FHline *fhline_create(void)
{
    return dbind_create(FHline);
}

/*---------------------------------------------------------------------------*/

void fhline_destroy(FHline **fhline)
{
    dbind_destroy(fhline, FHline);
}

/*---------------------------------------------------------------------------*/

void fhline_synchro(const FHline *fhline, Line *line)
{
    cassert_no_null(fhline);
    line_length(line, fhline->length);
}
