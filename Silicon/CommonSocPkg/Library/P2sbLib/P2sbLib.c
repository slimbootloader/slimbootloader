/** @file
  P2SB sideband access lib

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PciSegmentLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/P2sbLib.h>
#include <Register/P2sbRegs.h>
#include <IndustryStandard/Pci30.h>

#define P2SB_PCR_ADDRESS(MmioBase, Pid, Offset)  ((UINTN) MmioBase | (UINT32) (((Offset) & 0x0F0000) << 8) | ((UINT8)(Pid) << 16) | (UINT16) ((Offset) & 0xFFFF))

/**
  Full function for executing P2SB SBI message
  Take care of that there is no lock protection when using SBI programming in both POST time and SMI.
  It will clash with POST time SBI programming when SMI happen.
  Programmer MUST do the save and restore operation while using the PchSbiExecution inside SMI
  to prevent from racing condition.
  This function will reveal P2SB and hide P2SB if it's originally hidden. If more than one SBI access
  needed, it's better to unhide the P2SB before calling and hide it back after done.

  When the return value is "EFI_SUCCESS", the "Response" do not need to be checked as it would have been
  SBI_SUCCESS. If the return value is "EFI_DEVICE_ERROR", then this would provide additional information
  when needed.

  @param[in] P2sbBase                   P2SB PCI address (PCI_LIB_ADDRESS(bus, device, fun, 0)
  @param[in] Pid                        Port ID of the SBI message
  @param[in] Offset                     Offset of the SBI message
  @param[in] Opcode                     Opcode
  @param[in] Posted                     Posted message
  @param[in] Fbe                        First byte enable
  @param[in] Bar                        Bar
  @param[in] Fid                        Function ID
  @param[in, out] Data32                Read/Write data
  @param[out] Response                  Response

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_DEVICE_ERROR              Transaction fail
  @retval EFI_INVALID_PARAMETER         Invalid parameter
  @retval EFI_TIMEOUT                   Timeout while waiting for response
**/
EFI_STATUS
P2SbSbiExecutionEx (
  IN     UINT64           P2sbBase,
  IN     P2SB_PID         Pid,
  IN     UINT64           Offset,
  IN     PCH_SBI_OPCODE   Opcode,
  IN     BOOLEAN          Posted,
  IN     UINT16           Fbe,
  IN     UINT16           Bar,
  IN     UINT16           Fid,
  IN OUT UINT32           *Data32,
  OUT    UINT8            *Response
  )
{
  INT32   Timeout;
  UINT16  SbiStat;

  //
  // Check opcode valid
  //
  switch (Opcode) {
    case MemoryRead:
    case MemoryWrite:
    case PciConfigRead:
    case PciConfigWrite:
    case PrivateControlRead:
    case PrivateControlWrite:
    case GpioLockUnlock:
      break;
    default:
      return EFI_INVALID_PARAMETER;
      break;
  }

  if (PciSegmentRead16 (P2sbBase + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    ASSERT (FALSE);
    return EFI_DEVICE_ERROR;
  }
  ///
  /// BWG Section 2.2.1
  /// 1. Poll P2SB PCI offset D8h[0] = 0b
  /// Make sure the previous opeartion is completed.
  ///
  Timeout = 0xFFFFFFF;
  SbiStat = 0;
  while (Timeout > 0) {
    SbiStat = PciSegmentRead16 (P2sbBase + R_P2SB_CFG_SBISTAT_V2);
    if ((SbiStat & B_P2SB_CFG_SBISTAT_INITRDY) == 0) {
      break;
    }
    Timeout--;
  }
  if (Timeout == 0) {
    return EFI_TIMEOUT;
  }
  //
  // Initial Response status
  //
  *Response = SBI_INVALID_RESPONSE;
  ///
  /// 2. Write P2SB PCI offset D0h[31:0] with Address and Destination Port ID
  ///
  PciSegmentWrite32 (P2sbBase + R_P2SB_CFG_SBIADDR_V2, (UINT32) ((Pid << 24) | (UINT16) Offset));
  ///
  /// 3. Write P2SB PCI offset DCh[31:0] with extended address, which is expected to be 0 in CNL PCH.
  ///
  PciSegmentWrite32 (P2sbBase + R_P2SB_CFG_SBIEXTADDR_V2, (UINT32) RShiftU64 (Offset, 16));
  ///
  /// 5. Set P2SB PCI offset D8h[15:8] = 00000110b for read
  ///    Set P2SB PCI offset D8h[15:8] = 00000111b for write
  //
  // Set SBISTAT[15:8] to the opcode passed in
  // Set SBISTAT[7] to the posted passed in
  //
  PciSegmentAndThenOr16 (
    (P2sbBase + R_P2SB_CFG_SBISTAT_V2),
    (UINT16) ~(B_P2SB_CFG_SBISTAT_OPCODE_V2 | B_P2SB_CFG_SBISTAT_POSTED),
    (UINT16) ((Opcode << 8) | (Posted << 7))
    );
  ///
  /// 6. Write P2SB PCI offset DAh[15:0] = F000h
  ///
  //
  // Set RID[15:0] = Fbe << 12 | Bar << 8 | Fid
  //
  PciSegmentWrite16 (
    (P2sbBase + R_P2SB_CFG_SBIRID_V2),
    (((Fbe & 0x000F) << 12) | ((Bar & 0x0007) << 8) | (Fid & 0x00FF))
    );

  switch (Opcode) {
    case MemoryWrite:
    case PciConfigWrite:
    case PrivateControlWrite:
    case GpioLockUnlock:
      ///
      /// 4. Write P2SB PCI offset D4h[31:0] with the intended data accordingly
      ///
      PciSegmentWrite32 ((P2sbBase + R_P2SB_CFG_SBIDATA_V2), *Data32);
      break;
    default:
      ///
      /// 4. Write P2SB PCI offset D4h[31:0] with dummy data such as 0,
      /// because all D0-DFh register range must be touched in CNL PCH
      /// for a successful SBI transaction.
      ///
      PciSegmentWrite32 ((P2sbBase + R_P2SB_CFG_SBIDATA_V2), 0);
      break;
  }
  ///
  /// 7. Set P2SB PCI offset D8h[0] = 1b, Poll P2SB PCI offset D8h[0] = 0b
  ///
  //
  // Set SBISTAT[0] = 1b, trigger the SBI operation
  //
  PciSegmentOr16 (P2sbBase + R_P2SB_CFG_SBISTAT_V2, (UINT16) B_P2SB_CFG_SBISTAT_INITRDY);
  //
  // Poll SBISTAT[0] = 0b, Polling for Busy bit
  //
  Timeout = 0xFFFFFFF;
  while (Timeout > 0) {
    SbiStat = PciSegmentRead16 (P2sbBase + R_P2SB_CFG_SBISTAT_V2);
    if ((SbiStat & B_P2SB_CFG_SBISTAT_INITRDY) == 0) {
      break;
    }
    Timeout--;
  }
  if (Timeout == 0) {
    //
    // If timeout, it's fatal error.
    //
    return EFI_TIMEOUT;
  } else {
    ///
    /// 8. Check if P2SB PCI offset D8h[2:1] = 00b for successful transaction
    ///
    *Response = (UINT8) ((SbiStat & B_P2SB_CFG_SBISTAT_RESPONSE_V2) >> N_P2SB_CFG_SBISTAT_RESPONSE);
    if (*Response == SBI_SUCCESSFUL) {
      switch (Opcode) {
        case MemoryRead:
        case PciConfigRead:
        case PrivateControlRead:
          ///
          /// 9. Read P2SB PCI offset D4h[31:0] for SBI data
          ///
          *Data32 = PciSegmentRead32 (P2sbBase + R_P2SB_CFG_SBIDATA_V2);
          break;
        default:
          break;
      }
      return EFI_SUCCESS;
    } else if (*Response == SBI_POWERDOWN) {
      return EFI_NO_RESPONSE;
    } else {
      return EFI_DEVICE_ERROR;
    }
  }
}

/**
  Execute P2SB SBI message
  Take care of that there is no lock protection when using SBI programming in both POST time and SMI.
  It will clash with POST time SBI programming when SMI happen.
  Programmer MUST do the save and restore opration while using the PchSbiExecution inside SMI
  to prevent from racing condition.
  This function will reveal P2SB and hide P2SB if it's originally hidden. If more than one SBI access
  needed, it's better to unhide the P2SB before calling and hide it back after done.

  When the return value is "EFI_SUCCESS", the "Response" do not need to be checked as it would have been
  SBI_SUCCESS. If the return value is "EFI_DEVICE_ERROR", then this would provide additional information
  when needed.

  @param[in] P2sbBase                   P2SB PCI address (PCI_LIB_ADDRESS(bus, device, fun, 0)
  @param[in] Pid                        Port ID of the SBI message
  @param[in] Fid                        Function ID
  @param[in] Offset                     Offset of the SBI message
  @param[in] Opcode                     Opcode
  @param[in] Posted                     Posted message
  @param[in, out] Data32                Read/Write data
  @param[out] Response                  Response

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_DEVICE_ERROR              Transaction fail
  @retval EFI_INVALID_PARAMETER         Invalid parameter
  @retval EFI_TIMEOUT                   Timeout while waiting for response
**/
EFI_STATUS
EFIAPI
P2SbSbiExecution (
  IN     UINT64          P2sbBase,
  IN     P2SB_PID        Pid,
  IN     UINT16          Fid,
  IN     UINT64          Offset,
  IN     PCH_SBI_OPCODE  Opcode,
  IN     BOOLEAN         Posted,
  IN OUT UINT32          *Data32,
  OUT    UINT8           *Response
  )
{
  return P2SbSbiExecutionEx (
           P2sbBase,
           Pid,
           Offset,
           Opcode,
           Posted,
           0x000F,
           0x0000,
           Fid,
           Data32,
           Response
           );
}


/**
  Read a register from P2SB

  @param[in] P2sbBase    P2SB PCI address (PCI_LIB_ADDRESS(bus, device, fun, 0) or SBREG_BAR address
  @param[in] Pid         Port ID of the SBI message
  @param[in] Fid         Function ID
  @param[in] Offset      Offset of the SBI message

  @retval EFI_SUCCESS    Successfully completed.
**/
UINT32
EFIAPI
P2sbRead32 (
  UINT64    P2sbBase,
  P2SB_PID  Pid,
  UINT16    Fid,
  UINT64    Offset
  )
{
  EFI_STATUS  Status;
  UINT32      Data32;
  UINT8       Response;

  if ((P2sbBase & 0xFFFFFF0000000) != 0) {
    Data32 = MmioRead32 (P2SB_PCR_ADDRESS (P2sbBase, Pid, Offset));
  } else {
    Status = P2SbSbiExecution (P2sbBase, Pid, Fid, Offset, PrivateControlRead, FALSE, &Data32, &Response);
    ASSERT_EFI_ERROR (Status);
  }
  return Data32;
}


/**
  Write a register from P2SB

  @param[in] P2sbBase    P2SB PCI address (PCI_LIB_ADDRESS(bus, device, fun, 0) or SBREG_BAR address
  @param[in] Pid         Port ID of the SBI message
  @param[in] Fid         Function ID
  @param[in] Offset      Offset of the SBI message
  @param[in] Data32      The data to write to register

  @retval EFI_SUCCESS    Successfully completed.
**/
UINT32
EFIAPI
P2sbWrite32 (
  UINT64    P2sbBase,
  P2SB_PID  Pid,
  UINT16    Fid,
  UINT64    Offset,
  UINT32    Data32
  )
{
  EFI_STATUS  Status;
  UINT8       Response;

  if ((P2sbBase & 0xFFFFFF0000000) != 0) {
    Status = MmioWrite32 (P2SB_PCR_ADDRESS (P2sbBase, Pid, Offset), Data32);
  } else {
    Status = P2SbSbiExecution (P2sbBase, Pid, Fid, Offset, PrivateControlWrite, FALSE, &Data32, &Response);
    ASSERT_EFI_ERROR (Status);
  }
  return Data32;

}


/**
  Performs a 32-bit and then or on the register

  @param[in] P2sbBase    P2SB PCI address (PCI_LIB_ADDRESS(bus, device, fun, 0) or SBREG_BAR address
  @param[in] Pid         Port ID of the SBI message
  @param[in] Offset      Offset of the register in the register bank
  @param[in] AndData     Data with which register should be AND-ed
  @param[in] OrData      Data with which register should be OR-ed

  @return The 32-bit register value written to register
**/
UINT32
EFIAPI
P2SbAndThenOr32 (
  IN UINT64           P2sbBase,
  IN P2SB_PID         Pid,
  IN UINT16           Fid,
  IN UINT32           Offset,
  IN UINT32           AndData,
  IN UINT32           OrData
  )
{
  UINT32              Value;

  Value = P2sbRead32 (P2sbBase, Pid, Fid, Offset);
  Value &= AndData;
  Value |= OrData;
  return P2sbWrite32 (P2sbBase, Pid, Fid, Offset, Value);
}


