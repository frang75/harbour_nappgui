/* Form panel */

#include "fpanel.h"
#include "nflib.h"
#include <gui/panel.h>
#include <geom2d/s2d.h>
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FPanel *fpanel_create(void)
{
    return dbind_create(FPanel);
}

/*---------------------------------------------------------------------------*/

void fpanel_destroy(FPanel **panel)
{
    dbind_destroy(panel, FPanel);
}

/*---------------------------------------------------------------------------*/

void fpanel_synchro(const FPanel *fpanel, Panel *panel)
{
    cassert_no_null(panel);
    panel_size(panel, s2df(fpanel->min_width, fpanel->min_height));
}

