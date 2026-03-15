/* Form editbox */

#include "fedit.h"
#include "nflib.inl"
#include <gui/edit.h>
#include <core/dbind.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FEdit *fedit_create(void)
{
    return dbind_create(FEdit);
}

/*---------------------------------------------------------------------------*/

void fedit_destroy(FEdit **fedit)
{
    dbind_destroy(fedit, FEdit);
}

/*---------------------------------------------------------------------------*/

void fedit_synchro(const FEdit *fedit, Edit *edit)
{
    cassert_no_null(fedit);
    edit_passmode(edit, fedit->passmode);
    edit_autoselect(edit, fedit->autosel);
    edit_align(edit, _nflib_halign(fedit->text_align));
    edit_width(edit, fedit->min_width);
}

