/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _BOARD_SUPPORT_LIB_H_
#define _BOARD_SUPPORT_LIB_H_

#include <Guid/OsBootOptionGuid.h>
#include <Library/FirmwareUpdateLib.h>

#define  MVBT_SIGNATURE               SIGNATURE_32 ('$', 'M', 'V', 'B')

typedef struct {
  UINT32        Signature;
  UINT8         EntryNum;
  UINT8         Reserved[3];
} VBT_MB_HDR;

typedef struct {
  UINT32        ImageId;
  UINT32        Length;
  UINT8         Data[];
} VBT_ENTRY_HDR;

/**
  A function pointer to get relative power number in mW

  @param[in]  BaseRatio     Base bus ratio
  @param[in]  CurrRatio     Current bus ratio to get relative power
  @param[in]  BasePower     Base power number in mW

  @retval                   Calculated power number in mW

**/
typedef UINT32 (EFIAPI *GET_RELATIVE_POWER_FUNC) (
  UINT16  BaseRatio,
  UINT16  CurrRatio,
  UINT32  BasePower
  );

/**
  All PSS calculation related parameters. GetRelativePower can be optional.

  @param[in]      TurboBusRatio     Turbo bus ratio
  @param[in]      MaxBusRatio       Maximum bus ratio
  @param[in]      MinBusRatio       Mimimum bus ratio
  @param[in]      PackageMaxPower   Maximum package power
  @param[in]      PackageMinPower   Minimum package power
  @param[in]      GetRelativePower  A func pointer to get relative power
  @param[in]      DoListAll         List all from LFM to Turbo

**/
typedef struct {
  UINT16                    TurboBusRatio;
  UINT16                    MaxBusRatio;
  UINT16                    MinBusRatio;
  UINT32                    PackageMaxPower;
  UINT32                    PackageMinPower;
  GET_RELATIVE_POWER_FUNC   GetRelativePower;
  BOOLEAN                   DoListAll;
} PSS_PARAMS;

/**
  Fill the boot option list data with CFGDATA info

  @param[in, out]   OsBootOptionList pointer to boot option list.
 **/
VOID
EFIAPI
FillBootOptionListFromCfgData (
  IN OUT   OS_BOOT_OPTION_LIST   *OsBootOptionList
);


/**
  Set the platform name with CFGDATA info

 **/
VOID
EFIAPI
PlatformNameInit (
  VOID
);

/**
  Load the configuration data blob from SPI flash into destination buffer.
  It supports the sources: PDR, BIOS for external Cfgdata.

  @param[in]    Dst        Destination address to load configuration data blob.
  @param[in]    Src        Source address to load configuration data blob.
  @param[in]    Len        The destination address buffer size.

  @retval  EFI_SUCCESS             Configuration data blob was loaded successfully.
  @retval  EFI_NOT_FOUND           Configuration data blob cannot be found.
  @retval  EFI_OUT_OF_RESOURCES    Destination buffer is too small to hold the
                                   configuration data blob.
  @retval  Others                  Failed to load configuration data blob.

**/
EFI_STATUS
EFIAPI
SpiLoadExternalConfigData (
  IN UINT32  Dst,
  IN UINT32  Src,
  IN UINT32  Len
  );

/**
  Check state machine.

  This function will check state machine to see if capsule is pending

  @param[in]    pFwUpdStatus     Pointer to FW_UPDATE_STATUS structure.

  @retval  EFI_SUCCESS           State machine initialized in reserved region.
  @retval  EFI_UNSUPPORTED       Failure occured during state machine init.
  @retval  others                Error occured during state machine init.
**/
EFI_STATUS
CheckStateMachine (
  IN FW_UPDATE_STATUS    *pFwUpdStatus
  );

/**
  Patch ACPI CPU Pss Table

  This function will patch PSS table. Caller MUST guarantee valid table address.

  @param[in,out]  PssTableAddr      Pointer to PSS Table to be updated
  @param[in]      PssParams         All PSS calculation related info
  @retval         EFI_SUCCESS       Patch done successfully
  @retval         others            Error occured during patching the table

**/
EFI_STATUS
AcpiPatchPssTable (
  IN OUT        UINT8                    *PssTableAddr,
  IN      CONST PSS_PARAMS               *PssParams
  );

/**
  Find the actual VBT image from the container.

  In case of multiple VBT tables are packed into a single FFS, the PcdGraphicsVbtAddress could
  point to the container address instead. This function checks this condition and locates the
  actual VBT table address within the container.

  @param[in] ImageId    Image ID for VBT binary to locate in the container

  @retval               Actual VBT address found in the container. 0 if not found.

**/
UINT32
LocateVbtByImageId (
  IN  UINT32     ImageId
  );

/**
  Get VBT address.

  This function gets VBT address, In case of multiple VBT
  tables, this function will call LocateVbtByImageId, otherwise
  returns PcdGraphicsVbtAddress.

  @retval               Actual VBT address found in the container. 0 if not found.

**/
UINTN
GetVbtAddress (
  );

/**
  Patch VBT to use a fixed display mode with the required resolution.

  @param[in]  VbtBuf    VBT binary buffer in memory to be patched.
  @param[in]  Xres      Requested mode X resolution.
  @param[in]  Yres      Requested mode Y resolution.

  @retval     EFI_SUCCESS        Fixed mode block in VBT has been patched to the required mode.
              EFI_NOT_FOUND      Could not find fixed mode block in VBT.
**/
EFI_STATUS
EFIAPI
SetVbtFixedMode (
  IN  UINT8     *VbtBuf,
  IN  UINT32     Xres,
  IN  UINT32     Yres
  );


/**
  Set framebuffer range as writecombining for performance.

  @param[in]    FrameBufferBase   Framebuffer base address.
                                  if 0, it will use framebuffer HOB to get the base.
  @param[in]    FrameBufferSize   Framebuffer size.
                                  if 0, it will use framebuffer HOB to get the size.
                                  if MAX_UINT32, it will parse the PCI bar to get the size.

  @retval  EFI_SUCCESS            The framebuffer cache was enabled successfully.
           EFI_NOT_FOUND          Failed to find the required GFX controller.
           EFI_UNSUPPORTED        The base and size cannot be supported.
           EFI_OUT_OF_RESOURCES   No enough MTRR to use.
**/
EFI_STATUS
EFIAPI
SetFrameBufferWriteCombining (
  IN  EFI_PHYSICAL_ADDRESS   FrameBufferBase,
  IN  UINT32                 FrameBufferSize
  );

#endif
