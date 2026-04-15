/* Form font */

#include "ffont.h"
#include "nflib.inl"
#include <draw2d/font.h>
#include <core/dbind.h>
#include <core/stream.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

Font *ffont_font(const FFont *ffont)
{
    real32_t size = 0;
    uint32_t style = 0;

    cassert_no_null(ffont);
    if (ffont->size <= 0)
        size = font_regular_size();
    else
        size = ffont->size;

    if (ffont->bold == TRUE)
        style |= ekFBOLD;

    if (ffont->italic == TRUE)
        style |= ekFITALIC;

    if (ffont->underline == TRUE)
        style |= ekFUNDERLINE;

    if (ffont->strikeout == TRUE)
        style |= ekFSTRIKEOUT;

    switch (ffont->family)
    {
    case ekFFAMILY_REGULAR:
        return font_system(size, style);
    case ekFFAMILY_MONOSPACE:
        return font_monospace(size, style);
    default:
        cassert_default(ffont->family);
    }

    return NULL;
}

/*---------------------------------------------------------------------------*/

void ffont_init(FFont *font)
{
    dbind_init(font, FFont);
}

/*---------------------------------------------------------------------------*/

void ffont_remove(FFont *font)
{
    dbind_remove(font, FFont);
}

/*---------------------------------------------------------------------------*/

void ffont_read_init_ex(Stream *stm, FFont *ffont, const uint16_t *vers)
{
    cassert_no_null(ffont);
    cassert_no_null(vers);
    cassert_unref(*vers >= 12, vers);
    ffont->family = stm_read_enum(stm, ffamily_t);
    ffont->size = stm_read_r32(stm);
    ffont->bold = stm_read_bool(stm);
    ffont->italic = stm_read_bool(stm);
    ffont->underline = stm_read_bool(stm);
    ffont->strikeout = stm_read_bool(stm);
}

/*---------------------------------------------------------------------------*/

void ffont_write(Stream *stm, const FFont *ffont)
{
    cassert_no_null(ffont);
    stm_write_enum(stm, ffont->family, ffamily_t);
    stm_write_r32(stm, ffont->size);
    stm_write_bool(stm, ffont->bold);
    stm_write_bool(stm, ffont->italic);
    stm_write_bool(stm, ffont->underline);
    stm_write_bool(stm, ffont->strikeout);
}
