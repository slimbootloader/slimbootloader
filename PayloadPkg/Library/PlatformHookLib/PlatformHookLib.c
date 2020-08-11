/** @file
  Payload implements one instance of Paltform Hook Library.

  Copyright (c) 2015 - 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/PlatformHookLib.h>
#include <Guid/SerialPortInfoGuid.h>

/**
  Get serial port stride register size.

  @retval  The serial port register stride size.

**/
UINT8
EFIAPI
GetSerialPortStrideSize (
  VOID
  )
{
  return (UINT8) PcdGet32 (PcdSerialRegisterStride);
}

/**
  Get serial port register base address.

  @retval  The serial port register base address.

**/
UINT32
EFIAPI
GetSerialPortBase (
  VOID
  )
{
  return (UINT32) PcdGet64 (PcdSerialRegisterBase);
}

/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not intiailzie the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @retval RETURN_SUCCESS       The platform specific initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  RETURN_STATUS                  Status;
  EFI_HOB_GUID_TYPE             *GuidHob;
  SERIAL_PORT_INFO              *PldSerialInfo;

  GuidHob = GetNextGuidHob (&gLoaderSerialPortInfoGuid, (VOID *)(UINTN)PcdGet32 (PcdPayloadHobList));
  if (GuidHob == NULL) {
    ASSERT (FALSE);
    return RETURN_NOT_FOUND;
  }

  PldSerialInfo = (SERIAL_PORT_INFO *)GET_GUID_HOB_DATA (GuidHob);
  if (PldSerialInfo->Type == 2) { //MMIO
    Status = PcdSetBoolS (PcdSerialUseMmio, TRUE);
  } else { //IO
    Status = PcdSetBoolS (PcdSerialUseMmio, FALSE);
  }
  if (RETURN_ERROR (Status)) {
    return Status;
  }
  Status = PcdSet64S (PcdSerialRegisterBase, (UINT64) PldSerialInfo->BaseAddr);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  Status = PcdSet32S (PcdSerialRegisterStride, PldSerialInfo->RegWidth);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  Status = PcdSet32S (PcdSerialBaudRate, PldSerialInfo->Baud);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  Status = PcdSet32S (PcdSerialClockRate, PldSerialInfo->InputHertz);
  if (RETURN_ERROR (Status)) {
    return Status;
  }

  return RETURN_SUCCESS;
}
