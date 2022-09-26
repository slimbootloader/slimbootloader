/** @file
  Shell command `gpio` to read and write the required GPIO Pin

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/ShellLib.h>
#include <Library/GpioLib.h>
#include <GpioPinsVer2Lp.h>
#include <GpioPinsVer4S.h>
#include <Library/PchInfoLib.h>
#include <Library/GpioSiLib.h>


/*GPIO Group number for ADLN and ADLP in Alphabetic order from A to Z*/
GPIO_GROUP GpioGrpAdlnpTable[] = {
  GPIO_VER2_LP_GROUP_GPP_A,
  GPIO_VER2_LP_GROUP_GPP_B,
  GPIO_VER2_LP_GROUP_GPP_C,
  GPIO_VER2_LP_GROUP_GPP_D,
  GPIO_VER2_LP_GROUP_GPP_E,
  GPIO_VER2_LP_GROUP_GPP_F,
  0x0,
  GPIO_VER2_LP_GROUP_GPP_H,
  GPIO_VER2_LP_GROUP_GPP_I,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  GPIO_VER2_LP_GROUP_VGPIO,
  GPIO_VER2_LP_GROUP_GPD,
  0x0,
  GPIO_VER2_LP_GROUP_GPP_R,
  GPIO_VER2_LP_GROUP_GPP_S,
  GPIO_VER2_LP_GROUP_GPP_T,
  GPIO_VER2_LP_GROUP_GPP_U,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0
};

/*GPIO Group number for ADLS in Alphabetic order from A to Z*/
GPIO_GROUP GpioGrpAdlsTable[] = {
  GPIO_VER4_S_GROUP_GPP_A,
  GPIO_VER4_S_GROUP_GPP_B,
  GPIO_VER4_S_GROUP_GPP_C,
  GPIO_VER4_S_GROUP_GPP_D,
  GPIO_VER4_S_GROUP_GPP_E,
  GPIO_VER4_S_GROUP_GPP_F,
  GPIO_VER4_S_GROUP_GPP_G,
  GPIO_VER4_S_GROUP_GPP_H,
  GPIO_VER4_S_GROUP_GPP_I,
  GPIO_VER4_S_GROUP_GPP_J,
  GPIO_VER4_S_GROUP_GPP_K,
  0x0,
  0x0,
  0x0,
  GPIO_VER4_S_GROUP_VGPIO,
  GPIO_VER4_S_GROUP_GPD,
  0x0,
  GPIO_VER4_S_GROUP_GPP_R,
  GPIO_VER4_S_GROUP_GPP_S,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0,
  0x0
};


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
  Calculate GPIO group value from group array index

  @param[in]  GrpArrayIndex GPIO group array index
  @param[out] GpioGroup     GPIO group
