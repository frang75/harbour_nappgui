/* Design layout (editable parameters) */

#include "dlayout.h"
#include <gui/label.h>
#include <gui/layout.h>
#include <draw2d/color.h>
#include <core/arrst.h>
#include <core/dbind.h>
#include <sewer/bmem.h>
#include <sewer/cassert.h>

/*---------------------------------------------------------------------------*/

static void i_init_empty_column(DColumn *col)
{
    cassert_no_null(col);
    col->margin_right = 0;
}

/*---------------------------------------------------------------------------*/

static void i_init_empty_row(DRow *row)
{
    cassert_no_null(row);
    row->margin_bottom = 0;
}

/*---------------------------------------------------------------------------*/

static void i_remove_column(DColumn *column)
{
    dbind_remove(column, DColumn);
}

/*---------------------------------------------------------------------------*/

static void i_remove_row(DRow *row)
{
    dbind_remove(row, DRow);
}

/*---------------------------------------------------------------------------*/

static void i_remove_cell(DCell *cell)
{
    dbind_remove(cell, DCell);
}

/*---------------------------------------------------------------------------*/

static void i_init_empty_cell(DCell *cell)
{
    cassert_no_null(cell);
    bmem_zero(cell, DCell);
    cell->type = ekCELL_TYPE_EMPTY;
    cell->halign = ekLEFT;
    cell->valign = ekTOP;
}

/*---------------------------------------------------------------------------*/

DLayout *dlayout_create(const uint32_t ncols, const uint32_t nrows)
{
    DLayout *layout = dbind_create(DLayout);
    cassert(ncols > 0);
    cassert(nrows > 0);

    /* Add columns to layout */
    {
        uint32_t i = 0;
        for (i = 0; i < ncols; ++i)
        {
            DColumn *col = arrst_new(layout->cols, DColumn);
            i_init_empty_column(col);
        }
    }

    /* Add rows to layout */
    {
        uint32_t i = 0;
        for (i = 0; i < nrows; ++i)
        {
            DRow *row = arrst_new(layout->rows, DRow);
            i_init_empty_row(row);
        }
    }

    /* Add cells to layout */
    {
        uint32_t n = ncols * nrows, i = 0;
        for (i = 0; i < n; ++i)
        {
            DCell *cell = arrst_new(layout->cells, DCell);
            i_init_empty_cell(cell);
        }
    }

    return layout;
}

/*---------------------------------------------------------------------------*/

void dlayout_destroy(DLayout **layout)
{
    dbind_destroy(layout, DLayout);
}

/*---------------------------------------------------------------------------*/

void dlayout_insert_col(DLayout *layout, const uint32_t col)
{
    uint32_t ncols = 0, nrows = 0, i = 0;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, DColumn);
    nrows = arrst_size(layout->rows, DRow);

    /* Insert and init (empty) the new cells */
    for (i = 0; i < nrows; ++i)
    {
        uint32_t inspos = ((ncols + 1) * i) + col;
        DCell *cell = arrst_insert_n(layout->cells, inspos, 1, DCell);
        i_init_empty_cell(cell);
    }

    /* Add a new column */
    {
        DColumn *ncol = arrst_insert_n(layout->cols, col, 1, DColumn);
        i_init_empty_column(ncol);
    }
}

/*---------------------------------------------------------------------------*/

void dlayout_remove_col(DLayout *layout, const uint32_t col)
{
    uint32_t ncols = 0, nrows = 0, i = 0;
    cassert_no_null(layout);
    cassert(col < arrst_size(layout->cols, DColumn));
    ncols = arrst_size(layout->cols, DColumn);
    nrows = arrst_size(layout->rows, DRow);

    /* Destroy the column cells */
    for (i = 0; i < nrows; ++i)
    {
        uint32_t delrow = nrows - i - 1;
        uint32_t delpos = (ncols * delrow) + col;
        arrst_delete(layout->cells, delpos, i_remove_cell, DCell);
    }

    /* Destroy the column */
    arrst_delete(layout->cols, col, i_remove_column, DColumn);
}

/*---------------------------------------------------------------------------*/

