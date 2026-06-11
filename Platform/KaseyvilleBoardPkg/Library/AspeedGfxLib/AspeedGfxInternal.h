/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef ASPEED_GFX_INTERNAL_H_
#define ASPEED_GFX_INTERNAL_H_

#include "AspeedGfxTypes.h"
#include "drm/drmP.h"
#include "drm/drm_mode.h"
#include "drm/drm_crtc.h"
#include "drm/drm_crtc_helper.h"
#include "drm/drm_fb_helper.h"

#define DEBUG_AST_REG   DEBUG_VERBOSE

/**
  Reads a DWORD from PCI configuration space

  @param[in]  dev The PCI device.
  @param[in]  where The offset within the PCI configuration space.
  @param[out] val Pointer to store the read value.

  @return 0 on success.

**/
static inline int pci_read_config_dword(struct pci_dev *dev, int where, u32 *val)
{
    UINT32 addr;
    addr = PCI_LIB_ADDRESS(dev->bus, (dev->devfn >> 3) & 0x1f, dev->devfn & 7, where);
    *val = PciRead32 (addr);
    return 0;
}

/**
  Writes a DWORD to PCI configuration space

  @param[in]  dev The PCI device.
  @param[in]  where The offset within the PCI configuration space.
  @param[in]  val The value to write.

  @return 0 on success.

**/
static inline int pci_write_config_dword(struct pci_dev *dev, int where,    u32 val)
{
    UINT32 addr;
    addr = PCI_LIB_ADDRESS(dev->bus, (dev->devfn >> 3) & 0x1f, dev->devfn & 7, where);
    PciWrite32 (addr, val);
    return 0;
}

/**
  Reads a BYTE from PCI configuration space

  @param[in]  dev The PCI device.
  @param[in]  where The offset within the PCI configuration space.
  @param[out] val Pointer to store the read value.

  @return 0 on success.

**/
static inline int pci_read_config_byte(struct pci_dev *dev, int where,  u8 *val)
{
    UINT32 addr;
    addr = PCI_LIB_ADDRESS(dev->bus, (dev->devfn >> 3) & 0x1f, dev->devfn & 7, where);
    *val = PciRead8 (addr);
    return 0;
}

/**
  Reads a 32-bit MMIO register.

  @param[in]  Address The MMIO register to read.

  @return The value read.
**/
static inline u32 ioread32(void __iomem *p) {
    u32 v = MmioRead32((UINTN)(p));
    DEBUG ((DEBUG_AST_REG, "ioread32 : 0x%p 0x%x\n", p, v));
    return v;
}

/**
  Writes a 32-bit MMIO register.

  @param[in]  Address The MMIO register to write.
  @param[in]  Value   The value to write to the MMIO register.

**/
static inline void iowrite32(u32 val, void __iomem *p) {
    MmioWrite32((UINTN)(p), val);
    DEBUG ((DEBUG_AST_REG, "iowrite32: 0x%p 0x%x\n", p, val));
}

/**
  Reads a 16-bit MMIO register.

  @param[in]  Address The MMIO register to read.

  @return The value read.
**/
static inline u16 ioread16(void __iomem *p) {
    u16 v = MmioRead16((UINTN)(p));
    DEBUG ((DEBUG_AST_REG, "ioread16 : 0x%p 0x%x\n", p, v));
    return v;
}

/**
  Writes a 16-bit MMIO register.

  @param[in]  Address The MMIO register to write.
  @param[in]  Value   The value to write to the MMIO register.

**/
static inline void iowrite16(u16 val, void __iomem *p) {
    MmioWrite16((UINTN)(p), val);
    DEBUG ((DEBUG_AST_REG, "iowrite16: 0x%p 0x%x\n", p, val));
}

/**
  Reads an 8-bit MMIO register.

  @param[in]  Address The MMIO register to read.

  @return The value read.
**/
static inline u8 ioread8(void __iomem *p) {
    u8 v = MmioRead8((UINTN)(p));
    DEBUG ((DEBUG_AST_REG, "ioread8  : 0x%p 0x%x\n", p, v));
    return v;
}

/**
  Writes an 8-bit MMIO register.

  @param[in]  Address The MMIO register to write.
  @param[in]  Value   The value to write to the MMIO register.
**/
static inline void iowrite8(u8 val, void __iomem *p) {
    MmioWrite8((UINTN)(p), val);
    DEBUG ((DEBUG_AST_REG, "iowrite8: 0x%p 0x%x\n", p, val));
}

/**
  Initialize Aspeed graphics and program mode settings.

  @param[in] dev_addr  Encoded PCI bus/device/function address.
  @param[in] timings   Display timing information used to set mode.

  @retval 0   Success or already initialized.
  @retval -1  Initialization failure.
**/
int aspeed_gfx_init(uint32_t dev_addr,
                    struct display_timing_info *timings);

/**
  Populate GOP mode information from the active DRM CRTC/framebuffer.

  @param[out] gop_mode  Pointer to GOP mode structure to fill.

  @retval 0   Success.
  @retval -1  Invalid state or missing input.
**/
int aspeed_gop_mode_info(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *gop_mode);

#endif
