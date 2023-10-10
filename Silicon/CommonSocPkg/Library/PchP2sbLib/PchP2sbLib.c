/** @file
  P2SB sideband access lib

  Copyright (c) 2023, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/SocInfoLib.h>
#include <Library/PciSegmentLib.h>
#include <Library/PchSbiAccessLib.h>
#include <Library/P2SbSidebandAccessLib.h>
#include <Register/P2sbRegs.h>
#include <IndustryStandard/Pci30.h>
#include <Library/GpioTopologyLib.h>
#include <Library/BaseMemoryLib.h>

#define P2SB_PCR_ADDRESS(MmioBase, Pid, Offset)  ((UINTN) MmioBase | (UINT32) (((Offset) & 0x0F0000) << 8) | ((UINT8)(Pid) << 16) | (UINT16) ((Offset) & 0xFFFF))

#define SA_SEG_NUM          0x00

/**
  Full function for executing P2SB SBI message
  Take care of that there is no lock protection when using SBI programming in both POST time and SMI.
  It will clash with POST time SBI programming when SMI happen.
  Programmer MUST do the save and restore opration while using the PchSbiExecution inside SMI
  to prevent from racing condition.
  This function will reveal P2SB and hide P2SB if it's originally hidden. If more than one SBI access
  needed, it's better to unhide the P2SB before calling and hide it back after done.

  When the return value is "EFI_SUCCESS", the "Response" do not need to be checked as it would have been
  SBI_SUCCESS. If the return value is "EFI_DEVICE_ERROR", then this would provide additional information
  when needed.

  @param[in] P2sbBase                   P2SB PCI config base
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
  IN     PCH_SBI_OPCODE  Opcode,
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

  @param[in] P2sbBase                   P2SB PCI config base
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
P2SbSbiExecution (
  IN     UINT64          P2sbBase,
  IN     P2SB_PID        Pid,
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
           0x0000,
           Data32,
           Response
           );
}

#ifndef MDEPKG_NDEBUG
/**
  Checks if the offset is valid for a given memory access width. Offset must align to width size.

  @param[in]  Offset  Offset of a register
  @param[in]  Size    Size of memory access in bytes

  @retval FALSE  Offset is not valid for a given memory access
  @retval TRUE   Offset is valid
**/
STATIC
BOOLEAN
P2SbIsPcrOffsetValid (
  IN UINT32  Offset,
  IN UINTN   Size
  )
{
  if (!IsP2sb20bPcrSupported ()) {
    if (((Offset & (Size - 1)) != 0) || (Offset > 0xFFFF)) {
      DEBUG ((DEBUG_WARN, "PCR offset error. Invalid Offset: %x Size: %x", Offset, Size));
      return FALSE;
    } else {
      return TRUE;
    }
  } else {
    if (((Offset & (Size - 1)) != 0) || (Offset > 0xFFFFF)) {
      DEBUG ((DEBUG_WARN, "PCR offset error. Invalid Offset: %x Size: %x", Offset, Size));
      return FALSE;
    } else {
      return TRUE;
    }
  }
}
#endif

/**
  Reads an 8-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank

  @return The 8-bit register value specified by Offset
**/
UINT8
P2SbSidebandRead8 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset
  )
{
  P2SB_SIDEBAND_REGISTER_ACCESS  *Private;

  Private = (P2SB_SIDEBAND_REGISTER_ACCESS *) This;

  switch (Private->AccessMethod) {
    case P2SbMmioAccess:
      Offset = Offset + Private->Offset;
      ASSERT (Private->P2SbCtrl.Mmio);
      return MmioRead8 (P2SB_PCR_ADDRESS (Private->P2SbCtrl.Mmio, Private->P2SbPid, Offset));
    default:
      DEBUG ((DEBUG_ERROR, "P2SB: Incorrect access method %d\n", Private->AccessMethod));
      return 0xFF;
  }
}

