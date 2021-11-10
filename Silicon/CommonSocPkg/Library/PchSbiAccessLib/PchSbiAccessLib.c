/** @file
  PCH SBI access library.

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Base.h>
#include <Uefi/UefiBaseType.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/BootloaderCommonLib.h>
#include <IndustryStandard/Pci30.h>

/**
  Hide P2SB device.

  @param[in]  P2sbBase                  Pci base address of P2SB controller.

  @retval EFI_SUCCESS                   Always return success.
**/
EFI_STATUS
HideP2sb (
  IN  UINTN                             P2sbBase
  )
{
  MmioWrite8 (P2sbBase + R_P2SB_CFG_E0 + 1, BIT0);
  return EFI_SUCCESS;
}

/**
  Reveal P2SB device.
  Also return the original P2SB status which is for Hidding P2SB or not after.
  If OrgStatus is not NULL, then TRUE means P2SB is unhidden,
  and FALSE means P2SB is hidden originally.

  @param[in]  P2sbBase                  Pci base address of P2SB controller.
  @param[out] OrgStatus                 Original P2SB hidding/unhidden status

  @retval EFI_SUCCESS                   Always return success.
**/
EFI_STATUS
RevealP2sb (
  IN  UINTN                             P2sbBase,
  OUT BOOLEAN                           *OrgStatus
  )
{
  BOOLEAN                               DevicePresent;

  DevicePresent = (MmioRead16 (P2sbBase + PCI_VENDOR_ID_OFFSET) != 0xFFFF);
  if (OrgStatus != NULL) {
    *OrgStatus = DevicePresent;
  }
  if (!DevicePresent) {
    MmioWrite8 (P2sbBase + R_P2SB_CFG_E0 + 1, 0);
  }
  return EFI_SUCCESS;
}

