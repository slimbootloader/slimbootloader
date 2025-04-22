/*
 * Copyright © 2006 Keith Packard
 * Copyright © 2007-2008 Dave Airlie
 * Copyright © 2007-2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * $FreeBSD$
 */

/*
 * The DRM mode setting helper functions are common code for drivers to use if
 * they wish.  Drivers are not forced to use this code in their
 * implementations but it would be useful if they code they do use at least
 * provides a consistent interface and operation to userspace
 */

#ifndef __DRM_CRTC_HELPER_H__
#define __DRM_CRTC_HELPER_H__

#include <drm/drm_crtc.h>

enum mode_set_atomic {
    LEAVE_ATOMIC_MODE_SET,
    ENTER_ATOMIC_MODE_SET,
};

/**
 * drm_crtc_helper_funcs - helper operations for CRTCs
 * @mode_set: set this mode
 *
 * The helper operations are called by the mid-layer CRTC helper.
 */
struct drm_crtc_helper_funcs {
   /*
    * Control power levels on the CRTC.  If the mode passed in is
    * unsupported, the provider must use the next lowest power level.
    */
    void (*dpms)(struct drm_crtc *crtc, int mode);
    void (*prepare)(struct drm_crtc *crtc);
    void (*commit)(struct drm_crtc *crtc);

    /* Actually set the mode */
    int (*mode_set)(struct drm_crtc *crtc, struct drm_display_mode *mode,
                    struct drm_display_mode *adjusted_mode, int x, int y,
                    struct drm_framebuffer *old_fb);

    /* Move the crtc on the current fb to the given position *optional* */
    int (*mode_set_base)(struct drm_crtc *crtc, int x, int y,
                         struct drm_framebuffer *old_fb);
    int (*mode_set_base_atomic)(struct drm_crtc *crtc,
                                struct drm_framebuffer *fb, int x, int y,
                                enum mode_set_atomic);

    /* reload the current crtc LUT */
    void (*load_lut)(struct drm_crtc *crtc);

    /* disable crtc when not in use - more explicit than dpms off */
    void (*disable)(struct drm_crtc *crtc);
};

static inline void drm_crtc_helper_add(struct drm_crtc *crtc,
                                       const struct drm_crtc_helper_funcs *funcs)
{
    crtc->helper_private = (void *)funcs;
}

#endif
