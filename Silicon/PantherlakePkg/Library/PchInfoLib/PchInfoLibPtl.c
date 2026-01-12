/** @file
  Pch information library for ADL.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
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
#include <Register/PtlPcdRegsLpc.h>
#include <Register/EspiRegs.h>
#include <Library/PchPciBdfLib.h>
#include <Library/PciSegmentLib.h>
#include <Library/PtlPcdInfoLib.h>

typedef struct {
  UINT32  Id;
  CHAR8   *Str;
} PTL_INFO_STR_TABLE;

GLOBAL_REMOVE_IF_UNREFERENCED PTL_INFO_STR_TABLE  mPtlPcdSeries[] = {
  {PTL_PCD_P, "PTL PCD P"},
  {PTL_PCD_H, "PTL PCD H"},
  {WCL_PCD,   "WCL PCD"},
  {PTL_PCD_UNKNOWN_SERIES, "Unknown"}
};

//
// PTL PCD P and H Mobile LPC Device IDs
//
GLOBAL_REMOVE_IF_UNREFERENCED PTL_INFO_STR_TABLE mPtlPcdSkuStrs[] = {
  {V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_0,   "PTL P Unfused"},
  {V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_1,   "P Super SKU (SSKU)"},
  {V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_2,   "P Premium"},
  {V_PTL_PCD_P_ESPI_CFG_DEVICE_ID_MB_3,   "P Base/Mainstream"},
  {V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_0,   "PTL H Unfused"},
  {V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_1,   "H Super SKU (SSKU)"},
  {V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_2,   "H Premium"},
  {V_PTL_PCD_H_ESPI_CFG_DEVICE_ID_MB_3,   "H Base/Mainstream"},
  {V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_0,     "WCL PCD:Unfused"},
  {V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_1,     "WCL PCD:A0"},
  {V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_2,     "WCL PCD:A1"},
  {V_WCL_PCD_ESPI_CFG_DEVICE_ID_MB_3,     "WCL PCD:B0"},
  {PTL_PCD_UNKNOWN_SKU,                  "Unknown"}
};

PCH_GENERATION
PchGeneration (
  VOID
  );

UINT8
PtlPcdGetUsbrKvmPortNum (
  VOID
  );

BOOLEAN
IsPchH (
  VOID
  );

/**
  Get Maximum Usb3 Port Number of XHCI Controller

  @retval Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
PtlPcdGetXhciMaxUsb3PortNum (
  VOID
  )
{
  return 2;
}

/**
  Get Maximum Usb2 Port Number of XHCI Controller

  @retval Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
PtlPcdGetXhciMaxUsb2PortNum (
  VOID
  )
{
  return 10;
}


/**
  Get Usb Maximum Physical Port Number

  @retval Usb Maximum Physical Port Number
**/
UINT8
PtlPcdGetUsb2MaxPhysicalPortNum (
  VOID
  )
{
  switch (PtlPcdGetSeries ()) {
    case PTL_PCD_P:
    case PTL_PCD_H:
      return 8;
    default:
      return 0;
  }
}

/**
  Translate RpIndex to controller index.

  @param[in] RpIndex  Root port index

  @return ControllerIndex
  @retval 0xFF Failed to lookup controller index.
**/
UINT32
PtlPcdRpIndexToControllerIndex(
  IN UINTN  RpIndex
  )
{
  if (PtlIsPcdP ()) {
    if (RpIndex < 4) {
      return 0;
    } else if (RpIndex < 8) {
      return 1;
    } else if (RpIndex < 10) {
      return 2;
    }
  } else if (IsWclPcd ()) {
    if (RpIndex < 4) {
      return 0;
    } else if (RpIndex < 6) {
      return 1;
    }
  } else {
    if (RpIndex < 4) {
      return 0;
    } else if (RpIndex < 8) {
      return 1;
    } else if (RpIndex < 10) {
      return 2;
    } else if (RpIndex < 11) {
      return 3;
    } else if (RpIndex < 12) {
      return 4;
    }
  }
  return 0xFF;
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
  return PtlPcdGetEspiDid ();
}


/**
  Return eSPI Device Id.

  @retval PTL PCD eSPI Device ID
**/
UINT16
PtlPcdGetEspiDid (
  VOID
  )
{
  UINT64  LpcBaseAddress;

  LpcBaseAddress = LpcPciCfgBase ();

  return PciSegmentRead16 (LpcBaseAddress + PCI_DEVICE_ID_OFFSET);
}

