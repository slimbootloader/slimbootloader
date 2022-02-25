/** @file
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system Bootloader, ASL code, and Graphics drivers.
  The code in this file will load the driver and initialize the interface
  Supporting Specification: OpRegion / Software SCI SPEC 0.70

  Acronyms:
    IGD:        Internal Graphics Device
    OpRegion:   ACPI Operational Region
    VBT:        Video BIOS Table (OEM customizable data)
    IPU:        Image Processing Unit

  Copyright (c) 2021 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/IgdOpRegionLib.h>
#include <GopConfig.h>
#include "IgdOpRegion.h"
#include <IgdOpRegionDefines.h>

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
This function will update the VBT checksum.

@param[in out] VbtPtr - Pointer to VBT table

@retval none
**/
VOID
UpdateVbtChecksum(
  VBT_TABLE_DATA *VbtPtr
  )
{
  UINT8           Checksum;
  UINT8           *VbtStartAddress;
  UINT8           *VbtEndAddress;

  VbtStartAddress = (UINT8 *)(UINTN)VbtPtr;
  VbtEndAddress = VbtStartAddress + (VbtPtr->VbtHeader.Table_Size);

  Checksum = 0;

  //
  // Compute the checksum
  //
  while (VbtStartAddress != VbtEndAddress) {
    Checksum = Checksum + (*VbtStartAddress);
    VbtStartAddress = VbtStartAddress + 1;
  }
  Checksum = Checksum - VbtPtr->VbtHeader.Checksum;
  Checksum = (UINT8)(0x100 - Checksum);

  //
  // Update the checksum
  //
  VbtPtr->VbtHeader.Checksum = Checksum;

}

/**
  Locate a VBT block within VBT bianry by its block ID.

  @param[in] VbtBuf     VBT binary buffer pointer.
  @param[in] BlockId    A VBT block ID to locate.

  @retval    NULL       Not found.
             Others     The pointer to the block header.
**/
UINT8 *
EFIAPI
LocateVbtBlockById (
  IN  UINT8     *VbtBuf,
  IN  UINT8      BlockId
  )
{
  VBT_HEADER               *VbtHdr;
  VBT_BIOS_DATA_HEADER     *BiosDataHdr;
  VBT_BLOCK_COMMON_HEADER  *BlkHdr;
  UINT32                    Offset;

  if ((VbtBuf != NULL) && (*(UINT32 *)VbtBuf == VBT_SIGNATURE)) {
    VbtHdr = (VBT_HEADER *)VbtBuf;
    Offset = VbtHdr->Bios_Data_Offset;
    BiosDataHdr = (VBT_BIOS_DATA_HEADER *)(VbtBuf + Offset);
    Offset = Offset + BiosDataHdr->BDB_Header_Size;
    while (Offset < BiosDataHdr->BDB_Size) {
      BlkHdr = (VBT_BLOCK_COMMON_HEADER *)(VbtBuf + Offset);
      if (BlkHdr->BlockId == BlockId) {
        return (UINT8 *)BlkHdr;
      }
      Offset += (BlkHdr->BlockSize + sizeof(VBT_BLOCK_COMMON_HEADER));
    }
  }

  return NULL;
}