void dlayout_insert_row(DLayout *layout, const uint32_t row)
{
    uint32_t ncols = 0, nrows = 0, i = 0;
    uint32_t inspos = 0;
    DCell *cells = NULL;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, DColumn);
    nrows = arrst_size(layout->rows, DRow);
    /* Cells insert position */
    inspos = row * ncols;
    /* Cells array is in row-major order. All row cells are together in memory */
    cells = arrst_insert_n(layout->cells, inspos, ncols, DCell);

    /* Initialize the new cells (empty) */
    for (i = 0; i < ncols; ++i)
    {
        i_init_empty_cell(cells + i);
    }

    /* Add a new row */
    {
        DRow *nrow = arrst_insert_n(layout->rows, row, 1, DRow);
        i_init_empty_row(nrow);
    }
}

/*---------------------------------------------------------------------------*/

void dlayout_remove_row(DLayout *layout, const uint32_t row)
{
    uint32_t i, ncols = 0, nrows = 0;
    cassert_no_null(layout);
    cassert(row < arrst_size(layout->rows, DRow));
    ncols = arrst_size(layout->cols, DColumn);
    nrows = arrst_size(layout->rows, DRow);

    /* Destroy the row cells */
    for (i = 0; i < ncols; ++i)
    {
        uint32_t delcol = ncols - i - 1;
        uint32_t delpos = (ncols * row) + delcol;
        arrst_delete(layout->cells, delpos, i_remove_cell, DCell);
    }

    /* Destroy the row */
    arrst_delete(layout->rows, row, i_remove_row, DRow);
}

/*---------------------------------------------------------------------------*/

void dlayout_add_layout(DLayout *layout, DLayout *sublayout, const uint32_t col, const uint32_t row)
{
    uint32_t i, ncols = 0;
    DCell *cell = NULL;
    cassert_no_null(layout);
    cassert_no_null(sublayout);
    ncols = arrst_size(layout->cols, DColumn);
    i = row * ncols + col;
    cell = arrst_get(layout->cells, i, DCell);
    i_remove_cell(cell);
    i_init_empty_cell(cell);
    cell->type = ekCELL_TYPE_LAYOUT;
    cell->content.layout = sublayout;
    cell->valign = ekJUSTIFY;
    cell->halign = ekJUSTIFY;
}

/*---------------------------------------------------------------------------*/

static color_t i_color(void)
{
    static uint32_t index = 0;
    switch(index++ % 3) {
    case 0:
        return kCOLOR_RED;
    case 1:
        return kCOLOR_BLUE;
    case 2:
        return kCOLOR_GREEN;
    cassert_default();
    }

    return kCOLOR_DEFAULT;
}

/*---------------------------------------------------------------------------*/

Layout *dlayout_gui_layout(const DLayout *layout)
{
    uint32_t ncols = 0, nrows = 0;
    Layout *glayout = NULL;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, DColumn);
    nrows = arrst_size(layout->rows, DRow);
    glayout = layout_create(ncols, nrows);

    /* Layout border margins */
    {
        real32_t mt = layout->margin_top;
        real32_t ml = layout->margin_left;
        real32_t mr = 0;
        real32_t mb = 0;

        {
            const DColumn *col = arrst_get_const(layout->cols, ncols - 1, DColumn);
            const DRow *row = arrst_get_const(layout->rows, nrows - 1, DRow);
            mr = col->margin_right;
            mb = row->margin_bottom;
        }

        layout_margin4(glayout, mt, mr, mb, ml);
    }

    /* Inter-column margins */
    {
        uint32_t i = 0;
        for (i = 0; i < ncols - 1; ++i)
        {
            const DColumn *col = arrst_get_const(layout->cols, i, DColumn);
            layout_hmargin(glayout, i, col->margin_right);
        }
    }

    /* Inter-row margins */
    {
        uint32_t i = 0;
        for (i = 0; i < nrows - 1; ++i)
        {
            const DRow *row = arrst_get_const(layout->rows, i, DRow);
            layout_vmargin(glayout, i, row->margin_bottom);
        }
    }

    /* Cells */
    {
        uint32_t i, j;
        const DCell *cells = arrst_all_const(layout->cells, DCell);
        for (j = 0; j < nrows; ++j)
        {
            for(i = 0; i < ncols; ++i)
            {
                switch(cells->type) {
                case ekCELL_TYPE_EMPTY:
                    break;

                /* TODO */
                case ekCELL_TYPE_LABEL:
                    cassert(FALSE);
                    break;

                case ekCELL_TYPE_LAYOUT:
                {
                    Layout *gsublayout = dlayout_gui_layout(cells->content.layout);
                    layout_layout(glayout, gsublayout, i, j);
                    break;
                }

                }

                cells += 1;
            }
        }
    }

    layout_bgcolor(glayout, i_color());
    return glayout;
}