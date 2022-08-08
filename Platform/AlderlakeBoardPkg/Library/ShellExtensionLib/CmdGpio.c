/** @file
  Shell command `gpio` to read and write the required GPIO Pin

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/GpioLib.h>


/**
  GPIO Read and write commands

  @param[in]  Shell        shell instance
  @param[in]  Argc         number of command line arguments
  @param[in]  Argv         command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandGpioFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  );


CONST SHELL_COMMAND mShellCommandGpio = {
  L"gpio",
  L"Read or write GPIO pin",
  &ShellCommandGpioFunc
};

/**
  Write GPIO pin value.

  @param[in]  GpioPad      GPIO pad
  @param[in]  Value        Value to write to the GPIO pin
**/
EFI_STATUS
EFIAPI
GpioWrite (
  IN UINT32 GpioPad,
  IN UINT32 Value
)
{
  EFI_STATUS Status = EFI_INVALID_PARAMETER;
  GPIO_CONFIG PadConfig;

  if (GpioGetPadConfig(GpioPad, &PadConfig)){
    ShellPrint (L"Write Failed! GpioGetPadConfig() failed\n");
    return EFI_ABORTED;
  }

  if((Value != 0) && (Value != 1)){
    ShellPrint (L"Write Failed! value: %d unsupported\n", Value);
    return EFI_ABORTED;
  }

  if((PadConfig.Direction == GpioDirOut) || (PadConfig.Direction == GpioDirInOut) ||
  (PadConfig.Direction == GpioDirInInvOut)){
    Status = GpioSetOutputValue(GpioPad, Value);
    if (!EFI_ERROR (Status)) {
        ShellPrint (L"GPIO write value: %d to pad %08x Success\n", Value, GpioPad);
    }
    else{
        ShellPrint (L"GPIO write Failed: %d\n", Status);
    }
  }
  else{
    ShellPrint (L"Write Failed! GPIO direction is not set as Output\n");
  }

  return Status;
}


/**
  Read GPIO Pin status.

  @param[in]  GpioPad         GPIO pad

**/
EFI_STATUS
EFIAPI
GpioRead (
  IN UINT32 GpioPad
)
{
  UINT32  Value;
  EFI_STATUS Status = EFI_INVALID_PARAMETER;
  GPIO_CONFIG PadConfig;

  if (GpioGetPadConfig(GpioPad, &PadConfig)){
    ShellPrint (L"Read Failed! GpioGetPadConfig() failed\n");
    return EFI_ABORTED;
  }

  if((PadConfig.Direction == GpioDirOut) || (PadConfig.Direction == GpioDirInOut) ||
  (PadConfig.Direction == GpioDirInInvOut)){
    Status = GpioGetOutputValue (GpioPad, &Value);
  }
  else if ((PadConfig.Direction == GpioDirIn) ||(PadConfig.Direction == GpioDirInInv)){
    Status = GpioGetInputValue(GpioPad, &Value);
  }
  else{
    ShellPrint (L"Read Failed! GPIO is not configured as Input or Output\n");
    return EFI_ABORTED;
  }

  if (!EFI_ERROR (Status)) {
    ShellPrint (L"GPIO pad %08x read value: %d\n", GpioPad, Value);
  }
  else{
    ShellPrint (L"GPIO read Failed: %d\n", Status);
  }

  return Status;
}

/**
  Read or write GPIO Pin.

  @param[in]  Shell        Shell instance
  @param[in]  Argc         Number of command line arguments
  @param[in]  Argv         Command line arguments

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ShellCommandGpioFunc (
  IN SHELL  *Shell,
  IN UINTN   Argc,
  IN CHAR16 *Argv[]
  )
{
  EFI_STATUS Status = EFI_INVALID_PARAMETER;
  UINT32 Value;
  UINT32 GpioPad;

  if ((Argc == 3) && (StrCmp (Argv[1], L"read") == 0)){
    GpioPad = (UINT32)StrHexToUintn (Argv[2]);
    Status = GpioRead(GpioPad);
  }
  else if ((Argc == 4) && (StrCmp (Argv[1], L"write") == 0)){
    GpioPad = (UINT32)StrHexToUintn (Argv[2]);
    Value = (UINT32)StrHexToUintn (Argv[3]);
    Status = GpioWrite(GpioPad, Value);
  }
  else{
    goto Usage;
  }

  return Status;

Usage:
  ShellPrint (L"Usage: %s read [GpioPad] \n", Argv[0]);
  ShellPrint (L"       %s write [GpioPad] [0|1]  (0 - set GPIO output level low; 1 - set GPIO output level high)\n", Argv[0]);
  ShellPrint (L"       #GPIOPad value (in hex) can be obtained from corresponding platform header file for the required GPIO pin\n");
  ShellPrint (L"       examples:\n");
  ShellPrint (L"       # Read GPIO pad 0x09020014 for GPIO pin A20\n");
  ShellPrint (L"         read 0x09020014\n");
  ShellPrint (L"       # Write 1 to GPIO pad 0x09020014, to set GPIO pin A20 output level as high\n");
  ShellPrint (L"         write 0x09020014 1\n");
  ShellPrint (L"       # Write 0 to GPIO pad 0x09020014, to set GPIO pin A20 output level as low\n");
  ShellPrint (L"         write 0x09020014 0\n");

  return EFI_ABORTED;
}