/**
  Writes an 8-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] Value   Value to write to register

  @return The 8-bit register value written to register
**/
UINT8
P2SbSidebandWrite8 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT8            Value
  )
{
  P2SB_SIDEBAND_REGISTER_ACCESS  *Private;

  Private = (P2SB_SIDEBAND_REGISTER_ACCESS *) This;


  switch (Private->AccessMethod) {
    case P2SbMmioAccess:
      Offset = Offset + Private->Offset;
      ASSERT (Private->P2SbCtrl.Mmio);
      MmioWrite8 (P2SB_PCR_ADDRESS (Private->P2SbCtrl.Mmio, Private->P2SbPid, Offset), Value);
      //
      // Readback to flush the write cycle and ensure ordering with primary
      //
      return MmioRead8 (P2SB_PCR_ADDRESS (Private->P2SbCtrl.Mmio, Private->P2SbPid, Offset));
    default:
      DEBUG ((DEBUG_ERROR, "P2SB: Incorrect access method %d\n", Private->AccessMethod));
      return 0xFF;
  }
}

/**
  Performs an 8-bit or on the register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] OrData  Data with which register should be OR-ed

  @return The 8-bit register value written to register
**/
UINT8
P2SbSidebandOr8 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT8            OrData
  )
{
  UINT8  Value;

  Value = P2SbSidebandRead8 (This, Offset);
  Value |= OrData;
  return P2SbSidebandWrite8 (This, Offset, Value);
}

/**
  Performs an 8-bit and on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed

  @return The 8-bit register value written to register
**/
UINT8
P2SbSidebandAnd8 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT8            AndData
  )
{
  UINT8  Value;

  Value = P2SbSidebandRead8 (This, Offset);
  Value &= AndData;
  return P2SbSidebandWrite8 (This, Offset, Value);
}

/**
  Performs an 8-bit and then or on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed
  @param[in] OrData   Data with which register should be OR-ed

  @return The 8-bit register value written to register
**/
UINT8
P2SbSidebandAndThenOr8 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT8            AndData,
  IN UINT8            OrData
  )
{
  UINT8  Value;

  Value = P2SbSidebandRead8 (This, Offset);
  Value &= AndData;
  Value |= OrData;
  return P2SbSidebandWrite8 (This, Offset, Value);
}

/**
  Reads a 16-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank

  @return The 16-bit register value specified by Offset
**/
UINT16
P2SbSidebandRead16 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset
  )
{
  P2SB_SIDEBAND_REGISTER_ACCESS  *Private;

#ifndef MDEPKG_NDEBUG
  ASSERT (P2SbIsPcrOffsetValid (Offset, 2));
#endif

  Private = (P2SB_SIDEBAND_REGISTER_ACCESS *) This;

  switch (Private->AccessMethod) {
    case P2SbMmioAccess:
      Offset = Offset + Private->Offset;
      ASSERT (Private->P2SbCtrl.Mmio);
      return MmioRead16 (P2SB_PCR_ADDRESS (Private->P2SbCtrl.Mmio, Private->P2SbPid, Offset));
    default:
      DEBUG ((DEBUG_ERROR, "P2SB: Incorrect access method %d\n", Private->AccessMethod));
      return 0xFFFF;
  }
}

/**
  Writes a 16-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] Value   Value to write to register

  @return The 16-bit register value written to register
**/
UINT16
P2SbSidebandWrite16 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT16           Value
  )
{
  P2SB_SIDEBAND_REGISTER_ACCESS  *Private;

#ifndef MDEPKG_NDEBUG
  ASSERT (P2SbIsPcrOffsetValid (Offset, 2));
#endif

  Private = (P2SB_SIDEBAND_REGISTER_ACCESS *) This;

  switch (Private->AccessMethod) {
    case P2SbMmioAccess:
      Offset = Offset + Private->Offset;
      ASSERT (Private->P2SbCtrl.Mmio);
      MmioWrite16 (P2SB_PCR_ADDRESS (Private->P2SbCtrl.Mmio, Private->P2SbPid, Offset), Value);
      //
      // Readback to flush the write cycle and ensure ordering with primary
      //
      return MmioRead16 (P2SB_PCR_ADDRESS (Private->P2SbCtrl.Mmio, Private->P2SbPid, Offset));
    default:
      DEBUG ((DEBUG_ERROR, "P2SB: Incorrect access method %d\n", Private->AccessMethod));
      return 0xFFFF;
  }
}