/**
  Update VBT data in Igd Op Region.

  @param[in] IsExtendedVbt          Is an extended VBT being used
  @param[in] GopVbtUpdateCallback   A callback routine for platform to update VBT.

  @retval    EFI_SUCCESS      VBT data was returned.
  @exception EFI_ABORTED      Intel VBT not found.
**/
EFI_STATUS
UpdateVbt (
  BOOLEAN                   IsExtendedVbt,
  GOP_VBT_UPDATE_CALLBACK   GopVbtUpdateCallback
  )
{
  VBT_TABLE_DATA                    *VbtPtr;
  VBT_GENERAL2_INFO                 *VbtGen2Info;
  CHILD_STRUCT                      *ChildStructPtr[ChildStruct_MAX];
  UINTN                             Index;

  if (mIgdOpRegion.OpRegion == NULL) {
    return EFI_NOT_FOUND;
  }

  if (IsExtendedVbt) {
    VbtPtr = (VBT_TABLE_DATA*) (((UINTN)mIgdOpRegion.OpRegion) + sizeof (IGD_OPREGION_STRUCTURE));
  } else {
    VbtPtr = (VBT_TABLE_DATA*) &(mIgdOpRegion.OpRegion->MBox4);
  }

  if (VbtPtr != NULL) {
    DEBUG ((DEBUG_INFO, "VBT data found '%.20a'\n", VbtPtr->VbtHeader.Product_String));
    DEBUG((DEBUG_INFO, "Updating GOP VBT fields using GOP Configuration Lib.\n"));
    VbtGen2Info = (VBT_GENERAL2_INFO *) LocateVbtBlockById ((UINT8 *)VbtPtr, VBT_BLK_ID_GENERAL2_INFO);
    if (VbtGen2Info != NULL) {
      for (Index = 0; Index < ChildStruct_MAX; Index++) {
        ChildStructPtr[Index] = (CHILD_STRUCT *)(((UINT8 *)VbtGen2Info->Child_Struct) + Index * VbtGen2Info->SizeChild_Struct);
      }

      if (GopVbtUpdateCallback != NULL) {
        GopVbtUpdateCallback (ChildStructPtr);
      }

      DEBUG((DEBUG_INFO, "VBT Updated.\n"));
      UpdateVbtChecksum (VbtPtr);
    }
    return EFI_SUCCESS;
  } else {
    DEBUG ((DEBUG_INFO, "Intel VBT not found\n"));
    return EFI_ABORTED;
  }

}

