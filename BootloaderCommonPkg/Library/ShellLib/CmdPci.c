/** @file
  Shell command `pci` to display PCI devices.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <IndustryStandard/Pci.h>
#include <Library/PciExpressLib.h>
#include "CmdPciUtil.h"

/**
  Read memory address given from mm command.

  @param[in]  Addr         Starting address to read from
  @param[in]  Width        Width of the value to read
  @param[in]  Count        Number of times past the start to read
  @param[in]  IsIoAddr     If the address given is an IO address

**/
VOID
EFIAPI
MmRead (
  IN UINTN   Addr,
  IN UINTN   Width,
  IN UINT32  Count,
  IN BOOLEAN IsIoAddr
);

/**
  Display PCI devices.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandPciFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );

CONST SHELL_COMMAND ShellCommandPci = {
  L"pci",
  L"Display PCI devices",
  &ShellCommandPciFunc
};

/**
  Display PCI devices.

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
EFIAPI
ShellCommandPciFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  UINTN                                   Bus, Dev, Func, Index;
  volatile PCI_DEVICE_INDEPENDENT_REGION *PciDev;
  PCI_CLASS_STRINGS                       ClassStrings;
  UINT32                                  ClassCode;
  BOOLEAN                                 Verbose;
  UINT8                                   Value;
  UINT32                                  PciAddr;
  UINT32                                  Count;
  UINTN                                   Addr;

  Verbose = FALSE;

  // Check flags
  Count   = 0;
  PciAddr = 0;
  for (Index = 1; Index < Argc; Index++) {
    if (StrCmp (Argv[Index], L"-h") == 0) {
      goto usage;
    }
    if (StrCmp (Argv[Index], L"-v") == 0) {
      Verbose = TRUE;
    } else {
      Value   = (UINT8)StrHexToUintn (Argv[Index]);
      switch (Count) {
      case 1:
        Value &= 0x1F;
        break;
      case 2:
        Value &= 0x07;
        break;
      default:
        break;
      }
      PciAddr = (PciAddr << 8) | Value;
      Count++;
    }
  }

  if ((Count != 0) && (Count != 3)) {
    goto usage;
  }

  if (Count == 3) {
    // Redirect to MM to dump PCI configuration
    PciAddr = PciAddr << 8;
    Addr = (UINTN) (PcdGet64 (PcdPciExpressBaseAddress) +
           PCI_EXPRESS_LIB_ADDRESS ((PciAddr >> 24) & 0xFF, (PciAddr >> 16) & 0xFF, (PciAddr >> 8) & 0xFF, PciAddr & 0xFF));
    MmRead (Addr, 4, 64, FALSE);
  } else {
    // Scan PCI config space and report present BDFs
    for (Bus = 0; Bus <= PCI_MAX_BUS; Bus++) {
      for (Dev = 0; Dev <= PCI_MAX_DEVICE; Dev++) {
        for (Func = 0; Func <= PCI_MAX_FUNC; Func++) {
          // Calculate MMIO address of PCI device configuration space
          PciDev = (volatile VOID *) (UINTN) (PcdGet64 (PcdPciExpressBaseAddress)
                                              + PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, 0));

          // Check Vendor ID to see if this device is present
          if (PciDev->VendorId == 0xffff) {
            if (Func == 0) {
              break;
            } else {
              continue;
            }
          }

          // Get string descriptions of the class/subclass/interface
          ClassCode = ( (PciDev->ClassCode[2] << 16 )
                        | (PciDev->ClassCode[1] << 8  )
                        | (PciDev->ClassCode[0])
                        );
          PciGetClassStrings (ClassCode, &ClassStrings);

          // Report bus/device/function
          ShellPrint (L"%02x:%02x.%x %04x:%04x %s\n", Bus, Dev, Func,
                      PciDev->VendorId,
                      PciDev->DeviceId,
                      ClassStrings.BaseClass);

          // Report extra device information if the verbose flag was given
          if (Verbose) {
            ShellPrint (L"Config Space Address: %p\n", PciDev);
            if (StrLen (ClassStrings.SubClass)) {
              ShellPrint (L"SubClass: %s", ClassStrings.SubClass);
              if (StrLen (ClassStrings.PIFClass)) {
                ShellPrint (L" (%s)", ClassStrings.PIFClass);
              }
            }
            ShellPrint (L"\n\n");
          }

          if ((Func == 0) && ((PciDev->HeaderType & HEADER_TYPE_MULTI_FUNCTION) == 0)) {
            // Skip sub functions, this is not a multi function device
            Func = PCI_MAX_FUNC;
          }

        }
      }
    }
  }

  return EFI_SUCCESS;

usage:
  ShellPrint (L"Usage: %s [bus dev func] [-v]\n", Argv[0]);
  ShellPrint (L"\n"
              L"Flags:\n"
              L"  -v     Verbose\n");
  return EFI_ABORTED;
}
