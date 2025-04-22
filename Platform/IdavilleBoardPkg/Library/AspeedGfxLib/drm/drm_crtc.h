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
#ifndef __DRM_CRTC_H__
#define __DRM_CRTC_H__

#include <drm/drm_mode.h>

struct drm_device;
struct drm_mode_set;
struct drm_format;
struct drm_framebuffer;

/*
 * Note on terminology:  here, for brevity and convenience, we refer to connector
 * control chips as 'CRTCs'.  They can control any type of connector, VGA, LVDS,
 * DVI, etc.  And 'screen' refers to the whole of the visible display, which
 * may span multiple monitors (and therefore multiple CRTC and connector
 * structures).
 */

enum drm_mode_status {
    MODE_OK     = 0,    /* Mode OK */
    MODE_HSYNC,         /* hsync out of range */
    MODE_VSYNC,         /* vsync out of range */
    MODE_H_ILLEGAL,     /* mode has illegal horizontal timings */
    MODE_V_ILLEGAL,     /* mode has illegal horizontal timings */
    MODE_BAD_WIDTH,     /* requires an unsupported linepitch */
    MODE_NOMODE,        /* no mode with a matching name */
    MODE_NO_INTERLACE,  /* interlaced mode not supported */
    MODE_NO_DBLESCAN,   /* doublescan mode not supported */
    MODE_NO_VSCAN,      /* multiscan mode not supported */
    MODE_MEM,           /* insufficient video memory */
    MODE_VIRTUAL_X,     /* mode width too large for specified virtual size */
    MODE_VIRTUAL_Y,     /* mode height too large for specified virtual size */
    MODE_MEM_VIRT,      /* insufficient video memory given virtual size */
    MODE_NOCLOCK,       /* no fixed clock available */
    MODE_CLOCK_HIGH,    /* clock required is too high */
    MODE_CLOCK_LOW,     /* clock required is too low */
    MODE_CLOCK_RANGE,   /* clock/mode isn't in a ClockRange */
    MODE_BAD_HVALUE,    /* horizontal timing was out of range */
    MODE_BAD_VVALUE,    /* vertical timing was out of range */
    MODE_BAD_VSCAN,     /* VScan value out of range */
    MODE_HSYNC_NARROW,  /* horizontal sync too narrow */
    MODE_HSYNC_WIDE,    /* horizontal sync too wide */
    MODE_HBLANK_NARROW, /* horizontal blanking too narrow */
    MODE_HBLANK_WIDE,   /* horizontal blanking too wide */
    MODE_VSYNC_NARROW,  /* vertical sync too narrow */
    MODE_VSYNC_WIDE,    /* vertical sync too wide */
    MODE_VBLANK_NARROW, /* vertical blanking too narrow */
    MODE_VBLANK_WIDE,   /* vertical blanking too wide */
    MODE_PANEL,         /* exceeds panel dimensions */
    MODE_INTERLACE_WIDTH, /* width too large for interlaced mode */
    MODE_ONE_WIDTH,     /* only one width is supported */
    MODE_ONE_HEIGHT,    /* only one height is supported */
    MODE_ONE_SIZE,      /* only one resolution is supported */
    MODE_NO_REDUCED,    /* monitor doesn't accept reduced blanking */
    MODE_UNVERIFIED = -3, /* mode needs to reverified */
    MODE_BAD = -2,      /* unspecified reason */
    MODE_ERROR  = -1    /* error condition */
};

struct drm_display_mode {
    /* Proposed mode values */
    int clock;              /* in kHz */
    int hdisplay;
    int hsync_start;
    int hsync_end;
    int htotal;
    int hskew;
    int vdisplay;
    int vsync_start;
    int vsync_end;
    int vtotal;
    int vscan;
    unsigned int flags;

    /* Addressable image size (may be 0 for projectors, etc.) */
    int width_mm;
    int height_mm;

    /* Actual mode we give to hw */
    int clock_index;
    int synth_clock;
    int crtc_hdisplay;
    int crtc_hblank_start;
    int crtc_hblank_end;
    int crtc_hsync_start;
    int crtc_hsync_end;
    int crtc_htotal;
    int crtc_hskew;
    int crtc_vdisplay;
    int crtc_vblank_start;
    int crtc_vblank_end;
    int crtc_vsync_start;
    int crtc_vsync_end;
    int crtc_vtotal;

    /* Driver private mode info */
    int private_size;
    int *private;
    int private_flags;

    int vrefresh;           /* in Hz */
    int hsync;              /* in kHz */
};

struct drm_format {
    uint8_t cpp[1];
};

