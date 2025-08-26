/* Form listbox */

#include "flistbox.h"
#include <gui/listbox.h>
#include <draw2d/image.h>
#include <geom2d/s2d.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>

/*---------------------------------------------------------------------------*/

FListBox *flistbox_create(void)
{
    return dbind_create(FListBox);
}

/*---------------------------------------------------------------------------*/

void flistbox_destroy(FListBox **flistbox)
{
    dbind_destroy(flistbox, FListBox);
}

/*---------------------------------------------------------------------------*/

void flistbox_synchro(const FListBox *flistbox, ListBox *listbox, const char_t *resource_path)
{
    cassert_no_null(flistbox);
    listbox_size(listbox, s2df(flistbox->min_width, flistbox->min_height));
    listbox_clear(listbox);

    arrst_foreach_const(elem, flistbox->elems, FElem)
        Image *image = NULL;
        if (resource_path != NULL)
        {
            String *path = str_cpath("%s/%s", resource_path, tc(elem->iconpath));
            image = image_from_file(tc(path), NULL);
            str_destroy(&path);
        }

        listbox_add_elem(listbox, tc(elem->text), image);
        ptr_destopt(image_destroy, &image, Image);
    arrst_end()
}
