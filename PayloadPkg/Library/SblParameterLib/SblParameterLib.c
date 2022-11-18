/** @file
  SBL parameters for specific OS.

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "SblParameter.h"

/**
  Decodes reset reason.

  @param[in]    ResetReason  reset reason id or enum

  @retval   String reset reason string
**/
STATIC
CHAR8 *
EFIAPI
GetResetReasonStr (
  IN  UINT8     ResetReason
  )
{
  // default reason string order for now if multiple reasons are set
  if ((ResetReason & ResetTcoWdt) != 0) {
    return "watchdog";
  } else if ((ResetReason & ResetWakeS3) != 0) {
    return "ResetWakeS3";
  } else if ((ResetReason & ResetWarm) != 0) {
    return "warm";
  } else if ((ResetReason & ResetCold) != 0) {
    return "cold";
  } else if ((ResetReason & ResetGlobal) != 0) {
    return "global";
  } else if ((ResetReason & ResetPowerOn) != 0) {
    return "power-on";
  } else if ((ResetReason & ResetUnknown) != 0) {
    return "unknown";
  } else {
    return "unspecified";
  }
}


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
  EFI_STATUS                Status;
  OS_CONFIG_DATA_HOB        *OsConfigData;
  LOADER_PLATFORM_INFO      *LoaderPlatformInfo;
  VOID                      *Buffer;
  CHAR8                     ParamValue[64];
  OS_BOOT_OPTION_LIST       *OsBootOptionList;
  UINT8                     ResetReason;
  UINT32                    SerialNumberLength;
  UINT8                     Data8;
  UINT32                    DiskBus;
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

  if (BootOption->ImageType != EnumImageTypeAdroid) {
    // currently these command line parameters are tested only with Android OS.
    return EFI_SUCCESS;
  }

  //
  // Allocate the reserved memory and update command line parameters
  //
  OsConfigData = (OS_CONFIG_DATA_HOB *) GetGuidHobData (NULL, NULL, &gOsConfigDataGuid);
  if ((OsConfigData != NULL) && (OsConfigData->OsCrashMemorySize != 0)) {
    Buffer = AllocateReservedPages (EFI_SIZE_TO_PAGES (OsConfigData->OsCrashMemorySize));
    if (Buffer == NULL) {
      DEBUG ((DEBUG_INFO, "Memory allocation error for OS crash memory\n"));
      return EFI_OUT_OF_RESOURCES;
    }

    //
    // Update OsCrashMemory Info
    //
    ReservedCmdlineData->OsCrashMemoryData.Addr = Buffer;
    ReservedCmdlineData->OsCrashMemoryData.Size = OsConfigData->OsCrashMemorySize;
    ReservedCmdlineData->OsCrashMemoryData.AllocType = ImageAllocateTypePage;

    AsciiSPrint (ParamValue, sizeof (ParamValue), " ramoops.mem_address=0x%p", Buffer);
    AsciiStrCatS (CommandLine, MaxCmdSize, ParamValue);

    AsciiSPrint (ParamValue, sizeof (ParamValue), " ramoops.mem_size=0x%x", OsConfigData->OsCrashMemorySize);
    AsciiStrCatS (CommandLine, MaxCmdSize, ParamValue);
  }

  // Add serial number
  LoaderPlatformInfo = (LOADER_PLATFORM_INFO *)GetLoaderPlatformInfoPtr();
  if (LoaderPlatformInfo != NULL) {
    SerialNumberLength = (UINT32)AsciiStrLen (LoaderPlatformInfo->SerialNumber);
    if (SerialNumberLength > 0 && SerialNumberLength <= MAX_SERIAL_NUMBER_LENGTH) {
      AsciiSPrint (ParamValue, sizeof (ParamValue), " androidboot.serialno=%a", LoaderPlatformInfo->SerialNumber);
      AsciiStrCatS (CommandLine, MaxCmdSize, ParamValue);
    }
  }

  // Add reset reason
  ResetReason = 0;
  OsBootOptionList = GetBootOptionList ();
  if (OsBootOptionList != NULL) {
    ResetReason = OsBootOptionList->ResetReason;
  }
  AsciiSPrint (ParamValue, sizeof (ParamValue), " reset=%a", GetResetReasonStr (ResetReason));
  AsciiStrCatS (CommandLine, MaxCmdSize, ParamValue);

  //
  // Add Crash mode parameter
  //
  if ((OsConfigData != NULL) && (OsConfigData->EnableCrashMode != 0)) {
    Data8 = (UINT8)~(ResetCold | ResetPowerOn | ResetGlobal | ResetWakeS3);
    if ((ResetReason & Data8) != 0) {
      AsciiStrCatS (CommandLine, MaxCmdSize, " boot_target=CRASHMODE");
    }
  }

  //
  // Add OS A/B boot info parameter
  //
  if ((BootOption->BootFlags & BOOT_FLAGS_MISC) != 0) {
    if ((BootOption->BootFlags & LOAD_IMAGE_FROM_BACKUP) == 0) {
      AsciiStrCatS (CommandLine, MaxCmdSize, " suffix=0");
    } else {
      AsciiStrCatS (CommandLine, MaxCmdSize, " suffix=1");
    }
  }

  //
  // Add boot media info
  //
  AsciiSPrint (ParamValue, sizeof (ParamValue), " bdev=%a", GetBootDeviceNameString (BootOption->DevType));
  AsciiStrCatS (CommandLine, MaxCmdSize, ParamValue);

  DiskBus = GetDeviceAddr (BootOption->DevType, BootOption->DevInstance);
  AsciiSPrint (ParamValue, sizeof (ParamValue), " diskbus=0x%x", DiskBus);
  AsciiStrCatS (CommandLine, MaxCmdSize, ParamValue);

  //
  // Add performance data parameters
  //
  Status = AddTimeStampsCommandLine (CommandLine, MaxCmdSize, ReservedCmdlineData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Add boot device list command line to OS from SPI flash (for fastboot).
  //
  Status = AppendBootDevices (BootOption, CommandLine, MaxCmdSize, ReservedCmdlineData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Add boot mode command line
  //
  Status = AddBootModeCommandLine (CommandLine, MaxCmdSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *CommandLineSize = (UINT32)AsciiStrLen (CommandLine);

  return Status;
}

