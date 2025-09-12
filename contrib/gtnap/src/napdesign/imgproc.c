/* Image processing */

#include "imgproc.h"
#include <draw2d/color.h>
#include <draw2d/image.h>
#include <draw2d/pixbuf.h>
#include <draw2d/palette.h>
#include <sewer/bmem.h>
#include <sewer/cassert.h>
#include <sewer/ptr.h>
#include <sewer/types.h>

/*---------------------------------------------------------------------------*/

static uint8_t i_otsu_threshold(const Pixbuf *pixbuf)
{
    register const byte_t *data = pixbuf_cdata(pixbuf);
    register uint32_t i, n = pixbuf_width(pixbuf) * pixbuf_height(pixbuf);
    uint32_t hist[256];
    real64_t mu = 0, scale = 1. / n;
    real64_t mu1 = 0, q1 = 0;
    real64_t max_sigma = 0, max_val = 0;
    cassert(pixbuf_format(pixbuf) == ekGRAY8);

    bmem_zero_n(hist, 256, uint32_t);

    for (i = 0; i < n; ++i, ++data)
        hist[*data] += 1;

    for (i = 0; i < 256; ++i)
        mu += i * (real64_t)hist[i];

    mu *= scale;

    for (i = 0; i < 256; ++i)
    {
        real64_t p_i, q2, mu2, sigma;

        p_i = hist[i] * scale;
        mu1 *= q1;
        q1 += p_i;
        q2 = 1. - q1;

        if (min_r64(q1, q2) < 1e-6 || max_r64(q1, q2) > 1. - 1e-6)
            continue;

        mu1 = (mu1 + i * p_i) / q1;
        mu2 = (mu - q1 * mu1) / q2;
        sigma = q1 * q2 * (mu1 - mu2) * (mu1 - mu2);
        if (sigma > max_sigma)
        {
            max_sigma = sigma;
            max_val = i;
        }
    }

    cassert(max_val < 256);
    return (uint8_t)max_val;
}

/*---------------------------------------------------------------------------*/

static Pixbuf *i_pixbuf_threshold(const Pixbuf *pixbuf, const uint8_t threshold, const bool_t invert)
{
    pixformat_t format = pixbuf_format(pixbuf);
    uint32_t w = pixbuf_width(pixbuf);
    uint32_t h = pixbuf_height(pixbuf);
    Pixbuf *opixbuf = NULL;
    uint32_t i, j, v;

    opixbuf = pixbuf_create(w, h, ekINDEX1);
    cassert_unref(format == ekGRAY8, format);
    for (i = 0; i < w; ++i)
    for (j = 0; j < h; ++j)
    {
        v = pixbuf_get(pixbuf, i, j);
        if (v < threshold)
            pixbuf_set(opixbuf, i, j, (uint32_t)!invert);
        else
            pixbuf_set(opixbuf, i, j, (uint32_t)invert);
    }

    return opixbuf;
}

/*---------------------------------------------------------------------------*/

static Pixbuf *i_binarize_otsu(const Pixbuf *pixbuf, const bool_t invert)
{
    Pixbuf *pixbin = NULL;
    uint8_t threshold = 0;
    cassert(pixbuf_format(pixbuf) == ekGRAY8);
    threshold = i_otsu_threshold(pixbuf);
    pixbin = i_pixbuf_threshold(pixbuf, threshold, invert);
    return pixbin;
}

/*---------------------------------------------------------------------------*/

static color_t i_effective_color(const color_t color)
{
    uint8_t r, g, b, a;
    color_get_rgba(color, &r, &g, &b, &a);
    return color_rgba(r, g, b, a);
}
    
/*---------------------------------------------------------------------------*/

Image *imgproc_binarize(const Image *image, const color_t color, const color_t bgcolor)
{
    Palette *palette = palette_binary(i_effective_color(color), i_effective_color(bgcolor));
    Pixbuf *pixbuf1 = image_pixels(image, ekGRAY8);
    Pixbuf *pixbuf2 = i_binarize_otsu(pixbuf1, TRUE);
    Image *bimage = image_from_pixbuf(pixbuf2, palette);
    pixbuf_destroy(&pixbuf1);
    pixbuf_destroy(&pixbuf2);
    palette_destroy(&palette);
    return bimage;
}

/*---------------------------------------------------------------------------*/

Image *imgproc_colorize(const Image *image, const color_t color)
{
    Image *nimage = NULL;
    Pixbuf *pixbuf = image_pixels(image, ekRGBA32);
    uint32_t i, n = pixbuf_width(pixbuf) * pixbuf_height(pixbuf);
    byte_t *data = pixbuf_data(pixbuf);
    uint8_t r, g, b;
    cassert(pixbuf_format(pixbuf) == ekRGBA32);
    color_get_rgb(color, &r, &g, &b);

    for (i = 0; i < n; ++i, data += 4)
    {
        uint8_t *c = cast(data, uint8_t);
        *c = r;
        *(c + 1) = g;
        *(c + 2) = b;
    }

    nimage = image_from_pixbuf(pixbuf, NULL);
    pixbuf_destroy(&pixbuf);
    return nimage;
}
