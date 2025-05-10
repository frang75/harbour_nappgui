/*
 * NAppGUI Cross-platform C SDK
 * 2015-2025 Francisco Garcia Collado
 * MIT Licence
 * https://nappgui.com/en/legal/license.html
 *
 * File: splitview.c
 *
 */

/* Split view */

#include "splitview.h"
#include "splitview.inl"
#include "component.inl"
#include "gui.inl"
#include "panel.inl"
#include <draw2d/guictx.h>
#include <geom2d/r2d.h>
#include <geom2d/s2d.h>
#include <geom2d/v2d.h>
#include <core/event.h>
#include <core/objh.h>
#include <sewer/bmath.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>
#include <sewer/types.h>

struct _splitview_t
{
    GuiComponent component;
    S2Df natural_size;
    S2Df current_size;
    uint32_t flags;
    split_mode_t pos_mode;
    split_mode_t divider_mode;
    real32_t div_pos;
    real32_t minsize0;
    real32_t minsize1;
    real32_t mindrag0;
    real32_t mindrag1;
    GuiComponent *child0;
    GuiComponent *child1;
    bool_t child0_visible;
    bool_t child1_visible;
    bool_t child0_tabstop;
    bool_t child1_tabstop;
    real32_t chid0_dim[2];
    real32_t chid1_dim[2];
};

static const real32_t i_MIN_VISIBLE_SIZE = 5;
static const real32_t i_DIVIDER_THICKNESS = 20;

/*---------------------------------------------------------------------------*/

