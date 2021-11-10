/** @file
  This file contains graphics initialization for QEMU

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <IndustryStandard/Pci30.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/GraphicsInitLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/OsBootOptionGuid.h>
#include <IndustryStandard/Bmp.h>
#include "EmuGfxInitLib.h"

#define  BPP     4

EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE   *mMode;
EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE    mModeData;
EFI_GRAPHICS_OUTPUT_MODE_INFORMATION mModeInfo;
QEMU_VIDEO_PRIVATE_DATA              mPrivate;

CONST QEMU_VIDEO_BOCHS_MODES mQemuVideoBochsModes[] = {
  {  640,  480 },
  {  800,  600 },
  { 1024,  768 },
};

/**
  Display I/O port write

  @param[in] Private              Private data structure pointer.
  @param[in] Reg                  Register offset.
  @param[in] Data                 Register data.

**/
VOID
BochsWrite (
  QEMU_VIDEO_PRIVATE_DATA  *Private,
  UINT16                   Reg,
  UINT16                   Data
  )
{
  if (Private->Bar2 == 0) {
    IoWrite16 (VBE_DISPI_IOPORT_INDEX, Reg);
    IoWrite16 (VBE_DISPI_IOPORT_DATA,  Data);
  } else {
    MmioWrite16  (Private->Bar2 + 0x500 + (Reg << 1), Data);
  }
}

/**
  Display I/O port read

  @param[in] Private              Private data structure pointer.
  @param[in] Reg                  Register offset.

  @retval                         Register data.
**/
UINT16
BochsRead (
  QEMU_VIDEO_PRIVATE_DATA  *Private,
  UINT16                   Reg
  )
{
  if (Private->Bar2 == 0) {
    IoWrite16 (VBE_DISPI_IOPORT_INDEX, Reg);
    return IoRead16 (VBE_DISPI_IOPORT_DATA);
  } else {
    return MmioRead16 (Private->Bar2 + 0x500 + (Reg << 1));
  }
}

/**
  VGA I/O port write

  @param[in] Private              Private data structure pointer.
  @param[in] Reg                  Register offset.
  @param[in] Data                 Register data.

**/
VOID
VgaOutb (
  QEMU_VIDEO_PRIVATE_DATA  *Private,
  UINTN                    Reg,
  UINT8                    Data
  )
{
  if (Private->Bar2 == 0) {
    IoWrite8 (Reg, Data);
  } else {
    MmioWrite8  (Private->Bar2 + 0x400 - 0x3c0 + Reg, Data);
  }
}

