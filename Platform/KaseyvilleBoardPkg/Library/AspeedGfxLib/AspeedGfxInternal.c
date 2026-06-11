/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "ast_drv.h"

static struct drm_device *m_drm_dev = NULL;

/**
  Update CRTC mode fields from platform display timing information.

  @param[in, out] mode     Pointer to DRM display mode to update.
  @param[in]      timings  Pointer to input timing parameters.
**/
static void aspeed_update_crtc_mode_info(struct drm_display_mode *mode,
                                         struct display_timing_info *timings)
{
  if (!mode || !timings)
    return;

  mode->clock             = timings->Clock;
  mode->vrefresh          = timings->RefreshRate;
  mode->hdisplay          = timings->Hdisp;
  mode->vdisplay          = timings->Vdisp;
  mode->crtc_hdisplay     = mode->hdisplay;
  mode->crtc_vdisplay     = mode->vdisplay;;

  mode->crtc_hsync_start  = mode->hdisplay + timings->Hfp;
  mode->crtc_hsync_end    = mode->crtc_hsync_start + timings->Hsync;
  mode->crtc_htotal       = mode->crtc_hsync_end + timings->Hbp;
  mode->crtc_hblank_start = MIN (mode->crtc_hsync_start, mode->crtc_hdisplay);
  mode->crtc_hblank_end   = MAX (mode->crtc_hsync_end, mode->crtc_htotal);

  mode->crtc_vsync_start  = mode->crtc_vdisplay + timings->Vfp;
  mode->crtc_vsync_end    = mode->crtc_vsync_start + timings->Vsync;
  mode->crtc_vtotal       = mode->crtc_vsync_end + timings->Vbp;
  mode->crtc_vblank_start = MIN (mode->crtc_vsync_start, mode->crtc_vdisplay);
  mode->crtc_vblank_end   = MAX (mode->crtc_vsync_end, mode->crtc_vtotal);

  mode->flags = 0;
  if (timings->Phsync)
    mode->flags |= DRM_MODE_FLAG_PHSYNC;
  else
    mode->flags |= DRM_MODE_FLAG_NHSYNC;
  if (timings->Pvsync)
    mode->flags |= DRM_MODE_FLAG_PVSYNC;
  else
    mode->flags |= DRM_MODE_FLAG_NVSYNC;
}

/**
  Populate GOP mode information from the active DRM CRTC/framebuffer.

  @param[out] gop_mode  Pointer to GOP mode structure to fill.

  @retval 0   Success.
  @retval -1  Invalid state or missing input.
**/
int aspeed_gop_mode_info(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *gop_mode)
{
  struct drm_device *dev;
  struct drm_crtc *crtc;
  struct drm_framebuffer *fb;

  dev = m_drm_dev;

  if (!dev || !gop_mode)
    return -1;

  crtc = dev->mode_config.crtc;
  if (!crtc)
    return -1;

  fb = crtc->primary->fb;
  if (!fb)
    return -1;

  gop_mode->Info->Version              = 0;
  gop_mode->Info->PixelFormat          = PixelBlueGreenRedReserved8BitPerColor;
  gop_mode->SizeOfInfo                 = sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
  gop_mode->FrameBufferBase            = dev->mode_config.fb_base;
  gop_mode->Info->HorizontalResolution = crtc->mode.hdisplay;
  gop_mode->Info->VerticalResolution   = crtc->mode.vdisplay;
  gop_mode->Info->PixelsPerScanLine    = fb->pitches[0] * 8 / (fb->format->cpp[0] * 8);
  gop_mode->FrameBufferSize            = gop_mode->Info->PixelsPerScanLine * gop_mode->Info->VerticalResolution * fb->format->cpp[0];
  gop_mode->MaxMode                    = 1;
  gop_mode->Mode                       = 0;

  return 0;
}

/**
  Initialize Aspeed graphics and program mode settings.

  @param[in] dev_addr  Encoded PCI bus/device/function address.
  @param[in] timings   Display timing information used to set mode.

  @retval 0   Success or already initialized.
  @retval -1  Initialization failure.
**/
int aspeed_gfx_init(uint32_t dev_addr,
                    struct display_timing_info *timings)
{
  struct pci_dev *pcidev;
  struct drm_device *dev;
  struct drm_crtc *crtc;
  struct drm_crtc_helper_funcs *helper_funcs;
  struct drm_display_mode adjusted_mode;
  const struct drm_mode_config_funcs *mode_funcs;
  struct drm_framebuffer *fb;

  if (!timings)
    return -1;

  if (m_drm_dev)
    return 0;

