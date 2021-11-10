/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _GPIO_INFO_LIB_H_
#define _GPIO_INFO_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <GpioConfig.h>

/**
  This function gets Group to GPE0 configuration

  @param[out] GpeDw0Value       GPIO Group to GPE_DW0 assignment
  @param[out] GpeDw1Value       GPIO Group to GPE_DW1 assignment
  @param[out] GpeDw2Value       GPIO Group to GPE_DW2 assignment
**/
VOID
EFIAPI
PmcGetGpioGpe (
  OUT UINT32    *GpeDw0Value,
  OUT UINT32    *GpeDw1Value,
  OUT UINT32    *GpeDw2Value
  );


/**
  Return opcode supported for writing to GPIO lock unlock register

  @retval UINT8   Lock Opcode
**/
UINT8
EFIAPI
GpioGetLockOpcode (
  VOID
  );


/**
  This internal procedure will check if group is within DeepSleepWell.

  @param[in]  Group               GPIO Group

  @retval GroupWell               TRUE:  This is DSW Group
                                  FALSE: This is not DSW Group
**/
BOOLEAN
EFIAPI
GpioIsDswGroup (
  IN  GPIO_GROUP         Group
  );


/**
  This procedure will retrieve address and length of GPIO info table

  @param[out]  GpioGroupInfoTableLength   Length of GPIO group table

  @retval Pointer to GPIO group table

**/
CONST GPIO_GROUP_INFO*
EFIAPI
GpioGetGroupInfoTable (
  OUT UINT32              *GpioGroupInfoTableLength
  );


/**
  Get GPIO Chipset ID specific to PCH generation and series

  @retval PCH Chipset ID
**/
UINT32
EFIAPI
GpioGetThisChipsetId (
  VOID
  );


/**
  Get information for GPIO Group required to program GPIO and PMC for desired 1-Tier GPE mapping

  @param[out] GpioGroupToGpeMapping        Table with GPIO Group to GPE mapping
  @param[out] GpioGroupToGpeMappingLength  GPIO Group to GPE mapping table length
**/
VOID
EFIAPI
GpioGetGroupToGpeMapping (
  OUT GPIO_GROUP_TO_GPE_MAPPING  **GpioGroupToGpeMapping,
  OUT UINT32                     *GpioGroupToGpeMappingLength
  );


/**
  This function provides GPIO Community PortIDs

  @param[out] NativePinsTable                Table with GPIO COMMx SBI PortIDs

  @retval     Number of communities
**/
UINT32
EFIAPI
GpioGetComSbiPortIds (
  OUT GPIO_PCH_SBI_PID    **GpioComSbiIds
  );


/**
  Get PCH PCR Address for this Register

  @param[in] Pid                        Port ID of the SBI message
  @param[in] Offset                     Offset of the Cfg Regsiter

  @retval UINT32                        Pch Pcr Address to access this register
**/
UINT32
EFIAPI
GetPchPcrAddress (
  IN     GPIO_PCH_SBI_PID               Pid,
  IN     UINT32                         Offset
  );


/**
  Get TypeC SBU Gpio Pad table

  @param[out] TableLength       Length of the TypeC SBU Gpio Pad table

  @retval Pointer to TypeC SBU Gpio Pad table
**/
GPIO_PAD*
EFIAPI
GpioGetTypeCSbuGpioPad (
  IN OUT   UINT32      *TableLength
  );


/**
  Get GPIO PCR Pad Cfg Offset

  @retval PAD CFG Offset
**/
UINT8
EFIAPI
GpioGetPcrPadCfgOffset(
  VOID
  );


/**
  This procedure will return GpioPad from Group and PadNumber

  @param[in] Group              GPIO group
  @param[in] PadNumber          GPIO PadNumber

  @retval GpioPad               GpioPad
**/
GPIO_PAD
EFIAPI
GpioGetGpioPadFromGroupAndPadNumber (
  IN GPIO_GROUP      Group,
  IN UINT32          PadNumber
  );


/**
  This procedure will return GpioPad from GroupIndex and PadNumber

  @param[in] GroupIndex         GPIO GroupIndex
  @param[in] PadNumber          GPIO PadNumber

  @retval GpioPad               GpioPad
**/
GPIO_PAD
EFIAPI
GpioGetGpioPadFromGroupIndexAndPadNumber (
  IN UINT32          GroupIndex,
  IN UINT32          PadNumber
  );


/**
  This procedure will get group from group index (0 based)

  @param[in] GroupIndex        Group Index

  @retval GpioGroup            Gpio Group
**/
GPIO_GROUP
EFIAPI
GpioGetGroupFromGroupIndex (
  IN UINT32        GroupIndex
  );


/**
  This procedure will get group index (0 based) from group

  @param[in] GpioGroup        Gpio Group

  @retval Value               Group Index
**/
UINT32
EFIAPI
GpioGetGroupIndexFromGroup (
  IN GPIO_GROUP        GpioGroup
  );


/**
  This procedure will get group number

  @param[in] GpioPad          Gpio Pad

  @retval Value               Group number
**/
GPIO_GROUP
EFIAPI
GpioGetGroupFromGpioPad (
  IN GPIO_PAD         GpioPad
  );


/**
  This procedure will get group index (0 based)

  @param[in] GpioPad          Gpio Pad

  @retval Value               Group Index
**/
UINT32
EFIAPI
GpioGetGroupIndexFromGpioPad (
  IN GPIO_PAD        GpioPad
  );


/**
  This procedure will get pad number (0 based) from Gpio Pad

  @param[in] GpioPad          Gpio Pad

  @retval Value               Pad Number
**/
UINT32
EFIAPI
GpioGetPadNumberFromGpioPad (
  IN GPIO_PAD        GpioPad
  );


/**
  This procedure will get Chipset ID from Gpio Pad

  @param[in] GpioPad          Gpio Pad

  @retval Value               PCH Chipset ID
**/
UINT32
EFIAPI
GpioGetChipsetIdFromGpioPad (
  IN GPIO_PAD        GpioPad
  );


/**
  This procedure will get Gpio Pad from Cfg Dword

  @param[in]  GpioItem         Pointer to the Gpio Cfg Data Item
  @param[out] GpioPad          Gpio Pad
**/
VOID
EFIAPI
GpioGetGpioPadFromCfgDw (
  IN  UINT32        *GpioItem,
  OUT GPIO_PAD      *GpioPad
  );

/**
  Convert GPIO group and pin number into GPIO pad.

  @param[in]  Group         Alphabet based GPIO group index.
  @param[in]  Pin           GPIO pin number.

  @retval                   GPIO pad, 0 indicates invalid pad.
**/
UINT32
GpioGroupPinToPad (
  IN UINT32     Group,
  IN UINT32     Pin
  );

#endif // _GPIO_INFO_LIB_H_
