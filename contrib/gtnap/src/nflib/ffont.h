/* Form font */

#include "nflib.hxx"

__EXTERN_C

_nflib_api Font *ffont_font(const FFont *ffont);

_nflib_api void ffont_init(FFont *font);

_nflib_api void ffont_remove(FFont *font);

_nflib_api void ffont_read_init_ex(Stream *stm, FFont *ffont, const uint16_t *vers);

_nflib_api void ffont_write(Stream *stm, const FFont *ffont);

__END_C