  pcidev = kzalloc(sizeof(*pcidev), GFP_KERNEL);
  if (!pcidev)
    return -1;
  pcidev->devfn = (((dev_addr >> 8) & 0x1f) << 3) | (dev_addr & 7);
  pcidev->bus   = (dev_addr >> 16) & 0xff;

  dev = kzalloc(sizeof(*dev), GFP_KERNEL);
  if (!dev)
    return -1;
  dev->pdev  = pcidev;

  ast_driver_load (dev, 0);

  crtc = dev->mode_config.crtc;
  if (!crtc)
    return -1;

  helper_funcs = (struct drm_crtc_helper_funcs *)crtc->helper_private;
  if (!helper_funcs)
    return -1;

  aspeed_update_crtc_mode_info(&crtc->mode, timings);

  mode_funcs = dev->mode_config.funcs;
  if (mode_funcs && mode_funcs->fb_create)
    mode_funcs->fb_create(dev, NULL, NULL);
  else
    return -1;

  CopyMem (&adjusted_mode, &crtc->mode, sizeof(struct drm_display_mode));
  helper_funcs->mode_set(crtc, &crtc->mode, &adjusted_mode, 0, 0, NULL);

  fb = crtc->primary->fb;
  SetMem32 ((void *)(uintptr_t)dev->mode_config.fb_base,
            fb->pitches[0] * crtc->mode.vdisplay, 0x0);

  helper_funcs->commit(crtc);

  m_drm_dev = dev;

  DEBUG ((DEBUG_INFO, "aspeed_gfx_init...done!\n"));

  return 0;
}

/**
  Allocate and attach a primary plane container for fbdev usage.

  @param[in] dev  DRM device instance.

  @retval 0   Success.
  @retval -1  Initialization failure.
**/
int ast_fbdev_init(struct drm_device *dev)
{
  struct drm_crtc *crtc;
  struct drm_primary *primary;

  crtc = dev->mode_config.crtc;
  if (!crtc)
    return -1;

  primary = kzalloc(sizeof(*primary), GFP_KERNEL);
  if (!primary)
    return -1;

  crtc->primary = primary;
  return 0;
}

/**
  Initialize DRM framebuffer object and bind it to the primary plane.

  @param[in] dev       DRM device instance.
  @param[in] ast_fb    AST framebuffer wrapper to initialize.
  @param[in] mode_cmd  Framebuffer creation parameters.
  @param[in] obj       GEM object backing store.

  @retval 0   Success.
  @retval -1  Initialization failure.
**/
int ast_framebuffer_init(struct drm_device *dev,
                         struct ast_framebuffer *ast_fb,
                         const struct drm_mode_fb_cmd2 *mode_cmd,
                         struct drm_gem_object *obj)
{
  struct drm_crtc *crtc;
  struct drm_framebuffer *fb;
  struct drm_format *format;

  crtc = dev->mode_config.crtc;
  if (!crtc)
    return -1;

  fb = &ast_fb->base;
  if (!fb)
    return -1;

  format = kzalloc(sizeof(*format), GFP_KERNEL);
  if (!format)
    return -1;
  format->cpp[0] = 4;

  fb->dev = dev;
  fb->format = format;
  fb->pitches[0] = (crtc->mode.hdisplay * format->cpp[0] + 7) & ~7;

  crtc->primary->fb = fb;

  DEBUG ((DEBUG_INFO, "fb(0x%x) %dx%d, pitches %d\n", dev->mode_config.fb_base,
                       crtc->mode.hdisplay, crtc->mode.vdisplay, fb->pitches[0]));

  return 0;
}

/**
  Register a CRTC object with the DRM mode configuration.

  @param[in] dev    DRM device instance.
  @param[in] crtc   CRTC object to register.
  @param[in] funcs  CRTC callback table.

  @retval 0  Success.
**/
int drm_crtc_init(struct drm_device *dev, struct drm_crtc *crtc,
                  const struct drm_crtc_funcs *funcs)
{
  crtc->dev = dev;
  dev->mode_config.crtc = crtc;

  return 0;
}

//
// Dummy functions below
//
/**
  Stub memory manager initialization.

  @param[in] ast  AST private data.

  @retval 0  Always succeeds.
**/
int ast_mm_init(struct ast_private *ast)
{
  return 0;
}

/**
  Stub firmware backup helper.

  @param[in] dev   DRM device instance.
  @param[in] addr  Destination buffer for firmware data.
  @param[in] size  Buffer size in bytes.

  @retval FALSE  Always returns FALSE in this stub.
**/
bool ast_backup_fw(struct drm_device *dev, u8 *addr, u32 size)
{
    return FALSE;
}
