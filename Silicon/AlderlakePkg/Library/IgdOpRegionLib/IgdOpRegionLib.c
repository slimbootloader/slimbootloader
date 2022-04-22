/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
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

#include "IgdOpRegion.h"
#include <IgdOpRegionDefines.h>
#include <GlobalNvsAreaDef.h>
#include "GopConfigLib.h"
#include <PlatformBoardId.h>

typedef
VOID
(EFIAPI *GOP_VBT_SPECIFIC_UPDATE) (
  IN CHILD_STRUCT **ChildStructPtr
  );

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
  Null function for GOP VBT update.

  @param[in]  ChildStructPtr  - CHILD_STRUCT

**/
VOID
EFIAPI
AdlGopVbtSpecificUpdateNull (
  IN CHILD_STRUCT **ChildStructPtr
  )
{
  DEBUG ((DEBUG_INFO, "VBT Default Supported, Updating not required\n"));
}

/**
  GOP VBT update for ADL P Ddr5 Edp + DP

  @param[in]  ChildStructPtr  - CHILD_STRUCT

**/
VOID
EFIAPI
AdlPDdr5GopVbtSpecificUpdate (
  IN CHILD_STRUCT **ChildStructPtr
)
{
  DEBUG ((DEBUG_INFO,"Update VBT for ADL-P DDR5 Edp + DP1.4\n"));
  // Disabling DDI-B (LFP1)
  ChildStructPtr[1]->DeviceClass = NO_DEVICE;
  // Enabling DP++ on DDI-B
  ChildStructPtr[2]->DeviceClass  = DISPLAY_PORT_HDMI_DVI_COMPATIBLE;
  ChildStructPtr[2]->DVOPort      = DISPLAY_PORT_B;
  ChildStructPtr[2]->AUX_Channel  = AUX_CHANNEL_B;
  ChildStructPtr[2]->DDCBus       = 0x2;
}

/**
  GOP VBT update for ADL P Ddr5 Edp + DP

  @param[in]  ChildStructPtr  - CHILD_STRUCT

**/
VOID
EFIAPI
AdlNLpddr5GopVbtSpecificUpdate (
  IN CHILD_STRUCT **ChildStructPtr
)
{
  DEBUG ((DEBUG_INFO,"Update VBT for ADL-N LPDDR5\n"));
  // Disabling DDI-B (LFP1)
  ChildStructPtr[1]->DeviceClass          = NO_DEVICE;
  // Enabling HDMI on DDI-B
  ChildStructPtr[2]->DeviceClass          = HDMI_DVI;
  ChildStructPtr[2]->DVOPort              = HDMI_B;
  ChildStructPtr[2]->AUX_Channel          = 0x0;
  ChildStructPtr[2]->HdmiLevelShifterConfig.Bits.HdmiMaxDataRateBits  = 0x0;
  ChildStructPtr[2]->DDCBus               = 0x2;
}

/**
  GOP VBT update for ADL N Ddr5 Edp + DP

  @param[in]  ChildStructPtr  - CHILD_STRUCT

**/
VOID
EFIAPI
AdlNddr5GopVbtSpecificUpdate (
  IN CHILD_STRUCT **ChildStructPtr
)
{
  DEBUG ((DEBUG_INFO,"Update VBT for ADL-N LPDDR5\n"));
  // Disabling DDI-A (LFP1)
  ChildStructPtr[0]->DeviceClass          = NO_DEVICE;
  // Disabling DDI-B (LFP2)
  ChildStructPtr[1]->DeviceClass          = NO_DEVICE;
  // Enabling HDMI on DDI-B
  ChildStructPtr[2]->DeviceClass          = HDMI_DVI;
  ChildStructPtr[2]->DVOPort              = HDMI_B;
  // Enabling HDMI on DDI-1 (TCP-2)
  ChildStructPtr[4]->DeviceClass          = HDMI_DVI;
  ChildStructPtr[4]->DVOPort              = HDMI_G;
  ChildStructPtr[4]->Flags_2.Bits.DpAltModeOverTypeCEnabled   = 0x0;
  ChildStructPtr[4]->Flags_2.Bits.TbtAltModeOverTypeCEnabled  = 0x0;
  // Disabling EFP4
  ChildStructPtr[5]->DeviceClass          = NO_DEVICE;
  // Disabling EFP5
  ChildStructPtr[6]->DeviceClass          = NO_DEVICE;
  // Enabling DP on DDI-A (EFP6)
  ChildStructPtr[7]->DeviceClass          = DISPLAY_PORT_ONLY;
  ChildStructPtr[7]->DVOPort              = DISPLAY_PORT_A;
  ChildStructPtr[7]->AUX_Channel          = AUX_CHANNEL_A;
  ChildStructPtr[7]->Flags_2.Bits.DpAltModeOverTypeCEnabled   = 0x0;
  ChildStructPtr[7]->Flags_2.Bits.TbtAltModeOverTypeCEnabled  = 0x0;
}

