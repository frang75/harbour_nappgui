/* Form panel */

#include "fpanel.h"
#include "nflib.h"
#include <gui/panel.h>
#include <gui/panel.inl>
#include <gui/layout.h>
#include <geom2d/s2d.h>
#include <core/arrpt.h>
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
    ArrPt(Layout) *layouts = _panel_layouts(panel);
    cassert_no_null(fpanel);

    if (arrpt_size(layouts, Layout) == 0)
    {
        Layout *layout = layout_create(1, 1);
        panel_layout(panel, layout);
    }

    panel_size(panel, s2df(fpanel->min_width, fpanel->min_height));
}

