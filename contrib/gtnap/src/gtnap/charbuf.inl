/*
    This is part of gtnap

    GtNapCharBuf: a plain character-cell grid (glyph + fg/bg color per cell),
    independent of any widget/window state. Used by GTNAP windows created
    through NAP_TERMINAL() to behave like a classic Harbour text-mode GT.
*/

#include "gtnap.ixx"

__EXTERN_C

extern GtNapCharBuf *gtnap_charbuf_create(const uint32_t rows, const uint32_t cols);

extern void gtnap_charbuf_destroy(GtNapCharBuf **buf);

/* Resets every cell to a blank space with 'fg'/'bg'. */
extern void gtnap_charbuf_clear(GtNapCharBuf *buf, const color_t fg, const color_t bg);

/* Writes 'utf8_text' starting at (row, col), clipped at the right edge (no wrap). */
extern void gtnap_charbuf_write(GtNapCharBuf *buf, const uint32_t row, const uint32_t col, const char_t *utf8_text, const color_t fg, const color_t bg, const bool_t blink);

/* 'frame' has 8 codepoints in classic Harbour box order: TL, T, TR, R, BR, B, BL, L. */
extern void gtnap_charbuf_box(GtNapCharBuf *buf, const uint32_t top, const uint32_t left, const uint32_t bottom, const uint32_t right, const uint32_t frame[8], const color_t fg, const color_t bg, const bool_t blink);

extern GtNapCharReg *gtnap_charbuf_save(const GtNapCharBuf *buf, const uint32_t top, const uint32_t left, const uint32_t bottom, const uint32_t right);

/* Does not take ownership of 'region': it can be restored more than once. */
extern void gtnap_charbuf_restore(GtNapCharBuf *buf, const uint32_t top, const uint32_t left, const GtNapCharReg *region);

extern void gtnap_charbuf_region_destroy(GtNapCharReg **region);

extern void gtnap_charbuf_draw(const GtNapCharBuf *buf, DCtx *ctx, const Font *font, const real32_t cell_width, const real32_t cell_height, const bool_t blink_visible);

__END_C
