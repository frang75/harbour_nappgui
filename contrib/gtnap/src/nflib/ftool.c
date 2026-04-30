/* Form tool button */

#include "ftool.h"
#include "nflib.h"
#include "nflib.inl"
#include <gui/button.h>
#include <draw2d/image.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FTool *ftool_create(void)
{
    return dbind_create(FTool);
}

/*---------------------------------------------------------------------------*/

void ftool_destroy(FTool **ftool)
{
    dbind_destroy(ftool, FTool);
}

/*---------------------------------------------------------------------------*/

void ftool_synchro(const FTool *ftool, Button *button, const char_t *resource_path)
{
    cassert_no_null(ftool);
    button_text(button, tc(ftool->text));
    button_tooltip(button, tc(ftool->tooltip));
    button_image_pos(button, _nflib_pos(ftool->imgpos));
    button_hpadding(button, ftool->hpadding);
    button_vpadding(button, ftool->vpadding);

    if (resource_path != NULL)
    {
        String *path = str_cpath("%s/%s", resource_path, tc(ftool->path));
        Image *image = image_from_file(tc(path), NULL);

        if (image != NULL)
        {
            button_image(button, image);
            image_destroy(&image);
        }
        else
        {
            const Image *rimage = nflib_default_icon();
            button_image(button, rimage);
        }

        str_destroy(&path);
    }
}
