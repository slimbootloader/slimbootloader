/** @file
  Header file for PchCycleDecodingLib.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_CYCLE_DECODING_LIB_H_
#define _PCH_CYCLE_DECODING_LIB_H_

/**
  Set PCH TCO base address.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. set Smbus PCI offset 54h [8] to enable TCO base address.
  2. program Smbus PCI offset 50h [15:5] to TCO base address.
  3. set Smbus PCI offset 54h [8] to enable TCO base address.
  4. program "TCO Base Address" PCR[DMI] + 2778h[15:5, 1] to [Smbus PCI offset 50h[15:5], 1].

  @param[in] Address                    Address for TCO base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address passed.
**/
EFI_STATUS
EFIAPI
PchTcoBaseSet (
  IN  UINT16                            Address
  );

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
  );

///
/// structure of LPC general IO range register
/// It contains base address, address mask, and enable status.
///
typedef struct {
  UINT32                                BaseAddr :16;
  UINT32                                Length   :15;
  UINT32                                Enable   : 1;
} PCH_LPC_GEN_IO_RANGE;

#define PCH_LPC_GEN_IO_RANGE_MAX        4
#define PCH_H_ESPI_GEN_IO_RANGE_MAX     5

///
/// structure of LPC general IO range register list
/// It lists all LPC general IO ran registers supported by PCH.
///
typedef struct {
  PCH_LPC_GEN_IO_RANGE                  Range[PCH_H_ESPI_GEN_IO_RANGE_MAX];
} PCH_LPC_GEN_IO_RANGE_LIST;

/**
  Set PCH LPC generic IO range.
  For generic IO range, the base address must align to 4 and less than 0xFFFF, and the length must be power of 2
  and less than or equal to 256. Moreover, the address must be length aligned.
  This function basically checks the address and length, which should not overlap with all other generic ranges.
  If no more generic range register available, it returns out of resource error.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  The IO ranges below 0x100 have fixed target. The target might be ITSS,RTC,LPC,PMC or terminated inside P2SB
  but all predefined and can't be changed. IO range below 0x100 will be skipped except 0x80-0x8F.
  Steps of programming generic IO range:
  1. Program LPC/eSPI PCI Offset 84h ~ 93h of Mask, Address, and Enable.
  2. Program LPC/eSPI Generic IO Range #, PCR[DMI] + 2730h ~ 273Fh to the same value programmed in LPC/eSPI PCI Offset 84h~93h.

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
  );

/**
  Get PCH LPC generic IO range list.
  This function returns a list of base address, length, and enable for all LPC generic IO range regsiters.

  @param[out] LpcGenIoRangeList         Return all LPC generic IO range register status.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address passed.
**/
EFI_STATUS
EFIAPI
PchLpcGenIoRangeGet (
  OUT PCH_LPC_GEN_IO_RANGE_LIST         *LpcGenIoRangeList
  );

/**
  Set PCH LPC memory range decoding.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. Program LPC/eSPI PCI 98h [0] to [0] to disable memory decoding first before changing base address.
  2. Program LPC/eSPI PCI 98h [31:16, 0] to [Address, 1].
  3. Program LPC/eSPI Memory Range, PCR[DMI] + 2740h to the same value programmed in LPC/eSPI PCI Offset 98h.

  @param[in] Address                    Address for memory base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address or length passed.
  @retval EFI_OUT_OF_RESOURCES          No more generic range available.
**/
EFI_STATUS
EFIAPI
PchLpcMemRangeSet (
  IN  UINT32                            Address
  );

/**
  Set PCH eSPI memory range 2 decoding.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. Program eSPI PCI A8h [0] to [0] to disable memory decoding first before changing base address.
  2. Program eSPI PCI A8h [31:16, 0] to [Address, 1].
  3. Program eSPI Memory Range, PCR[DMI] + 27C0h to the same value programmed in eSPI PCI Offset A8h.

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
  );

/**
  Get PCH LPC memory range decoding address.

  @param[out] Address                   Address of LPC memory decoding base address.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_INVALID_PARAMETER         Invalid base address passed.
**/
EFI_STATUS
EFIAPI
PchLpcMemRangeGet (
  OUT UINT32                            *Address
  );

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
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. if GCS.BBS is 0 (SPI), program SPI PCI offset D8h to BiosDecodeEnable.
     if GCS.BBS is 1 (LPC/eSPi), program LPC/eSPI PCI offset D8h to BiosDecodeEnable.
  2. program LPC/eSPI/SPI BIOS Decode Enable, PCR[DMI] + 2744h to the same value programmed in LPC/eSPI or SPI PCI Offset D8h.

  @param[in] BiosDecodeEnable           Bios decode enable setting.

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
EFIAPI
PchBiosDecodeEnableSet (
  IN  UINT16                            BiosDecodeEnable
  );

/**
  Set PCH LPC IO decode ranges.
  Program LPC I/O Decode Ranges, PCR[DMI] + 2770h[15:0] to the same value programmed in LPC offset 80h.
  Please check EDS for detail of Lpc IO decode ranges bit definition.
  Bit  12: FDD range
  Bit 9:8: LPT range
  Bit 6:4: ComB range
  Bit 2:0: ComA range

  @param[in] LpcIoDecodeRanges          Lpc IO decode ranges bit settings.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchLpcIoDecodeRangesSet (
  IN  UINT16                            LpcIoDecodeRanges
  );

/**
  Set PCH LPC IO enable decoding.
  Setup LPC I/O Enables, PCR[DMI] + 2774h[15:0] to the same value program in LPC offset 82h.
  Note: Bit[15:10] of the source decode register is Read-Only. The IO range indicated by the Enables field
  in LPC 82h[13:10] is always forwarded by DMI to subtractive agent for handling.
  Please check EDS for detail of Lpc IO decode ranges bit definition.

  @param[in] LpcIoEnableDecoding        Lpc IO enable decoding bit settings.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
EFIAPI
PchLpcIoEnableDecodingSet (
  IN  UINT16                            LpcIoEnableDecoding
  );

/**
  Set PCH IO port 80h cycle decoding to PCIE root port.
  System BIOS is likely to do this very soon after reset before PCI bus enumeration, it must ensure that
  the IO Base Address field (PCIe:1Ch[7:4]) contains a value greater than the IO Limit field (PCIe:1Ch[15:12])
  before setting the IOSE bit. Otherwise the bridge will positively decode IO range 000h - FFFh by its default
  IO range values.
  This cycle decoding is allowed to set when DMIC.SRL is 0.
  Programming steps:
  1. Program "RPR Destination ID", PCR[DMI] + 274Ch[31:16] to the Dest ID of RP.
  2. Program "Reserved Page Route", PCR[DMI] + 274Ch[11] to '1'. Use byte write on GCS+1 and leave the BILD bit which is RWO.
  3. Program IOSE bit of PCIE:Reg04h[0] to '1'  for PCH to send such IO cycles to PCIe bus for subtractive decoding.

  @param[in] RpPhyNumber                PCIE root port physical number.

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
EFIAPI
PchIoPort80DecodeSet (
  IN  UINTN                             RpPhyNumber
  );

/**
  Get IO APIC regsiters base address.

  @param[out] IoApicBase                Buffer of IO APIC regsiter address

  @retval EFI_SUCCESS                   Successfully completed.
**/
EFI_STATUS
PchIoApicBaseGet (
  OUT UINT32                            *IoApicBase
  );

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
  );

#endif // _PCH_CYCLE_DECODING_LIB_H_
