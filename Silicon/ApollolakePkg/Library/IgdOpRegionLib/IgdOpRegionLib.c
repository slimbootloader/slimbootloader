/** @file

  Copyright (c) 1999 - 2014, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

Module Name:
  IgdOpRegionLib.c
Abstract:
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system BIOS, ASL code, and Graphics drivers.
  The code in this file will load the driver and initialize the interface
  Supporting Specification: OpRegion / Software SCI SPEC 0.70
  Acronyms:
    IGD:        Internal Graphics Device
    NVS:        ACPI Non Volatile Storage
    OpRegion:   ACPI Operational Region
    VBT:        Video BIOS Table (OEM customizable data)
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
#include <GlobalNvsArea.h>

UINT8 gSVER[12] = "Intel";
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
GetFvVbt (
  OUT VBIOS_VBT_STRUCTURE **VbtFileBuffer
  )
{
  EFI_PHYSICAL_ADDRESS     VbtAddress = 0;

  // Get the vbt address
  VbtAddress = PcdGet32(PcdGraphicsVbtAddress);
  DEBUG ((DEBUG_INFO, "VbtAddress =0x%x \n", VbtAddress));

  ///
  /// Check VBT signature
  ///
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

  Update VBT data in Igd Op Region.

  @param[out] VbtFileBuffer    Pointer to VBT data buffer.

  @retval    EFI_SUCCESS      VBT data was returned.
  @exception EFI_UNSUPPORTED  Invalid signature in VBT data.

**/
EFI_STATUS
GetVbt()
{
  VBIOS_VBT_STRUCTURE           *VbtFileBuffer;
  VBIOS_VBT_STRUCTURE           *ExtVbtPtr;
  UINT32                         ExtVbtSize;

  ExtVbtPtr     = NULL;
  VbtFileBuffer = NULL;
  GetFvVbt (&VbtFileBuffer);
  if (VbtFileBuffer != NULL) {
      DEBUG ((DEBUG_INFO, "VBT data found\n"));
      DEBUG ((DEBUG_INFO, "VbtFileBuffer->HeaderVbtSize = 0x%x \n", VbtFileBuffer->HeaderVbtSize));
      mIgdOpRegion.OpRegion->MBox3.Rvda = 0;
      mIgdOpRegion.OpRegion->MBox3.Rvds = 0;
      ///
      /// extended VBT support
      ///
      if (VbtFileBuffer->HeaderVbtSize > 6*SIZE_1KB){
        DEBUG ((DEBUG_INFO, "extended VBT supported\n"));
        ExtVbtSize =  mIgdOpRegion.OpRegion->MBox3.Rvds;
        if (VbtFileBuffer->HeaderVbtSize > ExtVbtSize) {
          ExtVbtPtr = AllocateZeroPool(VbtFileBuffer->HeaderVbtSize);
          mIgdOpRegion.OpRegion->MBox3.Rvda = (UINT64)(UINTN)ExtVbtPtr;
        }
        mIgdOpRegion.OpRegion->MBox3.Rvds = VbtFileBuffer->HeaderVbtSize;
        CopyMem((VOID *)(UINTN)mIgdOpRegion.OpRegion->MBox3.Rvda, (VOID *)(UINTN)VbtFileBuffer, mIgdOpRegion.OpRegion->MBox3.Rvds);
      } else {
        DEBUG ((DEBUG_INFO, "Not extended VBT\n"));
        // Raw Vbt Size <= 6k
        CopyMem(mIgdOpRegion.OpRegion->Vbt.Gvd1, VbtFileBuffer, VbtFileBuffer->HeaderVbtSize);
        mIgdOpRegion.OpRegion->MBox3.Rvda = (UINT64)(UINTN)(mIgdOpRegion.OpRegion->Vbt.Gvd1);
        mIgdOpRegion.OpRegion->MBox3.Rvds = VbtFileBuffer->HeaderVbtSize;
      } // End of Size condition

      return EFI_SUCCESS;
  } else {
    ///
    /// Intel vbt not found.
    ///
    DEBUG ((DEBUG_INFO, "Intel VBT not found\n"));
    return EFI_ABORTED;
  }
}

