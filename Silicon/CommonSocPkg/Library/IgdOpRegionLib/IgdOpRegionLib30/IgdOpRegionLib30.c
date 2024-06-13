/**@file
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system Bootloader, ASL code, and Graphics drivers.
  The code in this file will load the driver and initialize the interface
  Supporting Specification: OpRegion / Software SCI SPEC 0.70

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <PiPei.h>

#include <Library/IgdOpRegionLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>

#include "IgdOpRegion30.h"
#include <IgdOpRegionDefines30.h>

//
// Global variables
//

IGD_OPREGION_PROTOCOL mIgdOpRegion;

/**

  Get VBT data.

  @param[out] VbtFileBuffer    Pointer to VBT data buffer.

  @retval    EFI_SUCCESS      VBT data was returned.
  @exception EFI_UNSUPPORTED  Invalid signature in VBT data.

**/
EFI_STATUS
GetVbt (
  OUT VBIOS_VBT_STRUCTURE **VbtFileBuffer
  )
{
  EFI_PHYSICAL_ADDRESS     VbtAddress = 0;

  // Get the vbt address
  VbtAddress = PcdGet32(PcdGraphicsVbtAddress);
  DEBUG ((DEBUG_INFO, "VbtAddress =0x%x \n", VbtAddress));

  // Check VBT signature
  *VbtFileBuffer = (VBIOS_VBT_STRUCTURE *) (UINTN) VbtAddress;
  if (*VbtFileBuffer != NULL) {
    if ((*((UINT32 *) ((*VbtFileBuffer)->HeaderSignature))) != VBT_SIGNATURE) {
      if (*VbtFileBuffer != NULL) {
        *VbtFileBuffer = NULL;
      }
      return EFI_UNSUPPORTED;
    }
  }

  return EFI_SUCCESS;
}