/**
  Performs a 16-bit or on the register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] OrData  Data with which register should be OR-ed

  @return The 16-bit register value written to register
**/
UINT16
P2SbSidebandOr16 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT16           OrData
  )
{
  UINT16  Value;

  Value = P2SbSidebandRead16 (This, Offset);
  Value |= OrData;
  return P2SbSidebandWrite16 (This, Offset, Value);
}

/**
  Performs a 16-bit and on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed

  @return The 16-bit register value written to register
**/
UINT16
P2SbSidebandAnd16 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT16           AndData
  )
{
  UINT16  Value;

  Value = P2SbSidebandRead16 (This, Offset);
  Value &= AndData;
  return P2SbSidebandWrite16 (This, Offset, Value);
}

/**
  Performs a 16-bit and then or on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed
  @param[in] OrData   Data with which register should be OR-ed

  @return The 16-bit register value written to register
**/
UINT16
P2SbSidebandAndThenOr16 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT16           AndData,
  IN UINT16           OrData
  )
{
  UINT16  Value;

  Value = P2SbSidebandRead16 (This, Offset);
  Value &= AndData;
  Value |= OrData;
  return P2SbSidebandWrite16 (This, Offset, Value);
}

/**
  Reads a 32-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank

  @return The 32-bit register value specified by Offset
**/
UINT32
P2SbSidebandRead32 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset
  )
{
  P2SB_SIDEBAND_REGISTER_ACCESS  *Private;

  Private = (P2SB_SIDEBAND_REGISTER_ACCESS *) This;
  Offset = Offset + Private->Offset;
  switch (Private->AccessMethod) {
     case P2SbMmioAccess:
#ifndef MDEPKG_NDEBUG
      ASSERT (P2SbIsPcrOffsetValid (Offset, 4));
#endif
      ASSERT (Private->P2SbCtrl.Mmio);
      return MmioRead32 (P2SB_PCR_ADDRESS (Private->P2SbCtrl.Mmio, Private->P2SbPid, Offset));
    default:
      DEBUG ((DEBUG_ERROR, "P2SB: Incorrect access method %d\n", Private->AccessMethod));
      return 0xFFFFFFFF;
  }
}

/**
  Writes a 32-bit register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] Value   Value to write to register

  @return The 32-bit register value written to register
**/
UINT32
P2SbSidebandWrite32 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT32           Value
  )
{
  P2SB_SIDEBAND_REGISTER_ACCESS  *Private;

  Private = (P2SB_SIDEBAND_REGISTER_ACCESS *) This;
  Offset = Offset + Private->Offset;

  switch (Private->AccessMethod) {
    case P2SbMmioAccess:
#ifndef MDEPKG_NDEBUG
      ASSERT (P2SbIsPcrOffsetValid (Offset, 4));
#endif
      ASSERT (Private->P2SbCtrl.Mmio);
      MmioWrite32 (P2SB_PCR_ADDRESS (Private->P2SbCtrl.Mmio, Private->P2SbPid, Offset), Value);
      //
      // Readback to flush the write cycle and ensure ordering with primary
      //
      return MmioRead32 (P2SB_PCR_ADDRESS (Private->P2SbCtrl.Mmio, Private->P2SbPid, Offset));
    default:
      DEBUG ((DEBUG_ERROR, "P2SB: Incorrect access method %d\n", Private->AccessMethod));
      return 0xFFFFFFFF;
  }
}

/**
  Performs a 32-bit or on the register

  @param[in] This    Pointer to REGISTER_ACCESS
  @param[in] Offset  Offset of the register in the register bank
  @param[in] OrData  Data with which register should be OR-ed

  @return The 32-bit register value written to register
**/
UINT32
P2SbSidebandOr32 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT32           OrData
  )
{
  UINT32  Value;

  Value = P2SbSidebandRead32 (This, Offset);
  Value |= OrData;
  return P2SbSidebandWrite32 (This, Offset, Value);
}

