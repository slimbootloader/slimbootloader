/*
 * Copyright (c) 2007 Dave Airlie <airlied@linux.ie>
 * Copyright (c) 2007 Jakob Bornecrantz <wallbraker@gmail.com>
 * Copyright (c) 2008 Red Hat Inc.
 * Copyright (c) 2007-2008 Tungsten Graphics, Inc., Cedar Park, TX., USA
 * Copyright (c) 2007-2008 Intel Corporation
 * Copyright (c) 2021 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * $FreeBSD$
 */

#ifndef _DRM_MODE_H
#define _DRM_MODE_H

/* Video mode flags */
/* bit compatible with the xorg definitions. */
#define DRM_MODE_FLAG_PHSYNC    (1<<0)
#define DRM_MODE_FLAG_NHSYNC    (1<<1)
#define DRM_MODE_FLAG_PVSYNC    (1<<2)
#define DRM_MODE_FLAG_NVSYNC    (1<<3)

/* DPMS flags */
/* bit compatible with the xorg definitions. */
#define DRM_MODE_DPMS_ON        0
#define DRM_MODE_DPMS_STANDBY   1
#define DRM_MODE_DPMS_SUSPEND   2
#define DRM_MODE_DPMS_OFF       3

struct drm_mode_fb_cmd2 {
    unsigned int fb_id;
    unsigned int width, height;
    unsigned int pixel_format; /* fourcc code from drm_fourcc.h */
    unsigned int flags; /* see above flags */

    /*
        * In case of planar formats, this ioctl allows up to 4
        * buffer objects with offets and pitches per plane.
        * The pitch and offset order is dictated by the fourcc,
        * e.g. NV12 (http://fourcc.org/yuv.php#NV12) is described as:
        *
        *   YUV 4:2:0 image with a plane of 8 bit Y samples
        *   followed by an interleaved U/V plane containing
        *   8 bit 2x2 subsampled colour difference samples.
        *
        * So it would consist of Y as offset[0] and UV as
        * offeset[1].  Note that offset[0] will generally
        * be 0.
        */
    unsigned int handles[4];
    unsigned int pitches[4]; /* pitch for each plane */
    unsigned int offsets[4]; /* offset of each plane */
};

#endif