/**
  Graphics OpRegion / Software SCI driver installation function.

  @param[in] GopVbtUpdateCallback   A callback routine for platform to update VBT.

  @retval EFI_SUCCESS     The driver installed without error.
  @retval EFI_ABORTED     The driver encountered an error and could not complete
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
  EFI_STATUS                    Status;
  VBIOS_VBT_STRUCTURE           *VbtFileBuffer;
  GOP_VBT_UPDATE_CALLBACK       VbtCallback;
  UINT8                         TurboIMON;

  Status = EFI_ABORTED;
  VbtFileBuffer = NULL;
  ExtendedVbtSize = 0;
  VbtCallback = NULL;

  GetVbt (&VbtFileBuffer);
  if (VbtFileBuffer == NULL) {
    return EFI_UNSUPPORTED;
  }

  if ((VbtFileBuffer->HeaderVbtSize) > (6 * SIZE_1KB)) {
    DEBUG ((DEBUG_INFO, "Extended VBT supported\n"));
    ExtendedVbtSize = ((VbtFileBuffer->HeaderVbtSize) & (UINT32)~(0x1FF)) + 0x200;
  }

  mIgdOpRegion.OpRegion = (IGD_OPREGION_STRUCTURE *) AllocatePool (sizeof(IGD_OPREGION_STRUCTURE) + ExtendedVbtSize);
  if (mIgdOpRegion.OpRegion == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  SetMem(mIgdOpRegion.OpRegion, sizeof(IGD_OPREGION_STRUCTURE) + ExtendedVbtSize, 0);

  // Update OpRegion address to Gnvs
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
  mIgdOpRegion.OpRegion->Header.OVER = (UINT32)(LShiftU64(HEADER_OPREGION_VER_GEN12, 16) + LShiftU64(HEADER_OPREGION_REV, 8));

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
  mIgdOpRegion.OpRegion->Header.PCON = 0x3;

  mIgdOpRegion.OpRegion->MBox3.BCLP = BACKLIGHT_BRIGHTNESS;

  mIgdOpRegion.OpRegion->MBox3.PFIT = (FIELD_VALID_BIT | PFIT_STRETCH);

  //
  // Reporting to driver for VR IMON Calibration. Bits [5-1] values supported 14A to 31A.
  //
  TurboIMON = (IgdPlatformInfo != NULL) ? IgdPlatformInfo->TurboIMON : 0x1F;
  mIgdOpRegion.OpRegion->MBox3.PCFT = (TurboIMON << 1) & 0x003E;

  ///
  /// Set Initial current Brightness
  ///
  mIgdOpRegion.OpRegion->MBox3.CBLV = (INIT_BRIGHT_LEVEL | FIELD_VALID_BIT);

  // <EXAMPLE> Create a static Backlight Brightness Level Duty cycle Mapping Table
  // Possible 20 entries (example used 11), each 16 bits as follows:
  // [15] = Field Valid bit, [14:08] = Level in Percentage (0-64h), [07:00] = Desired duty cycle (0 - FFh).

  mIgdOpRegion.OpRegion->MBox3.BCLM[0] = (0x0000 + WORD_FIELD_VALID_BIT);  ///< 0%
  mIgdOpRegion.OpRegion->MBox3.BCLM[1] = (0x0A19 + WORD_FIELD_VALID_BIT);  ///< 10%
  mIgdOpRegion.OpRegion->MBox3.BCLM[2] = (0x1433 + WORD_FIELD_VALID_BIT);  ///< 20%
  mIgdOpRegion.OpRegion->MBox3.BCLM[3] = (0x1E4C + WORD_FIELD_VALID_BIT);  ///< 30%
  mIgdOpRegion.OpRegion->MBox3.BCLM[4] = (0x2866 + WORD_FIELD_VALID_BIT);  ///< 40%
  mIgdOpRegion.OpRegion->MBox3.BCLM[5] = (0x327F + WORD_FIELD_VALID_BIT);  ///< 50%
  mIgdOpRegion.OpRegion->MBox3.BCLM[6] = (0x3C99 + WORD_FIELD_VALID_BIT);  ///< 60%
  mIgdOpRegion.OpRegion->MBox3.BCLM[7] = (0x46B2 + WORD_FIELD_VALID_BIT);  ///< 70%
  mIgdOpRegion.OpRegion->MBox3.BCLM[8] = (0x50CC + WORD_FIELD_VALID_BIT);  ///< 80%
  mIgdOpRegion.OpRegion->MBox3.BCLM[9] = (0x5AE5 + WORD_FIELD_VALID_BIT);  ///< 90%
  mIgdOpRegion.OpRegion->MBox3.BCLM[10] = (0x64FF + WORD_FIELD_VALID_BIT);  ///< 100%

  mIgdOpRegion.OpRegion->MBox3.IUER = 0x00;

  if (ExtendedVbtSize > 0) {
    mIgdOpRegion.OpRegion->MBox3.RVDA = sizeof (IGD_OPREGION_STRUCTURE); // Relative offset at the end of Op-region.
    mIgdOpRegion.OpRegion->MBox3.RVDS = ExtendedVbtSize; // Aligned VBT Data Size to 512 bytes.
    CopyMem ((CHAR8 *)(UINTN)(mIgdOpRegion.OpRegion) + sizeof (IGD_OPREGION_STRUCTURE), VbtFileBuffer, ExtendedVbtSize);
  } else {
    mIgdOpRegion.OpRegion->MBox3.RVDA = 0;
    mIgdOpRegion.OpRegion->MBox3.RVDS = 0;
    CopyMem (mIgdOpRegion.OpRegion->MBox4.RVBT, VbtFileBuffer, VbtFileBuffer->HeaderVbtSize);
  }

  VbtCallback = NULL;
  if (IgdPlatformInfo != NULL) {
    VbtCallback = IgdPlatformInfo->callback;
  }
  Status = UpdateVbt (ExtendedVbtSize > 0, VbtCallback);

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
