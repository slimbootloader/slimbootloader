/** @file
  Pch information library for MTL.

  Copyright (c) 2020 - 2025, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Pci30.h>
#include <Library/PrintLib.h>
#include <Library/PchInfoLib.h>
#include <PchAccess.h>
#include "PchInfoLibPrivate.h"
#include <PchHda.h>
#include <Register/MtlSocRegsLpc.h>
#include <Register/EspiRegs.h>
#include <Library/PchPciBdfLib.h>
#include <Library/PciSegmentLib.h>
#include <Library/MtlIoeInfoHob.h>
#include <Library/MtlSocInfoLib.h>

PCH_SERIES
PchSeriesFromLpcDid (
  IN UINT16 LpcDeviceId
  );

PCH_STEPPING
PchSteppingFromRevId (
  IN UINT8 RevId
  );

VOID
PchPrintSteppingStr (
  OUT CHAR8        *Buffer,
  IN  UINT32       BufferSize,
  IN  PCH_STEPPING PchStep
  );

typedef struct {
  UINT32  Id;
  CHAR8   *Str;
} MTL_INFO_STR_TABLE;

GLOBAL_REMOVE_IF_UNREFERENCED MTL_INFO_STR_TABLE mMtlSocSkuStrs[] = {
  {V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_0,   "MTL SoC M Unfused"},
  {V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_1,   "P Super SKU (SSKU)"},
  {V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_2,   "P Premium"},
  {V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_3,   "P Base/Mainstream"},
  {V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_4,   "MTL SKU 4"},
  {V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_5,   "MTL SKU 5"},
  {V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_6,   "M Super SKU (SSKU)"},
  {V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_7,   "M Premium"},
  {V_MTL_SOC_M_LPC_CFG_DEVICE_ID_MB_8,   "MTL SKU 8"},
  {V_MTL_SOC_S_LPC_CFG_DEVICE_ID_MB_0,   "MTL S Unfused"},
  {V_MTL_SOC_S_LPC_CFG_DEVICE_ID_MB_6,   "MTL S SKU 6"},
  {V_MTL_SOC_S_LPC_CFG_DEVICE_ID_MB_10,  "MTL S Super SKU (SSKU)"},
  {V_MTL_SOC_S_LPC_CFG_DEVICE_ID_MB_11,  "MTL S Premium"}
};

GLOBAL_REMOVE_IF_UNREFERENCED MTL_INFO_STR_TABLE  mMtlSocSeries[] = {
  {MTL_SOC_M, "MTL SOC M"},
  {MTL_SOC_S, "MTL SOC S"},
  {MTL_SOC_UNKNOWN_SERIES, "Unknown"}
};

GLOBAL_REMOVE_IF_UNREFERENCED MTL_INFO_STR_TABLE  mMtlIoeSeries[] = {
  {MTL_IOE_M, "MTL IOE M"},
  {MTL_IOE_P, "MTL IOE P"},
  {MTL_IOE_UNKNOWN_SERIES, "Unknown"}
};

/**
  Return eSPI Device Id.

  @retval  MTL SoC eSPI Device ID
**/
UINT16
MtlSocGetEspiDid (
  VOID
  )
{
  UINT64  LpcBaseAddress;

  LpcBaseAddress = LpcPciCfgBase ();

  return PciSegmentRead16 (LpcBaseAddress + PCI_DEVICE_ID_OFFSET);
}

/**
  Return LPC Device Id

  @retval PCH_LPC_DEVICE_ID         PCH Lpc Device ID
**/
UINT16
PchGetLpcDid (
  VOID
  )
{
  return MtlSocGetEspiDid ();
}

/**
  Return SoC Series

  @retval MTL_SOC_SERIES     SoC Series
**/
MTL_SOC_SERIES
MtlSocGetSeries (
  VOID
  )
{
  switch (MtlSocGetEspiDid () & B_ESPI_CFG_DID) {
    case V_LPC_CFG_DID_MTL_SOC_M:
      return MTL_SOC_M;
    case V_LPC_CFG_DID_MTL_SOC_S:
      return MTL_SOC_S;
    default:
      return MTL_SOC_UNKNOWN_SERIES;
  }
}

