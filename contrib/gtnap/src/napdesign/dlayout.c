/* Designer layout */

#include "dlayout.h"
#include "imgproc.h"
#include "res_designer.h"
#include <nflib/nflib.h>
#include <nflib/flayout.h>
#include <gui/gui.h>
#include <gui/button.h>
#include <gui/label.h>
#include <gui/layout.h>
#include <gui/layouth.h>
#include <gui/listbox.h>
#include <gui/tableviewh.h>
#include <gui/edit.h>
#include <gui/cell.h>
#include <gui/popup.h>
#include <gui/drawctrl.inl>
#include <draw2d/color.h>
#include <draw2d/dctx.h>
#include <draw2d/draw.h>
#include <draw2d/drawg.h>
#include <draw2d/font.h>
#include <draw2d/image.h>
#include <geom2d/r2d.h>
#include <geom2d/v2d.h>
#include <geom2d/t2d.h>
#include <core/arrpt.h>
#include <core/arrst.h>
#include <core/heap.h>
#include <core/strings.h>
#include <sewer/bmath.h>
#include <sewer/bmem.h>
#include <sewer/bstd.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>

/*---------------------------------------------------------------------------*/

void dlayout_global_init(void)
{
}

/*---------------------------------------------------------------------------*/

static void i_destroy_image(Image **image)
{
    cassert_no_null(image);
    if (*image != NULL)
        image_destroy(image);
}

/*---------------------------------------------------------------------------*/

static void i_remove_cell(DCell *cell)
{
    cassert_no_null(cell);
    if (cell->sublayout != NULL)
        dlayout_destroy(&cell->sublayout);
    arrpt_destopt(&cell->nimages, i_destroy_image, Image);
    arrpt_destopt(&cell->simages, i_destroy_image, Image);
}

/*---------------------------------------------------------------------------*/

DLayout *dlayout_from_flayout(const FLayout *flayout, const char_t *resource_path, const DColors *colors)
{
    DLayout *layout = heap_new(DLayout);
    uint32_t i, ncols = flayout_ncols(flayout);
    uint32_t j, nrows = flayout_nrows(flayout);
    DCell *dcell = NULL;
    cassert_no_null(flayout);
    layout->cols = arrst_create(DColumn);
    layout->rows = arrst_create(DRow);
    layout->cells = arrst_create(DCell);
    arrst_new_n0(layout->cols, ncols, DColumn);
    arrst_new_n0(layout->rows, nrows, DRow);
    arrst_new_n0(layout->cells, ncols * nrows, DCell);
    dcell = arrst_all(layout->cells, DCell);
    for (j = 0; j < nrows; ++j)
    {
        for (i = 0; i < ncols; ++i)
        {
            const FCell *fcell = flayout_ccell(flayout, i, j);
            if (fcell->type == ekCELL_TYPE_TOOL)
            {
                String *path = str_printf("%s%s", resource_path, tc(fcell->widget.tool->path));
                Image *image = image_from_file(tc(path), NULL);

                if (image != NULL)
                {
                    dlayout_set_image(layout, image, i, j, colors);
                    image_destroy(&image);
                }
                else
                {
                    dlayout_set_image(layout, nflib_default_icon(), i, j, colors);
                }

                str_destroy(&path);
            }
            else if (fcell->type == ekCELL_TYPE_IMAGE)
            {
                Image *image = NULL;
                if (str_empty(fcell->widget.image->path) == FALSE)
                {
                    String *path = str_printf("%s%s", resource_path, tc(fcell->widget.image->path));
                    image = image_from_file(tc(path), NULL);
                    str_destroy(&path);
                }
                dlayout_set_image(layout, image, i, j, colors);
                ptr_destopt(image_destroy, &image, Image);
            }
            else if (fcell->type == ekCELL_TYPE_POPUP)
            {
                dlayout_synchro_elems(layout, i, j, fcell->widget.popup->elems, resource_path, colors);
            }
            else if (fcell->type == ekCELL_TYPE_LISTBOX)
            {
                arrst_foreach_const(elem, fcell->widget.listbox->elems, FElem)
                    Image *image = NULL;
                    if (str_empty(elem->iconpath) == FALSE)
                    {
                        String *path = str_printf("%s%s", resource_path, tc(elem->iconpath));
                        image = image_from_file(tc(path), NULL);
                        str_destroy(&path);
                    }
                    dlayout_add_image(layout, image, i, j, colors);
                    ptr_destopt(image_destroy, &image, Image);
                arrst_end()
            }
            else if (fcell->type == ekCELL_TYPE_LAYOUT)
            {
                dcell->sublayout = dlayout_from_flayout(fcell->widget.layout, resource_path, colors);
            }

            dcell += 1;
        }
    }

    return layout;
}

/*---------------------------------------------------------------------------*/

void dlayout_destroy(DLayout **layout)
{
    cassert_no_null(layout);
    cassert_no_null(*layout);
    arrst_destroy(&(*layout)->cols, NULL, DColumn);
    arrst_destroy(&(*layout)->rows, NULL, DRow);
    arrst_destroy(&(*layout)->cells, i_remove_cell, DCell);
    heap_delete(layout, DLayout);
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
        arrst_insert_n0(layout->cells, inspos, 1, DCell);
    }

    /* Add a new column */
    arrst_insert_n0(layout->cols, col, 1, DColumn);
}

/*---------------------------------------------------------------------------*/

void dlayout_remove_col(DLayout *layout, const uint32_t col)
{
    uint32_t ncols = 0, nrows = 0, i = 0;
    cassert_no_null(layout);
    cassert(arrst_size(layout->cols, DColumn) >= 1);
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
    arrst_delete(layout->cols, col, NULL, DColumn);
}

/*---------------------------------------------------------------------------*/

void dlayout_insert_row(DLayout *layout, const uint32_t row)
{
    uint32_t ncols = 0;
    uint32_t inspos = 0;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, DColumn);
    /* Cells insert position */
    inspos = row * ncols;
    /* Cells array is in row-major order. All row cells are together in memory */
    arrst_insert_n0(layout->cells, inspos, ncols, DCell);
    /* Add a new row */
    arrst_insert_n0(layout->rows, row, 1, DRow);
}

/*---------------------------------------------------------------------------*/

void dlayout_remove_row(DLayout *layout, const uint32_t row)
{
    uint32_t i, ncols = 0;
    cassert_no_null(layout);
    cassert(arrst_size(layout->rows, DRow) >= 1);
    cassert(row < arrst_size(layout->rows, DRow));
    ncols = arrst_size(layout->cols, DColumn);

    /* Destroy the row cells */
    for (i = 0; i < ncols; ++i)
    {
        uint32_t delcol = ncols - i - 1;
        uint32_t delpos = (ncols * row) + delcol;
        arrst_delete(layout->cells, delpos, i_remove_cell, DCell);
    }

    /* Destroy the row */
    arrst_delete(layout->rows, row, NULL, DRow);
}

/*---------------------------------------------------------------------------*/

static ___INLINE DCell *i_cell(DLayout *layout, const uint32_t col, const uint32_t row)
{
    uint32_t ncols = UINT32_MAX;
    uint32_t pos = UINT32_MAX;
    cassert_no_null(layout);
    ncols = arrst_size(layout->cols, DColumn);
    pos = row * ncols + col;
    return arrst_get(layout->cells, pos, DCell);
}

/*---------------------------------------------------------------------------*/

