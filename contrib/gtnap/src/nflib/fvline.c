/* Form vertical separator */

#include "fvline.h"
#include "nflib.inl"
#include <gui/line.h>
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FVline *fvline_create(void)
{
    return dbind_create(FVline);
}

/*---------------------------------------------------------------------------*/

void fvline_destroy(FVline **fvline)
{
    dbind_destroy(fvline, FVline);
}

/*---------------------------------------------------------------------------*/

void fvline_synchro(const FVline *fvline, Line *line)
{
    cassert_no_null(fvline);
    line_length(line, fvline->length);
}