/**
  Return Pch Series

  @retval PCH_SERIES            Pch Series
**/
PCH_SERIES
PchSeries (
  VOID
  )
{
  MTL_SOC_SERIES  SocSeries;

  SocSeries = MtlSocGetSeries ();
  if ((SocSeries == MTL_SOC_M) || (SocSeries == ARL_SOC_M)) {
    return PCH_M;
  } else if (SocSeries == MTL_SOC_S) {
    return PCH_S;
  }

  return PCH_UNKNOWN_SERIES;
}

/**
  Return SoC South stepping type

  @retval SoC South stepping type
**/
UINT8
MtlSocSouthGetStepping (
  VOID
  )
{
  return PciSegmentRead8 (LpcPciCfgBase () + PCI_REVISION_ID_OFFSET);
}

/**
  Return Pch stepping type

  @retval PCH_STEPPING            Pch stepping type
**/
PCH_STEPPING
PchStepping (
  VOID
  )
{
  return (PCH_STEPPING) MtlSocSouthGetStepping ();
}

/**
  Checks if SoC is supported.

  @return TRUE if SOC is supported by BIOS.
**/
BOOLEAN
MtlSocIsSupported (
  VOID
  )
{
  UINT16         LpcDeviceId;
  UINT16         LpcVendorId;
  UINT64         LpcBaseAddress;

  LpcBaseAddress = LpcPciCfgBase ();

  LpcDeviceId = PciSegmentRead16 (LpcBaseAddress + PCI_DEVICE_ID_OFFSET);
  LpcVendorId = PciSegmentRead16 (LpcBaseAddress + PCI_VENDOR_ID_OFFSET);

  ///
  /// Verify that this is a supported chipset
  ///
  if ((LpcVendorId == V_PCH_INTEL_VENDOR_ID) && (MtlSocGetSeries () != MTL_SOC_UNKNOWN_SERIES)) {
    return TRUE;
  } else {
    DEBUG ((DEBUG_ERROR, "BIOS doesn't support the LpcDeviceId: 0x%04x!\n", LpcDeviceId));
    return FALSE;
  }
}

/**
  Determine if PCH is supported

  @retval TRUE                    PCH is supported
  @retval FALSE                   PCH is not supported
**/
BOOLEAN
IsPchSupported (
  VOID
  )
{
  return MtlSocIsSupported ();
}

/**
  Check if this is PCH P series

  @retval TRUE                It's PCH P series
  @retval FALSE               It's not PCH P series
**/
BOOLEAN
IsPchP (
  VOID
  )
{
  return (PchSeries () == PCH_P);
}

/**
  Check if this is PCH M series

  @retval TRUE                It's PCH M series
  @retval FALSE               It's not PCH M series
**/
BOOLEAN
IsPchM (
  VOID
  )
{
  return (PchSeries () == PCH_M);
}

/**
  Check if this is PCH S series

  @retval TRUE                It's PCH S series
  @retval FALSE               It's not PCH S series
**/
BOOLEAN
IsPchS (
  VOID
  )
{
  return (PchSeries () == PCH_S);
}

//MTL

/**
  Macro that converts PCI Segment, PCI Bus, PCI Device, PCI Function,
  and PCI Register to an address that can be passed to the PCI Segment Library functions.

  Computes an address that is compatible with the PCI Segment Library functions.
  The unused upper bits of Segment, Bus, Device, Function,
  and Register are stripped prior to the generation of the address.

  @param  Segment   PCI Segment number.  Range 0..65535.
  @param  Bus       PCI Bus number.  Range 0..255.
  @param  Device    PCI Device number.  Range 0..31.
  @param  Function  PCI Function number.  Range 0..7.
  @param  Register  PCI Register number.  Range 0..255 for PCI.  Range 0..4095 for PCI Express.

  @return The address that is compatible with the PCI Segment Library functions.

**/
#define PCI_SEGMENT_LIB_ADDRESS(Segment, Bus, Device, Function, Register) \
  ((Segment != 0) ? \
    ( ((Register) & 0xfff)                 | \
      (((Function) & 0x07) << 12)          | \
      (((Device) & 0x1f) << 15)            | \
      (((Bus) & 0xff) << 20)               | \
      (LShiftU64 ((Segment) & 0xffff, 32))   \
    ) :                                      \
    ( ((Register) & 0xfff)                 | \
      (((Function) & 0x07) << 12)          | \
      (((Device) & 0x1f) << 15)            | \
      (((Bus) & 0xff) << 20)                 \
    )                                        \
  )

