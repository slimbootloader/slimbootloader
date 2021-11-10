/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _STAGE2_BOARD_INIT_LIB_INTERNAL_H_
#define _STAGE2_BOARD_INIT_LIB_INTERNAL_H_

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/GpioLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/SocInitLib.h>
#include <Library/BoardInitLib.h>
#include <Library/ConfigDataLib.h>
#include <Library/SerialPortLib.h>
#include <Library/VariableLib.h>
#include <Library/BootloaderCoreLib.h>
#include <Library/BlMemoryAllocationLib.h>
#include <Library/FspSupportLib.h>
#include <Library/BoardSupportLib.h>
#include <Library/ContainerLib.h>
#include <Library/GraphicsInitLib.h>
#include <Library/HobLib.h>
#include <Library/PlatformHookLib.h>
#include <Library/SmbiosInitLib.h>
#include <Library/PrintLib.h>
#include <Guid/GraphicsInfoHob.h>
#include <Guid/SystemTableInfoGuid.h>
#include <Guid/SerialPortInfoGuid.h>
#include <Guid/SmmInformationGuid.h>
#include <FspsUpd.h>
#include <BlCommon.h>
#include <GlobalNvsArea.h>
#include <PlatformBase.h>
#include <ConfigDataDefs.h>
#include "RegAccess.h"
#include "GpioTbl.h"

#pragma pack (1)

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER      Header;
  UINT64                           Reserved;
} EFI_ACPI_TEST_TABLE;

typedef struct {
  UINT32     Signature;
  UINT32     ResX;
  UINT32     ResY;
} GRAPHICS_DATA;

#pragma pack ()


#define  GRAPHICS_DATA_SIG         SIGNATURE_32 ('Q', 'G', 'F', 'X')

#define  EFI_ACPI_OEM_ID           {'O','E','M','I','D',' '}   // OEMID 6 bytes long
#define  EFI_ACPI_OEM_TABLE_ID     SIGNATURE_64('O','E','M','T','A','B','L','E') // OEM table id 8 bytes long
#define  EFI_ACPI_OEM_REVISION     0x00000001
#define  EFI_ACPI_CREATOR_ID       SIGNATURE_32('C','R','E','A')
#define  EFI_ACPI_CREATOR_REVISION 0x01000001

/**
  Enable legacy region decoding.

**/
VOID
EFIAPI
EnableLegacyRegions (
  VOID
);

#endif
