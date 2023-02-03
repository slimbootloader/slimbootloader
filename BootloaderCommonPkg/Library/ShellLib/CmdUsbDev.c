/** @file
  Shell command `usbdev` to view all usb device list

  Copyright (c) 2022 - 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BootloaderCommonLib.h>
#include <Library/ShellLib.h>
#include <Library/UsbBlockIoLib.h>
#include <Library/UsbInitLib.h>
#include <Library/UsbBusLib.h>
#include <Guid/OsBootOptionGuid.h>

/**
  Usb enumeration and listing commands

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandUsbDevFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );


CONST SHELL_COMMAND ShellCommandUsbDev = {
  L"usbdev",
  L"Display USB Devices",
  &ShellCommandUsbDevFunc
};


/**
  Usb enumeration and listing commands

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandUsbDevFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  EFI_STATUS         Status;
  UINTN              BaseAddress;
  UINT32             UsbIoCount;
  UINTN              Index;
  PEI_USB_IO_PPI     **UsbIoArray;
  CONST CHAR16       *NameStr;
  UINTN              StringSize;
  UINTN              StringIndex;
  UINTN              CharFound;
  UINT16             DeviceCounter;
  PLT_DEVICE_TABLE   *DeviceTable;
  PLT_DEVICE         *Device;
  UINTN              DeviceIndexNumber;

  DeviceIndexNumber = 1;
  DeviceTable = (PLT_DEVICE_TABLE *)GetDeviceTable();

  for(DeviceCounter = 0; DeviceCounter < DeviceTable->DeviceNumber; DeviceCounter++) {
    Device = &DeviceTable->Device[DeviceCounter];

    if (Device->Type != OsBootDeviceUsb) {
      continue;
    }

    BaseAddress = Device->Dev.DevAddr;
    if (!(BaseAddress & 0xFF000000)) {
      BaseAddress = TO_MM_PCI_ADDRESS (BaseAddress);
    }

    Status = InitUsbDevices (BaseAddress);
    if (!EFI_ERROR(Status)) {
      Status = GetUsbDevices ((PEI_USB_IO_PPI **)&UsbIoArray, &UsbIoCount);
    }
    if (EFI_ERROR(Status)) {
      ShellPrint (L"Failed to initialize USB bus !\n");
      return Status;
    }


    for (Index = 0; Index < UsbIoCount; Index++) {
      NameStr = GetUsbDeviceNameString (UsbIoArray[Index]);
      if (NameStr == NULL) {
        NameStr = L"N/A";
      }
      StringSize = StrSize (NameStr);

      CharFound = 0;
      for (StringIndex = 0; StringIndex < StringSize; StringIndex++) {
        if (NameStr[StringIndex] == 0) {
          break;
        }
        //
        // Checking the NameStr is not combination of spaces and tabs
        //
        if (((char)NameStr[StringIndex]) != 32 && ((char)NameStr[StringIndex]) != 9) {
          CharFound++;
        }
      }
      if (CharFound == 0) {
        NameStr = L"Unnamed Device";
      }
      ShellPrint (L"    USB Device %2d: %s\n", DeviceIndexNumber++, NameStr);
    }

    Status = DeinitUsbDevices();
    if (EFI_ERROR(Status)) {
      ShellPrint (L"Failed to Deinitialize USB bus !\n");
      return Status;
    }
  }

  return EFI_SUCCESS;
}