/**
  Get Device Id from IOE Pmc device.

  @retval UINT16  Device ID from IOE PMC Device.
**/
UINT16
PmcGetIoeDevId (
  VOID
  )
{
  UINT64 IoePmcAddr;

  IoePmcAddr = PCI_SEGMENT_LIB_ADDRESS (
                 DEFAULT_PCI_SEGMENT_NUMBER_PCH,
                 DEFAULT_PCI_BUS_NUMBER_PCH,
                 PCI_DEVICE_NUMBER_IOE_PMC,
                 PCI_FUNCTION_NUMBER_IOE_PMC,
                 0
                 );

  return PciSegmentRead16 (IoePmcAddr + PCI_DEVICE_ID_OFFSET);
}

/**
  Determine IOE series based on IOE Device ID

  @param[in]  DevId       IOE Device ID
  @retval     IOE_SERIES  IOE Series
**/
MTL_IOE_SERIES
MtlIoeSeriesFromDevId (
  IN UINT16 DevId
  )
{
  switch (DevId) {
    case MTL_IOE_M_PMC_DEVICE_ID:
      return MTL_IOE_M;
    case MTL_IOE_P_PMC_DEVICE_ID:
      return MTL_IOE_P;
    default:
      return MTL_IOE_UNKNOWN_SERIES;
  }
}

/**
  Return IOE Series

  @retval IOE_SERIES    IOE Series
**/
MTL_IOE_SERIES
MtlIoeGetSeries (
  VOID
  )
{
  UINT16       IoePmcDevId;

  IoePmcDevId = PmcGetIoeDevId ();
  if (IoePmcDevId != 0xFFFF) {
    return MtlIoeSeriesFromDevId (IoePmcDevId);
  }

  DEBUG ((DEBUG_INFO, "--> AimR PchInfo : after IF\n"));
  return MtlIoeSeriesFromDevId (IoePmcDevId);
}

/**
  Checks if MTL SoC is of M series.

  @return TRUE if MTL SoC is of M series.
**/
BOOLEAN
MtlIsSocM (
  VOID
  )
{
  return (MtlSocGetSeries () == MTL_SOC_M);
}

/**
  Checks if MTL SoC is of S series.

  @return TRUE if MTL SoC is of S series.
**/
BOOLEAN
MtlIsSocS (
  VOID
  )
{
  return (MtlSocGetSeries () == MTL_SOC_S);
}

/**
  Checks if MTL IOE is of P series.

  @return TRUE if MTL IOE is of P series.
**/
BOOLEAN
MtlIsIoeP (
  VOID
  )
{
  if (MtlIoeGetSeries () == MTL_IOE_P) {
    return TRUE;
  } else {
    return FALSE;
  }
}

//MTL

/**
  Check if this is EBG PCH generation

  @retval TRUE                It's EBG PCH
  @retval FALSE               It's not EBG PCH
**/
BOOLEAN
IsEbgPch (
  VOID
  )
{
  return FALSE;
}

/**
  Check if this is MTL SOC generation

  @retval TRUE                It's MTL SOC
  @retval FALSE               It's not MTL SOC
**/
BOOLEAN
IsMtlSoc (
  VOID
  )
{
  return (PchGeneration () == MTL_SOC);
}

/**
  Print MTL SoC-M South stepping string into buffer.

  @param [out]     Buffer               Output buffer of string
  @param [in]      BufferSize           Buffer size.
                                        Must not be less than MTL_SOC_INFO_STEPPING_STR_MIN_LENGTH

  @retval EFI_SUCCESS                   String copied successfully
  @retval EFI_INVALID_PARAMETER         The stepping is not supported, or parameters are NULL
  @retval EFI_BUFFER_TOO_SMALL          Input buffer size is too small
**/
EFI_STATUS
MtlSocSouthGetSteppingStr (
  OUT CHAR8  *Buffer,
  IN UINT32  BufferSize
  )
{
  SOC_M_SOUTH_STEPPING SocSouthStepping;
  CHAR8               *SteppingId;

  if ((Buffer == NULL) || (BufferSize == 0)) {
    DEBUG ((DEBUG_ERROR, "MtlSocSouthGetSteppingStr: Error! Invalid parameter\n"));
    return EFI_INVALID_PARAMETER;
  }
  if (BufferSize < MTL_SOC_INFO_STEPPING_STR_MIN_LENGTH) {
    DEBUG ((DEBUG_ERROR, "MtlSocSouthGetSteppingStr: Error! Buffer Too Small\n"));
    return EFI_BUFFER_TOO_SMALL;
  }

  SocSouthStepping = MtlSocSouthGetStepping ();

  switch (SocSouthStepping)
  {
    case SocSouthA0:
      SteppingId = "A0";
      break;
    case SocSouthA1:
      SteppingId = "A1";
      break;
    case SocSouthA2:
      SteppingId = "A2";
      break;
    case SocSouthB0:
      SteppingId = "B0";
      break;
    case SocSouthC0:
      SteppingId = "C0";
      break;
    default :
      SteppingId = "A0";
      break;
  }
  AsciiSPrint (Buffer, BufferSize, SteppingId);
  return EFI_SUCCESS;
}

