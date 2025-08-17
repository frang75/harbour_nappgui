/* Form popup */

#include "fpopup.h"
#include <gui/popup.h>
#include <draw2d/image.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>

/*---------------------------------------------------------------------------*/

FPopUp *fpopup_create(void)
{
    return dbind_create(FPopUp);
}

/*---------------------------------------------------------------------------*/

void fpopup_destroy(FPopUp **fpopup)
{
    dbind_destroy(fpopup, FPopUp);
}

/*---------------------------------------------------------------------------*/

void fpopup_synchro(const FPopUp *fpopup, PopUp *popup, const char_t *resource_path)
{
    cassert_no_null(fpopup);
    popup_clear(popup);

    arrst_foreach_const(elem, fpopup->elems, FElem)
        Image *image = NULL;
        if (resource_path != NULL)
        {
            String *path = str_cpath("%s/%s", resource_path, tc(elem->iconpath));
            image = image_from_file(tc(path), NULL);
            str_destroy(&path);
        }

        popup_add_elem(popup, tc(elem->text), image);
        ptr_destopt(image_destroy, &image, Image);
    arrst_end()
}