void dlayout_remove_cell(DLayout *layout, const uint32_t col, const uint32_t row)
{
    DCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    i_remove_cell(cell);
}

/*---------------------------------------------------------------------------*/

void dlayout_add_layout(DLayout *layout, DLayout *sublayout, const uint32_t col, const uint32_t row)
{
    DCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    cassert_no_null(sublayout);
    cassert(cell->sublayout == NULL);
    cell->sublayout = sublayout;
}

/*---------------------------------------------------------------------------*/

static void i_set_image(DLayout *layout, const Image *image, const uint32_t index, const uint32_t col, const uint32_t row, const DColors *colors)
{
    DCell *cell = i_cell(layout, col, row);
    uint32_t n = UINT32_MAX;
    Image *nimage = NULL;
    Image *simage = NULL;

    cassert_no_null(cell);
    cassert_no_null(colors);
    if (cell->nimages == NULL)
    {
        cassert(cell->simages == NULL);
        cell->nimages = arrpt_create(Image);
        cell->simages = arrpt_create(Image);
    }

    n = arrpt_size(cell->nimages, Image);
    cassert(n == arrpt_size(cell->simages, Image));
    if (image != NULL)
    {
        nimage = imgproc_binarize(image, colors->main, colors->back);
        simage = imgproc_binarize(image, colors->select, colors->back);
    }

    /* Change the current image */
    if (index < n)
    {
        Image **pnimage = arrpt_all(cell->nimages, Image);
        Image **psimage = arrpt_all(cell->simages, Image);
        if (pnimage[index] != NULL)
        {
            image_destroy(&pnimage[index]);
            image_destroy(&psimage[index]);
        }
        else
        {
            cassert(psimage[index] == NULL);
        }

        pnimage[index] = nimage;
        psimage[index] = simage;
    }
    /* Add image at the end */
    else
    {
        cassert(index == n || index == UINT32_MAX);
        arrpt_append(cell->nimages, nimage, Image);
        arrpt_append(cell->simages, simage, Image);
    }
}

/*---------------------------------------------------------------------------*/

void dlayout_set_image(DLayout *layout, const Image *image, const uint32_t col, const uint32_t row, const DColors *colors)
{
    i_set_image(layout, image, 0, col, row, colors);
}

/*---------------------------------------------------------------------------*/

void dlayout_add_image(DLayout *layout, const Image *image, const uint32_t col, const uint32_t row, const DColors *colors)
{
    i_set_image(layout, image, UINT32_MAX, col, row, colors);
}

/*---------------------------------------------------------------------------*/

void dlayout_del_image(DLayout *layout, const uint32_t index, const uint32_t col, const uint32_t row)
{
    DCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    arrpt_delete(cell->nimages, index, i_destroy_image, Image);
    arrpt_delete(cell->simages, index, i_destroy_image, Image);
}

/*---------------------------------------------------------------------------*/

void dlayout_clear_images(DLayout *layout, const uint32_t col, const uint32_t row)
{
    DCell *cell = i_cell(layout, col, row);
    cassert_no_null(cell);
    if (cell->nimages != NULL)
    {
        arrpt_clear(cell->nimages, i_destroy_image, Image);
        arrpt_clear(cell->simages, i_destroy_image, Image);
    }
    else
    {
        cassert(cell->simages == NULL);
    }
}

/*---------------------------------------------------------------------------*/

void dlayout_synchro_elems(DLayout *layout, const uint32_t col, const uint32_t row, const ArrSt(FElem) *elems, const char_t *resource_path, const DColors *colors)
{
    dlayout_clear_images(layout, col, row);
    arrst_foreach_const(elem, elems, FElem)
        Image *image = NULL;
        if (str_empty(elem->iconpath) == FALSE)
        {
            String *path = str_cpath("%s/%s", resource_path, tc(elem->iconpath));
            image = image_from_file(tc(path), NULL);
            str_destroy(&path);
        }

        dlayout_add_image(layout, image, col, row, colors);
        ptr_destopt(image_destroy, &image, Image);
    arrst_end()
}

/*---------------------------------------------------------------------------*/

void dlayout_synchro_visual(DLayout *layout, const Layout *glayout, const V2Df origin)
{
    DColumn *col = NULL;
    DRow *row = NULL;
    DCell *cell = NULL;
    uint32_t ncols, nrows, i, j;
    real32_t total_width = 0, total_height = 0;
    real32_t inner_width = 0, inner_height = 0;
    real32_t mtop = 0, mbottom = 0, mleft = 0, mright = 0;
    real32_t x, y;
    cassert_no_null(layout);
    col = arrst_all(layout->cols, DColumn);
    row = arrst_all(layout->rows, DRow);
    cell = arrst_all(layout->cells, DCell);
    ncols = arrst_size(layout->cols, DColumn);
    nrows = arrst_size(layout->rows, DRow);
    cassert(ncols == layout_ncols(glayout));
    cassert(nrows == layout_nrows(glayout));
    mtop = layout_get_margin_top(glayout);
    mbottom = layout_get_margin_bottom(glayout);
    mleft = layout_get_margin_left(glayout);
    mright = layout_get_margin_right(glayout);

    /* Compute the columns width and total layout widths */
    total_width = 0;
    inner_width = 0;
    for (i = 0; i < ncols; ++i)
    {
        col[i].width = layout_get_hsize(glayout, i);
        col[i].rect.pos.x = origin.x + mleft + inner_width;
        col[i].rect.pos.y = origin.y + mtop;
        col[i].rect.size.width = col[i].width;             
        inner_width += col[i].width;

        if (i < ncols - 1)
            inner_width += layout_get_hmargin(glayout, i);
        else
            total_width = inner_width + mleft + mright;
    }

    /* Compute the rows height and total layout heights */
    total_height = 0;
    inner_height = 0;
    for (j = 0; j < nrows; ++j)
    {
        row[j].height = layout_get_vsize(glayout, j);
        row[j].rect.pos.x = origin.x + mleft;
        row[j].rect.pos.y = origin.y + mtop + inner_height;
        row[j].rect.size.height = row[j].height;
        inner_height += row[j].height;
        if (j < nrows - 1)
            inner_height += layout_get_vmargin(glayout, j);
        else
            total_height = inner_height + mtop + mbottom;
    }

    /* Global layout rectangle */
    layout->rect = r2df(origin.x, origin.y, total_width, total_height);

    /* Complete the columns and rows */
    for (i = 0; i < ncols; ++i)
        col[i].rect.size.height = total_height - mtop - mbottom;

    for (j = 0; j < nrows; ++j)
        row[j].rect.size.width = total_width - mleft - mright;

    /* Compute the vertical rectangles */
    layout->rect_left = r2df(origin.x, origin.y, mleft, total_height);
    x = origin.x + mleft;
    for (i = 0; i < ncols; ++i)
    {
        x += col[i].width;
        if (i < ncols - 1)
        {
            real32_t cmright = layout_get_hmargin(glayout, i);
            col[i].margin_rect = r2df(x, origin.y, cmright, total_height);
            x += cmright;
        }
        else
        {
            col[i].margin_rect = kR2D_ZEROf;
        }
    }
    layout->rect_right = r2df(x, origin.y, mright, total_height);

    /* Compute the horizontal rectangles */
    layout->rect_top = r2df(origin.x, origin.y, total_width, mtop);
    y = origin.y + mtop;
    for (j = 0; j < nrows; ++j)
    {
        y += row[j].height;
        if (j < nrows - 1)
        {
            real32_t cmbottom = layout_get_vmargin(glayout, j);
            row[j].margin_rect = r2df(origin.x, y, total_width, cmbottom);
            y += cmbottom;
        }
        else
        {
            row[j].margin_rect = kR2D_ZEROf;
        }
    }
    layout->rect_bottom = r2df(origin.x, y, total_width, mbottom);

    /* Compute the cells rectangles */
    {
        DCell *dcell = cell;
        y = origin.y + mtop;

        for (j = 0; j < nrows; ++j)
        {
            x = origin.x + mleft;
            for (i = 0; i < ncols; ++i)
            {
                const Cell *gcell = layout_cell(cast(glayout, Layout), i, j);
                real32_t hsize = cell_get_hsize(gcell);
                real32_t vsize = cell_get_vsize(gcell);
                align_t halign = cell_get_halign(gcell);
                align_t valign = cell_get_valign(gcell);
                real32_t cellx = 0;
                real32_t celly = 0;
                cassert_no_null(gcell);
                cassert(hsize <= col[i].width);
                cassert(vsize <= row[j].height);

                switch (halign)
                {
                case ekLEFT:
                    cellx = x;
                    break;
                case ekCENTER:
                    cellx = x + (col[i].width - hsize) / 2;
                    break;
                case ekRIGHT:
                    cellx = x + (col[i].width - hsize);
                    break;
                case ekJUSTIFY:
                    cellx = x;
                    break;
                default:
                    cellx = x;
                    break;
                }

                switch (valign)
                {
                case ekTOP:
                    celly = y;
                    break;
                case ekCENTER:
                    celly = y + (row[j].height - vsize) / 2;
                    break;
                case ekBOTTOM:
                    celly = y + (row[j].height - vsize);
                    break;
                case ekJUSTIFY:
                    celly = y;
                    break;
                default:
                    celly = y;
                    break;
                }

                if (dcell->sublayout != NULL)
                {
                    Layout *subglayout = layout_get_layout(cast(glayout, Layout), i, j);
                    dlayout_synchro_visual(dcell->sublayout, subglayout, v2df(cellx, celly));
                }

                dcell->rect = r2df(x, y, col[i].width, row[j].height);
                dcell->content_rect = r2df(cellx, celly, hsize, vsize);
                dcell += 1;

                x += col[i].width;
                if (i < ncols - 1)
                    x += layout_get_hmargin(glayout, i);
            }

            y += row[j].height;
            if (j < nrows - 1)
                y += layout_get_vmargin(glayout, j);
        }
    }
}