/**
  GOP VBT update for ADL S DDR4

  @param[in]  ChildStructPtr  - CHILD_STRUCT

**/
// Currently this routine is not in use
#if 0
VOID
AdlSAdpSGopVbtSpecificUpdate (
  IN CHILD_STRUCT **ChildStructPtr
)
{
  ChildStructPtr[1]->DeviceClass    = HDMI_DVI;
  ChildStructPtr[1]->DVOPort        = DISPLAY_PORT_B;
  ChildStructPtr[1]->AUX_Channel    = 0;
  ChildStructPtr[1]->DDCBus         = 2;

  ChildStructPtr[2]->DeviceClass    = DISPLAY_PORT_HDMI_DVI_COMPATIBLE;
  ChildStructPtr[2]->DVOPort        = DISPLAY_PORT_C;
  ChildStructPtr[2]->AUX_Channel    = AUX_CHANNEL_C;
  ChildStructPtr[2]->DDCBus         = 3;

  ChildStructPtr[3]->DeviceClass    = HDMI_DVI;
  ChildStructPtr[3]->DVOPort        = DISPLAY_PORT_D;
  ChildStructPtr[2]->AUX_Channel    = 0;
  ChildStructPtr[3]->DDCBus         = 4;

  ChildStructPtr[4]->DeviceClass    = DISPLAY_PORT_HDMI_DVI_COMPATIBLE;
  ChildStructPtr[4]->DVOPort        = DISPLAY_PORT_E;
  ChildStructPtr[4]->AUX_Channel    = AUX_CHANNEL_E;
}
#endif

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
  Update VBT data in Igd Op Region.

  @param[in] VbtTablePtr      Pointer to VBT_TABLE_DATA

  @retval    EFI_SUCCESS      VBT data was returned.
  @exception EFI_ABORTED      Intel VBT not found.
