/** @file
  Shell command `pci` to display PCI devices.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/ShellLib.h>
#include <IndustryStandard/Pci.h>
#include <Library/PciExpressLib.h>
#include "CmdPciUtil.h"

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

  Verbose = FALSE;

  // Check flags
  for (Index = 0; Index < Argc; Index++) {
    if (StrCmp (Argv[Index], L"-h") == 0) {
      goto usage;
    }
    if (StrCmp (Argv[Index], L"-v") == 0) {
      Verbose = TRUE;
    }
  }

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
      }
    }
  }

  return EFI_SUCCESS;

usage:
  ShellPrint (L"Usage: %s [-v]\n", Argv[0]);
  ShellPrint (L"\n"
              L"Flags:\n"
              L"  -v     Verbose\n");
  return EFI_ABORTED;
}