**/
EFI_STATUS
EFIAPI
GpioGetGroup(
  IN UINT32 GrpArrayIndex,
  OUT UINT32 *GpioGroup)
{
  *GpioGroup = 0;
  if (IsPchS()) {
    *GpioGroup = GpioGrpAdlsTable[GrpArrayIndex];
  }
  else if (IsPchP() || IsPchN()) {
    *GpioGroup = GpioGrpAdlnpTable[GrpArrayIndex];
  }

  if (*GpioGroup == 0) {
    ShellPrint(L"This GPIO group is not present in ADL platform\n");
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Calculate GPIO group from command line argument

  @param[in]  Subcmd      GPIO command argument
  @param[out] GroupValue  GPIO group value
**/
EFI_STATUS
EFIAPI
ProcessCmd(
  IN CHAR16  *Subcmd,
  OUT UINT32 *GroupValue)
{
  UINT32 GroupName;
  EFI_STATUS Status = EFI_SUCCESS;
  if (StrLen(Subcmd) == 1){
    if ((Subcmd[0] >= 'A') && (Subcmd[0] <= 'Z')) {
      GroupName = (UINT32)(Subcmd[0] - 'A');
    }
    else if ((Subcmd[0] >= 'a') && (Subcmd[0] <= 'z')) {
      GroupName = (UINT32)(Subcmd[0] - 'a');
    }
    else {
      ShellPrint(L"Invalid GPIO group\n");
      Status = EFI_INVALID_PARAMETER;
    }
  }
  else {
      ShellPrint(L"Invalid GPIO group\n");
      Status = EFI_INVALID_PARAMETER;
  }

  if (!EFI_ERROR(Status)) {
    Status = GpioGetGroup(GroupName, GroupValue);
  }

  return Status;
}

/**
  Calculate GPIO pad value from pad group and pin number.

  @param[in]  PadGroup      GPIO pad group
  @param[in]  PinNumber     GPIO pin number
**/
UINT32
EFIAPI
GpioPadcalc(
    IN UINT32 PadGroup,
    IN UINT32 PinNumber
)
{
  UINT32 GpioPad;
  UINT32 GroupIndex;

  GpioPad = 0;
  GroupIndex = GpioGetGroupIndexFromGroup(PadGroup);

  if (IsPchS()) {
    GpioPad = (PinNumber | (GroupIndex << 16)) | (GPIO_VER4_S_CHIPSET_ID << 24);
  }
  else if (IsPchP()) {
    GpioPad = (PinNumber | (GroupIndex << 16)) | (GPIO_VER2_LP_CHIPSET_ID << 24);
  }
  else {
    ShellPrint(L"Unknown PCH\n");
  }
  return GpioPad;
 }

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

  if((PadConfig.Direction == GpioDirOut) || (PadConfig.Direction == GpioDirInOut) ||
  (PadConfig.Direction == GpioDirInInvOut)){
    Status = GpioSetOutputValue(GpioPad, Value);
    if (!EFI_ERROR (Status)) {
      ShellPrint(L"GPIO write success\n");
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
    ShellPrint(L"Read success! Read value: %d\n", Value);
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
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 Value;
  UINT32 GpioPad;
  UINT32 GroupValue;
  UINT32 GpioPin;
  UINT32 Index = 1;

  if ((Argc == 4) && (StrCmp (Argv[Index], L"read") == 0)){
    Index ++;
    Status = ProcessCmd(Argv[Index], &GroupValue);

    if (!EFI_ERROR(Status)) {
      GpioPin = (UINT32)StrDecimalToUintn(Argv[3]);
      //In ADL platform all pin number ranges from 0 to 23
      if ((GpioPin >= 0) && (GpioPin < 24)){
        GpioPad = GpioPadcalc(GroupValue, GpioPin);
        if (GpioPad != 0) {
          Status = GpioRead(GpioPad);
        }
      }
      else {
        ShellPrint(L"Invalid Pin Number\n");
        Status = EFI_INVALID_PARAMETER;
      }
    }
  }
  else if ((Argc == 5) && (StrCmp (Argv[Index], L"write") == 0)){
    Index ++;
    Status = ProcessCmd(Argv[Index], &GroupValue);

    if (!EFI_ERROR(Status)) {
      GpioPin = (UINT32)StrDecimalToUintn(Argv[3]);
      //In ADL platform all pin number ranges between 0 to 23
      if ((GpioPin >= 0) && (GpioPin < 24)){
        GpioPad = GpioPadcalc(GroupValue, GpioPin);
        if (GpioPad != 0) {
          Value = (UINT32)StrDecimalToUintn(Argv[4]);
          if ((Value != 0) && (Value != 1)) {
            ShellPrint(L"Error! GPIO supported write values [0|1]\n");
          }
          else {
            Status = GpioWrite(GpioPad, Value);
          }
        }
      }
      else {
        ShellPrint(L"Invalid Pin Number\n");
        Status = EFI_INVALID_PARAMETER;
      }
    }
  }
  else{
    goto Usage;
  }

  return Status;

Usage:
  ShellPrint (L"Usage: %s read [GPIO Group] [Pin Number] \n", Argv[0]);
  ShellPrint (L"       %s write [GPIO Group] [Pin Number] [0|1]  (0 - set GPIO output level low; 1 - set GPIO output level high)\n", Argv[0]);
  ShellPrint (L"       [GPIO Group] example list\n");
  ShellPrint (L"       # GPP_A - A\n");
  ShellPrint (L"       # GPP_B - B\n");
  ShellPrint (L"       # GROUP_VGPIO - O\n");
  ShellPrint (L"       # GROUP_GPD - P\n");
  ShellPrint (L"       Example Commands:\n");
  ShellPrint (L"       # Read GPIO pin A20\n");
  ShellPrint (L"         gpio read A 20\n");
  ShellPrint (L"       # Write 1 to set output level as High for GPIO pin A20 \n");
  ShellPrint (L"         gpio write A 20 1\n");
  ShellPrint (L"       # Write 0 to set output level as Low for GPIO pin A20 \n");
  ShellPrint (L"         gpio write A 20 0\n");

  return EFI_ABORTED;
}