**/
EFI_STATUS
UpdateVbt (
  IN VOID   *VbtTablePtr
  )
{
  VBT_TABLE_DATA                    *VbtPtr;
  CHILD_STRUCT                      *ChildStructPtr[ChildStruct_MAX];
  UINTN                             Index;
  GOP_VBT_SPECIFIC_UPDATE           GopVbtSpecificUpdate;
  UINT16                            BoardId;

  GopVbtSpecificUpdate = NULL;
  VbtPtr = (VBT_TABLE_DATA *)VbtTablePtr;
  if (VbtPtr == NULL) {
    return EFI_ABORTED;
  }

  DEBUG ((DEBUG_INFO, "VBT data found '%.20a'\n", VbtPtr->VbtHeader.Product_String));

  BoardId = GetPlatformId ();

  DEBUG((DEBUG_INFO, "Updating GOP VBT fields using GOP Configuration Lib.\n"));

  ChildStructPtr[0] = &(VbtPtr->VbtGen2Info.Child_Struct[0]);
  for (Index = 0; Index < (ChildStruct_MAX - 1); Index++) {
    ChildStructPtr[Index + 1] = (CHILD_STRUCT *)(((UINT8 *)ChildStructPtr[Index]) + VbtPtr->VbtGen2Info.SizeChild_Struct);
  }

  switch (BoardId) {
  case PLATFORM_ID_ADL_S_ADP_S_CRB:
  case PLATFORM_ID_ADL_S_ADP_S_DDR5_UDIMM_1DC_CRB:
  case PLATFORM_ID_TEST_S_DDR5_UDIMM_RVP:
  case PLATFORM_ID_TEST_S_DDR5_SODIMM_RVP:
  case PLATFORM_ID_ADL_S_ADP_S_DDR4_SODIMM_CRB:
  case PLATFORM_ID_ADL_S_ADP_S_DDR5_SODIMM_CRB:
    DEBUG((DEBUG_INFO, "UpdateVbt: PLATFORM_ID_ADL_S_ADP_S_CRB .....\n"));
    GopVbtSpecificUpdate = (GOP_VBT_SPECIFIC_UPDATE)(UINTN)&AdlGopVbtSpecificUpdateNull;
    break;
  case PLATFORM_ID_ADL_P_DDR5_RVP:
  case PLATFORM_ID_ADL_PS_DDR5_RVP:
  case PLATFORM_ID_ADL_PS_DDR5_CRB:
    DEBUG((DEBUG_INFO, "UpdateVbt: PLATFORM_ID_ADL_P_DDR5_RVP .....\n"));
    GopVbtSpecificUpdate = (GOP_VBT_SPECIFIC_UPDATE)(UINTN)&AdlPDdr5GopVbtSpecificUpdate;
    break;
  case PLATFORM_ID_ADL_P_LP4_RVP:
  case PLATFORM_ID_ADL_P_LP5_RVP:
    DEBUG((DEBUG_INFO, "UpdateVbt: BoardIdAdlPLp4/5Rvp .....\n"));
    GopVbtSpecificUpdate = (GOP_VBT_SPECIFIC_UPDATE)(UINTN)&AdlGopVbtSpecificUpdateNull;
    break;
  case PLATFORM_ID_ADL_N_DDR5_CRB:
    DEBUG((DEBUG_INFO, "UpdateVbt: BoardIdAdlNDdr5 .....\n"));
    GopVbtSpecificUpdate = (GOP_VBT_SPECIFIC_UPDATE)(UINTN)&AdlNddr5GopVbtSpecificUpdate;
    break;
  case PLATFORM_ID_ADL_N_LPDDR5_RVP:
    DEBUG((DEBUG_INFO, "UpdateVbt: BoardIdAdlNLpddr5 .....\n"));
    GopVbtSpecificUpdate = (GOP_VBT_SPECIFIC_UPDATE)(UINTN)&AdlNLpddr5GopVbtSpecificUpdate;
    break;
  default:
    DEBUG((DEBUG_INFO, "UpdateVbt: Unsupported board Id %x .....\n", BoardId));
    break;
  }
  ASSERT(GopVbtSpecificUpdate != NULL);
  if (GopVbtSpecificUpdate) {
    GopVbtSpecificUpdate (ChildStructPtr);
  }

  DEBUG((DEBUG_INFO, "VBT Updated.\n"));
  UpdateVbtChecksum (VbtPtr);

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
IgdOpRegionInit (
  VOID
  )
{
  GLOBAL_NVS_AREA               *Gnvs;
  UINT16                        Data16;
  UINT16                        ExtendedVbtSize;
  EFI_STATUS                    Status;
  VBIOS_VBT_STRUCTURE           *VbtFileBuffer;

  Status = EFI_ABORTED;
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

  mIgdOpRegion.OpRegion = (IGD_OPREGION_STRUCTURE *) AllocatePool (sizeof(IGD_OPREGION_STRUCTURE) + ExtendedVbtSize);
  if (mIgdOpRegion.OpRegion == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  SetMem(mIgdOpRegion.OpRegion, sizeof(IGD_OPREGION_STRUCTURE) + ExtendedVbtSize, 0);

  //
  // Update OpRegion address to Gnvs
  //
  Gnvs = (GLOBAL_NVS_AREA *)(UINTN)PcdGet32(PcdAcpiGnvsAddress);
  Gnvs->SaNvs.IgdOpRegionAddress = (UINT32)(UINTN)(mIgdOpRegion.OpRegion);

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

  //Set External Gfx Adapter field is valid
  mIgdOpRegion.OpRegion->Header.PCON |= BIT8;

  //Assume No External Gfx Adapter
  mIgdOpRegion.OpRegion->Header.PCON &= (UINT32) (~BIT7);

  mIgdOpRegion.OpRegion->MBox3.BCLP = BACKLIGHT_BRIGHTNESS;

  mIgdOpRegion.OpRegion->MBox3.PFIT = (FIELD_VALID_BIT | PFIT_STRETCH);

  //
  // Reporting to driver for VR IMON Calibration. Bits [5-1] values supported 14A to 31A.
  //
  mIgdOpRegion.OpRegion->MBox3.PCFT = (Gnvs->SaNvs.GfxTurboIMON << 1) & 0x003E;

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