/**
  Qemu GFX mode initialization

  @param[in] Width      Resolution width.
  @param[in] Height     Resolution height.
  @param[in] Bpp        Bytes per pixel.
  @param[in] GfxDevAddr GFX device address.
  @param[in] PciBase    PCI resource base to use.

  @retval  EFI_SUCCESS      Bochs GFX was initialized successfully.
           EFI_UNSUPPORTED  Not supported graphics controller

**/
EFI_STATUS
BochsInitMode (
  IN  UINT16  Width,
  IN  UINT16  Height,
  IN  UINT8   Bpp,
  IN  UINT32  GfxDevAddr,
  IN  UINT32  PciBarBase
)
{
  QEMU_VIDEO_PRIVATE_DATA  *Private;
  UINT32         Address;
  UINT32         Data;
  UINT8          Bus;
  UINT8          Dev;
  UINT8          Func;

  Bus  = ((GfxDevAddr) >> 16) & 0xFF;
  Dev  = ((GfxDevAddr) >> 8) & 0x1F;
  Func = GfxDevAddr & 0x07;

  Address = PCI_LIB_ADDRESS (Bus, Dev, Func, PCI_VENDOR_ID_OFFSET);
  if ((PciRead32(Address) != QEMU_VGA_VID_DID) && (PciRead32(Address) != QEMU_VGA_VID_DID2)) {
    return EFI_UNSUPPORTED;
  }

  Private = &mPrivate;
  Address = PCI_LIB_ADDRESS (Bus, Dev, Func, PCI_BASE_ADDRESSREG_OFFSET + 0 * 4);
  Data    = PciRead32 (Address) & ~0xF;
  if (Data == 0) {
    // PCI resource is not programmed yet, use temporary resource
    PciWrite32 (Address, PciBarBase);
    PciWrite32 (Address + 2 * 4, PciBarBase + 0x0800000);
    Data = PciBarBase;
  }
  mMode->FrameBufferBase = Data;

  Address = PCI_LIB_ADDRESS (Bus, Dev, Func, PCI_BASE_ADDRESSREG_OFFSET + 2 * 4);
  mPrivate.Bar2 = PciRead32 (Address) & ~0xF;

  Address = PCI_LIB_ADDRESS (Bus, Dev, Func, PCI_COMMAND_OFFSET);
  PciWrite8 (Address, EFI_PCI_COMMAND_BUS_MASTER | EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_IO_SPACE);
  BochsWrite (Private, VBE_DISPI_INDEX_ENABLE,      0);
  BochsWrite (Private, VBE_DISPI_INDEX_BANK,        0);
  BochsWrite (Private, VBE_DISPI_INDEX_X_OFFSET,    0);
  BochsWrite (Private, VBE_DISPI_INDEX_Y_OFFSET,    0);
  BochsWrite (Private, VBE_DISPI_INDEX_BPP,         Bpp * 8);
  BochsWrite (Private, VBE_DISPI_INDEX_XRES,        Width);
  BochsWrite (Private, VBE_DISPI_INDEX_VIRT_WIDTH,  Width);
  BochsWrite (Private, VBE_DISPI_INDEX_YRES,        Height);
  BochsWrite (Private, VBE_DISPI_INDEX_VIRT_HEIGHT, Height);
  BochsWrite (Private, VBE_DISPI_INDEX_ENABLE,      VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

  VgaOutb    (Private, ATT_ADDRESS_REGISTER,     0x20);

  return EFI_SUCCESS;
}


/*
  This function performs the PEI module initialization.

  @param[in]   GraphicsPolicyPtr   Pointer to the graphics policy structure.
  @param[out]  GfxInfoHob          Poniter to receive the GFX info HOB.

  @retval   EFI_SUCCESS       If initialization is successful.
  @retval   EFI_DEVICE_ERROR  If initialization fails.
**/
EFI_STATUS
EFIAPI
GraphicsInit (
  IN  UINT32                       GfxDevPciBase,
  IN  GRAPHICS_INIT_POLICY        *GfxPolicy
  )
{
  EFI_STATUS     Status;
  UINT16         ResX;
  UINT16         ResY;
  UINTN          Index;
  UINTN          GfxMode;

  if ((GfxDevPciBase == 0) || (GfxPolicy == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "[INFO]:[EmuGraphicsInit]: \n"));

  GfxMode = 1;
  for (Index = 0; Index < ARRAY_SIZE(mQemuVideoBochsModes); Index++) {
    if ((mQemuVideoBochsModes[Index].ResX == GfxPolicy->DisaplayTimingInfo.Hdisp) &&
        (mQemuVideoBochsModes[Index].ResY == GfxPolicy->DisaplayTimingInfo.Vdisp)) {
      GfxMode = Index;
      break;
    }
  }

  ResX = (UINT16)mQemuVideoBochsModes[GfxMode].ResX;
  ResY = (UINT16)mQemuVideoBochsModes[GfxMode].ResY;

  mMode = &mModeData;
  mMode->Info = &mModeInfo;

  mMode->Info->Version              = 0;
  mMode->Info->PixelFormat          = PixelBlueGreenRedReserved8BitPerColor;
  mMode->SizeOfInfo                 = sizeof (EFI_GRAPHICS_OUTPUT_MODE_INFORMATION);
  mMode->FrameBufferBase            = 0;
  mMode->Info->HorizontalResolution = ResX;
  mMode->Info->VerticalResolution   = ResY;
  mMode->Info->PixelsPerScanLine    = (((ResX * BPP) + 63) & 0xFFFFFFC0) / BPP;
  mMode->FrameBufferSize            = mMode->Info->PixelsPerScanLine * ResY * BPP;
  mMode->MaxMode                    = 1;
  mMode->Mode                       = 0;

  Status = BochsInitMode (ResX, ResY, BPP, GfxDevPciBase, GfxPolicy->PciTempResourceBase);
  if (!EFI_ERROR(Status)) {
    // Print Mode information recived from GraphicsPeim
    DEBUG ((DEBUG_INFO, "  FrameBufferBase     : 0x%x\n", mMode->FrameBufferBase));
    DEBUG ((DEBUG_INFO, "  HorizontalResolution: %d\n", mMode->Info->HorizontalResolution));
    DEBUG ((DEBUG_INFO, "  VerticalResolution  : %d\n", mMode->Info->VerticalResolution));
  }

  return Status;
}


/**
  Get GOP MODE Info

  @param[out] Mode          EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE data pointer to be updated

  @retval  EFI_SUCCESS             Getting GOP MODE Info Successfully
  @retval  EFI_INVALID_PARAMETER   Mode pointer is NULL
  @retval  EFI_NOT_FOUND           No valid mode was found

**/
EFI_STATUS
EFIAPI
GetGraphicOutputModeInfo (
  OUT EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE *Mode
  )
{
  if (Mode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mMode->MaxMode == 0) {
    return EFI_NOT_FOUND;
  }

  CopyMem (Mode, mMode, sizeof(EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE));

  return EFI_SUCCESS;
}
