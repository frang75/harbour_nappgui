/* Form progressbar */

#include "fprogress.h"
#include <gui/progress.h>
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FProgress *fprogress_create(void)
{
    return dbind_create(FProgress);
}

/*---------------------------------------------------------------------------*/

void fprogress_destroy(FProgress **fprogress)
{
    dbind_destroy(fprogress, FProgress);
}

/*---------------------------------------------------------------------------*/

void fprogress_synchro(const FProgress *fprogress, Progress *progress)
{
    cassert_no_null(fprogress);
    progress_min_width(progress, fprogress->min_width);
    progress_value(progress, .5f);
}