/**
  Return SoC Series

  @retval PTL_PCD_SERIES     SoC Series
**/
PTL_PCD_SERIES
PtlPcdGetSeries (
  VOID
  )
{
  switch (PtlPcdGetEspiDid () & B_ESPI_CFG_DID) {
    case V_ESPI_CFG_DID_PTL_PCD_P:
      return PTL_PCD_P;
    case V_ESPI_CFG_DID_PTL_PCD_H:
      return PTL_PCD_H;
    case V_ESPI_CFG_DID_WCL_PCD:
      return WCL_PCD;
    default:
      return PTL_PCD_UNKNOWN_SERIES;
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
  PTL_SOC_SERIES  SocSeries;

  SocSeries = PtlPcdGetSeries ();
  DEBUG((DEBUG_INFO," SoC Series = 0x%x\n", SocSeries));
  if (SocSeries == PTL_PCD_P) {
    return PCH_P;
  } else if (SocSeries == WCL_PCD) {
    return PCH_WCL;
  } else if (SocSeries == PTL_PCD_H) {
    return PCH_H;
  }

  return PCH_UNKNOWN_SERIES;
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
  return (PCH_STEPPING) PtlPcdGetStepping ();
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
  return PtlPcdIsSupported ();
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
  return FALSE;
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

/**
  Check if this is PTL SOC generation

  @retval TRUE                It's PTL SOC
  @retval FALSE               It's not PTL SOC
**/
BOOLEAN
IsPtlPcd (
  VOID
  )
{
  return (PchGeneration () == PTL_SOC);
}

/**
  Checks if PCD is of WCL PCD series.

  @return TRUE if SoC is of WCL PCD series.
**/
BOOLEAN
IsWclPcd (
  VOID
  )
{
  return (PtlPcdGetSeries () == WCL_PCD);
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
  return PtlPcdGetSteppingStr (Buffer, BufferSize);
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
  return PtlPcdGetSkuStr ();
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
  return PTL_SOC;
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
  return (PchSeries () == PCH_H);
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
  return FALSE;
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
  return PtlPcdGetSeriesStr ();
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
  if (PtlIsPcdP()) {
    return 7;
  } else if (IsWclPcd ()) {
    return 6;
  } else {
    return 9;
  }
}

/**
  Translate RpIndex to Controller index.
  @param[in] RpIndex  Root port index
  @return ControllerIndex
  @retval 0xFF Failed to lookup controller index.
**/
UINT32
RpIndexToControllerIndex (
  IN UINTN  RpIndex
  )
{
  return PtlPcdRpIndexToControllerIndex (RpIndex);
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
  return PCH_MAX_HDA_DMIC_LINK_NUM;
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
  return PCH_MAX_HDA_SNDW_LINK_NUM;
}

/**
  Get Pch Maximum Hda Sndw Multilane

  @retval Pch Maximum Hda Sndw Multilane Number
**/
UINT8
GetPchHdaMaxSndwMultilaneNum (
  VOID
  )
{
  return PCH_MAX_HDA_SNDW_MULTILANE_NUM;
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
  return PCH_MAX_HDA_SSP_LINK_NUM;
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
  return PtlPcdGetUsb2MaxPhysicalPortNum ();
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
  return PtlPcdGetXhciMaxUsb2PortNum ();
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
  return PtlPcdGetXhciMaxUsb3PortNum ();
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
  if (PtlIsPcdP () || PtlIsPcdH () || IsWclPcd ()) {
    return 1;
  }
  return 0;
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
  return PCH_MAX_ISH_UART_CONTROLLERS;
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
  return PCH_MAX_ISH_I2C_CONTROLLERS;
}

/**
  Get Pch Maximum ISH I3C Controller number

  @retval Pch Maximum ISH I3C controllers number
**/
UINT8
GetPchMaxIshI3cControllersNum (
  VOID
  )
{
  return PCH_MAX_ISH_I3C_CONTROLLERS;
}

/**
  Get Maximum LPSS I3C interfaces number

  @retval Pch Maximum LPSS I3C interfaces number
**/
UINT8
GetMaxI3cInterfacesNum (
  VOID
  )
{
  return PCH_MAX_SERIALIO_I3C_CONTROLLERS;
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
  return PCH_MAX_ISH_SPI_CONTROLLERS;
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
  return PCH_MAX_ISH_GP_PINS;
}

/**
  Get Pch Maximum LPSS I2C controllers number

  @retval Pch Maximum LPSS I2C controllers number
**/
UINT8
GetMaxI2cInterfacesNum (
  VOID
  )
{
  return 6;
}

/**
  Get Pch Maximum LPSS SPI controllers number

  @retval Pch Maximum LPSS SPI controllers number
**/
UINT8
GetMaxSpiInterfacesNum (
  VOID
  )
{
  return 3;
}


/**
  Get Pch Maximum LPSS UART controllers number

  @retval Pch Maximum LPSS UART controllers number
**/
UINT8
GetPchMaxSerialIoUartControllersNum (
  VOID
  )
{
  return 3;
}

/**
  Get Pch Maximum THC controllers number
  @retval Pch Maximum THC controllers number
**/
UINT8
GetPchMaxThcControllersNum (
  VOID
  )
{
  return 2;
}

/**
  Checks if SoC is supported.

  @return TRUE if SOC is supported by BIOS.
**/
BOOLEAN
PtlPcdIsSupported (
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
  if ((LpcVendorId == V_PCH_INTEL_VENDOR_ID) && (PtlPcdGetSeries () != PTL_PCD_UNKNOWN_SERIES)) {
    return TRUE;
  } else {
    DEBUG ((DEBUG_WARN, "SBL doesn't support the LpcDeviceId: 0x%04x!\n", LpcDeviceId));
    return FALSE;
  }
}

/**
  Get PTL SoC series string.

  @return Pointer to NULL terminated string with PTL SoC series name.
**/
CHAR8*
PtlPcdGetSeriesStr (
  VOID
  )
{
  UINTN           Index;
  PTL_SOC_SERIES  SocSeries;

  SocSeries = PtlPcdGetSeries ();
  for (Index = 0; Index < ARRAY_SIZE (mPtlPcdSeries); Index++) {
    if (SocSeries == mPtlPcdSeries[Index].Id) {
      return mPtlPcdSeries[Index].Str;
    }
  }

  return "Unknown";
}

/**
  Get PTL SoC SKU str.

  @retval Static ASCII string of PTL SoC SKU
**/
CHAR8*
PtlPcdGetSkuStr (
  VOID
  )
{
  UINTN          Index;
  UINT16         LpcDid;

  LpcDid = PtlPcdGetEspiDid ();

  for (Index = 0; Index < ARRAY_SIZE (mPtlPcdSkuStrs); Index++) {
    if (LpcDid == mPtlPcdSkuStrs[Index].Id) {
      return mPtlPcdSkuStrs[Index].Str;
    }
  }

  return "Undefined SKU";
}

/**
  Checks if PTL PCD is of P series.

  @return TRUE if PTL SoC is of P series.
**/
BOOLEAN
PtlIsPcdP (
  VOID
  )
{
  return (PtlPcdGetSeries () == PTL_PCD_P);
}

/**
  Checks if PTL PCD is of H series.

  @return TRUE if PTL SoC is of H series.
**/
BOOLEAN
PtlIsPcdH (
  VOID
  )
{
  return (PtlPcdGetSeries () == PTL_PCD_H);
}

/**
  Return SoC stepping type

  @retval PCH_STEPPING   SoC stepping type
**/
PTL_SOC_STEPPING
PtlPcdGetStepping (
  VOID
  )
{
  return PciSegmentRead8 (LpcPciCfgBase () + PCI_REVISION_ID_OFFSET);
}

/**
  Print PTL SoC stepping string into buffer.

  @param [out]     Buffer               Output buffer of string
  @param [in]      BufferSize           Buffer size.
                                        Must not be less than PTL_SOC_INFO_STEPPING_STR_MIN_LENGTH

  @retval EFI_SUCCESS                   String copied successfully
  @retval EFI_INVALID_PARAMETER         The stepping is not supported, or parameters are NULL
  @retval EFI_BUFFER_TOO_SMALL          Input buffer size is too small
**/
EFI_STATUS
PtlPcdGetSteppingStr (
  OUT CHAR8  *Buffer,
  IN UINT32  BufferSize
  )
{
  PTL_SOC_STEPPING  SocStepping;

  if ((Buffer == NULL) || (BufferSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  if (BufferSize < PTL_PCD_INFO_STEPPING_STR_MIN_LENGTH) {
    return EFI_BUFFER_TOO_SMALL;
  }

  SocStepping = PtlPcdGetStepping ();
  AsciiSPrint (Buffer, BufferSize, "%c%c", 'A' + (SocStepping >> 4), '0' + (SocStepping & 0xF));
  return EFI_SUCCESS;
}

//PTL

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
  Get number of PCIe controller on PTL SOC.

  @return Number of controllers on the SoC.
**/
UINT8
PtlPcdGetMaxPcieControllerNum (
  VOID
  )
{
  UINT8  NumOfControllers;

  if (PtlIsPcdP ()) {
    NumOfControllers = 10;
  } else if (IsWclPcd ()) {
    NumOfControllers = 6;
  } else {
    NumOfControllers = 12;
  }

  return NumOfControllers;
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
  Checks if PCH is attached to the SoC.

  @return TRUE if PCH is attached.
**/
BOOLEAN
PtlPcdIsPchAttached (
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
  // We assume that on PtlPcdS PCH-S is always present.
  //
  return PtlIsPcdH ();
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
  return PtlIsPcdP () || (PtlIsPcdH () && PtlPcdIsPchAttached ());
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
  return PtlIsPcdP ();
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
  if (PtlIsPcdH ()) {
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
  Get Pch Maximum Serial IO I3C controllers number

  @retval Pch Maximum Serial IO I3C controllers number
**/
UINT8
GetPchMaxSerialIoI3cControllersNum (
  VOID
  )
{
  return 3;
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
  return 6;
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
  return 3;
}


/**
  Get USB-R Port Number for KVM session

  @retval USB-R Port Number for KVM session
**/
UINT8
PtlPcdGetUsbrKvmPortNum (
  VOID
  )
{
  return PtlPcdGetUsb2MaxPhysicalPortNum ();
}

/**
  Get USB-R Port Number for Storage session

  @retval USB-R Port Number for Storage session
**/
UINT8
PtlPcdGetUsbrStoragePortNum (
  VOID
  )
{
  return PtlPcdGetUsb2MaxPhysicalPortNum () + 1;
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
  return PCH_MAX_SERIALIO_SPI_CHIP_SELECTS;
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
  return PCH_MAX_THC_CONTROLLERS;
}