/**
  Get PCH stepping ASCII string.
  Function determines major and minor stepping versions and writes them into a buffer.
  The return string is zero terminated

  @param [out]     Buffer               Output buffer of string
  @param [in]      BufferSize           Buffer size.
                                        Must not be less then PCH_STEPPING_STR_LENGTH_MAX

  @retval EFI_SUCCESS                   String copied successfully
  @retval EFI_INVALID_PARAMETER         The stepping is not supported, or parameters are NULL
  @retval EFI_BUFFER_TOO_SMALL          Input buffer size is too small
**/
EFI_STATUS
PchGetSteppingStr (
  OUT    CHAR8                          *Buffer,
  IN     UINT32                         BufferSize
  )
{
  return MtlSocSouthGetSteppingStr (Buffer, BufferSize);
}

/**
  Get MTL SoC SKU str.

  @retval Static ASCII string of MTL SoC SKU
**/
CHAR8*
MtlSocGetSkuStr (
  VOID
  )
{
  UINTN          Index;
  UINT16         LpcDid;

  LpcDid = MtlSocGetEspiDid ();

  for (Index = 0; Index < ARRAY_SIZE (mMtlSocSkuStrs); Index++) {
    if (LpcDid == mMtlSocSkuStrs[Index].Id) {
      return mMtlSocSkuStrs[Index].Str;
    }
  }

  return "Undefined SKU";
}

/**
  Get PCH Sku ASCII string
  The return string is zero terminated.

  @retval Static ASCII string of PCH Sku
**/
CHAR8*
PchGetSkuStr (
  VOID
  )
{
  return MtlSocGetSkuStr ();
}

/**
  Get number of PCIe controller on MTL SOC.

  @return Number of controllers on the SoC.
**/
UINT8
MtlSocGetMaxPcieControllerNum (
  VOID
  )
{
  UINT8  NumOfControllers;

  if (MtlIsSocM ()) {
    NumOfControllers = 3;
    if (MtlIsIoeP ()) {
      NumOfControllers += 3;
    }
  } else { // MTL-S
    NumOfControllers = 2;
    if (MtlIsIoeP ()) {
      NumOfControllers += 2;
    }
  }

  return NumOfControllers;
}

/**
  Get Pch Maximum Pcie Controller Number

  @retval Pch Maximum Pcie Root Port Number
**/
UINT8
GetPchMaxPcieControllerNum (
  VOID
  )
{
  return MtlSocGetMaxPcieControllerNum ();
}

/**
  return support status for P2SB PCR 20-bit addressing

  @retval    TRUE
  @retval    FALSE
**/
BOOLEAN
IsP2sb20bPcrSupported (
  VOID
  )
{
  return TRUE;
}

/**
  Return Pch Generation

  @retval PCH_GENERATION      Pch Generation
**/
PCH_GENERATION
PchGeneration (
  VOID
  )
{
  return MTL_SOC;
}

/**
  Check if this is Server PCH

  @retval TRUE                It's a Server PCH
  @retval FALSE               It's not a Server PCH
**/
BOOLEAN
IsPchServer (
  VOID
  )
{
  return FALSE;
}

/**
  Check if this is PCH H series

  @retval TRUE                It's PCH H series
  @retval FALSE               It's not PCH H series
**/
BOOLEAN
IsPchH (
  VOID
  )
{
  return FALSE;
}

/**
  Check if this is PCH N series

  @retval TRUE                It's PCH N series
  @retval FALSE               It's not PCH N series
**/
BOOLEAN
IsPchN (
  VOID
  )
{
  return FALSE;
}

