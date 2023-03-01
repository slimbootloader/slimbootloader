/** @file
  PCH cycle decoding configuration and query library.

  Copyright (c) 2013 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent**/
#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <PchAccess.h>
#include <Library/PchInfoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PchP2sbLib.h>
#include <Library/PchCycleDecodingLib.h>
#include <Library/BaseMemoryLib.h>

/**
  Returns PCH LPC device PCI base address.

  @retval                   PCH LPC PCI base address.
**/
STATIC
UINT64
LpcPciBase (
  VOID
  )
{
  return PCI_LIB_ADDRESS (
              DEFAULT_PCI_BUS_NUMBER_PCH,
              PCI_DEVICE_NUMBER_PCH_LPC,
              PCI_FUNCTION_NUMBER_PCH_LPC,
              0
              );
}

/**
  Set PCH LPC/eSPI and eSPI 2nd generic IO range.
  For generic IO range, the base address must align to 4 and less than 0xFFFF, and the length must be power of 2
  and less than or equal to 256. Moreover, the address must be length aligned.
  This function basically checks the address and length, which should not overlap with all other generic ranges.
  If no more generic range register available, it returns out of resource error.
  This cycle decoding is also required on DMI side
  Some IO ranges below 0x100 have fixed target. The target might be ITSS,RTC,LPC,PMC or terminated inside P2SB
  but all predefined and can't be changed. IO range below 0x100 will be rejected in this function except below ranges:
    0x00-0x1F,
    0x44-0x4B,
    0x54-0x5F,
    0x68-0x6F,
    0x80-0x8F,
    0xC0-0xFF
  Steps of programming generic IO range:
  1. Program LPC/eSPI PCI Offset 84h ~ 93h of Mask, Address, and Enable.
  2. Program LPC/eSPI Generic IO Range in DMI
  3. If eSPI 2nd slave is supported, program eSPI PCI Offset A4h of Mask, Address, and Enable.
  4. If eSPI 2nd slave is supported, program eSPI Generic IO Range in DMI.

  @param[in] Address                    Address for generic IO range base address.
  @param[in] Length                     Length of generic IO range.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address or length passed.
  @retval EFI_OUT_OF_RESOURCES          No more generic range available.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchLpcGenIoRangeSet (
  IN  UINT16                            Address,
  IN  UINTN                             Length
  )
{
  EFI_STATUS                            Status;
  PCH_LPC_GEN_IO_RANGE_LIST             LpcGenIoRangeList;
  UINT64                                LpcBase;
  UINTN                                 Index;
  UINTN                                 BaseAddr;
  UINTN                                 MaskLength;
  UINTN                                 TempMaxAddr;
  UINT32                                Data32;
  UINTN                                 ArraySize;
  UINTN                                 LpcEspiGenIoRangeMax;
  UINT32                                EspiPcbc;
  static struct EXCEPT_RANGE {
    UINT8 Start;
    UINT8 Length;
  } ExceptRanges[] = { {0x00, 0x20}, {0x44, 0x08}, {0x54, 0x0C}, {0x68, 0x08}, {0x80, 0x10}, {0xC0, 0x40} };

  //
  // Note: Inside this function, don't use debug print since it's could used before debug print ready.
  //

  //
  // For generic IO range, the base address must align to 4 and less than 0xFFFF,
  // the length must be power of 2 and less than or equal to 256, and the address must be length aligned.
  // IO range below 0x100 will be rejected in this function except below ranges:
  //   0x00-0x1F,
  //   0x44-0x4B,
  //   0x54-0x5F,
  //   0x68-0x6F,
  //   0x80-0x8F,
  //   0xC0-0xFF
  //
  if (((Length & (Length - 1)) != 0)  ||
      ((Address & (UINT16) ~B_LPC_CFG_GENX_DEC_IOBAR) != 0) ||
      (Length > 256)) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  if (Address < 0x100) {
    ArraySize = sizeof (ExceptRanges) / sizeof (struct EXCEPT_RANGE);
    for (Index = 0; Index < ArraySize; Index++) {
      if ((Address >= ExceptRanges[Index].Start) &&
          ((Address + Length) <= ((UINTN) ExceptRanges[Index].Start + (UINTN) ExceptRanges[Index].Length))) {
        break;
      }
    }
    if (Index >= ArraySize) {
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
    }
  }

  //
  // check if range overlap
  //
  Status  = PchLpcGenIoRangeGet (&LpcGenIoRangeList);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return Status;
  }

  LpcBase = LpcPciBase ();

  LpcEspiGenIoRangeMax = PCH_LPC_GEN_IO_RANGE_MAX;
  EspiPcbc = PciRead32 ((UINTN)(LpcBase + R_ESPI_CFG_PCBC));
  if (IsPchH () && ((EspiPcbc & B_ESPI_CFG_PCBC_ESPI_EN) != 0)) {
    LpcEspiGenIoRangeMax = PCH_H_ESPI_GEN_IO_RANGE_MAX;
  }

  for (Index = 0; Index < LpcEspiGenIoRangeMax; Index++) {
    BaseAddr = LpcGenIoRangeList.Range[Index].BaseAddr;
    MaskLength   = LpcGenIoRangeList.Range[Index].Length;
    if (BaseAddr == 0) {
      continue;
    }
    if (((Address >= BaseAddr) && (Address < (BaseAddr + MaskLength))) ||
        (((Address + Length) > BaseAddr) && ((Address + Length) <= (BaseAddr + MaskLength)))) {
      if ((Address >= BaseAddr) && (Length <= MaskLength)) {
        //
        // return SUCCESS while range is covered.
        //
        return EFI_SUCCESS;
      }

      if ((Address + Length) > (BaseAddr + MaskLength)) {
        TempMaxAddr = Address + Length;
      } else {
        TempMaxAddr = BaseAddr + MaskLength;
      }
      if (Address > BaseAddr) {
        Address = (UINT16) BaseAddr;
      }
      Length = TempMaxAddr - Address;
      break;
    }
  }
  //
  // If no range overlap
  //
  if (Index >= LpcEspiGenIoRangeMax) {
    //
    // Find a empty register
    //
    for (Index = 0; Index < LpcEspiGenIoRangeMax; Index++) {
      BaseAddr = LpcGenIoRangeList.Range[Index].BaseAddr;
      if (BaseAddr == 0) {
        break;
      }
    }
    if (Index >= LpcEspiGenIoRangeMax) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

#if 0
  Status = PchDmiSetLpcGenIoRange (Address, Length, Index);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
#endif

  //
  // Program LPC/eSPI generic IO range register accordingly.
  //
  Data32 =  (UINT32) (((Length - 1) << 16) & B_LPC_CFG_GENX_DEC_IODRA);
  Data32 |= (UINT32) Address;
  Data32 |= B_LPC_CFG_GENX_DEC_EN;

  if (Index < PCH_LPC_GEN_IO_RANGE_MAX) {
    //
    // Program LPC/eSPI PCI Offset 84h ~ 93h of Mask, Address, and Enable.
    //
    PciWrite32 (
      (UINTN)(LpcBase + R_LPC_CFG_GEN1_DEC + Index * 4),
      Data32
      );
  }
  //
  // eSPI second slave IO decoding (Gets here only if eSPI second slave is supported by Pch)
  //
  else {
    //
    // Program eSPI PCI Offset A4h of Mask, Address, and Enable.
    //
    PciWrite32 (
      (UINTN)(LpcBase + R_ESPI_CFG_CS1GIR1),
      Data32
      );
  }
  return Status;
}

/**
  Get PCH LPC/eSPI generic IO range list.
  This function returns a list of base address, length, and enable for all LPC/eSPI generic IO range regsiters.

  @param[out] LpcGenIoRangeList         Return all LPC/eSPI generic IO range register status.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address passed.
**/
EFI_STATUS
EFIAPI
PchLpcGenIoRangeGet (
  OUT PCH_LPC_GEN_IO_RANGE_LIST         *LpcGenIoRangeList
  )
{
  UINTN                                 Index;
  UINT64                                LpcBase;
  UINT32                                Data32;
  UINT32                                EspiPcbc;

  //
  // Note: Inside this function, don't use debug print since it's could used before debug print ready.
  //

  if (LpcGenIoRangeList == NULL) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  LpcBase = LpcPciBase ();

  for (Index = 0; Index < PCH_LPC_GEN_IO_RANGE_MAX; Index++) {
    Data32 = PciRead32 ((UINTN)(LpcBase + R_LPC_CFG_GEN1_DEC + Index * 4));
    LpcGenIoRangeList->Range[Index].BaseAddr = Data32 & B_LPC_CFG_GENX_DEC_IOBAR;
    LpcGenIoRangeList->Range[Index].Length   = ((Data32 & B_LPC_CFG_GENX_DEC_IODRA) >> 16) + 4;
    LpcGenIoRangeList->Range[Index].Enable   = Data32 & B_LPC_CFG_GENX_DEC_EN;
  }
  EspiPcbc = PciRead32 ((UINTN)(LpcBase + R_ESPI_CFG_PCBC));
  if (IsPchH () && ((EspiPcbc & B_ESPI_CFG_PCBC_ESPI_EN) != 0)) {
    Data32 = PciRead32 ((UINTN)(LpcBase + R_ESPI_CFG_CS1GIR1));
    LpcGenIoRangeList->Range[PCH_LPC_GEN_IO_RANGE_MAX].BaseAddr = Data32 & B_LPC_CFG_GENX_DEC_IOBAR;
    LpcGenIoRangeList->Range[PCH_LPC_GEN_IO_RANGE_MAX].Length   = ((Data32 & B_LPC_CFG_GENX_DEC_IODRA) >> 16) + 4;
    LpcGenIoRangeList->Range[PCH_LPC_GEN_IO_RANGE_MAX].Enable   = Data32 & B_LPC_CFG_GENX_DEC_EN;
  }
  return EFI_SUCCESS;
}