/**
  Graphics OpRegion / Software SCI driver installation function.

  @param[in] void              - None
  @retval EFI_SUCCESS          - The driver installed without error.
  @retval EFI_OUT_OF_RESOURCES - Could no allocate space for the IGD Op Region
  @retval EFI_ABORTED          - The driver encountered an error and could not complete
                                 installation of the ACPI tables.
**/
EFI_STATUS
EFIAPI
IgdOpRegionInit (
  IN IGD_OP_PLATFORM_INFO    *IgdPlatformInfo
  )
{
  UINT16                        Data16;
  UINT16                        ExtendedVbtSize;
  VBIOS_VBT_STRUCTURE           *VbtFileBuffer;
  EFI_STATUS                    Status;
  UINT32 localBcm1[30] = {0x80000000,
                            0x800A0019,
                            0x80140033,
                            0x801E004C,
                            0x80280066,
                            0x8032007F,
                            0x803C0099,
                            0x804600B2,
                            0x805000CC,
                            0x805A00E5,
                            0x806400FF,
                            0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00};
  UINT32 localBcm2[30] = {0x80000000,
                            0x800A0019,
                            0x80140033,
                            0x801E004C,
                            0x80280066,
                            0x8032007F,
                            0x803C0099,
                            0x804600B2,
                            0x805000CC,
                            0x805A00E5,
                            0x806400FF,
                            0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00,0x00,
                            0x00,0x00,0x00};

  VbtFileBuffer = NULL;
  ExtendedVbtSize = 0;

  GetVbt (&VbtFileBuffer);
  if (VbtFileBuffer == NULL) {
    return EFI_UNSUPPORTED;
  }

  if ((VbtFileBuffer->HeaderVbtSize) > (6 * SIZE_1KB)) {
    DEBUG ((DEBUG_INFO, "Extended VBT supported\n"));
    ExtendedVbtSize = ((VbtFileBuffer->HeaderVbtSize) & (UINT32)~(0x1FF)) + 0x200;
  }

  mIgdOpRegion.OpRegion = (IGD_OPREGION_STRUCTURE_VER_3_0 *) AllocatePool (sizeof(IGD_OPREGION_STRUCTURE_VER_3_0) + ExtendedVbtSize);
  if (mIgdOpRegion.OpRegion == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  SetMem(mIgdOpRegion.OpRegion, sizeof(IGD_OPREGION_STRUCTURE_VER_3_0) + ExtendedVbtSize, 0);

  //
  // Update OpRegion address to Gnvs
  //
  Status = PcdSet32S (PcdIgdOpRegionAddress, (UINT32)(UINTN)(mIgdOpRegion.OpRegion));
  //
  // Initialize OpRegion Header
  // Temp comment out  due to booting issue
  //
  CopyMem (mIgdOpRegion.OpRegion->Header.SIGN, HEADER_SIGNATURE, sizeof(HEADER_SIGNATURE));
  //
  // Set OpRegion Size in KBs
  //
  mIgdOpRegion.OpRegion->Header.SIZE = HEADER_SIZE / 1024;
  mIgdOpRegion.OpRegion->Header.OVER = (UINT32)(LShiftU64(HEADER_OPREGION_VER, 16) + LShiftU64(HEADER_OPREGION_REV, 8));
  mIgdOpRegion.OpRegion->Header.DVER[0] = 0x32;
  mIgdOpRegion.OpRegion->Header.DVER[1] = 0x32;
  mIgdOpRegion.OpRegion->Header.DVER[2] = 0x2E;
  mIgdOpRegion.OpRegion->Header.DVER[3] = 0x30;

  //
  // All Mailboxes are supported.
  //
  mIgdOpRegion.OpRegion->Header.MBOX = HEADER_MBOX_SUPPORT;

  //
  // Initialize OpRegion Mailbox 1 (Public ACPI Methods).
  //
  // Note - The initial setting of mailbox 1 fields is implementation specific.
  // Adjust them as needed many even coming from user setting in setup.
  //
  //
  // Initialize OpRegion Mailbox 3 (ASLE Interrupt and Power Conservation).
  //
  // Note - The initial setting of mailbox 3 fields is implementation specific.
  // Adjust them as needed many even coming from user setting in setup.
  //
  //
  // Do not initialize TCHE. This field is written by the graphics driver only.
  //
  //
  // The ALSI field is generally initialized by ASL code by reading the embedded controller.
  //
  mIgdOpRegion.OpRegion->Header.PCON = 0x2;
  mIgdOpRegion.OpRegion->Header.PCON |= BIT8; //Set External Gfx Adapter field is valid
  mIgdOpRegion.OpRegion->Header.PCON &= (UINT32) (~BIT7); //Assume No External Gfx Adapter

  ///
  /// Initialize OpRegion Mailbox 2 (Brightness communication).
  ///
  ///
  /// Set Initial current Brightness
  ///
  mIgdOpRegion.OpRegion->MBox2.BCL1 = BACKLIGHT_BRIGHTNESS;
  mIgdOpRegion.OpRegion->MBox2.BCL2 = BACKLIGHT_BRIGHTNESS;
  mIgdOpRegion.OpRegion->MBox2.CBL1 = (INIT_BRIGHT_LEVEL | FIELD_VALID_BIT);
  mIgdOpRegion.OpRegion->MBox2.CBL2 = (INIT_BRIGHT_LEVEL | FIELD_VALID_BIT);
  ///
  /// Static Backlight Brightness Level Duty cycle Mapping Table
  ///
  for (int Index = 0; Index < 30; Index++) {
    mIgdOpRegion.OpRegion->MBox2.BCM1[Index] = localBcm1[Index];
    mIgdOpRegion.OpRegion->MBox2.BCM2[Index] = localBcm2[Index];
  }

  if (ExtendedVbtSize > 0) {
    mIgdOpRegion.OpRegion->MBox3.RVDA = sizeof (IGD_OPREGION_STRUCTURE_VER_3_0); // Relative offset at the end of Op-region.
    mIgdOpRegion.OpRegion->MBox3.RVDS = ((VbtFileBuffer->HeaderVbtSize) & (UINT32)~(0x1FF)) + 0x200;; // Aligned VBT Data Size to 512 bytes.
    CopyMem ((CHAR8 *)(UINTN)(mIgdOpRegion.OpRegion) + sizeof (IGD_OPREGION_STRUCTURE_VER_3_0), VbtFileBuffer, ExtendedVbtSize);
  } else {
    mIgdOpRegion.OpRegion->MBox3.RVDA = 0;
    mIgdOpRegion.OpRegion->MBox3.RVDS = 0;
    CopyMem (mIgdOpRegion.OpRegion->MBox4.RVBT, VbtFileBuffer, VbtFileBuffer->HeaderVbtSize);
  }

  // Initialize hardware state:
  // Set ASLS Register to the OpRegion physical memory address.
  // Set SWSCI register bit 15 to a "1" to activate SCI interrupts.

  PciWrite32(PCI_LIB_ADDRESS(IGD_BUS, IGD_DEV, IGD_FUN_0, IGD_ASLS_OFFSET), (UINT32)(UINTN)(mIgdOpRegion.OpRegion));

  Data16 = PciRead16(PCI_LIB_ADDRESS(IGD_BUS, IGD_DEV, IGD_FUN_0, IGD_SWSCI_OFFSET));
  Data16 &= ~BIT0;
  Data16 |= BIT15;
  PciWrite16(PCI_LIB_ADDRESS(IGD_BUS, IGD_DEV, IGD_FUN_0, IGD_SWSCI_OFFSET), Data16);

  DEBUG ((DEBUG_INFO, "IgdOpRegion ended\n"));

  return Status;
}