/**
  Check if this is PCH LP series

  @retval TRUE     It's PCH LP series
  @retval FALSE    It's not PCH LP series
**/
BOOLEAN
IsPchLp (
  VOID
  )
{
  return (PchSeries () == PCH_LP);
}

/**
  Get MTL SoC series string.

  @return Pointer to NULL terminated string with MTL SoC series name.
**/
CHAR8*
MtlSocGetSeriesStr (
  VOID
  )
{
  UINTN           Index;
  MTL_SOC_SERIES  SocSeries;

  SocSeries = MtlSocGetSeries ();
  for (Index = 0; Index < ARRAY_SIZE (mMtlIoeSeries); Index++) {
    if (SocSeries == mMtlSocSeries[Index].Id) {
      return mMtlSocSeries[Index].Str;
    }
  }

  return "Unknown";
}

/**
  Get PCH series ASCII string.

  @retval PCH Series string
**/
CHAR8*
PchGetSeriesStr (
  VOID
  )
{
  return MtlSocGetSeriesStr ();
}

/**
  Get Pch Maximum Pcie Clock Number

  @retval Pch Maximum Pcie Clock Number
**/
UINT8
GetPchMaxPcieClockNum (
  VOID
  )
{
  UINT8  ClockNum;

  ClockNum = 0;
  if (MtlIsSocM ()) {
    ClockNum = 6;
  } else if (MtlIsSocS ()) {
    ClockNum = 4;
  }

  if (MtlIsIoeP ()) {
    ClockNum += 3;
  }

  return ClockNum;

}

/**
  Get Pch Maximum Pcie ClockReq Number

  @retval Pch Maximum Pcie ClockReq Number
**/
UINT8
GetPchMaxPcieClockReqNum (
  VOID
  )
{
  return GetPchMaxPcieClockNum ();
}

/**
  Get Pch Maximum Type C Port Number

  @retval Pch Maximum Type C Port Number
**/
UINT8
GetPchMaxTypeCPortNum (
  VOID
  )
{
  return 4;
}

/**
  Check whether integrated TSN is supported by PCH Series.

  @retval TRUE                    TSN is supported in current PCH
  @retval FALSE                   TSN is not supported on current PCH
**/
BOOLEAN
PchIsTsnSupported (
  VOID
  )
{
  return FALSE;
}

/**
  Check if this chipset supports Gbe controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
PchIsGbeSupported (
  VOID
  )
{
  return TRUE;
}

/**
  Checks if PCH is attached to the SoC.

  @return TRUE if PCH is attached.
**/
BOOLEAN
MtlSocIsPchAttached (
  VOID
  )
{
  //
  // SOC only build has a limited flash memory. Returning FALSE for that build optimizes
  // the build by preventing linking the PCH code which is not needed for SOC only build.
  //
#if FixedPcdGetBool (PcdSocCoreBootEnable)
  return FALSE;
#else
  //
  // We assume that on MtlSocS PCH-S is always present.
  //
  return MtlIsSocS ();
#endif
}

/**
  Check whether ISH is supported by PCH Series.

  @retval TRUE                    ISH is supported in current PCH
  @retval FALSE                   ISH is not supported on current PCH
**/
BOOLEAN
PchIsIshSupported (
  VOID
  )
{
  return MtlIsSocM () || (MtlIsSocS () && MtlSocIsPchAttached ());
}

/**
  Get number of PCIe root ports on MTL SOC

  @retval Number of root ports on the SoC
**/
UINT8
MtlSocGetMaxPciePortNum (
  VOID
  )
{
  UINT8  NumOfRootPorts;

  if (MtlIsSocM ()) {
    NumOfRootPorts = 9;
    if (MtlIsIoeP ()) {
      NumOfRootPorts += 3;
    }
  } else { // MTL-S
    NumOfRootPorts = 3;
    if (MtlIsIoeP ()) {
      NumOfRootPorts += 2;
    }
  }

  return NumOfRootPorts;
}

/**
  Get Pch Maximum Pcie Root Port Number

  @retval Pch Maximum Pcie Root Port Number
**/
UINT8
GetPchMaxPciePortNum (
  VOID
  )
{
  return MtlSocGetMaxPciePortNum ();
}