/*---------------------------------------------------------------------------*/

void dlayout_elem_at_pos(const DLayout *dlayout, const FLayout *flayout, const Layout *glayout, const real32_t x, const real32_t y, ArrSt(DSelect) *selpath)
{
    cassert_no_null(dlayout);
    if (r2d_containsf(&dlayout->rect, x, y) == TRUE)
    {
        DSelect *sel = arrst_new(selpath, DSelect);
        sel->dlayout = cast(dlayout, DLayout);
        sel->flayout = cast(flayout, FLayout);
        sel->glayout = cast(glayout, Layout);

        if (r2d_containsf(&dlayout->rect_left, x, y) == TRUE)
        {
            sel->elem = ekLAYELEM_MARGIN_LEFT;
            sel->col = UINT32_MAX;
            sel->row = UINT32_MAX;
            return;
        }

        if (r2d_containsf(&dlayout->rect_top, x, y) == TRUE)
        {
            sel->elem = ekLAYELEM_MARGIN_TOP;
            sel->col = UINT32_MAX;
            sel->row = UINT32_MAX;
            return;
        }

        if (r2d_containsf(&dlayout->rect_right, x, y) == TRUE)
        {
            sel->elem = ekLAYELEM_MARGIN_RIGHT;
            sel->col = UINT32_MAX;
            sel->row = UINT32_MAX;
            return;
        }

        if (r2d_containsf(&dlayout->rect_bottom, x, y) == TRUE)
        {
            sel->elem = ekLAYELEM_MARGIN_BOTTOM;
            sel->col = UINT32_MAX;
            sel->row = UINT32_MAX;
            return;
        }

        arrst_foreach_const(col, dlayout->cols, DColumn)
            if (col_i < col_total - 1)
            {
                if (r2d_containsf(&col->margin_rect, x, y) == TRUE)
                {
                    sel->elem = ekLAYELEM_MARGIN_COLUMN;
                    sel->col = col_i;
                    sel->row = UINT32_MAX;
                    return;
                }
            }
        arrst_end()

        arrst_foreach_const(row, dlayout->rows, DRow)
            if (row_i < row_total - 1)
            {
                if (r2d_containsf(&row->margin_rect, x, y) == TRUE)
                {
                    sel->elem = ekLAYELEM_MARGIN_ROW;
                    sel->col = UINT32_MAX;
                    sel->row = row_i;
                    return;
                }
            }
        arrst_end()

        arrst_foreach_const(cell, dlayout->cells, DCell)
            if (r2d_containsf(&cell->rect, x, y) == TRUE)
            {
                uint32_t ncols = arrst_size(dlayout->cols, DColumn);
                sel->elem = ekLAYELEM_CELL;
                sel->col = cell_i % ncols;
                sel->row = cell_i / ncols;
                if (cell->sublayout != NULL)
                {
                    const FCell *fcell = flayout_ccell(flayout, sel->col, sel->row);
                    Layout *gsublayout = layout_get_layout(cast(glayout, Layout), sel->col, sel->row);
                    dlayout_elem_at_pos(cell->sublayout, fcell->widget.layout, gsublayout, x, y, selpath);
                }

                return;
            }
        arrst_end()
    }
}

/*---------------------------------------------------------------------------*/

static R2Df i_cell_rect(const DLayout *dlayout, const DSelect *sel)
{
    uint32_t ncols, pos;
    const DCell *cell = NULL;
    cassert_no_null(dlayout);
    cassert_no_null(sel);
    cassert(dlayout == sel->dlayout);
    cassert(sel->elem == ekLAYELEM_CELL);
    ncols = arrst_size(dlayout->cols, DColumn);
    pos = sel->row * ncols + sel->col;
    cell = arrst_get_const(dlayout->cells, pos, DCell);
    return cell->rect;
}

/*---------------------------------------------------------------------------*/

static R2Df i_get_rect(const DLayout *dlayout, const DSelect *sel)
{
    cassert_no_null(dlayout);
    cassert_no_null(sel);
    cassert(dlayout == sel->dlayout);
    switch (sel->elem)
    {
    case ekLAYELEM_MARGIN_LEFT:
        return dlayout->rect_left;

    case ekLAYELEM_MARGIN_TOP:
        return dlayout->rect_top;

    case ekLAYELEM_MARGIN_RIGHT:
        return dlayout->rect_right;

    case ekLAYELEM_MARGIN_BOTTOM:
        return dlayout->rect_bottom;

    case ekLAYELEM_MARGIN_COLUMN:
    {
        const DColumn *col = arrst_get_const(dlayout->cols, sel->col, DColumn);
        return col->margin_rect;
    }

    case ekLAYELEM_MARGIN_ROW:
    {
        const DRow *row = arrst_get_const(dlayout->rows, sel->row, DRow);
        return row->margin_rect;
    }

    case ekLAYELEM_LAYOUT:
        return dlayout->rect;

    case ekLAYELEM_CELL:
        return i_cell_rect(dlayout, sel);

    default:
        cassert_default(sel->elem);
    }

    return kR2D_ZEROf;
}