/**
  Execute PCH SBI message
  Take care of that there is no lock protection when using SBI programming in both POST time and SMI.
  It will clash with POST time SBI programming when SMI happen.
  Programmer MUST do the save and restore opration while using the PchSbiExecution inside SMI
  to prevent from racing condition.
  This function will reveal P2SB and hide P2SB if it's originally hidden. If more than one SBI access
  needed, it's better to unhide the P2SB before calling and hide it back after done.

  When the return value is "EFI_SUCCESS", the "Response" do not need to be checked as it would have been
  SBI_SUCCESS. If the return value is "EFI_DEVICE_ERROR", then this would provide additional information
  when needed.

  @param[in] Pid                        Port ID of the SBI message
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
PchSbiExecution (
  IN     PCH_SBI_PID                    Pid,
  IN     UINT64                         Offset,
  IN     PCH_SBI_OPCODE                 Opcode,
  IN     BOOLEAN                        Posted,
  IN OUT UINT32                         *Data32,
  OUT    UINT8                          *Response
  )
{
  //
  // Check address valid
  //
  if (((UINT32) Offset & 0x3) != 0) {
    //
    // Warning message for the address not DWORD alignment.
    //
    DEBUG ((DEBUG_INFO, "PchSbiExecution: Be careful that the address is not DWORD alignment.\n"));
  }

  return PchSbiExecutionEx ( Pid,
           Offset,
           Opcode,
           Posted,
           0x000F,
           0x0000,
           0x0000,
           Data32,
           Response
           );
}

/**
  Full function for executing PCH SBI message
  Take care of that there is no lock protection when using SBI programming in both POST time and SMI.
  It will clash with POST time SBI programming when SMI happen.
  Programmer MUST do the save and restore opration while using the PchSbiExecution inside SMI
  to prevent from racing condition.
  This function will reveal P2SB and hide P2SB if it's originally hidden. If more than one SBI access
  needed, it's better to unhide the P2SB before calling and hide it back after done.

  When the return value is "EFI_SUCCESS", the "Response" do not need to be checked as it would have been
  SBI_SUCCESS. If the return value is "EFI_DEVICE_ERROR", then this would provide additional information
  when needed.

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
EFIAPI
PchSbiExecutionEx (
  IN     PCH_SBI_PID                    Pid,
  IN     UINT64                         Offset,
  IN     PCH_SBI_OPCODE                 Opcode,
  IN     BOOLEAN                        Posted,
  IN     UINT16                         Fbe,
  IN     UINT16                         Bar,
  IN     UINT16                         Fid,
  IN OUT UINT32                         *Data32,
  OUT    UINT8                          *Response
  )
{
  EFI_STATUS                            Status;
  UINTN                                 P2sbBase;
  BOOLEAN                               P2sbOrgStatus;
  UINTN                                 Timeout;
  UINT16                                SbiStat;

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

  P2sbOrgStatus = FALSE;
  P2sbBase      = MM_PCI_ADDRESS (0, PCI_DEVICE_NUMBER_PCH_P2SB, PCI_FUNCTION_NUMBER_PCH_P2SB, 0);
  RevealP2sb (P2sbBase, &P2sbOrgStatus);
  if (MmioRead16 ((UINTN)P2sbBase + PCI_VENDOR_ID_OFFSET) == 0xFFFF) {
    ASSERT (FALSE);
    return EFI_DEVICE_ERROR;
  }
  ///
  /// BWG Section 2.2.1
  /// 1. Poll P2SB PCI offset D8h[0] = 0b
  /// Make sure the previous opeartion is completed.
  ///
  Timeout = 0xFFFFFFF;
  while (Timeout > 0) {
    SbiStat = MmioRead16 (P2sbBase + R_P2SB_CFG_SBISTAT);
    if ((SbiStat & B_P2SB_CFG_SBISTAT_INITRDY) == 0) {
      break;
    }
    Timeout--;
  }
  if (Timeout == 0) {
    Status = EFI_DEVICE_ERROR;
    goto ExitPchSbiExecutionEx;
  }
  //
  // Initial Response status
  //
  *Response = SBI_INVALID_RESPONSE;
  Status    = EFI_SUCCESS;
  SbiStat   = 0;
  ///
  /// 2. Write P2SB PCI offset D0h[31:0] with Address and Destination Port ID
  ///
  MmioWrite32 (P2sbBase + R_P2SB_CFG_SBIADDR, (UINT32) ((Pid << 24) | (UINT16) Offset));
  ///
  /// 3. Write P2SB PCI offset DCh[31:0] with extended address, which is expected to be 0 in SKL PCH.
  ///
  MmioWrite32 (P2sbBase + R_P2SB_CFG_SBIEXTADDR, (UINT32) RShiftU64 (Offset, 16));
  ///
  /// 5. Set P2SB PCI offset D8h[15:8] = 00000110b for read
  ///    Set P2SB PCI offset D8h[15:8] = 00000111b for write
  //
  // Set SBISTAT[15:8] to the opcode passed in
  // Set SBISTAT[7] to the posted passed in
  //
  MmioAndThenOr16 (
    (P2sbBase + R_P2SB_CFG_SBISTAT),
    (UINT16) ~(B_P2SB_CFG_SBISTAT_OPCODE | B_P2SB_CFG_SBISTAT_POSTED),
    (UINT16) ((Opcode << 8) | (Posted << 7))
    );
  ///
  /// 6. Write P2SB PCI offset DAh[15:0] = F000h
  ///
  //
  // Set RID[15:0] = Fbe << 12 | Bar << 8 | Fid
  //
  MmioWrite16 (
    (P2sbBase + R_P2SB_CFG_SBIRID),
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
      MmioWrite32 ((P2sbBase + R_P2SB_CFG_SBIDATA), *Data32);
      break;
    default:
      ///
      /// 4. Write P2SB PCI offset D4h[31:0] with dummy data such as 0,
      /// because all D0-DFh register range must be touched in SKL PCH
      /// for a successful SBI transaction.
      ///
      MmioWrite32 ((P2sbBase + R_P2SB_CFG_SBIDATA), 0);
      break;
  }
  ///
  /// 7. Set P2SB PCI offset D8h[0] = 1b, Poll P2SB PCI offset D8h[0] = 0b
  ///
  //
  // Set SBISTAT[0] = 1b, trigger the SBI operation
  //
  MmioOr16 (P2sbBase + R_P2SB_CFG_SBISTAT, (UINT16) B_P2SB_CFG_SBISTAT_INITRDY);
  //
  // Poll SBISTAT[0] = 0b, Polling for Busy bit
  //
  Timeout = 0xFFFFFFF;
  while (Timeout > 0) {
    SbiStat = MmioRead16 (P2sbBase + R_P2SB_CFG_SBISTAT);
    if ((SbiStat & B_P2SB_CFG_SBISTAT_INITRDY) == 0) {
      break;
    }
    Timeout--;
  }
  if (Timeout == 0) {
    //
    // If timeout, it's fatal error.
    //
    Status = EFI_DEVICE_ERROR;
  } else {
    ///
    /// 8. Check if P2SB PCI offset D8h[2:1] = 00b for successful transaction
    ///
    *Response = (UINT8) ((SbiStat & B_P2SB_CFG_SBISTAT_RESPONSE) >> N_P2SB_CFG_SBISTAT_RESPONSE);
    if (*Response == SBI_SUCCESSFUL) {
      switch (Opcode) {
        case MemoryRead:
        case PciConfigRead:
        case PrivateControlRead:
          ///
          /// 9. Read P2SB PCI offset D4h[31:0] for SBI data
          ///
          *Data32 = MmioRead32 (P2sbBase + R_P2SB_CFG_SBIDATA);
          break;
        default:
          break;
      }
      Status = EFI_SUCCESS;
    } else {
      Status = EFI_DEVICE_ERROR;
    }
  }

ExitPchSbiExecutionEx:
  if (!P2sbOrgStatus) {
    HideP2sb (P2sbBase);
  }
  return Status;
}