/**
  Graphics OpRegion / Software SCI driver installation function.

  @param ImageHandle     Handle for this drivers loaded image protocol.
  @param SystemTable     EFI system table.

  @retval EFI_SUCCESS    The driver installed without error.
  @retval EFI_ABORTED    The driver encountered an error and could not complete
                         installation of the ACPI tables.

**/
EFI_STATUS
IgdOpRegionInit (
  VOID
)
{
  GLOBAL_NVS_AREA              *Gnvs;
  UINT16                        Data16;
  EFI_STATUS                    Status = EFI_ABORTED;

  if (mIgdOpRegion.OpRegion == NULL) {
    mIgdOpRegion.OpRegion = (IGD_OPREGION_STRUC *) AllocateZeroPool (sizeof(IGD_OPREGION_STRUC));

    //
    // Update OpRegion address to Gnvs
    //
    Gnvs = (GLOBAL_NVS_AREA *)(UINTN)PcdGet32(PcdAcpiGnvsAddress);
    Gnvs->PlatformNvs.IgdOpRegionAddress = (UINT32)(UINTN)(mIgdOpRegion.OpRegion);

    //
    // Initialize OpRegion Header
    //
    CopyMem (mIgdOpRegion.OpRegion->Header.Sign, HEADER_SIGNATURE, sizeof(HEADER_SIGNATURE));

    //
    // Set OpRegion Size in KBs
    //
    mIgdOpRegion.OpRegion->Header.Size = HEADER_SIZE/1024;
    mIgdOpRegion.OpRegion->Header.Over = (UINT32) (LShiftU64 (HEADER_OPREGION_VER, 16) + LShiftU64 (HEADER_OPREGION_REV, 8));
    CopyMem(mIgdOpRegion.OpRegion->Header.Sver, gSVER, sizeof(gSVER));
    DEBUG ((DEBUG_INFO, "System BIOS ID is %a\n", mIgdOpRegion.OpRegion->Header.Sver));

    //
    // All Mailboxes are supported.
    //
    mIgdOpRegion.OpRegion->Header.MBox = HEADER_MBOX_SUPPORT;

    //
    // Initialize OpRegion Mailbox 1 (Public ACPI Methods).
    //
    // Note - The initial setting of mailbox 1 fields is implementation specific.
    // Adjust them as needed many even coming from user setting in setup.
    //
    // Workaround to solve LVDS is off after entering OS in desktop platform
    //
    mIgdOpRegion.OpRegion->MBox1.Clid = 1;

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
    mIgdOpRegion.OpRegion->MBox3.Bclp = BACKLIGHT_BRIGHTNESS;
    mIgdOpRegion.OpRegion->MBox3.Pfit = FIELD_VALID_BIT | PFIT_STRETCH; // Default to stretched
    mIgdOpRegion.OpRegion->MBox3.Pcft = 0; /* should be (IMON << 1) & 0x3e */
    mIgdOpRegion.OpRegion->MBox3.Cblv = FIELD_VALID_BIT | INIT_BRIGHT_LEVEL;
    mIgdOpRegion.OpRegion->MBox3.Bclm[0] = WORD_FIELD_VALID_BIT + 0x0000;
    mIgdOpRegion.OpRegion->MBox3.Bclm[1] = WORD_FIELD_VALID_BIT + 0x0a19;
    mIgdOpRegion.OpRegion->MBox3.Bclm[2] = WORD_FIELD_VALID_BIT + 0x1433;
    mIgdOpRegion.OpRegion->MBox3.Bclm[3] = WORD_FIELD_VALID_BIT + 0x1e4c;
    mIgdOpRegion.OpRegion->MBox3.Bclm[4] = WORD_FIELD_VALID_BIT + 0x2866;
    mIgdOpRegion.OpRegion->MBox3.Bclm[5] = WORD_FIELD_VALID_BIT + 0x327f;
    mIgdOpRegion.OpRegion->MBox3.Bclm[6] = WORD_FIELD_VALID_BIT + 0x3c99;
    mIgdOpRegion.OpRegion->MBox3.Bclm[7] = WORD_FIELD_VALID_BIT + 0x46b2;
    mIgdOpRegion.OpRegion->MBox3.Bclm[8] = WORD_FIELD_VALID_BIT + 0x50cc;
    mIgdOpRegion.OpRegion->MBox3.Bclm[9] = WORD_FIELD_VALID_BIT + 0x5ae5;
    mIgdOpRegion.OpRegion->MBox3.Bclm[10] = WORD_FIELD_VALID_BIT + 0x64ff;
  }

  Status = GetVbt ();
  if (EFI_ERROR (Status)) {
    return Status;
  }
  /*
   * Initialize hardware state:
   * Set ASLS Register to the OpRegion physical memory address.
   * Set SWSCI register bit 15 to a "1" to activate SCI interrupts.
   *
   * TODO This needs to happen in S3 resume, too.
   * Maybe it should move to the finalize handler
  */
  PciWrite32(PCI_LIB_ADDRESS(IGD_BUS, IGD_DEV, IGD_FUN_0, IGD_ASLS_OFFSET), (UINT32)(UINTN)(mIgdOpRegion.OpRegion));
  Data16 = PciRead16(PCI_LIB_ADDRESS(IGD_BUS, IGD_DEV, IGD_FUN_0, IGD_SWSCI_OFFSET));
  Data16 &= ~BIT0;
  Data16 |= BIT15;
  PciWrite16(PCI_LIB_ADDRESS(IGD_BUS, IGD_DEV, IGD_FUN_0, IGD_SWSCI_OFFSET), Data16);

  DEBUG ((DEBUG_INFO, "IgdOpRegion ended\n"));
  return Status;
}