/**
  Set PCH LPC/eSPI IO decode ranges.
  Program LPC/eSPI I/O Decode Ranges in DMI to the same value programmed in LPC/eSPI PCI offset 80h.
  Please check EDS for detail of LPC/eSPI IO decode ranges bit definition.
  Bit  12: FDD range
  Bit 9:8: LPT range
  Bit 6:4: ComB range
  Bit 2:0: ComA range

  @param[in] LpcIoDecodeRanges          LPC/eSPI IO decode ranges bit settings.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchLpcIoDecodeRangesSet (
  IN  UINT16                            LpcIoDecodeRanges
  )
{
  UINT64                                LpcBaseAddr;
  EFI_STATUS                            Status;

  //
  // Note: Inside this function, don't use debug print since it's could used before debug print ready.
  //

  LpcBaseAddr = LpcPciBase ();

  //
  // check if setting is identical
  //
  if (LpcIoDecodeRanges == PciRead16 ((UINTN)(LpcBaseAddr + R_LPC_CFG_IOD))) {
    return EFI_SUCCESS;
  }

#if 0
  Status = PchDmiSetLpcIoDecodeRanges (LpcIoDecodeRanges);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
//#else
  PchPcrWrite16 (PID_DMI, R_PCH_DMI_PCR_LPCIOD, LpcIoDecodeRanges);
#endif

  //
  // program LPC/eSPI PCI offset 80h.
  //
  PciWrite16 ((UINTN)(LpcBaseAddr + R_LPC_CFG_IOD), LpcIoDecodeRanges);

  return Status;
}

/**
  Set PCH LPC/eSPI IO enable decoding.
  Setup LPC/eSPI I/O Enables in DMI to the same value program in LPC/eSPI PCI offset 82h.
  Note: Bit[15:10] of the source decode register is Read-Only. The IO range indicated by the Enables field
  in LPC/eSPI PCI offset 82h[13:10] is always forwarded by DMI to subtractive agent for handling.
  Please check EDS for detail of Lpc/eSPI IO decode ranges bit definition.

  @param[in] LpcIoEnableDecoding        LPC/eSPI IO enable decoding bit settings.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchLpcIoEnableDecodingSet (
  IN  UINT16                            LpcIoEnableDecoding
  )
{
  UINT64      LpcBaseAddr;
  EFI_STATUS  Status;

  //
  // Note: Inside this function, don't use debug print since it's could used before debug print ready.
  //

  LpcBaseAddr = LpcPciBase ();

  if (LpcIoEnableDecoding == PciRead16 ((UINTN)(LpcBaseAddr + R_LPC_CFG_IOE))) {
    return EFI_SUCCESS;
  }

#if 0
  Status = PchDmiSetLpcIoEnable (LpcIoEnableDecoding);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
//#else
  PchPcrWrite16 (PID_DMI, R_PCH_DMI_PCR_LPCIOE, LpcIoEnableDecoding);
#endif

  //
  // program LPC/eSPI PCI offset 82h.
  //
  PciWrite16 ((UINTN)(LpcBaseAddr + R_LPC_CFG_IOE), LpcIoEnableDecoding);

  return Status;
}

