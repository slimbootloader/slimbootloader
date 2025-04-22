/** @file
  SBL parameters for specific OS.

  Copyright (c) 2018 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SblParameter.h"


/**
  Add boot device parameter into command line

  Based on boot option list, if OS is booted from SPI device, generate
  a boot device list and pass the list address to OS from command line.

  @param[in]     BootOption           Current boot option
  @param[out]    CommandLine          Command line for adding new parameters
  @param[in]     MaxCmdSize           The max size of buffer CommandLine
  @param[in,out] ReservedCmdlineData  Reserved memory info about kernel command line for OS

  @retval  EFI_SUCCESS                New parameter added to command line
  @retval  Others                     Has error to add parameter
**/
EFI_STATUS
AppendBootDevices (
  IN     OS_BOOT_OPTION         *BootOption,
  OUT    CHAR8                  *CommandLine,
  IN     UINT32                  MaxCmdSize,
  IN OUT RESERVED_CMDLINE_DATA  *ReservedCmdlineData
  )
{
  OS_BOOT_OPTION_LIST       *OsBootOptionList;
  OS_BOOT_DEVICE_LIST       *BootDevicesData;
  UINT32                    BootDevicesSize;
  UINT32                    Index;
  CHAR8                     ParamValue[64];
  OS_BOOT_OPTION            *OsBootOption;

  if ((BootOption->DevType != OsBootDeviceSpi) && (BootOption->DevType != OsBootDeviceMemory)) {
    return EFI_SUCCESS;
  }

  //
  // Get Boot Image Info
  //
  OsBootOptionList = GetBootOptionList ();
  if ((OsBootOptionList == NULL) || (OsBootOptionList->OsBootOptionCount == 0)) {
    DEBUG ((DEBUG_ERROR, "Get OS boot option fail. \n"));
    return EFI_NOT_FOUND;
  }

  //
  // Allocate memory from the reserved region
  // Allocating pool might expose unwanted bootloader data to OS
  //
  BootDevicesSize = sizeof (OS_BOOT_DEVICE_LIST) + sizeof (OS_BOOT_DEVICE) * OsBootOptionList->OsBootOptionCount;
  BootDevicesData = (OS_BOOT_DEVICE_LIST *) AllocateReservedPages (EFI_SIZE_TO_PAGES (BootDevicesSize));
  if (BootDevicesData == NULL) {
    DEBUG ((DEBUG_INFO, "Could not allocate memory for boot device buffer"));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Update Boot Device Info
  //
  ReservedCmdlineData->BootDevicesData.Addr = (VOID *)BootDevicesData;
  ReservedCmdlineData->BootDevicesData.Size = BootDevicesSize;
  ReservedCmdlineData->BootDevicesData.AllocType = ImageAllocateTypePage;

  BootDevicesData->Revision        = OsBootOptionList->Revision;
  BootDevicesData->BootDeviceCount = OsBootOptionList->OsBootOptionCount;
  for (Index = 0; Index < BootDevicesData->BootDeviceCount; Index++) {
    OsBootOption = &OsBootOptionList->OsBootOption[Index];
    BootDevicesData->BootDevice[Index].DevType = (UINT8) OsBootOption->DevType;
    BootDevicesData->BootDevice[Index].HwPart  = OsBootOption->HwPart;
    BootDevicesData->BootDevice[Index].DevAddr = GetDeviceAddr (OsBootOption->DevType, OsBootOption->DevInstance);
  }

  AsciiSPrint (ParamValue, sizeof (ParamValue), " bootdevices=0x%x", BootDevicesData);
  AsciiStrCatS (CommandLine, MaxCmdSize, ParamValue);

  return EFI_SUCCESS;
}


/**
  Add Boot mode parameters to command line

  This function will get boot mode data from NVRAM, and pass it to OS
  kernel command line.

  @param[out]    CommandLine       Command line for adding new parameters
  @param[in]     MaxCmdSize        The max size of buffer CommandLine

  @retval  EFI_SUCCESS             Add boot mode success
  @retval  Others                  Add boot mode failure
**/
EFI_STATUS
AddBootModeCommandLine (
  OUT     CHAR8             *CommandLine,
  IN     UINT32             MaxCmdSize
  )
{
  CHAR8                     ParamValue[64];
  UINT8                     Magic;
  UINT8                     BootTarget;

  if (CommandLine == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // nvram msg is stored in CMOS offset 0x10 ~ 0x20
  // Here just do a simple magic check, will add more check together with
  // getting other fields later.
  //

  // Check Magic value
  IoWrite8 (0x74, 0x10);
  Magic = IoRead8 (0x75);

  if (Magic != 0x12) {
    DEBUG ((DEBUG_INFO, "CMOS msg invalid Magic [0x%x]\n", Magic));
    BootTarget = 0;
  } else {
    // Read boot target
    IoWrite8 (0x74, 0x17);
    BootTarget  = IoRead8 (0x75);
    BootTarget &= 0x1F;

    // Clear CMOS data by clear Magic field.
    IoWrite8 (0x74, 0x10);
    IoWrite8 (0x75, 0);
  }

  AsciiSPrint (ParamValue, sizeof (ParamValue), " boot=0x%x", BootTarget);
  AsciiStrCatS (CommandLine, MaxCmdSize, ParamValue);

  return EFI_SUCCESS;
}


/**
  Add time stamps parameters to command line

  This function will append boot perfomance data parameter to
  current command line.

  @param[out]     CommandLine           Command line for adding new parameters
  @param[in]      MaxCmdSize            The max size of buffer CommandLine
  @param[in,out]  ReservedCmdlineData   Reserved memory info about kernel command line for OS

  @return         EFI_SUCCESS           Add timestamp info to command line successfully
  @return         Others                An error while adding timestamp info

**/
EFI_STATUS
AddTimeStampsCommandLine (
  OUT    CHAR8                  *CommandLine,
  IN     UINT32                  MaxCmdSize,
  IN OUT RESERVED_CMDLINE_DATA  *ReservedCmdlineData
  )
{
  UINT32                    OsPerfDataSize;
  OS_PERF_DATA              *OsPerfData;
  BL_PERF_DATA              *BlPerfData;
  CHAR8                     ParamValue[64];

  if (CommandLine == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Get payload timestamps data
  //
  BlPerfData = GetPerfDataPtr ();
  if (BlPerfData == NULL) {
    DEBUG ((DEBUG_INFO, "Could not get payload performance data"));
    return EFI_NOT_FOUND;
  }
  //
  // Allocate memory from the reserved region
  // Allocating pool might expose unwanted bootloader data to OS
  //
  OsPerfDataSize = sizeof (OS_PERF_DATA) + sizeof (UINT64) * BlPerfData->PerfIndex;
  OsPerfData = AllocateReservedPages (EFI_SIZE_TO_PAGES (OsPerfDataSize));
  if (OsPerfData == NULL) {
    DEBUG ((DEBUG_INFO, "Could not allocate memory for TimeStamps buffer"));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Update Reserved Commandline Info
  //
  ReservedCmdlineData->OsPerfData.Addr = (VOID *)OsPerfData;
  ReservedCmdlineData->OsPerfData.Size = OsPerfDataSize;
  ReservedCmdlineData->OsPerfData.AllocType = ImageAllocateTypePage;

  OsPerfData->Signature = PERF_DATA_SIGNATURE;
  OsPerfData->Flags     = 0;
  OsPerfData->Count     = (UINT16)BlPerfData->PerfIndex;
  OsPerfData->Frequency = BlPerfData->FreqKhz;
  CopyMem (OsPerfData->TimeStamp, BlPerfData->TimeStamp, sizeof (UINT64) * BlPerfData->PerfIndex);
  AsciiSPrint (ParamValue, sizeof (ParamValue), " timestamps=%d@0x%x", BlPerfData->PerfIndex, OsPerfData);
  AsciiStrCatS (CommandLine, MaxCmdSize, ParamValue);

  return EFI_SUCCESS;
}


/**
  Add Sbl specific command line parameters

  Some Os requires SBL specific command line parameters.

  @param[in]     BootOption           Current boot option
  @param[out]    CommandLine          Command line for adding new parameters
  @param[in,out] CommandLineSize      The max size of buffer CommandLine when input.
                                      and the actual Command line string size, including NULL
                                      terminator when return.
  @param[in,out] ReservedCmdlineData  Reserved memory info about kernel command line for OS

**/
EFI_STATUS
EFIAPI
AddSblCommandLine (
  IN     OS_BOOT_OPTION         *BootOption,
  OUT    CHAR8                  *CommandLine,
  IN OUT UINT32                 *CommandLineSize,
  IN OUT RESERVED_CMDLINE_DATA  *ReservedCmdlineData
  )
{
  UINT32                    MaxCmdSize;

  MaxCmdSize = *CommandLineSize;

  //
  // Add OS Mender boot info parameter
  //
  if ((BootOption->BootFlags & BOOT_FLAGS_MENDER) != 0) {
    if ((BootOption->BootFlags & LOAD_IMAGE_FROM_BACKUP) == 0) {
      if (AsciiStrStr (CommandLine, " root=PARTLABEL=primary") == NULL) {
        AsciiStrCatS (CommandLine, MaxCmdSize, " root=PARTLABEL=primary");
      }
    } else {
      if (AsciiStrStr (CommandLine, " root=PARTLABEL=secondary") == NULL) {
        AsciiStrCatS (CommandLine, MaxCmdSize, " root=PARTLABEL=secondary");
      }
    }
  }

  return EFI_SUCCESS;
}

