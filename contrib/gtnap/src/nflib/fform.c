/* Form */

#include "fform.h"
#include "flayout.h"
#include "nflib.inl"
#include <core/dbind.h>
#include <core/heap.h>
#include <core/stream.h>
#include <core/strings.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

static const uint16_t i_FORM_HEADER = 0xFE55;
static const uint16_t i_VERSION = 0;

/*---------------------------------------------------------------------------*/

FForm *fform_create(void)
{
    FForm *form = heap_new0(FForm);
    form->description = str_c("");
    form->layout = flayout_create(1, 1);
    return form;
}

/*---------------------------------------------------------------------------*/

FForm *fform_read(Stream *stm)
{
    FForm *form = heap_new0(FForm);
    uint16_t header = stm_read_u16(stm);

    /* New form structure */
    if (header == i_FORM_HEADER)
    {
        uint16_t version = stm_read_u16(stm);
        form->description = str_read(stm);
        form->layout = flayout_read(stm);
        unref(version);
    }
    /* Old version, just a FLayout */
    else
    {
        form->description = str_c("");
        form->layout = flayout_read_with_vers(stm, header);
    }

    return form;
}

/*---------------------------------------------------------------------------*/

void fform_destroy(FForm **form)
{
    cassert_no_null(form);
    cassert_no_null(*form);
    str_destroy(&(*form)->description);
    flayout_destroy(&(*form)->layout);
    heap_delete(form, FForm);
    /* TODO: Change by */
    /* dbind_destroy(layout, FLayout); */
}

/*---------------------------------------------------------------------------*/

void fform_write(Stream *stm, const FForm *form)
{
    cassert_no_null(form);
    stm_write_u16(stm, i_FORM_HEADER);
    stm_write_u16(stm, i_VERSION);
    str_write(stm, form->description);
    flayout_write(stm, form->layout);
}