static real32_t i_convert_clamp_divpos(const split_mode_t from_mode, const split_mode_t to_mode, const real32_t divpos, const real32_t size)
{
    cassert(size > 0);
    switch (from_mode)
    {
    case ekSPLIT_NORMAL:
        cassert(divpos >= 0 && divpos <= 1);
        switch (to_mode)
        {
        case ekSPLIT_NORMAL:
            return divpos;
        case ekSPLIT_FIXED0:
            return bmath_clampf(bmath_ceilf(divpos * size), 0, size);
        case ekSPLIT_FIXED1:
            return bmath_clampf(bmath_ceilf((1 - divpos) * size), 0, size);
            cassert_default();
        }

    case ekSPLIT_FIXED0:
    {
        real32_t pos = bmath_clampf(divpos, 0, size);
        switch (to_mode)
        {
        case ekSPLIT_NORMAL:
            return pos / size;
        case ekSPLIT_FIXED0:
            return pos;
        case ekSPLIT_FIXED1:
            return size - pos;
            cassert_default();
        }
    }

    case ekSPLIT_FIXED1:
    {
        real32_t pos = bmath_clampf(divpos, 0, size);
        switch (to_mode)
        {
        case ekSPLIT_NORMAL:
            return 1 - (pos / size);
        case ekSPLIT_FIXED0:
            return size - pos;
        case ekSPLIT_FIXED1:
            return pos;
            cassert_default();
        }
    }

        cassert_default();
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

static real32_t i_divpos_to_px(SplitView *split, const real32_t size)
{
    cassert_no_null(split);
    split->div_pos = i_convert_clamp_divpos(split->pos_mode, split->divider_mode, split->div_pos, size);
    split->pos_mode = split->divider_mode;

    switch (split->divider_mode)
    {
    case ekSPLIT_NORMAL:
        cassert(split->div_pos >= 0 && split->div_pos <= 1);
        return bmath_ceilf(split->div_pos * size);

    case ekSPLIT_FIXED0:
        cassert(split->div_pos >= 0);
        return bmath_clampf(split->div_pos, 0, size);

    case ekSPLIT_FIXED1:
        cassert(split->div_pos >= 0);
        return size - bmath_clampf(split->div_pos, 0, size);

        cassert_default();
    }

    return 0;
}

/*---------------------------------------------------------------------------*/

static bool_t i_child0_displayed(SplitView *split)
{
    cassert_no_null(split);
    if (split->child0 == NULL)
        return FALSE;

    if (split->child0_visible == FALSE)
        return FALSE;

    return TRUE;
}

/*---------------------------------------------------------------------------*/

static bool_t i_child1_displayed(SplitView *split)
{
    cassert_no_null(split);
    if (split->child1 == NULL)
        return FALSE;

    if (split->child1_visible == FALSE)
        return FALSE;

    return TRUE;
}

/*---------------------------------------------------------------------------*/

static void i_adjust_minimum(const real32_t min0, const real32_t min1, real32_t *div_pos, real32_t *size, const bool_t can_expand)
{
    cassert_no_null(div_pos);
    cassert_no_null(size);
    cassert(min0 >= 0);
    cassert(min1 >= 0);
    cassert(*div_pos >= 0);

    if (min0 + min1 > *size)
    {
        if (can_expand == TRUE)
            *size = min0 + min1;
    }

    if (min0 > 0 && *div_pos < min0)
        *div_pos = min0;

    if (min1 > 0 && *size - *div_pos < min1)
        *div_pos = *size - min1;

    *div_pos = bmath_clampf(*div_pos, 0, *size);
}

/*---------------------------------------------------------------------------*/

static R2Df i_rect_track(SplitView *split, const S2Df *size)
{
    bool_t display0 = i_child0_displayed(split);
    bool_t display1 = i_child1_displayed(split);
    R2Df rect_track = kR2D_ZEROf;
    cassert_no_null(split);
    cassert_no_null(size);
    if (display0 == TRUE && display1 == TRUE)
    {
        if (split_get_type(split->flags) == ekSPLIT_HORZ)
        {
            real32_t divider_y = i_divpos_to_px(split, size->height);
            rect_track.pos.x = 0;
            rect_track.pos.y = bmath_ceilf(divider_y - .5f * i_DIVIDER_THICKNESS);
            rect_track.size.width = size->width;
            rect_track.size.height = i_DIVIDER_THICKNESS;
        }
        else
        {
            real32_t divider_x = i_divpos_to_px(split, size->width);
            rect_track.pos.x = bmath_ceilf(divider_x - .5f * i_DIVIDER_THICKNESS);
            rect_track.pos.y = 0;
            rect_track.size.width = i_DIVIDER_THICKNESS;
            rect_track.size.height = size->height;
        }
    }
    /* The child0 will fill all the splitview area */
    else if (display0 == TRUE)
    {
        /* If child1 is hidden by user, no rect_track will be defined */
        if (split->child1_visible == TRUE)
        {
            /* The rect_track at bottom */
            if (split_get_type(split->flags) == ekSPLIT_HORZ)
            {
                rect_track.pos.x = 0;
                rect_track.pos.y = size->height - i_DIVIDER_THICKNESS;
                rect_track.size.width = size->width;
                rect_track.size.height = i_DIVIDER_THICKNESS;
            }
            /* The rect_track at right */
            else
            {
                rect_track.pos.x = size->width - i_DIVIDER_THICKNESS;
                rect_track.pos.y = 0;
                rect_track.size.width = i_DIVIDER_THICKNESS;
                rect_track.size.height = size->height;
            }
        }
        else
        {
            rect_track = kR2D_ZEROf;
        }
    }
    /* The child1 will fill all the splitview area */
    else if (display1 == TRUE)
    {
        /* If child0 is hidden by user, no rect_track will be defined */
        if (split->child0_visible == TRUE)
        {
            /* The rect_track at top */
            if (split_get_type(split->flags) == ekSPLIT_HORZ)
            {
                rect_track.pos = kV2D_ZEROf;
                rect_track.size.width = size->width;
                rect_track.size.height = i_DIVIDER_THICKNESS;
            }
            /* The rect_track at left */
            else
            {
                rect_track.pos = kV2D_ZEROf;
                rect_track.size.width = i_DIVIDER_THICKNESS;
                rect_track.size.height = size->height;
            }
        }
        else
        {
            rect_track = kR2D_ZEROf;
        }
    }
    /* No child displayed (unusual) */
    else
    {
        rect_track = kR2D_ZEROf;
    }

    return rect_track;
}

/*---------------------------------------------------------------------------*/

static real32_t i_frames(SplitView *split, const S2Df *size, R2Df *rect0, R2Df *rect1, const bool_t can_expand)
{
    bool_t display0 = i_child0_displayed(split);
    bool_t display1 = i_child1_displayed(split);
    cassert_no_null(split);
    cassert_no_null(size);
    cassert_no_null(rect0);
    cassert_no_null(rect1);
    if (display0 == TRUE && display1 == TRUE)
    {
        if (split_get_type(split->flags) == ekSPLIT_HORZ)
        {
            real32_t divider_y = i_divpos_to_px(split, size->height);
            real32_t total_height = size->height;
            i_adjust_minimum(min_r32(split->minsize0, split->mindrag0), min_r32(split->minsize1, split->mindrag1), &divider_y, &total_height, can_expand);
            rect0->pos = kV2D_ZEROf;
            rect0->size.width = size->width;
            rect0->size.height = divider_y;
            rect1->pos.x = 0;
            rect1->pos.y = rect0->size.height;
            rect1->size.width = size->width;
            rect1->size.height = total_height - rect0->size.height;
            return divider_y;
        }
        else
        {
            real32_t divider_x = i_divpos_to_px(split, size->width);
            real32_t total_width = size->width;
            i_adjust_minimum(max_r32(split->minsize0, split->mindrag0), max_r32(split->minsize1, split->mindrag1), &divider_x, &total_width, can_expand);
            rect0->pos = kV2D_ZEROf;
            rect0->size.width = divider_x;
            rect0->size.height = size->height;
            rect1->pos.x = rect0->size.width;
            rect1->pos.y = 0;
            rect1->size.width = total_width - rect0->size.width;
            rect1->size.height = size->height;
            return divider_x;
        }
    }
    /* The child0 will fill all the splitview area */
    else if (display0 == TRUE)
    {
        rect0->pos = kV2D_ZEROf;
        rect0->size = *size;
        *rect1 = kR2D_ZEROf;
        if (split_get_type(split->flags) == ekSPLIT_HORZ)
            return size->height;
        else
            return size->width;
    }
    /* The child1 will fill all the splitview area */
    else if (display1 == TRUE)
    {
        *rect0 = kR2D_ZEROf;
        rect1->pos = kV2D_ZEROf;
        rect1->size = *size;
        return 0;
    }
    /* No child displayed (unusual) */
    else
    {
        *rect0 = kR2D_ZEROf;
        *rect1 = kR2D_ZEROf;
        return 0;
    }
}

/*---------------------------------------------------------------------------*/

static void i_resize_child(const R2Df *rect, GuiComponent *child)
{
    cassert_no_null(rect);
    if (rect->size.width > 0 && rect->size.height > 0)
    {
        if (child != NULL)
        {
            _component_set_frame(child, &rect->pos, &rect->size);
            _component_visible(child, TRUE);
        }
    }
    else if (child != NULL)
    {
        _component_visible(child, FALSE);
    }
}

/*---------------------------------------------------------------------------*/

static void i_recompute_rect_track(SplitView *split)
{
    R2Df rect_track;
    cassert_no_null(split);
    rect_track = i_rect_track(split, &split->current_size);
    if (rect_track.size.width > 0 && rect_track.size.height > 0)
        split->component.context->func_split_track_area(split->component.ositem, rect_track.pos.x, rect_track.pos.y, rect_track.size.width, rect_track.size.height);
    else
        split->component.context->func_split_track_area(split->component.ositem, 0, 0, 0, 0);
}

/*---------------------------------------------------------------------------*/

static void i_recompute_children(SplitView *split)
{
    R2Df rect0, rect1;
    bool_t recompute = TRUE;
    cassert_no_null(split);

    while (recompute == TRUE)
    {
        S2Df size0 = kS2D_ZEROf;
        S2Df size1 = kS2D_ZEROf;
        real32_t divpos_px = 0;

        divpos_px = i_frames(split, &split->current_size, &rect0, &rect1, FALSE);
        if (rect0.size.width > 0 && rect0.size.height > 0)
        {
            real32_t dim0 = 0, dim1 = 0;
            _component_natural(split->child0, 0, &dim0, &dim1);
            _component_natural(split->child0, 1, &dim0, &dim1);
            _component_expand(split->child0, 0, dim0, rect0.size.width, &size0.width);
            _component_expand(split->child0, 1, dim1, rect0.size.height, &size0.height);
        }

        if (rect1.size.width > 0 && rect1.size.height > 0)
        {
            real32_t dim0 = 0, dim1 = 0;
            _component_natural(split->child1, 0, &dim0, &dim1);
            _component_natural(split->child1, 1, &dim0, &dim1);
            _component_expand(split->child1, 0, dim0, rect1.size.width, &size1.width);
            _component_expand(split->child1, 1, dim1, rect1.size.height, &size1.height);
        }

        /* Any of the children may not accept the new dimensions. */
        if (split_get_type(split->flags) == ekSPLIT_HORZ)
        {
            real32_t height = rect0.size.height + rect1.size.width;
            real32_t nheight = size0.height + size1.height;

            /* The divider constraint cannot be met */
            if (bmath_absf(height - nheight) >= 1)
            {
                cassert(nheight > height);
                if (bmath_absf(rect0.size.height - size0.height) < 1)
                {
                    split->div_pos = i_convert_clamp_divpos(ekSPLIT_FIXED0, split->divider_mode, height - size1.height, height);
                    split->mindrag1 = size1.height;
                }
                else
                {
                    split->div_pos = i_convert_clamp_divpos(ekSPLIT_FIXED1, split->divider_mode, height - size0.height, height);
                    split->mindrag0 = size0.height;
                }
            }
            /* Constraint can be met */
            else
            {
                split->div_pos = i_convert_clamp_divpos(ekSPLIT_FIXED0, split->divider_mode, divpos_px, height);
                recompute = FALSE;
            }
        }
        else
        {
            real32_t width = rect0.size.width + rect1.size.width;
            real32_t nwidth = size0.width + size1.width;

            /* The divider constraint cannot be met */
            if (bmath_absf(width - nwidth) >= 1)
            {
                cassert(nwidth > width);
                if (bmath_absf(rect0.size.width - size0.width) < 1)
                {
                    split->div_pos = i_convert_clamp_divpos(ekSPLIT_FIXED0, split->divider_mode, width - size1.width, width);
                    split->mindrag1 = size1.width;
                }
                else
                {
                    split->div_pos = i_convert_clamp_divpos(ekSPLIT_FIXED1, split->divider_mode, width - size0.width, width);
                    split->mindrag0 = size0.width;
                }
            }
            /* Constraint can be met */
            else
            {
                split->div_pos = i_convert_clamp_divpos(ekSPLIT_FIXED0, split->divider_mode, divpos_px, width);
                recompute = FALSE;
            }
        }
    }

    _component_locate(split->child0);
    _component_locate(split->child1);
    i_resize_child(&rect0, split->child0);
    i_resize_child(&rect1, split->child1);
}

/*---------------------------------------------------------------------------*/

static void i_OnDrag(SplitView *split, Event *e)
{
    const EvMouse *params = event_params(e, EvMouse);
    if (event_type(e) == ekGUI_EVENT_DRAG)
    {
        real32_t mouse_pos = 0, size = 0, st_pos = 0, npos = 0;
        cassert_no_null(split);
        cassert(params->button == ekGUI_MOUSE_LEFT);
        if (split_get_type(split->flags) == ekSPLIT_HORZ)
        {
            mouse_pos = params->y;
            size = split->current_size.height;
            st_pos = params->ly;
        }
        else
        {
            mouse_pos = params->x;
            size = split->current_size.width;
            st_pos = params->lx;
        }

        split->div_pos = i_convert_clamp_divpos(ekSPLIT_FIXED0, split->divider_mode, mouse_pos, size);
        i_recompute_children(split);
        npos = i_convert_clamp_divpos(split->divider_mode, ekSPLIT_FIXED0, split->div_pos, size);
        //bstd_printf("Drag. StPos: %g MousePos: %g NPos: %g\n", st_pos, mouse_pos, split->div_pos);
    }
    else
    {
        /* End dragging */
        cassert(event_type(e) == ekGUI_EVENT_UP);
        i_recompute_rect_track(split);
        split->mindrag0 = 0;
        split->mindrag1 = 0;
    }
}

/*---------------------------------------------------------------------------*/

static SplitView *i_create(const uint32_t flags)
{
    const GuiCtx *context = guictx_get_current();
    void *ositem = context->func_create[ekGUI_TYPE_SPLITVIEW](flags);
    SplitView *split = obj_new0(SplitView);
    _component_init(&split->component, context, ekGUI_TYPE_SPLITVIEW, &ositem);
    split->natural_size = s2df(-1, -1);
    split->flags = flags;
    split->div_pos = .5f;
    split->pos_mode = ekSPLIT_NORMAL;
    split->divider_mode = ekSPLIT_NORMAL;
    context->func_split_OnDrag(split->component.ositem, obj_listener(split, i_OnDrag, SplitView));
    return split;
}

/*---------------------------------------------------------------------------*/

SplitView *splitview_horizontal(void)
{
    return i_create(ekSPLIT_HORZ);
}

/*---------------------------------------------------------------------------*/

SplitView *splitview_vertical(void)
{
    return i_create(ekSPLIT_VERT);
}

/*---------------------------------------------------------------------------*/

static void i_add_child(SplitView *split, GuiComponent *component, const bool_t tabstop)
{
    cassert_no_null(split);
    cassert_no_null(component);
    if (split->child0 == NULL)
    {
        split->child0 = component;
        split->child0_tabstop = tabstop;
        split->child0_visible = TRUE;
    }
    else
    {
        cassert(split->child1 == NULL);
        split->child1 = component;
        split->child1_tabstop = tabstop;
        split->child1_visible = TRUE;
    }

    split->component.context->func_split_attach_control(split->component.ositem, component->ositem);
}

/*---------------------------------------------------------------------------*/

void splitview_size(SplitView *split, const S2Df size)
{
    cassert_no_null(split);
    split->natural_size = size;
}

/*---------------------------------------------------------------------------*/

void splitview_view(SplitView *split, View *view, const bool_t tabstop)
{
    i_add_child(split, cast(view, GuiComponent), tabstop);
}

/*---------------------------------------------------------------------------*/

void splitview_text(SplitView *split, TextView *view, const bool_t tabstop)
{
    i_add_child(split, cast(view, GuiComponent), tabstop);
}

/*---------------------------------------------------------------------------*/

void splitview_split(SplitView *split, SplitView *child)
{
    i_add_child(split, cast(child, GuiComponent), TRUE);
}

/*---------------------------------------------------------------------------*/

void splitview_panel(SplitView *split, Panel *panel)
{
    i_add_child(split, cast(panel, GuiComponent), TRUE);
}

/*---------------------------------------------------------------------------*/

void splitview_mode(SplitView *split, const split_mode_t mode)
{
    cassert_no_null(split);
    split->divider_mode = mode;
}

/*---------------------------------------------------------------------------*/

void splitview_pos(SplitView *split, const split_mode_t mode, const real32_t pos)
{
    cassert_no_null(split);
    cassert((mode == ekSPLIT_NORMAL && (pos >= 0 && pos <= 1)) || pos >= 0);
    split->div_pos = pos;
    split->pos_mode = mode;
}

/*---------------------------------------------------------------------------*/

void splitview_visible0(SplitView *split, const bool_t visible)
{
    cassert_no_null(split);
    if (split->child0_visible != visible)
    {
        split->child0_visible = visible;
        if (split->current_size.width > 0 && split->current_size.height > 0)
        {
            i_recompute_children(split);
            i_recompute_rect_track(split);
        }
    }
}

/*---------------------------------------------------------------------------*/

void splitview_visible1(SplitView *split, const bool_t visible)
{
    cassert_no_null(split);
    if (split->child1_visible != visible)
    {
        split->child1_visible = visible;
        if (split->current_size.width > 0 && split->current_size.height > 0)
        {
            i_recompute_children(split);
            i_recompute_rect_track(split);
        }
    }
}

/*---------------------------------------------------------------------------*/

void splitview_minsize0(SplitView *split, const real32_t size)
{
    cassert_no_null(split);
    cassert(size >= 0);
    split->minsize0 = size;
}

/*---------------------------------------------------------------------------*/

void splitview_minsize1(SplitView *split, const real32_t size)
{
    cassert_no_null(split);
    cassert(size >= 0);
    split->minsize1 = size;
}

/*---------------------------------------------------------------------------*/

void _splitview_destroy(SplitView **split)
{
    cassert_no_null(split);
    cassert_no_null(*split);
    if ((*split)->child0 != NULL)
    {
        (*split)->component.context->func_split_detach_control((*split)->component.ositem, (*split)->child0->ositem);
        _component_destroy(&(*split)->child0);
    }

    if ((*split)->child1 != NULL)
    {
        (*split)->component.context->func_split_detach_control((*split)->component.ositem, (*split)->child1->ositem);
        _component_destroy(&(*split)->child1);
    }

    _component_destroy_imp(&(*split)->component);
    obj_delete(split, SplitView);
}

/*---------------------------------------------------------------------------*/

void _splitview_natural(SplitView *split, const uint32_t di, real32_t *dim0, real32_t *dim1)
{
    cassert_no_null(split);
    cassert_no_null(dim0);
    cassert_no_null(dim1);

    if (split->child0 != NULL && split->child0_visible == TRUE)
        _component_natural(split->child0, di, &split->chid0_dim[0], &split->chid0_dim[1]);
    else
        split->chid0_dim[di] = 0;

    if (split->child1 != NULL && split->child1_visible == TRUE)
        _component_natural(split->child1, di, &split->chid1_dim[0], &split->chid1_dim[1]);
    else
        split->chid1_dim[di] = 0;

    if (di == 0)
    {
        if (split->natural_size.width > 0)
            *dim0 = split->natural_size.width;
        else if (split_get_type(split->flags) == ekSPLIT_HORZ)
            *dim0 = max_r32(split->chid0_dim[di], split->chid1_dim[di]);
        else
            *dim0 = split->chid0_dim[di] + split->chid1_dim[di];
    }
    else
    {
        cassert(di == 1);
        if (split->natural_size.height > 0)
            *dim1 = split->natural_size.height;
        else if (split_get_type(split->flags) == ekSPLIT_HORZ)
            *dim1 = split->chid0_dim[di] + split->chid1_dim[di];
        else
            *dim1 = max_r32(split->chid0_dim[di], split->chid1_dim[di]);
    }
}

/*---------------------------------------------------------------------------*/

void _splitview_expand(SplitView *split, const uint32_t di, const real32_t current_size, const real32_t required_size, real32_t *final_size)
{
    R2Df rect0, rect1;
    real32_t csize0 = 0, csize1 = 0;
    real32_t fsize0 = 0, fsize1 = 0;
    cassert_no_null(split);
    cassert_no_null(final_size);
    cassert(sizeof(split->current_size) == 2 * sizeof(real32_t));
    unref(current_size);

    cast(&split->current_size, real32_t)[di] = required_size;
    if (split->current_size.height < i_MIN_VISIBLE_SIZE)
        split->current_size.height = 256;

    i_frames(split, &split->current_size, &rect0, &rect1, TRUE);

    csize0 = cast(&rect0.size, real32_t)[di];
    csize1 = cast(&rect1.size, real32_t)[di];

    if (csize0 > 0)
        _component_expand(split->child0, di, split->chid0_dim[di], csize0, &fsize0);
    if (csize1 > 0)
        _component_expand(split->child1, di, split->chid1_dim[di], csize1, &fsize1);

    if ((split_get_type(split->flags) == ekSPLIT_HORZ && di == 0) || (split_get_type(split->flags) == ekSPLIT_VERT && di == 1))
        *final_size = max_r32(fsize0, fsize1);
    else
        *final_size = fsize0 + fsize1;

    cast(&current_size, real32_t)[di] = *final_size;
}

/*---------------------------------------------------------------------------*/

void _splitview_taborder(const SplitView *split, Window *window)
{
    bool_t display0 = FALSE;
    bool_t display1 = FALSE;
    cassert_no_null(split);
    display0 = i_child0_displayed(cast(split, SplitView));
    display1 = i_child1_displayed(cast(split, SplitView));
    if (display0 == TRUE && split->child0_tabstop == TRUE)
        _component_taborder(split->child0, window);
    if (display1 == TRUE && split->child1_tabstop == TRUE)
        _component_taborder(split->child1, window);
}

/*---------------------------------------------------------------------------*/

void _splitview_OnResize(SplitView *split, const S2Df *size)
{
    cassert_no_null(split);
    cassert_no_null(size);
    i_recompute_children(split);
    i_recompute_rect_track(split);
    //bstd_printf("OnResize %g\n", split->div_pos);
    cassert_unref(bmath_absf(size->width - split->current_size.width) < 1, size);
    cassert_unref(bmath_absf(size->height - split->current_size.height) < 1, size);
}

/*---------------------------------------------------------------------------*/

static void i_accum_panels(const SplitView *split, uint32_t *num_panels, Panel **panels);

/*---------------------------------------------------------------------------*/

static void i_accum_child_panels(const GuiComponent *component, uint32_t *num_panels, Panel **panels)
{
    cassert_no_null(num_panels);
    cassert_no_null(panels);
    if (component != NULL)
    {
        if (component->type == ekGUI_TYPE_PANEL)
        {
            panels[*num_panels] = cast(component, Panel);
            *num_panels += 1;
            cassert(*num_panels < GUI_COMPONENT_MAX_PANELS);
        }
        else if (component->type == ekGUI_TYPE_SPLITVIEW)
        {
            i_accum_panels(cast(component, SplitView), num_panels, panels);
        }
    }
}

/*---------------------------------------------------------------------------*/

static void i_accum_panels(const SplitView *split, uint32_t *num_panels, Panel **panels)
{
    cassert_no_null(split);
    i_accum_child_panels(split->child0, num_panels, panels);
    i_accum_child_panels(split->child1, num_panels, panels);
}

/*---------------------------------------------------------------------------*/

void _splitview_panels(const SplitView *split, uint32_t *num_panels, Panel **panels)
{
    *num_panels = 0;
    i_accum_panels(split, num_panels, panels);
}
