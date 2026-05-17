/* Form tabs */

#include "ftabs.h"
#include <gui/tabs.h>
#include <draw2d/image.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>

/*---------------------------------------------------------------------------*/

FTabs *ftabs_create(void)
{
    return dbind_create(FTabs);
}

/*---------------------------------------------------------------------------*/

void ftabs_destroy(FTabs **ftabs)
{
    dbind_destroy(ftabs, FTabs);
}

/*---------------------------------------------------------------------------*/

void ftabs_synchro(const FTabs *ftabs, Tabs *tabs, const char_t *resource_path)
{
    cassert_no_null(ftabs);
    tabs_clear(tabs);

    arrst_foreach_const(elem, ftabs->elems, FElem)
        Image *image = NULL;
        if (resource_path != NULL)
        {
            String *path = str_cpath("%s/%s", resource_path, tc(elem->iconpath));
            image = image_from_file(tc(path), NULL);
            str_destroy(&path);
        }

        tabs_add_elem(tabs, tc(elem->text), image);
        ptr_destopt(image_destroy, &image, Image);
    arrst_end()
}
