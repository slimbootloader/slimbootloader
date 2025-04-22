/** @file
  Base IPC library implementation.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PmcIpcLib.h>
#include <Library/SaCommonDefinitions.h>
#include <Library/InternalIpcLib.h>
#include "ScRegs/RegsPmc.h"

/**
  Checks the IPC channel is available for a new request.

  @retval  EFI_SUCCESS       Ready for a new IPC.
  @retval  EFI_NOT_READY     Not ready for a new IPC.

**/
EFI_STATUS
EFIAPI
ReadyForNewIpc (
  VOID
  )
{
  UINT32    PciCfgReg;
  UINT32    Value = 0;
  UINT32    Addr32;
  UINTN     PmcIpc1RegBaseAddr = 0;

  PciCfgReg = (UINT32) MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PMC_IPC1, PCI_FUNCTION_NUMBER_PMC_IPC1, 0x10);  //Bar0 Reg
  Addr32 = MmioRead32 (PciCfgReg) & 0xFFFFFFF0;
  PciCfgReg = (UINT32) MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PMC_IPC1, PCI_FUNCTION_NUMBER_PMC_IPC1, 0x4);  //Sts Reg
  Value = MmioRead32 (PciCfgReg);

  if (Addr32 != 0  &&  Addr32 != 0xFFFFFFF0 && ((Value & 0x06) == 0x06)) {
    PmcIpc1RegBaseAddr = Addr32;
  } else {
    return EFI_NOT_READY;
  }

  return InternalReadyForNewIpc (PmcIpc1RegBaseAddr);
}


/**
  Sends an IPC from the x86 CPU to the SCU and waits for the SCU to process the
  specified opcode.

  @param[in]       Command          The Command identifier to send in the IPC packet.
  @param[in]       SubCommand       The SubCommand identifier to send in the IPC packet.
  @param[in, out]  Buffer           Pointer to buffer associated with MessageId.  This is an optional
                                    patrametert that may be NULL for some MessageId values.
  @param[in, out]  BufferSize       The size, in bytes, of Buffer.  Ignored if Buffer is NULL.

  @retval          IPC_STATUS

**/
EFI_STATUS
EFIAPI
IpcSendCommandEx (
  IN      UINT32  Command,
  IN      UINT8   SubCommand,
  IN OUT  VOID    *Buffer,    OPTIONAL
  IN OUT  UINTN   BufferSize
  )
{
  UINT32    PciCfgReg;
  UINT32    Value = 0;
  UINT32    Addr32;
  UINTN     PmcIpc1RegBaseAddr = 0;

  PciCfgReg = (UINT32) MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PMC_IPC1, PCI_FUNCTION_NUMBER_PMC_IPC1, 0x10);  //Bar0 Reg
  Addr32 = MmioRead32 (PciCfgReg) & 0xFFFFFFF0;
  PciCfgReg = (UINT32) MmPciAddress(0, 0, PCI_DEVICE_NUMBER_PMC_IPC1, PCI_FUNCTION_NUMBER_PMC_IPC1, 0x4);  //Sts Reg
  Value = MmioRead32 (PciCfgReg);

  if (Addr32 != 0  &&  Addr32 != 0xFFFFFFF0 && ((Value & 0x06) == 0x06)) {
    PmcIpc1RegBaseAddr = Addr32;
  } else {
    DEBUG ((DEBUG_ERROR, "Error IPC1 command failed MMIO address %x, status =%x \n", Addr32, Value));
    return EFI_NOT_READY;
  }

  return InternalIpcSendCommandEx (PmcIpc1RegBaseAddr, Command, SubCommand, Buffer, BufferSize);
}


/**
  Sends an IPC from the x86 CPU to the PMC and waits for the PMC to process the
  specified opcode.

  !! Use it only when you are sure that IPC PCI config space is no longer valid !!

  @param[in]      PciBar0               PCI config BAR0 for IPC.
  @param[in]      Command               The Command identifier to send in the IPC packet.
  @param[in]      SubCommand            The SubCommand identifier to send in the IPC packet.
  @param[in, out] Buffer                Pointer to buffer associated with MessageId.  This is an optional
                                        patrametert that may be NULL for some MessageId values.
  @param[in, out] BufferSize            The size, in bytes, of Buffer.  Ignored if Buffer is NULL.

**/
EFI_STATUS
EFIAPI
IpcSendCommandBar0Ex (
  IN      UINT32  PciBar0,
  IN      UINT32  Command,
  IN      UINT8   SubCommand,
  IN OUT  VOID    *Buffer,    OPTIONAL
  IN OUT  UINTN   BufferSize
  )
{
  if (PciBar0 != 0 && PciBar0 != 0xFFFFFFF0) {
    return InternalIpcSendCommandEx (PciBar0, Command, SubCommand, Buffer, BufferSize);
  }
  DEBUG ((DEBUG_ERROR, "IpcSendCommandBar0Ex Error IPC1 command failed, not valid address %x\n", PciBar0));

  return EFI_NOT_READY;
}

