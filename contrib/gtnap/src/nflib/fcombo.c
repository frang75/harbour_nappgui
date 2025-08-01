/* Form combobox */

#include "fcombo.h"
#include "nflib.inl"
#include <gui/combo.h>
#include <core/dbind.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

FCombo *fcombo_create(void)
{
    return dbind_create(FCombo);
}

/*---------------------------------------------------------------------------*/

void fcombo_synchro(const FCombo *fcombo, Combo *combo)
{
    cassert_no_null(fcombo);
    combo_passmode(combo, fcombo->passmode);
    combo_autoselect(combo, fcombo->autosel);
    combo_align(combo, _nflib_halign(fcombo->text_align));
    combo_min_width(combo, fcombo->min_width);
}