/**
  Get Pch Maximum Hda Dmic Link

  @retval Pch Maximum Hda Dmic Link
**/
UINT8
GetPchHdaMaxDmicLinkNum (
  VOID
  )
{
  return 2;
}

/**
  Get Pch Maximum Hda Sndw Link

  @retval Pch Maximum Hda Sndw Link
**/
UINT8
GetPchHdaMaxSndwLinkNum (
  VOID
  )
{
  return 4;
}

/**
  Get Pch Maximum Hda Ssp Link

  @retval Pch Maximum Hda Ssp Link
**/
UINT8
GetPchHdaMaxSspLinkNum (
  VOID
  )
{
  return 3;
}

/**
  Get Usb Maximum Physical Port Number

  @retval Usb Maximum Physical Port Number
**/
UINT8
MtlSocGetUsb2MaxPhysicalPortNum (
  VOID
  )
{
  switch (MtlSocGetSeries ()) {
    case MTL_SOC_M:
      return 10;
    default:
      return 0;
  }
}

/**
  Get Maximum Usb2 Port Number of XHCI Controller

  @retval Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
MtlSocGetXhciMaxUsb2PortNum (
  VOID
  )
{
  switch (MtlSocGetSeries ()) {
    case MTL_SOC_M:
      return 12;
    default:
      return 0;
  }
}

/**
  Get Serial IO SPI controller PCIe Device Number

  @param[in]  SpiNumber       Serial IO SPI controller index

  @retval Serial IO SPI controller PCIe Device Number
**/
UINT8
EFIAPI
MtlPchSerialIoSpiDevNumber (
  IN UINT8       SpiNumber
  )
{
  if (GetPchMaxSerialIoSpiControllersNum () <= SpiNumber) {
    ASSERT (FALSE);
    return 0xFF;
  }
  switch (SpiNumber) {
    case 0:
      return PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI0;
    case 1:
      return PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI1;
    case 2:
      return PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI2;
    case 3:
      return PCI_DEVICE_NUMBER_PCH_SERIAL_IO_SPI3;
    default:
      ASSERT (FALSE);
      return 0xFF;
  }
}

/**
  Get Serial IO SPI controller PCIe Function Number

  @param[in]  SpiNumber       Serial IO SPI controller index

  @retval Serial IO SPI controller PCIe Function Number
**/
UINT8
EFIAPI
MtlPchSerialIoSpiFuncNumber (
  IN UINT8       SpiNumber
  )
{
  if (GetPchMaxSerialIoSpiControllersNum () <= SpiNumber) {
    ASSERT (FALSE);
    return 0xFF;
  }
  switch (SpiNumber) {
    case 0:
      return PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI0;
    case 1:
      return PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI1;
    case 2:
      return PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI2;
    case 3:
      return PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_SPI3;
    default:
      ASSERT (FALSE);
      return 0xFF;
  }
}

/**
  Get Serial IO SPI controller address that can be passed to the PCI Segment Library functions.

  @param[in]  SpiNumber       Serial IO SPI controller index

  @retval Serial IO SPI controller address in PCI Segment Library representation
**/
UINT64
EFIAPI
MtlPchSerialIoSpiPciCfgBase (
  IN UINT8        SpiNumber
  )
{
  return PCI_SEGMENT_LIB_ADDRESS (
           DEFAULT_PCI_SEGMENT_NUMBER_PCH,
           DEFAULT_PCI_BUS_NUMBER_PCH,
           MtlPchSerialIoSpiDevNumber (SpiNumber),
           MtlPchSerialIoSpiFuncNumber (SpiNumber),
           0
           );
}

/**
  Get Serial IO I2C controller PCIe Device Number

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller PCIe Device Number
**/
UINT8
EFIAPI
MtlPchSerialIoI2cDevNumber (
  IN UINT8       I2cNumber
  )
{
  if (GetPchMaxSerialIoI2cControllersNum () <= I2cNumber) {
    ASSERT (FALSE);
    return 0xFF;
  }
  switch (I2cNumber) {
    case 0:
      return PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C0;
    case 1:
      return PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C1;
    case 2:
      return PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C2;
    case 3:
      return PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C3;
    case 4:
      return PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C4;
    case 5:
      return PCI_DEVICE_NUMBER_PCH_SERIAL_IO_I2C5;
    default:
      ASSERT (FALSE);
      return 0xFF;
  }
}

