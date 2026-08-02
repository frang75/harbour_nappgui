/*
    This is part of gtnap
*/

#include "charbuf.inl"
#include <draw2d/draw.h>
#include <core/heap.h>
#include <sewer/cassert.h>
#include <sewer/unicode.h>

typedef struct _gtnap_charcell_t GtNapCharCell;

struct _gtnap_charcell_t
{
    uint32_t codepoint;
    color_t fg;
    color_t bg;
    bool_t blink;
};

struct _gtnap_charbuf_t
{
    uint32_t rows;
    uint32_t cols;
    GtNapCharCell *cells;
};

struct _gtnap_charreg_t
{
    uint32_t rows;
    uint32_t cols;
    GtNapCharCell *cells;
};

/*---------------------------------------------------------------------------*/

static void i_put(GtNapCharBuf *buf, const uint32_t row, const uint32_t col, const uint32_t codepoint, const color_t fg, const color_t bg, const bool_t blink)
{
    if (row < buf->rows && col < buf->cols)
    {
        GtNapCharCell *cell = buf->cells + (row * buf->cols) + col;
        cell->codepoint = codepoint;
        cell->fg = fg;
        cell->bg = bg;
        cell->blink = blink;
    }
}

/*---------------------------------------------------------------------------*/

GtNapCharBuf *gtnap_charbuf_create(const uint32_t rows, const uint32_t cols)
{
    GtNapCharBuf *buf = heap_new0(GtNapCharBuf);
    buf->rows = rows;
    buf->cols = cols;
    buf->cells = heap_new_n0(rows * cols, GtNapCharCell);
    return buf;
}

/*---------------------------------------------------------------------------*/

void gtnap_charbuf_destroy(GtNapCharBuf **buf)
{
    cassert_no_null(buf);
    cassert_no_null(*buf);
    heap_delete_n(&(*buf)->cells, (*buf)->rows * (*buf)->cols, GtNapCharCell);
    heap_delete(buf, GtNapCharBuf);
}

/*---------------------------------------------------------------------------*/

void gtnap_charbuf_clear(GtNapCharBuf *buf, const color_t fg, const color_t bg)
{
    uint32_t i, n;
    cassert_no_null(buf);
    n = buf->rows * buf->cols;
    for (i = 0; i < n; ++i)
    {
        buf->cells[i].codepoint = (uint32_t)' ';
        buf->cells[i].fg = fg;
        buf->cells[i].bg = bg;
        buf->cells[i].blink = FALSE;
    }
}

/*---------------------------------------------------------------------------*/

void gtnap_charbuf_write(GtNapCharBuf *buf, const uint32_t row, const uint32_t col, const char_t *utf8_text, const color_t fg, const color_t bg, const bool_t blink)
{
    const char_t *p = utf8_text;
    uint32_t c = col;

    cassert_no_null(buf);
    cassert_no_null(utf8_text);

    while (*p != 0 && c < buf->cols)
    {
        uint32_t nbytes = 0;
        uint32_t codepoint = unicode_to_u32b(p, ekUTF8, &nbytes);
        i_put(buf, row, c, codepoint, fg, bg, blink);
        c += 1;
        p += nbytes;
    }
}

/*---------------------------------------------------------------------------*/

void gtnap_charbuf_box(GtNapCharBuf *buf, const uint32_t top, const uint32_t left, const uint32_t bottom, const uint32_t right, const uint32_t frame[8], const color_t fg, const color_t bg, const bool_t blink)
{
    uint32_t i, j;

    cassert_no_null(buf);
    cassert_no_null(frame);

    i_put(buf, top, left, frame[0], fg, bg, blink);
    i_put(buf, top, right, frame[2], fg, bg, blink);
    i_put(buf, bottom, right, frame[4], fg, bg, blink);
    i_put(buf, bottom, left, frame[6], fg, bg, blink);

    for (i = left + 1; i < right; ++i)
    {
        i_put(buf, top, i, frame[1], fg, bg, blink);
        i_put(buf, bottom, i, frame[5], fg, bg, blink);
    }

    for (j = top + 1; j < bottom; ++j)
    {
        i_put(buf, j, right, frame[3], fg, bg, blink);
        i_put(buf, j, left, frame[7], fg, bg, blink);
    }
}

/*---------------------------------------------------------------------------*/

GtNapCharReg *gtnap_charbuf_save(const GtNapCharBuf *buf, const uint32_t top, const uint32_t left, const uint32_t bottom, const uint32_t right)
{
    GtNapCharReg *region;
    uint32_t rows, cols;
    uint32_t i, j;

    cassert_no_null(buf);
    cassert(bottom >= top);
    cassert(right >= left);

    rows = bottom - top + 1;
    cols = right - left + 1;

    region = heap_new0(GtNapCharReg);
    region->rows = rows;
    region->cols = cols;
    region->cells = heap_new_n0(rows * cols, GtNapCharCell);

    for (j = 0; j < rows; ++j)
    {
        uint32_t src_row = top + j;
        for (i = 0; i < cols; ++i)
        {
            uint32_t src_col = left + i;
            if (src_row < buf->rows && src_col < buf->cols)
                region->cells[(j * cols) + i] = buf->cells[(src_row * buf->cols) + src_col];
        }
    }

    return region;
}

/*---------------------------------------------------------------------------*/

void gtnap_charbuf_restore(GtNapCharBuf *buf, const uint32_t top, const uint32_t left, const GtNapCharReg *region)
{
    uint32_t i, j;

    cassert_no_null(buf);
    cassert_no_null(region);

    for (j = 0; j < region->rows; ++j)
    {
        for (i = 0; i < region->cols; ++i)
        {
            const GtNapCharCell *cell = region->cells + (j * region->cols) + i;
            i_put(buf, top + j, left + i, cell->codepoint, cell->fg, cell->bg, cell->blink);
        }
    }
}

/*---------------------------------------------------------------------------*/

void gtnap_charbuf_region_destroy(GtNapCharReg **region)
{
    cassert_no_null(region);
    cassert_no_null(*region);
    heap_delete_n(&(*region)->cells, (*region)->rows * (*region)->cols, GtNapCharCell);
    heap_delete(region, GtNapCharReg);
}

/*---------------------------------------------------------------------------*/

void gtnap_charbuf_draw(const GtNapCharBuf *buf, DCtx *ctx, const Font *font, const real32_t cell_width, const real32_t cell_height, const bool_t blink_visible)
{
    uint32_t i, j;
    char_t glyph[8];

    cassert_no_null(buf);
    cassert_no_null(ctx);

    draw_font(ctx, font);
    draw_text_align(ctx, ekLEFT, ekTOP);

    for (j = 0; j < buf->rows; ++j)
    {
        real32_t y = (real32_t)j * cell_height;

        for (i = 0; i < buf->cols; ++i)
        {
            const GtNapCharCell *cell = buf->cells + (j * buf->cols) + i;
            real32_t x = (real32_t)i * cell_width;

            draw_fill_color(ctx, cell->bg);
            draw_rect(ctx, ekFILL, x, y, cell_width, cell_height);

            if (cell->codepoint != 0 && cell->codepoint != (uint32_t)' ' && (cell->blink == FALSE || blink_visible == TRUE))
            {
                uint32_t nbytes = unicode_to_char(cell->codepoint, glyph, ekUTF8);
                glyph[nbytes] = 0;
                draw_text_color(ctx, cell->fg);
                draw_text(ctx, glyph, x, y);
            }
        }
    }
}
