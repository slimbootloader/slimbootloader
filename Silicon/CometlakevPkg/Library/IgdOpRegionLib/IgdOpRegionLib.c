/** @file
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system Bootloader, ASL code, and Graphics drivers.
  The code in this file will load the driver and initialize the interface
  Supporting Specification: OpRegion / Software SCI SPEC 0.70

  Acronyms:
    IGD:        Internal Graphics Device
    NVS:        ACPI Non Volatile Storage
    OpRegion:   ACPI Operational Region
    VBT:        Video BIOS Table (OEM customizable data)
    IPU:        Image Processing Unit

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
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

#include <IgdOpRegion.h>
#include <IgdOpRegionDefines.h>
#include <GlobalNvsAreaDef.h>

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

    // Check VBT size.
    if ((*VbtFileBuffer)->HeaderVbtSize > 6*SIZE_1KB) {
      (*VbtFileBuffer)->HeaderVbtSize = (UINT16) 6*SIZE_1KB;
    }
  }

  return EFI_SUCCESS;
}

/**
  Update VBT data in Igd Op Region.

  @retval    EFI_SUCCESS      VBT data was returned.
  @exception EFI_ABORTED      Intel VBT not found.
**/
EFI_STATUS
UpdateVbt(
  VOID
  )
{
  VBIOS_VBT_STRUCTURE           *VbtFileBuffer = NULL;

  GetVbt (&VbtFileBuffer);
  if (VbtFileBuffer != NULL) {
    DEBUG ((DEBUG_INFO, "VBT data found\n"));
    DEBUG ((DEBUG_INFO, "VbtFileBuffer->HeaderVbtSize = 0x%x \n", VbtFileBuffer->HeaderVbtSize));

    // Initialize Video BIOS version with its build number.
    mIgdOpRegion.OpRegion->Header.Vver[0] = VbtFileBuffer->CoreBlockBiosBuild[0];
    mIgdOpRegion.OpRegion->Header.Vver[1] = VbtFileBuffer->CoreBlockBiosBuild[1];
    mIgdOpRegion.OpRegion->Header.Vver[2] = VbtFileBuffer->CoreBlockBiosBuild[2];
    mIgdOpRegion.OpRegion->Header.Vver[3] = VbtFileBuffer->CoreBlockBiosBuild[3];
    CopyMem (mIgdOpRegion.OpRegion->Vbt.Gvd1, VbtFileBuffer, VbtFileBuffer->HeaderVbtSize);

    return EFI_SUCCESS;
  } else {
    DEBUG ((DEBUG_INFO, "Intel VBT not found\n"));
    return EFI_ABORTED;
  }

}

/**
  Graphics OpRegion / Software SCI driver installation function.

  @param[in] void         - None
  @retval EFI_SUCCESS     - The driver installed without error.
  @retval EFI_ABORTED     - The driver encountered an error and could not complete
                            installation of the ACPI tables.
**/
EFI_STATUS
IgdOpRegionInit (
  VOID
  )
{
  GLOBAL_NVS_AREA               *Gnvs;
  UINT16                        Data16;
  EFI_STATUS                    Status = EFI_ABORTED;

  mIgdOpRegion.OpRegion = (IGD_OPREGION_STRUC *) AllocatePool (sizeof(IGD_OPREGION_STRUC));

  if (mIgdOpRegion.OpRegion == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SetMem(mIgdOpRegion.OpRegion, sizeof(IGD_OPREGION_STRUC), 0);

  //
  // Update OpRegion address to Gnvs
  //
  Gnvs = (GLOBAL_NVS_AREA *)(UINTN)PcdGet32(PcdAcpiGnvsAddress);
  Gnvs->SaNvs.IgdOpRegionAddress = (UINT32)(UINTN)(mIgdOpRegion.OpRegion);

  //
  // Initialize OpRegion Header
  //
  CopyMem (mIgdOpRegion.OpRegion->Header.Sign, HEADER_SIGNATURE, sizeof(HEADER_SIGNATURE));

  //
  // Set OpRegion Size in KBs
  //
  mIgdOpRegion.OpRegion->Header.Size = HEADER_SIZE/1024;
  mIgdOpRegion.OpRegion->Header.Over = (UINT32) (LShiftU64 (HEADER_OPREGION_VER, 16) + LShiftU64 (HEADER_OPREGION_REV, 8));

  //
  // All Mailboxes are supported.
  //
  mIgdOpRegion.OpRegion->Header.MBox = (HD_MBOX5 + HD_MBOX4 + HD_MBOX3 + HD_MBOX2 + HD_MBOX1);

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
  mIgdOpRegion.OpRegion->Header.PCon = 0x3;

  mIgdOpRegion.OpRegion->MBox3.Bclp = BACKLIGHT_BRIGHTNESS;

  mIgdOpRegion.OpRegion->MBox3.Pfit = (FIELD_VALID_BIT | PFIT_STRETCH);

  //
  // Reporting to driver for VR IMON Calibration. Bits [5-1] values supported 14A to 31A.
  //
  mIgdOpRegion.OpRegion->MBox3.Pcft = (Gnvs->SaNvs.GfxTurboIMON << 1) & 0x003E;

  ///
  /// Set Initial current Brightness
  ///
  mIgdOpRegion.OpRegion->MBox3.Cblv = (INIT_BRIGHT_LEVEL | FIELD_VALID_BIT);

  // <EXAMPLE> Create a static Backlight Brightness Level Duty cycle Mapping Table
  // Possible 20 entries (example used 11), each 16 bits as follows:
  // [15] = Field Valid bit, [14:08] = Level in Percentage (0-64h), [07:00] = Desired duty cycle (0 - FFh).

  mIgdOpRegion.OpRegion->MBox3.Bclm[0] = (0x0000 + WORD_FIELD_VALID_BIT);  ///< 0%
  mIgdOpRegion.OpRegion->MBox3.Bclm[1] = (0x0A19 + WORD_FIELD_VALID_BIT);  ///< 10%
  mIgdOpRegion.OpRegion->MBox3.Bclm[2] = (0x1433 + WORD_FIELD_VALID_BIT);  ///< 20%
  mIgdOpRegion.OpRegion->MBox3.Bclm[3] = (0x1E4C + WORD_FIELD_VALID_BIT);  ///< 30%
  mIgdOpRegion.OpRegion->MBox3.Bclm[4] = (0x2866 + WORD_FIELD_VALID_BIT);  ///< 40%
  mIgdOpRegion.OpRegion->MBox3.Bclm[5] = (0x327F + WORD_FIELD_VALID_BIT);  ///< 50%
  mIgdOpRegion.OpRegion->MBox3.Bclm[6] = (0x3C99 + WORD_FIELD_VALID_BIT);  ///< 60%
  mIgdOpRegion.OpRegion->MBox3.Bclm[7] = (0x46B2 + WORD_FIELD_VALID_BIT);  ///< 70%
  mIgdOpRegion.OpRegion->MBox3.Bclm[8] = (0x50CC + WORD_FIELD_VALID_BIT);  ///< 80%
  mIgdOpRegion.OpRegion->MBox3.Bclm[9] = (0x5AE5 + WORD_FIELD_VALID_BIT);  ///< 90%
  mIgdOpRegion.OpRegion->MBox3.Bclm[10] = (0x64FF + WORD_FIELD_VALID_BIT);  ///< 100%

  mIgdOpRegion.OpRegion->MBox3.Iuer = 0x00;

  Status = UpdateVbt();

  if (EFI_ERROR (Status)) {
    return Status;
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