/**
  Get Serial IO I2C controller PCIe Function Number

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller PCIe Function Number
**/
UINT8
EFIAPI
MtlPchSerialIoI2cFuncNumber (
  IN UINT8       I2cNumber
  )
{
  if (GetPchMaxSerialIoI2cControllersNum () <= I2cNumber) {
    ASSERT (FALSE);
    return 0xFF;
  }
  switch (I2cNumber) {
    case 0:
      return PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C0;
    case 1:
      return PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C1;
    case 2:
      return PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C2;
    case 3:
      return PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C3;
    case 4:
      return PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C4;
    case 5:
      return PCI_FUNCTION_NUMBER_PCH_SERIAL_IO_I2C5;
    default:
      ASSERT (FALSE);
      return 0xFF;
  }
}

/**
  Get Serial IO I2C controller address that can be passed to the PCI Segment Library functions.

  @param[in]  I2cNumber       Serial IO I2C controller index

  @retval Serial IO I2C controller address in PCI Segment Library representation
**/
UINT64
EFIAPI
MtlPchSerialIoI2cPciCfgBase (
  IN UINT8        I2cNumber
  )
{
  return PCI_SEGMENT_LIB_ADDRESS (
           DEFAULT_PCI_SEGMENT_NUMBER_PCH,
           DEFAULT_PCI_BUS_NUMBER_PCH,
           MtlPchSerialIoI2cDevNumber (I2cNumber),
           MtlPchSerialIoI2cFuncNumber (I2cNumber),
           0
           );
}

/**
 Get Maximum Usb3 Port Number of XHCI Controller

  @retval Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
MtlSocGetXhciMaxUsb3PortNum (
  VOID
  )
{
  switch (MtlSocGetSeries ()) {
    case MTL_SOC_M:
      return 2;
    default:
      return 0;
  }
}

/**
  Get Pch Usb2 Maximum Physical Port Number

  @retval Pch Usb2 Maximum Physical Port Number
**/
UINT8
GetPchUsb2MaxPhysicalPortNum (
  VOID
  )
{
  if (MtlIsSocM ()) {
    return MtlSocGetUsb2MaxPhysicalPortNum ();
  } else {
    return 0;
  }
}

/**
  Get Pch Maximum Usb2 Port Number of XHCI Controller

  @retval Pch Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
GetPchXhciMaxUsb2PortNum (
  VOID
  )
{
  if (MtlIsSocM ()) {
    return MtlSocGetXhciMaxUsb2PortNum ();
  } else {
    return 0;
  }
}

/**
  Get Pch Maximum Usb3 Port Number of XHCI Controller

  @retval Pch Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
GetPchXhciMaxUsb3PortNum (
  VOID
  )
{
  if (MtlIsSocM ()) {
    return MtlSocGetXhciMaxUsb3PortNum ();
  } else {
    return 0;
  }
}


/**
  Gets the maximum number of UFS controller supported by this chipset.

  @return Number of supported UFS controllers
**/
UINT8
PchGetMaxUfsNum (
  VOID
  )
{
  if (MtlIsSocM ()) {
    return 1;
  } else {
    return 0;
  }
}


/**
  Check if this chipset supports eMMC controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchEmmcSupported (
  VOID
  )
{
  return FALSE;
}

/**
  Check if this chipset supports SD controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchSdCardSupported (
  VOID
  )
{
  return FALSE;
}

/**
  Check if this chipset supports THC controller

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchThcSupported (
  VOID
  )
{
  return MtlIsSocM ();
}

/**
  Check if this chipset supports HSIO BIOS Sync

  @retval BOOLEAN  TRUE if supported, FALSE otherwise
**/
BOOLEAN
IsPchChipsetInitSyncSupported (
  VOID
  )
{
  return FALSE;
}

/**
  Check if link between PCH and CPU is an P-DMI

  @retval    TRUE           P-DMI link
  @retval    FALSE          Not an P-DMI link
**/
BOOLEAN
IsPchWithPdmi (
  VOID
  )
{
  return FALSE;
}