/**
  Performs a 32-bit and on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed

  @return The 32-bit register value written to register
**/
UINT32
P2SbSidebandAnd32 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT32           AndData
  )
{
  UINT32  Value;

  Value = P2SbSidebandRead32 (This, Offset);
  Value &= AndData;
  return P2SbSidebandWrite32 (This, Offset, Value);
}

/**
  Performs a 32-bit and then or on the register

  @param[in] This     Pointer to REGISTER_ACCESS
  @param[in] Offset   Offset of the register in the register bank
  @param[in] AndData  Data with which register should be AND-ed
  @param[in] OrData   Data with which register should be OR-ed

  @return The 32-bit register value written to register
**/
UINT32
P2SbSidebandAndThenOr32 (
  IN REGISTER_ACCESS  *This,
  IN UINT32           Offset,
  IN UINT32           AndData,
  IN UINT32           OrData
  )
{
  UINT32  Value;

  Value = P2SbSidebandRead32 (This, Offset);
  Value &= AndData;
  Value |= OrData;
  return P2SbSidebandWrite32 (This, Offset, Value);
}

/**
  Builds P2SB sideband access.

  @param[in]  P2SbPid             Port id
  @param[in]  Fid                 Function id
  @param[in]  RegisterSpace       Target register space
  @param[in]  AccessMethod        Access method
  @param[in]  PostedWrites        If TRUE writes sent through sideband msg will be posted
  @param[out] P2SbSidebandAccess  On output an initialized sideband access descriptor

  @retval TRUE   Access initialized successfuly
  @retval FALSE  Failed to initialize access
**/
BOOLEAN
BuildP2SbSidebandAccess (
  IN P2SB_PID                        P2SbPid,
  IN UINT16                          Fid,
  IN P2SB_REGISTER_SPACE             RegisterSpace,
  IN P2SB_SIDEBAND_ACCESS_METHOD     AccessMethod,
  IN BOOLEAN                         PostedWrites,
  OUT P2SB_SIDEBAND_REGISTER_ACCESS  *P2SbSidebandAccess
  )
{
  P2SbSidebandAccess->Access.Read8        = P2SbSidebandRead8;
  P2SbSidebandAccess->Access.Write8       = P2SbSidebandWrite8;
  P2SbSidebandAccess->Access.Or8          = P2SbSidebandOr8;
  P2SbSidebandAccess->Access.And8         = P2SbSidebandAnd8;
  P2SbSidebandAccess->Access.AndThenOr8   = P2SbSidebandAndThenOr8;
  P2SbSidebandAccess->Access.Read16       = P2SbSidebandRead16;
  P2SbSidebandAccess->Access.Write16      = P2SbSidebandWrite16;
  P2SbSidebandAccess->Access.Or16         = P2SbSidebandOr16;
  P2SbSidebandAccess->Access.And16        = P2SbSidebandAnd16;
  P2SbSidebandAccess->Access.AndThenOr16  = P2SbSidebandAndThenOr16;
  P2SbSidebandAccess->Access.Read32       = P2SbSidebandRead32;
  P2SbSidebandAccess->Access.Write32      = P2SbSidebandWrite32;
  P2SbSidebandAccess->Access.Or32         = P2SbSidebandOr32;
  P2SbSidebandAccess->Access.And32        = P2SbSidebandAnd32;
  P2SbSidebandAccess->Access.AndThenOr32  = P2SbSidebandAndThenOr32;

  P2SbSidebandAccess->AccessMethod        = AccessMethod;
  P2SbSidebandAccess->P2SbPid             = P2SbPid;
  P2SbSidebandAccess->Fid                 = Fid;
  P2SbSidebandAccess->RegisterSpace       = RegisterSpace;
  P2SbSidebandAccess->PostedWrites        = PostedWrites;
  P2SbSidebandAccess->P2SbCtrl            = SocGetP2SbController();
  P2SbSidebandAccess->Offset              = 0;

  return TRUE;
}