/*---------------------------------------------------------------------------*/

static bool_t i_is_cell_sel(const DSelect *sel, const DLayout *dlayout, const uint32_t col, const uint32_t row)
{
    cassert_no_null(sel);
    if (sel->dlayout != dlayout)
        return FALSE;
    if (sel->elem != ekLAYELEM_CELL)
        return FALSE;
    if (sel->col != col)
        return FALSE;
    if (sel->row != row)
        return FALSE;
    return TRUE;
}

/*---------------------------------------------------------------------------*/

static V2Df i_image_transform(T2Df *t2d, const scale_t scale, const R2Df *cell_rect, const real32_t img_width, const real32_t img_height)
{
    real32_t ratio_x = 1.f;
    real32_t ratio_y = 1.f;
    cassert_no_null(cell_rect);
    switch (scale)
    {
    case ekSCALE_NONE:
    case ekSCALE_FIT:
        ratio_x = 1.f;
        ratio_y = 1.f;
        break;

    case ekSCALE_AUTO:
        ratio_x = cell_rect->size.width / img_width;
        ratio_y = cell_rect->size.height / img_height;
        break;

    case ekSCALE_ASPECT:
        ratio_x = cell_rect->size.width / img_width;
        ratio_y = cell_rect->size.height / img_height;
        if (ratio_x < ratio_y)
            ratio_y = ratio_x;
        else
            ratio_x = ratio_y;
        break;

    default:
        cassert_default(scale);
    }

    {
        real32_t fimg_width = bmath_roundf(ratio_x * img_width);
        real32_t fimg_height = bmath_roundf(ratio_y * img_height);
        real32_t fx = bmath_floorf(.5f * (cell_rect->size.width - fimg_width));
        real32_t fy = bmath_floorf(.5f * (cell_rect->size.height - fimg_height));
        V2Df origin = v2df(-fx, -fy);
        t2d_movef(t2d, kT2D_IDENTf, fx + cell_rect->pos.x, fy + cell_rect->pos.y);
        t2d_scalef(t2d, t2d, ratio_x, ratio_y);
        return origin;
    }
}

/*---------------------------------------------------------------------------*/

static const Image *i_get_image(const DCell *cell, const uint32_t index, const bool_t sel)
{
    cassert_no_null(cell);
    if (sel == TRUE)
        return arrpt_get_const(cell->simages, index, Image);
    else
        return arrpt_get_const(cell->nimages, index, Image);
}

/*---------------------------------------------------------------------------*/

static void i_draw_arrow(DCtx *ctx, const real32_t x, const real32_t y, const real32_t width, const real32_t height)
{
    V2Df points[3];
    points[0].x = x + width - 5;
    points[0].y = y + height / 2 - 4;
    points[1].x = points[0].x - 8;
    points[1].y = points[0].y;
    points[2].x = (points[0].x + points[1].x) / 2;
    points[2].y = points[0].y + 6;
    draw_polygon(ctx, ekFILL, points, 3);
}

/*---------------------------------------------------------------------------*/

static void i_draw_grid(DCtx *ctx, const DColors *colors, const R2Df *rect)
{
    const real32_t sep = 10;
    uint32_t nc, nr, i, j;
    real32_t x, y;
    cassert_no_null(colors);
    cassert_no_null(rect);
    nc = (uint32_t)bmath_ceilf(rect->size.width / sep);
    nr = (uint32_t)bmath_ceilf(rect->size.height / sep);
    x = rect->pos.x;
    draw_fill_color(ctx, colors->main);
    for (i = 0; i < nc; ++i, x += sep)
    {
        y = rect->pos.y;
        for (j = 0; j < nr; ++j, y += sep)
            draw_circle(ctx, ekFILL, x, y, .5f);
    }
}

/*---------------------------------------------------------------------------*/

static void i_draw_frame(DCtx *ctx, const Font *font, const DColors *colors, const char_t *form_name, const R2Df *rect)
{
    const real32_t HEADER = 30;
    const real32_t EDGE = 10;
    color_t c[2];
    real32_t s[2] = {0, 1};
    cassert_no_null(colors);
    cassert_no_null(rect);
    c[0] = colors->title0;
    c[1] = colors->title1;
    draw_fill_color(ctx, c[1]);
    draw_rect(ctx, ekFILL, rect->pos.x - EDGE, rect->pos.y - 2, EDGE, rect->size.height + 4);
    draw_rect(ctx, ekFILL, rect->pos.x + rect->size.width, rect->pos.y - 2, EDGE, rect->size.height + 4);
    draw_rect(ctx, ekFILL, rect->pos.x - EDGE, rect->pos.y + rect->size.height, rect->size.width + EDGE * 2, EDGE);
    draw_fill_linear(ctx, c, s, 2, 0, rect->pos.y - HEADER, 0, rect->pos.y);
    draw_rect(ctx, ekFILL, rect->pos.x - EDGE, rect->pos.y - HEADER, rect->size.width + EDGE * 2, HEADER);
    draw_line_color(ctx, kCOLOR_BLACK);
    draw_rect(ctx, ekSTROKE, rect->pos.x - EDGE, rect->pos.y - HEADER, rect->size.width + EDGE * 2, rect->size.height + HEADER + EDGE);

    /* Icon and title */
    {
        real32_t iwidth = (real32_t)image_width(colors->nap_icon);
        real32_t iheight = (real32_t)image_height(colors->nap_icon);
        real32_t xpos = rect->pos.x;
        real32_t ypos = rect->pos.y - iheight - 4;
        String *name = str_printf("%s - %s", gui_text(TEXT_FORM), form_name);
        draw_image(ctx, colors->nap_icon, xpos, ypos);
        draw_text_width(ctx, rect->size.width - (real32_t)iwidth - 5);
        draw_font(ctx, font);
        drawctrl_text(ctx, tc(name), (int32_t)(xpos + iwidth + 5), (int32_t)ypos, ekCTRL_STATE_NORMAL);
        str_destroy(&name);
    }
}

/*---------------------------------------------------------------------------*/