/**
  Check whether ATX Shutdown (PS_ON) is supported.

  @retval    TRUE           ATX Shutdown (PS_ON) is supported in PCH
  @retval    FALSE          ATX Shutdown (PS_ON) is not supported by PCH
**/
BOOLEAN
IsPchPSOnSupported (
  VOID
  )
{
  if (MtlIsSocS ()) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check if link between PCH and CPU is an OP-DMI

  @retval    TRUE           OP-DMI link
  @retval    FALSE          Not an OP-DMI link
**/
BOOLEAN
IsPchWithOpdmi (
  VOID
  )
{
  return FALSE;
}

/**
  Check if link between PCH and CPU is an F-DMI

  @retval    TRUE           F-DMI link
  @retval    FALSE          Not an F-DMI link
**/
BOOLEAN
IsPchWithFdmi (
  VOID
  )
{
  return FALSE;
}
/**
  Get Pch Maximum ISH UART Controller number

  @retval Pch Maximum ISH UART controllers number
**/
UINT8
GetPchMaxIshUartControllersNum (
  VOID
  )
{
  return 2;
}

/**
  Get Pch Maximum ISH I2C Controller number

  @retval Pch Maximum ISH I2C controllers number
**/
UINT8
GetPchMaxIshI2cControllersNum (
  VOID
  )
{
  return 3;
}

/**
  Get Pch Maximum ISH SPI Controller number

  @retval Pch Maximum ISH SPI controllers number
**/
UINT8
GetPchMaxIshSpiControllersNum (
  VOID
  )
{
  return 1;
}

/**
  Get Pch Maximum ISH SPI Controller Cs pins number

  @retval Pch Maximum ISH SPI controller Cs pins number
**/
UINT8
GetPchMaxIshSpiControllerCsPinsNum (
  VOID
  )
{
  return 1;
}

/**
  Get Pch Maximum ISH GP number

  @retval Pch Maximum ISH GP number
**/
UINT8
GetPchMaxIshGpNum (
  VOID
  )
{
  return 12;
}

/**
  Get Pch Maximum Serial IO I3C controllers number

  @retval Pch Maximum Serial IO I3C controllers number
**/
UINT8
GetPchMaxSerialIoI3cControllersNum (
  VOID
  )
{
  if (MtlIsSocS ()) {
    return 2;
  } else if (MtlIsSocM ()) {
    return 2;
  } else {
    return 0;
  }
}

/**
  Get Pch Maximum Serial IO I2C controllers number

  @retval Pch Maximum Serial IO I2C controllers number
**/
UINT8
GetPchMaxSerialIoI2cControllersNum (
  VOID
  )
{
  if (MtlIsSocM ()) {
    return 6;
  } else {
    return 0;
  }
}

/**
  Get Pch Maximum Serial IO SPI controllers number

  @retval Pch Maximum Serial IO SPI controllers number
**/
UINT8
GetPchMaxSerialIoSpiControllersNum (
  VOID
  )
{
  if (MtlIsSocS ()) {
    return 4;
  } else if (MtlIsSocM ()) {
    return 3;
  } else {
    return 0;
  }
}

/**
  Get Pch Maximum Serial IO UART controllers number

  @retval Pch Maximum Serial IO UART controllers number
**/
UINT8
GetPchMaxSerialIoUartControllersNum (
  VOID
  )
{
  if (MtlIsSocS ()) {
    return 4;
  } else if (MtlIsSocM ()) {
    return 3;
  } else {
    return 0;
  }
}


/**
  Get Pch Maximum Serial IO SPI Chip Selects count

  @retval Pch Maximum Serial IO SPI Chip Selects number
**/
UINT8
GetPchMaxSerialIoSpiChipSelectsNum (
  VOID
  )
{
  return 2;
}

/**
  Get Pch Maximum THC count

  @retval Pch Maximum THC count number
**/
UINT8
GetPchMaxThcCount (
  VOID
  )
{
  return 2;
}

/**
  Get Maximum Sata Controller Number

  @retval Maximum Sata Controller Number
**/
UINT8
MaxSataControllerNum (
  VOID
  )
{
  //
  // Defined in SoC integration header
  //
  return MAX_SATA_CONTROLLER;
}

/**
  Get Maximum Sata Port Number

  @param[in]  SataCtrlIndex       SATA controller index

  @retval     Maximum Sata Port Number
**/
UINT8
MaxSataPortNum (
  IN UINT32      SataCtrlIndex
  )
{
  ASSERT (SataCtrlIndex < MaxSataControllerNum ());

  if (IsPchS ()) {
    return MAX_SATA_PORTS_PCH_S;
  } else {
    return MAX_SATA_PORTS_MTL_SOC_M;
  }
}


