/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MtrrLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/OsBootOptionGuid.h>
#include <Register/Intel/Cpuid.h>
#include <Register/Intel/ArchitecturalMsr.h>
#include <IndustryStandard/Pci30.h>

#define  MTRR_CACHE_WRITE_COMBINING  1

/**
  Set framebuffer range as writecombining for performance.

  @param[in]    FrameBufferBase   Framebuffer base address.
                                  if 0, it will use framebuffer HOB to get the base.
  @param[in]    FrameBufferSize   Framebuffer size.
                                  if 0, it will use framebuffer HOB to get the size.
                                  if MAX_UINT32, it will parse the PCI bar to get the size.

  @retval  EFI_SUCCESS            The framebuffer cache was enabled successfully.
  @retval  EFI_NOT_FOUND          Failed to find the required GFX controller.
  @retval  EFI_UNSUPPORTED        The base and size cannot be supported.
  @retval  EFI_OUT_OF_RESOURCES   No enough MTRR to use.

  **/
EFI_STATUS
EFIAPI
SetFrameBufferWriteCombining (
  IN  EFI_PHYSICAL_ADDRESS   FrameBufferBase,
  IN  UINT32                 FrameBufferSize
  )
{
  UINT8              CmdVal;
  UINT32             BarVal;
  UINT32             MsrIdx;
  UINT32             MsrMax;
  UINT32             Data;
  UINTN              GfxPciBase;
  UINT64             FbSize;
  INTN               SizeBit;
  INTN               BaseBit;
  UINT32             VidDid;
  UINT32             Offset;
  UINT32             BarIndex;
  BOOLEAN            Found;
  MSR_IA32_MTRRCAP_REGISTER          MsrCap;
  MSR_IA32_MTRR_PHYSMASK_REGISTER    MsrMask;
  MSR_IA32_MTRR_PHYSBASE_REGISTER    MsrBase;
  CPUID_VIR_PHY_ADDRESS_SIZE_EAX     VirPhyAddressSize;
  EFI_PEI_GRAPHICS_DEVICE_INFO_HOB  *GfxDevHob;
  EFI_PEI_GRAPHICS_INFO_HOB         *GfxInfoHob;

  // If FrameBufferSize is MAX_UINT32, try to parse the PCI bar to get the size
  if (FrameBufferSize == MAX_UINT32) {
    GfxDevHob = (EFI_PEI_GRAPHICS_DEVICE_INFO_HOB *)GetGuidHobData (NULL, NULL, &gEfiGraphicsDeviceInfoHobGuid);
    if (GfxDevHob == NULL) {
      DEBUG ((DEBUG_VERBOSE, "Failed to find GFX device info HOB\n"));
      return EFI_NOT_FOUND;
    }
    if (GfxDevHob->BarIndex >= PCI_MAX_BAR) {
      DEBUG ((DEBUG_VERBOSE, "Invalid GFX device PCI bar index\n"));
      return EFI_UNSUPPORTED;
    }
    GfxPciBase = GetDeviceAddr (PlatformDeviceGraphics, 0);
    if (GfxPciBase == 0) {
      DEBUG ((DEBUG_VERBOSE, "Failed to find GFX device from platform device table\n"));
      return EFI_NOT_FOUND;
    }

    // Parse the PCI bar size
    // Only need to parse lower 32bit since Framebuffer is less than 4GB
    GfxPciBase = TO_MM_PCI_ADDRESS (GfxPciBase);
    VidDid = MmioRead32 (GfxPciBase);
    if (VidDid != (UINT32)((GfxDevHob->DeviceId << 16) + GfxDevHob->VendorId)) {
      DEBUG ((DEBUG_VERBOSE, "GFX device VID/DID mismatch\n"));
      return EFI_NOT_FOUND;
    }

    // Detect logic bar index
    BarIndex = 0;
    Found = FALSE;
    for (Offset = 0x10, BarIndex = 0; Offset <= 0x24 && BarIndex < PCI_MAX_BAR; BarIndex++) {
      if (BarIndex == GfxDevHob->BarIndex) {
        Found = TRUE;
        break;
      }
      BarVal = MmioRead32 (GfxPciBase + BarIndex);
      if (BarVal > 0) {
        if (((BarVal & BIT0) == 0) && ((BarVal & BIT2) != 0)) {
          // 64bit bar
          Offset += 4;
        }
      }
      Offset += 4;
    }
    if (!Found) {
      DEBUG ((DEBUG_VERBOSE, "GFX bar index is invalid\n"));
      return EFI_NOT_FOUND;
    }

    CmdVal = MmioRead8  (GfxPciBase + PCI_COMMAND_OFFSET);
    BarVal = MmioRead32 (GfxPciBase + Offset);
    MmioWrite8  (GfxPciBase + PCI_COMMAND_OFFSET, 0);
    MmioWrite32 (GfxPciBase + Offset, MAX_UINT32);
    Data = MmioRead32 (GfxPciBase + Offset) & ~(SIZE_4KB - 1);
    MmioWrite32 (GfxPciBase + Offset, BarVal);
    MmioWrite8  (GfxPciBase + PCI_COMMAND_OFFSET, CmdVal);
    DEBUG ((DEBUG_VERBOSE, "GFX BAR old value 0x%08x, new value 0x%08x\n", BarVal, Data));

    // Calculate bar size
    SizeBit = LowBitSet32 (Data);
    if (SizeBit < 0) {
      // The size is greater than 4GB
      DEBUG ((DEBUG_VERBOSE, "GFX BAR size is greater than 4GB\n"));
      return EFI_UNSUPPORTED;
    }
    FrameBufferSize = 1 << SizeBit;
    DEBUG ((DEBUG_VERBOSE, "GFX framebuffer BAR size 0x%08x @ BAR%d\n", FrameBufferSize, GfxDevHob->BarIndex));
  }

  // If FrameBufferBase or FrameBufferSize is 0, try to use Framebuffer HOB to get base and size.
  if ((FrameBufferBase == 0) || (FrameBufferSize == 0)) {
    GfxInfoHob = (EFI_PEI_GRAPHICS_INFO_HOB *)GetGuidHobData (NULL, NULL, &gEfiGraphicsInfoHobGuid);
    if (GfxInfoHob == NULL) {
      DEBUG ((DEBUG_VERBOSE, "Failed to find GFX info HOB\n"));
      return EFI_NOT_FOUND;
    }
    if (FrameBufferBase == 0) {
      FrameBufferBase = GfxInfoHob->FrameBufferBase;
    }
    if (FrameBufferSize == 0) {
      FrameBufferSize = GfxInfoHob->FrameBufferSize;
    }
    DEBUG ((DEBUG_VERBOSE, "Use framebuffer 0x%llx, size 0x%x\n", FrameBufferBase, FrameBufferSize));
  }

  // FrameBufferSize needs to be power of 2, if not, round up
  SizeBit = HighBitSet32 (FrameBufferSize);
  if (FrameBufferSize != LShiftU64 (1, SizeBit)) {
    SizeBit++;
    FbSize = LShiftU64 (1, SizeBit);
  } else {
    FbSize = FrameBufferSize;
  }

  // FrameBufferBase needs to be aligned at the size, at least 4KB
  BaseBit = LowBitSet64 (FrameBufferBase);
  if ((SizeBit < 12) || (SizeBit > BaseBit)) {
    DEBUG ((DEBUG_VERBOSE, "Framebuffer base and size are not aligned\n"));
    return EFI_UNSUPPORTED;
  }

  // Find a free MTRR pair
  MsrCap.Uint64 = AsmReadMsr64 (MSR_IA32_MTRRCAP);
  MsrMax = MSR_IA32_MTRR_PHYSBASE0 + 2 * MsrCap.Bits.VCNT;
  for (MsrIdx = MSR_IA32_MTRR_PHYSBASE0; MsrIdx < MsrMax; MsrIdx += 2) {
     MsrMask.Uint64 = AsmReadMsr64 (MsrIdx + 1);
     if (MsrMask.Bits.V == 0) {
       break;
     }
  }
  if (MsrIdx == MsrMax) {
    DEBUG ((DEBUG_VERBOSE, "Failed to find free MTRR to set GFX cache"));
    return EFI_OUT_OF_RESOURCES;
  }

  // Enable Framebuffer as WC.
  AsmCpuid (CPUID_VIR_PHY_ADDRESS_SIZE, &VirPhyAddressSize.Uint32, NULL, NULL, NULL);
  MsrBase.Uint64    = FrameBufferBase;
  MsrBase.Bits.Type = MTRR_CACHE_WRITE_COMBINING;
  AsmWriteMsr64 (MsrIdx,  MsrBase.Uint64);
  MsrMask.Uint64  = (~(FbSize - 1)) & (LShiftU64 (1, VirPhyAddressSize.Bits.PhysicalAddressBits) - 1);
  MsrMask.Bits.V  = 1;
  AsmWriteMsr64 (MsrIdx + 1, MsrMask.Uint64);

  DEBUG ((DEBUG_INFO, "GFX framebuffer WC is enabled @ 0x%llx:0x%llx\n", FrameBufferBase, FbSize));
  return EFI_SUCCESS;
}