static void i_draw_layout(const DLayout *dlayout, const FLayout *flayout, const Layout *glayout, const DSelect *hover, const DSelect *sel, const widget_t swidget, const Font *default_font, const DColors *colors, DCtx *ctx)
{
    uint32_t ncols, nrows, i, j, radio_i = 0;
    const DCell *dcell = NULL;
    cassert_no_null(dlayout);
    cassert_no_null(flayout);
    cassert_no_null(hover);
    cassert_no_null(colors);
    ncols = arrst_size(dlayout->cols, DColumn);
    nrows = arrst_size(dlayout->rows, DRow);
    dcell = arrst_all_const(dlayout->cells, DCell);
    cassert(ncols == flayout_ncols(flayout));
    cassert(nrows == flayout_nrows(flayout));

    for (j = 0; j < nrows; ++j)
    {
        for (i = 0; i < ncols; ++i)
        {
            const FCell *fcell = flayout_ccell(flayout, i, j);
            Cell *gcell = layout_cell(cast(glayout, Layout), i, j);
            color_t wcolor = i_is_cell_sel(hover, dlayout, i, j) ? colors->select : colors->main;

            draw_r2df(ctx, ekSTROKE, &dcell->rect);

            switch (fcell->type)
            {
            case ekCELL_TYPE_EMPTY:
                break;

            case ekCELL_TYPE_LABEL:
            {
                const Label *glabel = cell_label(gcell);
                const Font *gfont = label_get_font(glabel);
                real32_t origin_x = 0;
                draw_font(ctx, gfont);
                draw_fill_color(ctx, colors->back);
                draw_text_color(ctx, wcolor);
                draw_rect(ctx, ekFILL, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_text_width(ctx, dcell->content_rect.size.width);

                switch (fcell->widget.label->align)
                {
                case ekHALIGN_LEFT:
                case ekHALIGN_JUSTIFY:
                    draw_text_halign(ctx, ekLEFT);
                    break;
                case ekHALIGN_CENTER:
                    draw_text_halign(ctx, ekCENTER);
                    break;
                case ekHALIGN_RIGHT:
                    origin_x = -dcell->content_rect.size.width;
                    draw_text_halign(ctx, ekRIGHT);
                    break;
                default:
                    cassert_default(fcell->widget.label->align);
                }

                if (fcell->widget.label->multiline == TRUE)
                {
                    draw_text(ctx, tc(fcell->widget.label->text), origin_x + dcell->content_rect.pos.x, dcell->content_rect.pos.y);
                }
                else
                {
                    /* TODO: Clipping */
                    drawctrl_text(ctx, tc(fcell->widget.label->text), (int32_t)dcell->content_rect.pos.x, (int32_t)dcell->content_rect.pos.y, ekCTRL_STATE_NORMAL);
                }

                draw_text_width(ctx, -1);
                draw_text_halign(ctx, ekLEFT);
                break;
            }

            case ekCELL_TYPE_BUTTON:
            {
                const Button *gbutton = cell_button(gcell);
                const Font *gfont = button_get_font(gbutton);
                real32_t twidth, theight;
                real32_t tx, ty;
                draw_font(ctx, gfont);
                draw_line_color(ctx, wcolor);
                draw_text_color(ctx, wcolor);
                draw_fill_color(ctx, colors->back);
                draw_line_width(ctx, 3);
                draw_rect(ctx, ekFILLSK, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_line_width(ctx, 1);
                draw_line_color(ctx, colors->main);
                font_extents(gfont, tc(fcell->widget.button->text), -1.f, &twidth, &theight);
                tx = dcell->content_rect.pos.x + ((dcell->content_rect.size.width - twidth) / 2);
                ty = dcell->content_rect.pos.y + ((dcell->content_rect.size.height - theight) / 2);
                drawctrl_text(ctx, tc(fcell->widget.button->text), (int32_t)tx, (int32_t)ty, ekCTRL_STATE_NORMAL);
                break;
            }

            case ekCELL_TYPE_CHECK:
            {
                const Button *gcheck = cell_button(gcell);
                const Font *gfont = button_get_font(gcheck);
                real32_t cwidth = (real32_t)drawctrl_check_width(ctx) - 2;
                real32_t cheight = (real32_t)drawctrl_check_height(ctx) - 2;
                real32_t twidth, theight;
                real32_t tx;
                draw_font(ctx, gfont);
                draw_line_color(ctx, wcolor);
                draw_text_color(ctx, wcolor);
                draw_fill_color(ctx, colors->back);
                font_extents(gfont, tc(fcell->widget.check->text), -1.f, &twidth, &theight);
                tx = dcell->content_rect.pos.x + (dcell->content_rect.size.width - twidth);
                draw_rect(ctx, ekFILL, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                drawctrl_text(ctx, tc(fcell->widget.check->text), (int32_t)tx, (int32_t)dcell->content_rect.pos.y, ekCTRL_STATE_NORMAL);
                draw_rect(ctx, ekSTROKE, dcell->content_rect.pos.x, dcell->content_rect.pos.y, cwidth, cheight);
                draw_line_color(ctx, colors->main);
                break;
            }

            case ekCELL_TYPE_RADIO:
            {
                const Button *gradio = cell_button(gcell);
                const Font *gfont = button_get_font(gradio);
                real32_t cradio = (real32_t)drawctrl_check_width(ctx) * .5f;
                real32_t twidth, theight;
                real32_t tx;
                draw_font(ctx, gfont);
                draw_line_color(ctx, wcolor);
                draw_text_color(ctx, wcolor);
                draw_fill_color(ctx, colors->back);
                font_extents(gfont, tc(fcell->widget.radio->text), -1.f, &twidth, &theight);
                tx = dcell->content_rect.pos.x + (dcell->content_rect.size.width - twidth);
                draw_rect(ctx, ekFILL, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                drawctrl_text(ctx, tc(fcell->widget.radio->text), (int32_t)tx, (int32_t)dcell->content_rect.pos.y, ekCTRL_STATE_NORMAL);
                draw_circle(ctx, ekSTROKE, dcell->content_rect.pos.x + cradio, dcell->content_rect.pos.y + cradio, cradio - 2);

                if (radio_i == 0)
                {
                    draw_fill_color(ctx, wcolor);
                    draw_circle(ctx, ekFILL, dcell->content_rect.pos.x + cradio, dcell->content_rect.pos.y + cradio, cradio - 4);
                }

                draw_line_color(ctx, colors->main);
                radio_i += 1;
                break;
            }

            case ekCELL_TYPE_TOOL:
            {
                const Image *image = i_get_image(dcell, 0, i_is_cell_sel(hover, dlayout, i, j));
                real32_t iwidth = (real32_t)image_width(image);
                real32_t iheight = (real32_t)image_height(image);
                real32_t tx, ty;
                draw_line_color(ctx, wcolor);
                draw_fill_color(ctx, colors->back);
                draw_line_width(ctx, 2);
                draw_rect(ctx, ekFILLSK, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_line_width(ctx, 1);
                draw_line_color(ctx, colors->main);
                tx = dcell->content_rect.pos.x + ((dcell->content_rect.size.width - iwidth) / 2);
                ty = dcell->content_rect.pos.y + ((dcell->content_rect.size.height - iheight) / 2);
                drawctrl_image(ctx, image, (int32_t)tx, (int32_t)ty);
                break;
            }

            case ekCELL_TYPE_POPUP:
            {
                draw_line_color(ctx, wcolor);
                draw_fill_color(ctx, colors->back);
                draw_line_width(ctx, 3);
                draw_rect(ctx, ekFILLSK, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_line_width(ctx, 1);

                if (arrst_size(fcell->widget.popup->elems, FElem) > 0)
                {
                    const Image *image = i_get_image(dcell, 0, i_is_cell_sel(hover, dlayout, i, j));
                    const FElem *elem = arrst_first_const(fcell->widget.popup->elems, FElem);
                    real32_t xoffset = 4;
                    real32_t twidth, theight;
                    real32_t tx, ty;

                    if (image != NULL)
                    {
                        real32_t imgwidth = (real32_t)image_width(image);
                        real32_t imgheight = (real32_t)image_height(image);
                        real32_t yoffset = (dcell->content_rect.size.height - imgheight) / 2;
                        draw_image(ctx, image, dcell->content_rect.pos.x + xoffset, dcell->content_rect.pos.y + yoffset);
                        xoffset += imgwidth + 4;
                    }

                    draw_text_color(ctx, wcolor);
                    font_extents(default_font, tc(elem->text), -1.f, &twidth, &theight);
                    tx = dcell->content_rect.pos.x + xoffset;
                    ty = dcell->content_rect.pos.y + ((dcell->content_rect.size.height - theight) / 2);
                    drawctrl_text(ctx, tc(elem->text), (int32_t)tx, (int32_t)ty, ekCTRL_STATE_NORMAL);
                }

                draw_fill_color(ctx, wcolor);
                i_draw_arrow(ctx, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_line_color(ctx, colors->main);
                break;
            }

            case ekCELL_TYPE_EDIT:
            {
                real32_t pattern[2] = {1, 2};
                draw_line_color(ctx, wcolor);
                draw_fill_color(ctx, colors->back);
                draw_line_width(ctx, 2);
                draw_rect(ctx, ekFILLSK, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_line_width(ctx, 1);
                draw_line_dash(ctx, pattern, 2);
                draw_rect(ctx, ekSTROKE, dcell->content_rect.pos.x + 5, dcell->content_rect.pos.y + 5, dcell->content_rect.size.width - 10, dcell->content_rect.size.height - 10);
                draw_line_dash(ctx, NULL, 0);
                draw_line_color(ctx, colors->main);
                break;
            }

            case ekCELL_TYPE_COMBO:
            {
                real32_t pattern[2] = {1, 2};
                draw_line_color(ctx, wcolor);
                draw_fill_color(ctx, colors->back);
                draw_line_width(ctx, 2);
                draw_rect(ctx, ekFILLSK, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_line_width(ctx, 1);
                draw_line_dash(ctx, pattern, 2);
                draw_rect(ctx, ekSTROKE, dcell->content_rect.pos.x + 5, dcell->content_rect.pos.y + 5, dcell->content_rect.size.width - 20, dcell->content_rect.size.height - 10);
                draw_line_dash(ctx, NULL, 0);
                draw_fill_color(ctx, wcolor);
                i_draw_arrow(ctx, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_line_color(ctx, colors->main);
                break;
            }

            case ekCELL_TYPE_LISTBOX:
            {
                draw_line_color(ctx, wcolor);
                draw_fill_color(ctx, colors->back);
                draw_line_width(ctx, 2);
                draw_rect(ctx, ekFILLSK, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_line_width(ctx, 1);

                if (arrst_size(fcell->widget.listbox->elems, FElem) > 0)
                {
                    const ListBox *glistbox = cell_listbox(gcell);
                    real32_t rheight = listbox_get_row_height(glistbox);
                    real32_t ypos = 0;

                    draw_text_color(ctx, wcolor);

                    /* TODO: Use clipping when ready */
                    arrst_foreach_const(elem, fcell->widget.listbox->elems, FElem)
                        if (dcell->content_rect.size.height >= ypos + rheight)
                        {
                            const Image *image = i_get_image(dcell, elem_i, i_is_cell_sel(hover, dlayout, i, j));
                            real32_t xoffset = 4;
                            real32_t twidth, theight;
                            real32_t tx, ty;

                            if (image != NULL)
                            {
                                real32_t imgwidth = (real32_t)image_width(image);
                                real32_t imgheight = (real32_t)image_height(image);
                                real32_t yoffset = (rheight - imgheight) / 2;
                                draw_image(ctx, image, dcell->content_rect.pos.x + xoffset, dcell->content_rect.pos.y + ypos + yoffset);
                                xoffset += imgwidth + 4;
                            }

                            font_extents(default_font, tc(elem->text), -1.f, &twidth, &theight);
                            tx = dcell->content_rect.pos.x + xoffset;
                            ty = dcell->content_rect.pos.y + ypos + ((rheight - theight) / 2);
                            drawctrl_text(ctx, tc(elem->text), (int32_t)tx, (int32_t)ty, ekCTRL_STATE_NORMAL);
                        }

                        ypos += rheight;
                    arrst_end();
                }

                draw_line_color(ctx, colors->main);
                break;
            }

            case ekCELL_TYPE_SLIDER:
            {
                real32_t tickness = 2;
                real32_t knob_width = 8;
                real32_t knob_margin = 2;
                real32_t ly = (dcell->content_rect.size.height - tickness) / 2;
                real32_t knob_x = (dcell->content_rect.size.width - knob_width) / 2;
                real32_t knob_height = dcell->content_rect.size.height - 2 * knob_margin;
                draw_fill_color(ctx, colors->back);
                draw_rect(ctx, ekFILL, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_fill_color(ctx, wcolor);
                draw_rect(ctx, ekFILL, dcell->content_rect.pos.x, dcell->content_rect.pos.y + ly, dcell->content_rect.size.width, tickness);
                draw_rect(ctx, ekFILL, dcell->content_rect.pos.x + knob_x, dcell->content_rect.pos.y + knob_margin, knob_width, knob_height);
                break;
            }

            case ekCELL_TYPE_VSLIDER:
            {
                real32_t tickness = 2;
                real32_t knob_height = 8;
                real32_t knob_margin = 2;
                real32_t lx = (dcell->content_rect.size.width - tickness) / 2;
                real32_t knob_y = (dcell->content_rect.size.height - knob_height) / 2;
                real32_t knob_width = dcell->content_rect.size.width - 2 * knob_margin;
                draw_fill_color(ctx, colors->back);
                draw_rect(ctx, ekFILL, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_fill_color(ctx, wcolor);
                draw_rect(ctx, ekFILL, dcell->content_rect.pos.x + lx, dcell->content_rect.pos.y, tickness, dcell->content_rect.size.height);
                draw_rect(ctx, ekFILL, dcell->content_rect.pos.x + knob_margin, dcell->content_rect.pos.y + knob_y, knob_width, knob_height);
                break;
            }

            case ekCELL_TYPE_PROGRESS:
            {
                real32_t step_width = 8;
                real32_t margin = 2;
                real32_t x = 0;
                uint32_t k, n;
                n = (uint32_t)bmath_floorf(dcell->content_rect.size.width / (step_width + margin));
                draw_fill_color(ctx, colors->back);
                draw_rect(ctx, ekFILL, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_fill_color(ctx, wcolor);
                for (k = 0; k < n; ++k)
                {
                    draw_rect(ctx, ekFILL, dcell->content_rect.pos.x + x, dcell->content_rect.pos.y + margin, step_width, dcell->content_rect.size.height - 2 * margin);
                    x += step_width + margin;
                }
                break;
            }

            case ekCELL_TYPE_TEXT:
            {
                real32_t pattern[2] = {1, 2};
                draw_line_color(ctx, wcolor);
                draw_fill_color(ctx, colors->back);
                draw_line_width(ctx, 2);
                draw_rect(ctx, ekFILLSK, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_line_width(ctx, 1);
                draw_line_dash(ctx, pattern, 2);
                draw_rect(ctx, ekSTROKE, dcell->content_rect.pos.x + 5, dcell->content_rect.pos.y + 5, dcell->content_rect.size.width - 10, dcell->content_rect.size.height - 10);
                draw_line_dash(ctx, NULL, 0);
                draw_line_color(ctx, colors->main);
                break;
            }

            case ekCELL_TYPE_IMAGE:
            {
                const Image *image = i_get_image(dcell, 0, i_is_cell_sel(hover, dlayout, i, j));
                if (image != NULL)
                {
                    T2Df t2d;
                    V2Df origin;
                    real32_t imgwidth = (real32_t)image_width(image);
                    real32_t imgheight = (real32_t)image_height(image);
                    origin = i_image_transform(&t2d, fcell->widget.image->scale, &dcell->content_rect, imgwidth, imgheight);
                    if (origin.x > 1 || origin.y > 1)
                    {
                        /* TODO!!!!!!! Use 2D drawing context clipping when available */
                        Image *clip = image_trim(image, (uint32_t)origin.x, (uint32_t)origin.y, (uint32_t)dcell->content_rect.size.width, (uint32_t)dcell->content_rect.size.height);
                        t2d_movef(&t2d, &t2d, origin.x, origin.y);
                        draw_matrixf(ctx, &t2d);
                        draw_image(ctx, clip, 0, 0);
                        image_destroy(&clip);
                    }
                    else
                    {
                        draw_matrixf(ctx, &t2d);
                        draw_image(ctx, image, 0, 0);
                    }
                    draw_matrixf(ctx, kT2D_IDENTf);
                }

                draw_line_color(ctx, wcolor);
                draw_line_width(ctx, 2);
                draw_rect(ctx, ekSTROKE, dcell->content_rect.pos.x + 1, dcell->content_rect.pos.y + 1, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_line_width(ctx, 1);
                break;
            }

            case ekCELL_TYPE_TABLEVIEW:
            {
                const TableView *gtable = cell_tableview(gcell);
                const Font *font = tableview_get_font(gtable);
                real32_t head_height = tableview_get_header_height(gtable);
                real32_t fheight = font_height(font);
                uint32_t k, n = tableview_get_num_columns(gtable);
                real32_t x = 0;
                draw_line_color(ctx, wcolor);
                draw_fill_color(ctx, colors->back);
                draw_font(ctx, font);
                draw_text_color(ctx, wcolor);
                for (k = 0; k < n; ++k)
                {
                    real32_t width = tableview_get_column_width(gtable, k);
                    align_t align = tableview_get_header_align(gtable, k);
                    const char_t *text = tableview_get_header_title(gtable, k);

                    if (x + width <= dcell->content_rect.size.width)
                    {
                        real32_t px = dcell->content_rect.pos.x + x;
                        real32_t py = dcell->content_rect.pos.y;
                        real32_t lx = dcell->content_rect.pos.x + x + width;
                        draw_rect(ctx, ekFILLSK, px, py, width, head_height);
                        draw_text_width(ctx, width);
                        draw_text_align(ctx, align, ekTOP);
                        switch (align)
                        {
                        case ekLEFT:
                        case ekJUSTIFY:
                            px += 4;
                            break;
                        case ekCENTER:
                            px += width / 2;
                            break;
                        case ekRIGHT:
                            px += width - 4;
                            break;
                        default:
                            cassert_default(align);
                        }

                        draw_text(ctx, text, px, py + (head_height - fheight) / 2);
                        draw_line(ctx, lx, py, lx, py + dcell->content_rect.size.height);
                    }
                    else
                    {
                        break;
                    }
                    x += width;
                }

                if (x < dcell->content_rect.size.width)
                {
                    real32_t px = dcell->content_rect.pos.x + x;
                    real32_t py = dcell->content_rect.pos.y;
                    real32_t width = dcell->content_rect.size.width - x;
                    draw_rect(ctx, ekFILLSK, px, py, width, head_height);
                }

                draw_line_width(ctx, 2);
                draw_rect(ctx, ekSTROKE, dcell->content_rect.pos.x, dcell->content_rect.pos.y, dcell->content_rect.size.width, dcell->content_rect.size.height);
                draw_line_width(ctx, 1);
                draw_line_color(ctx, colors->main);
                break;
            }

            case ekCELL_TYPE_LAYOUT:
            {
                Layout *gsublayout = cell_layout(gcell);
                i_draw_layout(dcell->sublayout, fcell->widget.layout, gsublayout, hover, sel, swidget, default_font, colors, ctx);
                break;
            }

            default:
                cassert_default(fcell->type);
            }

            dcell += 1;
        }
    }

    /* This layout has the hover element */
    if (hover->dlayout == dlayout)
    {
        R2Df rect = i_get_rect(dlayout, hover);
        draw_line_color(ctx, colors->select);
        if (hover->elem != ekLAYELEM_CELL)
        {
            draw_r2df(ctx, ekSTROKE, &rect);
            draw_line_color(ctx, colors->main);
        }
        else
        {
            const FCell *fcell = flayout_ccell(flayout, hover->col, hover->row);
            draw_r2df(ctx, ekSTROKE, &rect);
            if (fcell->type == ekCELL_TYPE_EMPTY && swidget != ekWIDGET_SELECT)
            {
                uint32_t iw = image_width(colors->add_icon);
                uint32_t ih = image_height(colors->add_icon);
                real32_t x = rect.pos.x + (rect.size.width - (real32_t)iw) / 2;
                real32_t y = rect.pos.y + (rect.size.height - (real32_t)ih) / 2;
                draw_image(ctx, colors->add_icon, x, y);
            }
        }
        draw_line_color(ctx, colors->main);
    }
}

/*---------------------------------------------------------------------------*/

static void i_draw_rect_bounds(const R2Df *rect, DCtx *ctx)
{
    real32_t rsize = 5;
    real32_t x1, x2, y1, y2;
    cassert_no_null(rect);
    x1 = rect->pos.x;
    x2 = x1 + rect->size.width;
    y1 = rect->pos.y;
    y2 = y1 + rect->size.height;
    draw_rect(ctx, ekFILL, x1, y1, rsize, rsize);
    draw_rect(ctx, ekFILL, x2 - rsize, y1, rsize, rsize);
    draw_rect(ctx, ekFILL, x1, y2 - rsize, rsize, rsize);
    draw_rect(ctx, ekFILL, x2 - rsize, y2 - rsize, rsize, rsize);
    draw_rect(ctx, ekFILL, (x1 + x2 - rsize) / 2, y1, rsize, rsize);
    draw_rect(ctx, ekFILL, (x1 + x2 - rsize) / 2, y2 - rsize, rsize, rsize);
    draw_rect(ctx, ekFILL, x1, (y1 + y2 - rsize) / 2, rsize, rsize);
    draw_rect(ctx, ekFILL, x2 - rsize, (y1 + y2 - rsize) / 2, rsize, rsize);
}

/*---------------------------------------------------------------------------*/

static void i_draw_bounds(const DSelect *sel, const DColors *colors, DCtx *ctx)
{
    cassert_no_null(sel);
    cassert_no_null(colors);
    /* This layout has the selected element */
    if (sel->dlayout != NULL)
    {
        switch (sel->elem)
        {
        case ekLAYELEM_MARGIN_LEFT:
        case ekLAYELEM_MARGIN_TOP:
        case ekLAYELEM_MARGIN_RIGHT:
        case ekLAYELEM_MARGIN_BOTTOM:
        case ekLAYELEM_MARGIN_COLUMN:
        case ekLAYELEM_MARGIN_ROW:
        case ekLAYELEM_LAYOUT:
            i_draw_rect_bounds(&sel->dlayout->rect, ctx);
            break;

        case ekLAYELEM_CELL:
        {
            R2Df rect = i_cell_rect(sel->dlayout, sel);
            i_draw_rect_bounds(&rect, ctx);
            break;
        }

        default:
            cassert_default(sel->elem);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_draw_skeleton(const DLayout *dlayout, const DColors *colors, DCtx *ctx)
{
    cassert_no_null(dlayout);
    cassert_no_null(colors);
    draw_line_color(ctx, colors->main);
    draw_r2df(ctx, ekSTROKE, &dlayout->rect);
    arrst_foreach_const(cell, dlayout->cells, DCell)
        draw_r2df(ctx, ekSTROKE, &cell->rect);
        if (cell->sublayout != NULL)
            i_draw_skeleton(cell->sublayout, colors, ctx);
    arrst_end()
}
   
/*---------------------------------------------------------------------------*/

static DCell *i_sel_cell(const DSelect *sel)
{
    cassert_no_null(sel);
    if (sel->dlayout == NULL)
        return NULL;

    if (sel->elem != ekLAYELEM_CELL && sel->elem != ekLAYELEM_LAYOUT)
        return NULL;

    {
        uint32_t ncols = arrst_size(sel->dlayout->cols, DColumn);
        uint32_t pos = sel->row * ncols + sel->col;
        return arrst_get(sel->dlayout->cells, pos, DCell);
    }
}

/*---------------------------------------------------------------------------*/

static void i_draw_shading(const DSelect *sel, const DColors *colors, DCtx *ctx)
{
    cassert_no_null(sel);
    cassert_no_null(colors);
    if (sel->dlayout != NULL)
    {
        const DCell *selcell = i_sel_cell(sel);
        arrst_foreach_const(cell, sel->dlayout->cells, DCell)
            if (cell == selcell)
                draw_fill_color(ctx, colors->cellhot);
            else
                draw_fill_color(ctx, colors->cell);
            draw_r2df(ctx, ekFILL, &cell->rect);
        arrst_end()

        /* Selected border */
        draw_fill_color(ctx, colors->cellhot);
        switch (sel->elem)
        {
        case ekLAYELEM_MARGIN_LEFT:
            draw_r2df(ctx, ekFILL, &sel->dlayout->rect_left);
            break;
        case ekLAYELEM_MARGIN_TOP:
            draw_r2df(ctx, ekFILL, &sel->dlayout->rect_top);
            break;
        case ekLAYELEM_MARGIN_RIGHT:
            draw_r2df(ctx, ekFILL, &sel->dlayout->rect_right);
            break;
        case ekLAYELEM_MARGIN_BOTTOM:
            draw_r2df(ctx, ekFILL, &sel->dlayout->rect_bottom);
            break;
        case ekLAYELEM_MARGIN_COLUMN:
        {
            const DColumn *col = arrst_get_const(sel->dlayout->cols, sel->col, DColumn);
            draw_r2df(ctx, ekFILL, &col->margin_rect);
            break;
        }
        case ekLAYELEM_MARGIN_ROW:
        {
            const DRow *row = arrst_get_const(sel->dlayout->rows, sel->row, DRow);
            draw_r2df(ctx, ekFILL, &row->margin_rect);
            break;
        }

        case ekLAYELEM_CELL:
        case ekLAYELEM_LAYOUT:
            break;

        default:
            cassert_default(sel->elem);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_draw_col_row(const DSelect *sel, const Font *font, const DColors *colors, DCtx *ctx)
{
    cassert_no_null(sel);
    cassert_no_null(colors);
    if (sel->dlayout != NULL && sel->elem != ekLAYELEM_CELL)
    {
        char_t text[64];
        const real32_t offset = 10;
        cassert_no_null(sel->dlayout);
        draw_line_width(ctx, 2);
        draw_font(ctx, font);

        if (sel->col != UINT32_MAX)
        {
            const DColumn *col = arrst_get_const(sel->dlayout->cols, sel->col, DColumn);
            R2Df rect = col->rect;
            rect.pos.y -= offset / 2;
            rect.size.height += offset;
            draw_line_color(ctx, colors->col);
            draw_text_color(ctx, colors->col);
            draw_r2df(ctx, ekSTROKE, &rect);
            bstd_sprintf(text, sizeof(text), "Col %d", sel->col);
            draw_text_align(ctx, ekCENTER, ekBOTTOM);
            draw_text(ctx, text, rect.pos.x + rect.size.width / 2, rect.pos.y);
        }

        if (sel->row != UINT32_MAX)
        {
            const DRow *row = arrst_get_const(sel->dlayout->rows, sel->row, DRow);
            R2Df rect = row->rect;
            rect.pos.x -= offset / 2;
            rect.size.width += offset;
            draw_line_color(ctx, colors->row);
            draw_text_color(ctx, colors->row);
            draw_r2df(ctx, ekSTROKE, &rect);
            bstd_sprintf(text, sizeof(text), "Row %d", sel->row);
            draw_text_align(ctx, ekLEFT, ekCENTER);
            draw_text(ctx, text, rect.pos.x + rect.size.width, rect.pos.y + rect.size.height / 2);
        }
        draw_line_width(ctx, 1);
    }
}

/*---------------------------------------------------------------------------*/

static void i_draw_cell_ids(const DSelect *sel, const Font *font, const DColors *colors, DCtx *ctx)
{
    cassert_no_null(sel);
    cassert_no_null(colors);
    if (sel->dlayout != NULL)
    {
        uint32_t i, ncols = arrst_size(sel->dlayout->cols, DColumn);
        uint32_t j, nrows = arrst_size(sel->dlayout->rows, DRow);
        const DCell *cell = arrst_all_const(sel->dlayout->cells, DCell);
        draw_font(ctx, font);
        draw_text_color(ctx, colors->main);
        draw_text_align(ctx, ekLEFT, ekTOP);
        for (j = 0; j < nrows; ++j)
        {
            for (i = 0; i < ncols; ++i, ++cell)
            {
                char_t text[64];
                bstd_sprintf(text, sizeof(text), "(%d,%d)", i, j);
                draw_text(ctx, text, cell->rect.pos.x, cell->rect.pos.y);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

void dlayout_draw(const DLayout *dlayout, const FLayout *flayout, const Layout *glayout, const DSelect *hover, const DSelect *sel, const widget_t swidget, const Font *default_font, const Font *bold_font, const cmode_t cmode, const DColors *colors, const char_t *form_name, DCtx *ctx)
{
    cassert_no_null(colors);
    cassert_no_null(dlayout);
    switch (cmode)
    {
    case ekCMODE_DETAIL:
        draw_line_color(ctx, colors->main);
        draw_fill_color(ctx, colors->panel);
        draw_r2df(ctx, ekFILLSK, &dlayout->rect);
        i_draw_grid(ctx, colors, &dlayout->rect);
        i_draw_frame(ctx, default_font, colors, form_name, &dlayout->rect);
        i_draw_layout(dlayout, flayout, glayout, hover, sel, swidget, default_font, colors, ctx);
        draw_fill_color(ctx, colors->main);
        i_draw_bounds(sel, colors, ctx);
        break;

    case ekCMODE_SKELETON:
        i_draw_shading(sel, colors, ctx);
        i_draw_skeleton(dlayout, colors, ctx);
        i_draw_col_row(sel, bold_font, colors, ctx);
        draw_fill_color(ctx, colors->main);
        i_draw_cell_ids(sel, default_font, colors, ctx);

        if (hover->dlayout != NULL)
        {
            R2Df hrect = i_get_rect(hover->dlayout, hover);
            draw_line_color(ctx, colors->select);
            draw_line_width(ctx, 2);
            draw_r2df(ctx, ekSTROKE, &hrect);
            draw_line_width(ctx, 1);
        }

        i_draw_bounds(sel, colors, ctx);
        break;

    default:
        cassert_default(cmode);
    }
}
