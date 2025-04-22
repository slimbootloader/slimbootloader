/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
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

static inline int pci_read_config_dword(struct pci_dev *dev, int where, u32 *val)
{
    UINT32 addr;
    addr = PCI_LIB_ADDRESS(dev->bus, (dev->devfn >> 3) & 0x1f, dev->devfn & 7, where);
    *val = PciRead32 (addr);
    return 0;
}

static inline int pci_write_config_dword(struct pci_dev *dev, int where,    u32 val)
{
    UINT32 addr;
    addr = PCI_LIB_ADDRESS(dev->bus, (dev->devfn >> 3) & 0x1f, dev->devfn & 7, where);
    PciWrite32 (addr, val);
    return 0;
}

static inline int pci_read_config_byte(struct pci_dev *dev, int where,  u8 *val)
{
    UINT32 addr;
    addr = PCI_LIB_ADDRESS(dev->bus, (dev->devfn >> 3) & 0x1f, dev->devfn & 7, where);
    *val = PciRead8 (addr);
    return 0;
}

static inline u32 ioread32(void __iomem *p) {
    u32 v = MmioRead32((UINTN)(p));
    DEBUG ((DEBUG_AST_REG, "ioread32 : 0x%p 0x%x\n", p, v));
    return v;
}

static inline void iowrite32(u32 val, void __iomem *p) {
    MmioWrite32((UINTN)(p), val);
    DEBUG ((DEBUG_AST_REG, "iowrite32: 0x%p 0x%x\n", p, val));
}

static inline u16 ioread16(void __iomem *p) {
    u16 v = MmioRead16((UINTN)(p));
    DEBUG ((DEBUG_AST_REG, "ioread16 : 0x%p 0x%x\n", p, v));
    return v;
}

static inline void iowrite16(u16 val, void __iomem *p) {
    MmioWrite16((UINTN)(p), val);
    DEBUG ((DEBUG_AST_REG, "iowrite16: 0x%p 0x%x\n", p, val));
}

static inline u8 ioread8(void __iomem *p) {
    u8 v = MmioRead8((UINTN)(p));
    DEBUG ((DEBUG_AST_REG, "ioread8  : 0x%p 0x%x\n", p, v));
    return v;
}

static inline void iowrite8(u8 val, void __iomem *p) {
    MmioWrite8((UINTN)(p), val);
    DEBUG ((DEBUG_AST_REG, "iowrite8: 0x%p 0x%x\n", p, val));
}

static inline u32 ioread_cbio32(void __iomem *p) {
    return IoRead32((uint16_t)((intptr_t)p));
}

static inline void iowrite_cbio32(u32 val, void __iomem *p) {
    IoWrite32 (val, (uint16_t)((intptr_t)p));
}

static inline u16 ioread_cbio16(void __iomem *p) {
    return IoRead16 ((uint16_t)((intptr_t)p));
}

static inline void iowrite_cbio16(u16 val, void __iomem *p) {
    IoWrite16 (val, (uint16_t)((intptr_t)p));
}

static inline u8 ioread_cbio8(void __iomem *p) {
    return IoRead8((uint16_t)((intptr_t)p));
}

static inline void iowrite_cbio8(u8 val, void __iomem *p) {
    IoWrite8 ((uint16_t)((intptr_t)p), val);
}

int aspeed_gfx_init(uint32_t dev_addr,
                    struct display_timing_info *timings);

int aspeed_gop_mode_info(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *gop_mode);

#endif
