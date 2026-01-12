/** @file
  @ Header file to get VMD information.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _VMD_INFO_LIB_H_
#define _VMD_INFO_LIB_H_

#include <Uefi.h>

#define VMD_MAX_DEVICES   31
#define MAX_OS_VISIBLE_BUSES_WITH_VMD       223  // OS visible range is 0-223. Bus range from 225 to 255 is reserved for VMD when it is enabled.
#define MAX_OS_VISIBLE_BUS_LENGTH_WITH_VMD  224  // Buses 0 to 223
#define VMD_DUMMY_DEVICE_ID                 (0x09AB) // Dummy Function DID

//
//VMD device will be assigned with (Segment/Bus/Device/Function) equals to (0/0/14/0)
//
#define VMD_BUS_NUM            0x0 // Bus      #0
#define VMD_DEV_NUM            0xE // Device   #14
#define VMD_FUN_NUM            0x0 // Function #0

//
// VMD Default Setup options
//
#define VMD_CFG_BAR_SIZE_DEFAULT           25  //32MB
#define VMD_MEM_BAR1_SIZE_DEFAULT          24  //16MB
#define VMD_MEM_BAR2_SIZE_DEFAULT          24  //16MB

#define VMD_32BIT_NONPREFETCH              0
#define VMD_64BIT_NONPREFETCH              1
#define VMD_64BIT_PREFETCH                 2


#define EFI_VMD_OS_VARIABLE_REVISION_ID 2
#define EFI_VMD_OS_VARIABLE_NAME        L"IntelVmdOsVariableV2"

extern EFI_GUID gEfiVmdFeatureVariableGuid;

#pragma pack (push,1)

typedef struct {
    UINT16  RpBus         :  8;
    UINT16  RpDevice      :  5;
    UINT16  RpFunction    :  3;
    UINT8   RpNumber;
} DEVICE_BDF_DATA;

//
// Structure of VMD EFI variable to be able to r/w by OS
// Byte0: VMD selection and Control
//    Bit[5:0]: VMD Request Entry Count (VREC): The number of valid VMD B:D:F Selection Entries.
//           Any VMD B:D:F Selection Entries beyond this count value are invalid.
//    Bit[15:6]: Reserved

typedef struct {
  struct {
    UINT16  VREC      :  6;  // VMD Request Entry Count (VREC): The number of valid VMD B:D:F Selection Entries. B:D:F Selection Entries beyond this count value are invalid.
    UINT16  Rsvd      :  10;
  } BYTE0_1;
  DEVICE_BDF_DATA DevDetails[VMD_MAX_DEVICES];
} EFI_VMD_OS_DATA;

#pragma pack (pop)


/**
  GetVmdBusNumber: Get VMD Bus Number

  @retval PCI bus number for VMD
**/
UINT8
EFIAPI
GetVmdBusNumber (
  VOID
  );

/**
  GetVmdDevNumber: Get VMD Dev Number

  @retval PCI dev number for VMD
**/
UINT8
EFIAPI
GetVmdDevNumber (
  VOID
  );

/**
  GetVmdFunNumber: Get VMD Fun Number

  @retval PCI fun number for VMD
**/
UINT8
EFIAPI
GetVmdFuncNumber (
  VOID
  );


/**
  IsVmdEnabled: Check if VMD is enabled by reading the VMD Device Id

  @retval TRUE  if  VMD is enabled
  @retval False if  VMD is not enabled
**/
BOOLEAN
EFIAPI
IsVmdEnabled (
  );


/**
  IsVmdLocked: Get the status of VMD lock for critical config space registers of VMD
re
  @retval EFI_SUCCESS if  fun number for VMD
**/
BOOLEAN
EFIAPI
IsVmdLocked (
  );


/**
  IsVmdSupported: Get the status of VMD support bit from capability register.

  @retval TRUE  if  VMD is supported
  @retval False if  VMD is not supported
**/
BOOLEAN
EFIAPI
IsVmdSupported (
  VOID
  );


/**
  ProgramVmdTempBars: Program VMD temporary bars.

  @param[in]     VmdCfgBarBase    temporary address for CfgBar.
  @param[in]     VmdMemBar1Base   temporary address for CfgBar.
  @param[in]     VmdMemBar1Base   temporary address for CfgBar.

  @retval VOID
**/
VOID
EFIAPI
ProgramVmdTempBars(
  IN UINT32 VmdCfgBarBase,
  IN UINT32 VmdMemBar1Base,
  IN UINT32 VmdMemBar2Base
  );

/**
  ClearVmdTempBars: Clear VMD temporary bars assigned during PEI phase.

  @retval VOID
**/
VOID
EFIAPI
ClearVmdTempBars(
  VOID
  );

#endif /* _VMD_INFO_LIB_H_ */