struct drm_framebuffer {
    struct drm_device *dev;
    unsigned int pitches[4];
    struct drm_format *format;
};

struct drm_primary {
    /* framebuffer the connector is currently bound to */
    struct drm_framebuffer *fb;
};

struct drm_crtc;

/**
 * drm_crtc_funcs - control CRTCs for a given device
 *
 * The drm_crtc_funcs structure is the central CRTC management structure
 * in the DRM.  Each CRTC controls one or more connectors (note that the name
 * CRTC is simply historical, a CRTC may control LVDS, VGA, DVI, TV out, etc.
 * connectors, not just CRTs).
 *
 * Each driver is responsible for filling out this structure at startup time,
 * in addition to providing other modesetting features, like i2c and DDC
 * bus accessors.
 */
struct drm_crtc_funcs {
    /* Reset CRTC state */
    void (*reset)(struct drm_crtc *crtc);

    /* cursor controls */
    int (*cursor_set)(struct drm_crtc *crtc, struct drm_file *file_priv,
                        uint32_t handle, uint32_t width, uint32_t height);
    int (*cursor_move)(struct drm_crtc *crtc, int x, int y);

    /* Set gamma on the CRTC */
    void (*gamma_set)(struct drm_crtc *crtc, u16 *r, u16 *g, u16 *b,
                        uint32_t start, uint32_t size);
    /* Object destroy routine */
    void (*destroy)(struct drm_crtc *crtc);

    int (*set_config)(struct drm_mode_set *set);
};

/**
 * drm_crtc - central CRTC control structure
 *
 * Each CRTC may have one or more connectors associated with it.  This structure
 * allows the CRTC to be controlled.
 */
struct drm_crtc {
    struct drm_device *dev;

    struct drm_primary *primary;

    /* Requested mode from modesetting. */
    struct drm_display_mode mode;

    const struct drm_crtc_funcs *funcs;

    /* CRTC gamma size for reporting to userspace */
    uint32_t gamma_size;
    uint16_t *gamma_store;

    /* if you are using the helper */
    void *helper_private;
};

/**
 * drm_connector - central DRM connector control structure
 *
 * Each connector may be connected to one or more CRTCs, or may be clonable by
 * another connector if they can share a CRTC.  Each connector also has a specific
 * position in the broader display (referred to as a 'screen' though it could
 * span multiple monitors).
 */
struct drm_connector {
    struct drm_device *dev;
};

/**
 * drm_encoder - central DRM encoder structure
 *
 * CRTCs drive pixels to encoders, which convert them into signals
 * appropriate for a given connector or set of connectors.
 */
struct drm_encoder {
   struct drm_device *dev;
};

/**
 * drm_mode_set - new values for a CRTC config change
 *
 * Represents a single crtc the connectors that it drives with what mode
 * and from which framebuffer it scans out from.
 *
 * This is used to set modes.
 */
struct drm_mode_set {
    struct drm_framebuffer *fb;
    struct drm_crtc *crtc;
    struct drm_display_mode *mode;

    uint32_t x;
    uint32_t y;
};

/**
 * struct drm_mode_config_funcs - basic driver provided mode setting functions
 *
 * Some global (i.e. not per-CRTC, connector, etc) mode setting functions that
 * involve drivers.
 */
struct drm_mode_config_funcs {
    struct drm_framebuffer *(*fb_create)(struct drm_device *dev,
                                         struct drm_file *file_priv,
                                         const struct drm_mode_fb_cmd2 *mode_cmd);
    void (*output_poll_changed)(struct drm_device *dev);
};

/**
 * drm_mode_config - Mode configuration control structure
 *
 * Core mode resource tracking structure.  All CRTC, encoders, and connectors
 * enumerated by the driver are added here, as are global properties.  Some
 * global restrictions are also here, e.g. dimension restrictions.
 */
struct drm_mode_config {
    struct drm_crtc *crtc;

    int min_width, min_height;
    int max_width, max_height;
    const struct drm_mode_config_funcs *funcs;
    resource_size_t fb_base;

    /* dumb ioctl parameters */
    uint32_t preferred_depth, prefer_shadow;
};

extern int drm_crtc_init(struct drm_device *dev,
                         struct drm_crtc *crtc,
                         const struct drm_crtc_funcs *funcs);

static inline void drm_mode_config_init(struct drm_device *dev)
{
}

static inline int drm_mode_crtc_set_gamma_size(struct drm_crtc *crtc,
                                  int gamma_size)
{
    return 0;
}

#endif /* __DRM_CRTC_H__ */
