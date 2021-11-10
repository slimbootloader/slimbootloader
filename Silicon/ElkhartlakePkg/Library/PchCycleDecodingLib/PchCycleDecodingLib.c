/** @file
  PCH cycle deocding configuration and query library.

  Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
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

#if 0
/**
  Set PCH TCO base address.
  This cycle decoding is required also on DMI side
  Programming steps:
  1. set Smbus PCI offset 54h [8] to enable TCO base address.
  2. program Smbus PCI offset 50h [15:5] to TCO base address.
  3. set Smbus PCI offset 54h [8] to enable TCO base address.
  4. program "TCO Base Address" in DMI

  @param[in] Address                    Address for TCO base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address passed.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchTcoBaseSet (
  IN  UINT16                            Address
  )
{
  UINT64                                SmbusBase;
  EFI_STATUS                            Status;


  if ((Address & ~B_PCH_SMBUS_TCOBASE_BAR) != 0) {
    DEBUG ((DEBUG_ERROR, "PchTcoBaseSet Error. Invalid Address: %x.\n", Address));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  Status = PchDmiSetTcoBase (Address);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  SmbusBase = PCI_LIB_ADDRESS (
                DEFAULT_PCI_BUS_NUMBER_PCH,
                PCI_DEVICE_NUMBER_PCH_SMBUS,
                PCI_FUNCTION_NUMBER_PCH_SMBUS,
                0
                );
  if (PciRead16 (SmbusBase) == 0xFFFF) {
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }
  //
  // Verify TCO base is not locked.
  //
  if ((PciRead8 (SmbusBase + R_PCH_SMBUS_TCOCTL) & B_PCH_SMBUS_TCOCTL_TCO_BASE_LOCK) != 0) {
    ASSERT (FALSE);
    return EFI_DEVICE_ERROR;
  }
  //
  // Disable TCO in SMBUS Device first before changing base address.
  // Byte access to not touch the TCO_BASE_LOCK bit
  //
  PciAnd8 (
    SmbusBase + R_PCH_SMBUS_TCOCTL + 1,
    (UINT8) ~(B_PCH_SMBUS_TCOCTL_TCO_BASE_EN >> 8)
    );
  //
  // Program TCO in SMBUS Device
  //
  PciAndThenOr16 (
    SmbusBase + R_PCH_SMBUS_TCOBASE,
    (UINT16) (~B_PCH_SMBUS_TCOBASE_BAR),
    Address
    );
  //
  // Enable TCO in SMBUS Device and lock TCO BASE
  //
  PciOr16 (
    SmbusBase + R_PCH_SMBUS_TCOCTL,
    B_PCH_SMBUS_TCOCTL_TCO_BASE_EN | B_PCH_SMBUS_TCOCTL_TCO_BASE_LOCK
    );

  return Status;
}

/**
  Get PCH TCO base address.

  @param[out] Address                   Address of TCO base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid pointer passed.
**/
EFI_STATUS
EFIAPI
PchTcoBaseGet (
  OUT UINT16                            *Address
  )
{
  if (Address == NULL) {
    DEBUG ((DEBUG_ERROR, "PchTcoBaseGet Error. Invalid pointer.\n"));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }
  //
  // Read "TCO Base Address" from DMI
  // Don't read TCO base address from SMBUS PCI register since SMBUS might be disabled.
  //
  *Address = PchDmiGetTcoBase ();

  return EFI_SUCCESS;
}
#endif

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
      ((Address & (UINT16) ~B_PCH_LPC_GENX_DEC_IOBAR) != 0) ||
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

  LpcBase = PCI_LIB_ADDRESS (
              DEFAULT_PCI_BUS_NUMBER_PCH,
              PCI_DEVICE_NUMBER_PCH_LPC,
              PCI_FUNCTION_NUMBER_PCH_LPC,
              0
              );

  LpcEspiGenIoRangeMax = PCH_LPC_GEN_IO_RANGE_MAX;
  EspiPcbc = PciRead32 ((UINTN)(LpcBase + R_PCH_ESPI_PCBC));
  if ((EspiPcbc & B_PCH_ESPI_PCBC_ESPI_EN) != 0) {
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
  Data32 =  (UINT32) (((Length - 1) << 16) & B_PCH_LPC_GENX_DEC_IODRA);
  Data32 |= (UINT32) Address;
  Data32 |= B_PCH_LPC_GENX_DEC_EN;

  if (Index < PCH_LPC_GEN_IO_RANGE_MAX) {
    //
    // Program LPC/eSPI PCI Offset 84h ~ 93h of Mask, Address, and Enable.
    //
    PciWrite32 (
      (UINTN)(LpcBase + R_PCH_LPC_GEN1_DEC + Index * 4),
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
      (UINTN)(LpcBase + R_PCH_ESPI_CS1GIR1),
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

  LpcBase      = PCI_LIB_ADDRESS (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC,
                   0
                   );

  for (Index = 0; Index < PCH_LPC_GEN_IO_RANGE_MAX; Index++) {
    Data32 = PciRead32 ((UINTN)(LpcBase + R_PCH_LPC_GEN1_DEC + Index * 4));
    LpcGenIoRangeList->Range[Index].BaseAddr = Data32 & B_PCH_LPC_GENX_DEC_IOBAR;
    LpcGenIoRangeList->Range[Index].Length   = ((Data32 & B_PCH_LPC_GENX_DEC_IODRA) >> 16) + 4;
    LpcGenIoRangeList->Range[Index].Enable   = Data32 & B_PCH_LPC_GENX_DEC_EN;
  }
  EspiPcbc = PciRead32 ((UINTN)(LpcBase + R_PCH_ESPI_PCBC));
  if ((EspiPcbc & B_PCH_ESPI_PCBC_ESPI_EN) != 0) {
    Data32 = PciRead32 ((UINTN)(LpcBase + R_PCH_ESPI_CS1GIR1));
    LpcGenIoRangeList->Range[PCH_LPC_GEN_IO_RANGE_MAX].BaseAddr = Data32 & B_PCH_LPC_GENX_DEC_IOBAR;
    LpcGenIoRangeList->Range[PCH_LPC_GEN_IO_RANGE_MAX].Length   = ((Data32 & B_PCH_LPC_GENX_DEC_IODRA) >> 16) + 4;
    LpcGenIoRangeList->Range[PCH_LPC_GEN_IO_RANGE_MAX].Enable   = Data32 & B_PCH_LPC_GENX_DEC_EN;
  }
  return EFI_SUCCESS;
}

#if 0
/**
  Set PCH LPC/eSPI memory range decoding.
  This cycle decoding is required to be set on DMI side
  Programming steps:
  1. Program LPC/eSPI PCI Offset 98h [0] to [0] to disable memory decoding first before changing base address.
  2. Program LPC/eSPI PCI Offset 98h [31:16, 0] to [Address, 1].
  3. Program LPC/eSPI Memory Range in DMI

  @param[in] Address                    Address for memory base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address or length passed.
  @retval EFI_OUT_OF_RESOURCES          No more generic range available.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchLpcMemRangeSet (
  IN  UINT32                            Address
  )
{
  UINT64                                LpcBase;
  EFI_STATUS                            Status;

  if ((Address & (~B_PCH_LPC_LGMR_MA)) != 0) {
    DEBUG ((DEBUG_ERROR, "PchLpcMemRangeSet Error. Invalid Address: %x.\n", Address));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  Status = PchDmiSetLpcMemRange (Address);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  LpcBase      = PCI_LIB_ADDRESS (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC,
                   0
                   );
  //
  // Program LPC/eSPI PCI Offset 98h [0] to [0] to disable memory decoding first before changing base address.
  //
  PciAnd32 (
    LpcBase + R_PCH_LPC_LGMR,
    (UINT32) ~B_PCH_LPC_LGMR_LMRD_EN
    );
  //
  // Program LPC/eSPI PCI Offset 98h [31:16, 0] to [Address, 1].
  //
  PciWrite32 (
    LpcBase + R_PCH_LPC_LGMR,
    (Address | B_PCH_LPC_LGMR_LMRD_EN)
    );

  return Status;
}

/**
  Set PCH eSPI memory range 2 decoding.
  This cycle decoding is required to be set on DMI side
  Programming steps:
  1. Program eSPI PCI A8h [0] to [0] to disable memory decoding first before changing base address.
  2. Program eSPI PCI A8h [31:16, 0] to [Address, 1].
  3. Program eSPI Memory Range 2 in DMI.

  @param[in] Address                    Address for memory base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address or length passed.
  @retval EFI_OUT_OF_RESOURCES          No more generic range available.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchEspiMemRange2Set (
  IN  UINT32                            Address
  )
{
  UINT64                                LpcBase;
  UINT32                                EspiPcbc;
  EFI_STATUS                            Status;

  if ((Address & (~B_PCH_LPC_LGMR_MA)) != 0) {
    DEBUG ((DEBUG_ERROR, "PchLpcMemRange2Set Error. Invalid Address: %x.\n", Address));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  Status = PchDmiSetLpcMemRange2 (Address);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  LpcBase      = PCI_LIB_ADDRESS (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC,
                   0
                   );

  //
  // PCH eSPI Enable Pin Strap check
  //
  EspiPcbc = PciRead32 (LpcBase + R_PCH_ESPI_PCBC);
  if ((EspiPcbc & B_PCH_ESPI_PCBC_ESPI_EN) != 0) {
    return EFI_UNSUPPORTED;
  }

  //
  // Program eSPI PCI Offset A8h [0] to [0] to disable memory decoding first before changing base address.
  //
  PciAnd32 (
    LpcBase + R_PCH_ESPI_CS1GMR1,
    (UINT32) ~B_PCH_LPC_LGMR_LMRD_EN
    );

  //
  // Program eSPI PCI Offset A8h [31:16, 0] to [Address, 1].
  //
  PciWrite32 (
    LpcBase + R_PCH_ESPI_CS1GMR1,
    (Address | B_PCH_LPC_LGMR_LMRD_EN)
    );

  return Status;
}

/**
  Get PCH LPC/eSPI memory range decoding address.

  @param[out] Address                   Address of LPC/eSPI memory decoding base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address passed.
**/
EFI_STATUS
EFIAPI
PchLpcMemRangeGet (
  OUT UINT32                            *Address
  )
{
  UINT64                                LpcBase;

  if (Address == NULL) {
    DEBUG ((DEBUG_ERROR, "PchLpcMemRangeGet Error. Invalid pointer.\n"));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  LpcBase      = PCI_LIB_ADDRESS (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC,
                   0
                   );
  *Address = PciRead32 (LpcBase + R_PCH_LPC_LGMR) & B_PCH_LPC_LGMR_MA;
  return EFI_SUCCESS;
}

/**
  Set PCH BIOS range deocding.
  This will check General Control and Status bit 10 (GCS.BBS) to identify SPI or LPC/eSPI and program BDE register accordingly.
  Please check EDS for detail of BiosDecodeEnable bit definition.
    bit 15: F8-FF Enable
    bit 14: F0-F8 Enable
    bit 13: E8-EF Enable
    bit 12: E0-E8 Enable
    bit 11: D8-DF Enable
    bit 10: D0-D7 Enable
    bit  9: C8-CF Enable
    bit  8: C0-C7 Enable
    bit  7: Legacy F Segment Enable
    bit  6: Legacy E Segment Enable
    bit  5: Reserved
    bit  4: Reserved
    bit  3: 70-7F Enable
    bit  2: 60-6F Enable
    bit  1: 50-5F Enable
    bit  0: 40-4F Enable
  This cycle decoding is also required in DMI
  Programming steps:
  1. if GCS.BBS is 0 (SPI), program SPI offset D8h to BiosDecodeEnable.
     if GCS.BBS is 1 (LPC/eSPi), program LPC offset D8h to BiosDecodeEnable.
  2. program LPC BIOS Decode Enable in DMI

  @param[in] BiosDecodeEnable           Bios decode enable setting.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchBiosDecodeEnableSet (
  IN  UINT16                            BiosDecodeEnable
  )
{
  UINT64                                BaseAddr;
  EFI_STATUS                            Status;

  Status = PchDmiSetBiosDecodeEnable (BiosDecodeEnable);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Check Boot BIOS Strap in DMI
  //
  if (PchDmiIsBootBiosStrapSetForSpi ()) {
    BaseAddr = PCI_LIB_ADDRESS (
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_PCH_SPI,
                 PCI_FUNCTION_NUMBER_PCH_SPI,
                 0
                 );
    //
    // If SPI, Program SPI offset D8h to BiosDecodeEnable.
    //
    PciWrite16 (BaseAddr + R_PCH_SPI_BDE, BiosDecodeEnable);
  } else {
    BaseAddr = PCI_LIB_ADDRESS (
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_PCH_LPC,
                 PCI_FUNCTION_NUMBER_PCH_LPC,
                 0
                 );
    //
    // If LPC/eSPi, program LPC offset D8h to BiosDecodeEnable.
    //
    PciWrite16 (BaseAddr + R_PCH_LPC_BDE, BiosDecodeEnable);
  }

  return Status;
}
#endif

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
  EFI_STATUS                            Status = EFI_SUCCESS;

  //
  // Note: Inside this function, don't use debug print since it's could used before debug print ready.
  //

  LpcBaseAddr  = PCI_LIB_ADDRESS (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC,
                   0
                   );
  //
  // check if setting is identical
  //
  if (LpcIoDecodeRanges == PciRead16 ((UINTN)(LpcBaseAddr + R_PCH_LPC_IOD))) {
    return EFI_SUCCESS;
  }

#if 0
  Status = PchDmiSetLpcIoDecodeRanges (LpcIoDecodeRanges);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
//#else
  PchPcrWrite16 (PID_DMI, R_PCH_PCR_DMI_LPCIOD, LpcIoDecodeRanges);
#endif

  //
  // program LPC/eSPI PCI offset 80h.
  //
  PciWrite16 ((UINTN)(LpcBaseAddr + R_PCH_LPC_IOD), LpcIoDecodeRanges);

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
  EFI_STATUS  Status = EFI_SUCCESS;

  //
  // Note: Inside this function, don't use debug print since it's could used before debug print ready.
  //

  LpcBaseAddr  = PCI_LIB_ADDRESS (
                   DEFAULT_PCI_BUS_NUMBER_PCH,
                   PCI_DEVICE_NUMBER_PCH_LPC,
                   PCI_FUNCTION_NUMBER_PCH_LPC,
                   0
                   );

  if (LpcIoEnableDecoding == PciRead16 ((UINTN)(LpcBaseAddr + R_PCH_LPC_IOE))) {
    return EFI_SUCCESS;
  }

#if 0
  Status = PchDmiSetLpcIoEnable (LpcIoEnableDecoding);
  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
//#else
  PchPcrWrite16 (PID_DMI, R_PCH_PCR_DMI_LPCIOE, LpcIoEnableDecoding);
#endif

  //
  // program LPC/eSPI PCI offset 82h.
  //
  PciWrite16 ((UINTN)(LpcBaseAddr + R_PCH_LPC_IOE), LpcIoEnableDecoding);

  return Status;
}

#if 0
/**
  Set PCH IO port 80h cycle decoding to PCIE root port.
  System BIOS is likely to do this very soon after reset before PCI bus enumeration.
  This cycle decoding is allowed to set when DMI is unlocked

  @param[in] RpNumber                PCIE root port physical number.

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
EFIAPI
PchIoPort80DecodeSet (
  IN  UINTN                             RpNumber
  )
{
  EFI_STATUS  Status;

  Status = PchDmiSetIoPort80Decode (RpNumber);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Get IO APIC regsiters base address.

  @param[out] IoApicBase                Buffer of IO APIC regsiter address

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
PchIoApicBaseGet (
  OUT UINT32                            *IoApicBase
  )
{
  *IoApicBase = PcdGet32 (PcdIoApicBaseAddress);
  return EFI_SUCCESS;
}

/**
  Get HPET base address.
  This function will be unavailable after P2SB is hidden by PSF.

  @param[out] HpetBase                  Buffer of HPET base address

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid offset passed.
**/
EFI_STATUS
PchHpetBaseGet (
  OUT UINT32                            *HpetBase
  )
{
  if (HpetBase == NULL) {
    DEBUG ((DEBUG_ERROR, "PchHpetBaseGet Error. Invalid pointer.\n"));
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  *HpetBase = PcdGet32 (PcdHpetBaseAddress);
  return EFI_SUCCESS;
}
#endif
