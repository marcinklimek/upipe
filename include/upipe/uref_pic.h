/*
 * Copyright (C) 2012 OpenHeadend S.A.R.L.
 *
 * Authors: Christophe Massiot
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/** @file
 * @short Upipe wrapper for picture ubuf and uref
 */

#ifndef _UPIPE_UREF_PIC_H_
/** @hidden */
#define _UPIPE_UREF_PIC_H_

#include <upipe/uref.h>
#include <upipe/uref_attr.h>
#include <upipe/ubuf.h>
#include <upipe/ubuf_pic.h>
#include <upipe/uref_pic_flow.h>

#include <stdint.h>
#include <stdbool.h>

UREF_ATTR_TEMPLATE(pic, hposition, "p.hposition", unsigned, uint64_t, horizontal position)
UREF_ATTR_TEMPLATE(pic, vposition, "p.vposition", unsigned, uint64_t, vertical position)
UREF_ATTR_TEMPLATE(pic, aspect, "p.aspect", rational, struct urational, aspect ratio)
UREF_ATTR_TEMPLATE_VOID(pic, interlaced, "p.interlaced", interlaced)
UREF_ATTR_TEMPLATE_VOID(pic, tff, "p.tff", top field first)
UREF_ATTR_TEMPLATE(pic, fields, "p.fields", small_unsigned, uint8_t, number of fields)

/** @This returns a new uref pointing to a new ubuf pointing to a picture.
 * This is equivalent to the two operations sequentially, and is a shortcut.
 *
 * @param uref_mgr management structure for this uref type
 * @param ubuf_mgr management structure for this ubuf type
 * @param hsize horizontal size in pixels
 * @param vsize vertical size in lines
 * @return pointer to uref or NULL in case of failure
 */
static inline struct uref *uref_pic_alloc(struct uref_mgr *uref_mgr,
                                          struct ubuf_mgr *ubuf_mgr,
                                          int hsize, int vsize)
{
    struct uref *uref = uref_alloc(uref_mgr);
    if (unlikely(uref == NULL))
        return NULL;

    struct ubuf *ubuf = ubuf_pic_alloc(ubuf_mgr, hsize, vsize);
    if (unlikely(ubuf == NULL)) {
        uref_free(uref);
        return NULL;
    }

    uref_attach_ubuf(uref, ubuf);
    return uref;
}

/** @see ubuf_pic_size */
static inline bool uref_pic_size(struct uref *uref,
                                 size_t *hsize_p, size_t *vsize_p,
                                 uint8_t *macropixel_p)
{
    if (uref->ubuf == NULL)
        return false;
    return ubuf_pic_size(uref->ubuf, hsize_p, vsize_p, macropixel_p);
}

/** @see ubuf_pic_plane_iterate */
static inline bool uref_pic_plane_iterate(struct uref *uref,
                                          const char **chroma_p)
{
    if (uref->ubuf == NULL)
        return false;
    return ubuf_pic_plane_iterate(uref->ubuf, chroma_p);
}

/** @see ubuf_pic_plane_size */
static inline bool uref_pic_plane_size(struct uref *uref, const char *chroma,
                                       size_t *stride_p,
                                       uint8_t *hsub_p, uint8_t *vsub_p,
                                       uint8_t *macropixel_size_p)
{
    if (uref->ubuf == NULL)
        return false;
    return ubuf_pic_plane_size(uref->ubuf, chroma, stride_p, hsub_p, vsub_p,
                               macropixel_size_p);
}

/** @see ubuf_pic_plane_read */
static inline bool uref_pic_plane_read(struct uref *uref, const char *chroma,
                                       int hoffset, int voffset,
                                       int hsize, int vsize,
                                       const uint8_t **buffer_p)
{
    if (uref->ubuf == NULL)
        return false;
    return ubuf_pic_plane_read(uref->ubuf, chroma, hoffset, voffset,
                               hsize, vsize, buffer_p);
}

/** @see ubuf_pic_plane_write */
static inline bool uref_pic_plane_write(struct uref *uref, const char *chroma,
                                       int hoffset, int voffset,
                                       int hsize, int vsize,
                                       uint8_t **buffer_p)
{
    if (uref->ubuf == NULL)
        return false;
    return ubuf_pic_plane_write(uref->ubuf, chroma, hoffset, voffset,
                                hsize, vsize, buffer_p);
}

/** @see ubuf_pic_plane_unmap */
static inline bool uref_pic_plane_unmap(struct uref *uref, const char *chroma,
                                       int hoffset, int voffset,
                                       int hsize, int vsize)
{
    if (uref->ubuf == NULL)
        return false;
    return ubuf_pic_plane_unmap(uref->ubuf, chroma, hoffset, voffset,
                                hsize, vsize);
}

/** @see ubuf_pic_resize */
static inline bool uref_pic_resize(struct uref *uref,
                                   int hskip, int vskip,
                                   int new_hsize, int new_vsize)
{
    if (uref->ubuf == NULL)
        return false;
    return ubuf_pic_resize(uref->ubuf, hskip, vskip, new_hsize, new_vsize);
}

/** @This allocates a new ubuf of size new_hsize/new_vsize, and copies part of
 * the old picture ubuf to the new one, switches the ubufs and frees
 * the old one.
 *
 * @param uref pointer to uref structure
 * @param ubuf_mgr management structure for the new ubuf
 * @param hskip number of pixels to skip at the beginning of each line (if < 0,
 * extend the picture leftwards)
 * @param vskip number of lines to skip at the beginning of the picture (if < 0,
 * extend the picture upwards)
 * @param new_hsize final horizontal size of the buffer, in pixels (if set
 * to -1, keep same line ends)
 * @param new_vsize final vertical size of the buffer, in lines (if set
 * to -1, keep same last line)
 * @return false in case of error
 */
static inline bool uref_pic_replace(struct uref *uref,
                                    struct ubuf_mgr *ubuf_mgr,
                                    int hskip, int vskip,
                                    int new_hsize, int new_vsize)
{
    if (uref->ubuf == NULL)
        return false;
    return ubuf_pic_replace(ubuf_mgr, &uref->ubuf, hskip, vskip,
                            new_hsize, new_vsize);
}

#endif
