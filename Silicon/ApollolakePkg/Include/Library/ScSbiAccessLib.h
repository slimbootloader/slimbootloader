/** @file
  Header file for ScSbiAccessLib.

  Copyright (c) 2012 - 2018, Intel Corporation. All rights reserved.<BR>

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SC_SBI_ACCESS_LIB_H_
#define _SC_SBI_ACCESS_LIB_H_

#define PCI_VENDOR_ID_OFFSET            0x00
#define PCI_DEVICE_ID_OFFSET            0x02

#define R_PCH_P2SB_E0                   0xE0

/**
  Read PCR register.
  It returns PCR register and size in 4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrRead32 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  OUT UINT32                            *OutData
  );

/**
  Read PCR register.
  It returns PCR register and size in 2bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrRead16 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  OUT UINT16                            *OutData
  );

/**
  Read PCR register.
  It returns PCR register and size in 1bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of this Port ID
  @param[out] OutData                   Buffer of Output Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrRead8 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  OUT UINT8                             *OutData
  );

/**
  Write PCR register.
  It programs PCR register and size in 4bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrWrite32 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT32                            InData
  );

/**
  Write PCR register.
  It programs PCR register and size in 2bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrWrite16 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT16                            InData
  );

/**
  Write PCR register.
  It programs PCR register and size in 1bytes.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  InData                    Input Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrWrite8 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT8                             InData
  );

/**
  Reads an 4-byte Pcr register, performs a bitwise AND followed by a bitwise
  inclusive OR, and writes the result back to the 4-byte Pcr register.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrAndThenOr32 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT32                            AndData,
  IN  UINT32                            OrData
  );

/**
  Reads an 2-byte Pcr register, performs a bitwise AND followed by a bitwise
  inclusive OR, and writes the result back to the 2-byte Pcr register.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrAndThenOr16 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT16                            AndData,
  IN  UINT16                            OrData
  );

/**
  Reads an 1-byte Pcr register, performs a bitwise AND followed by a bitwise
  inclusive OR, and writes the result back to the 1-byte Pcr register.
  The Offset should not exceed 0xFFFF and must be aligned with size.

  @param[in]  Pid                       Port ID
  @param[in]  Offset                    Register offset of Port ID.
  @param[in]  AndData                   AND Data. Must be the same size as Size parameter.
  @param[in]  OrData                    OR Data. Must be the same size as Size parameter.

  @retval     EFI_SUCCESS               Successfully completed.
  @retval     EFI_INVALID_PARAMETER     Invalid offset passed.

**/
EFI_STATUS
PchPcrAndThenOr8 (
  IN  SC_SBI_PID                        Pid,
  IN  UINT16                            Offset,
  IN  UINT8                             AndData,
  IN  UINT8                             OrData
  );

/**
  Hide P2SB device.

  @param[in]  P2sbBase                  Pci base address of P2SB controller.

  @retval     EFI_SUCCESS               Always return success.

**/
EFI_STATUS
PchHideP2sb (
  IN UINTN                                 P2sbBase
  );

/**
  Reveal P2SB device.
  Also return the original P2SB status which is for Hidding P2SB or not after.
  If OrgStatus is not NULL, then TRUE means P2SB is unhidden,
  and FALSE means P2SB is hidden originally.

  @param[in]  P2sbBase                  Pci base address of P2SB controller.
  @param[out] OrgStatus                 Original P2SB hidding/unhidden status

  @retval     EFI_SUCCESS               Always return success.

**/
EFI_STATUS
PchRevealP2sb (
  IN UINTN                                 P2sbBase,
  OUT BOOLEAN                              *OrgStatus
  );

//
// implemented in PchSbiAccess.c
//

/**
  PCH SBI Register structure

**/
typedef struct {
  UINT32            SbiAddr;
  UINT32            SbiExtAddr;
  UINT32            SbiData;
  UINT16            SbiStat;
  UINT16            SbiRid;
} PCH_SBI_REGISTER_STRUCT;

/**
  PCH SBI opcode definitions

**/
typedef enum {
  MemoryRead             = 0x0,
  MemoryWrite            = 0x1,
  PciConfigRead          = 0x4,
  PciConfigWrite         = 0x5,
  PrivateControlRead     = 0x6,
  PrivateControlWrite    = 0x7,
  GpioLockUnlock         = 0x13
} PCH_SBI_OPCODE;

/**
  PCH SBI response status definitions

**/
typedef enum {
  SBI_SUCCESSFUL          = 0,
  SBI_UNSUCCESSFUL        = 1,
  SBI_POWERDOWN           = 2,
  SBI_MIXED               = 3,
  SBI_INVALID_RESPONSE
} PCH_SBI_RESPONSE;

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

  @param[in]      Pid                        Port ID of the SBI message
  @param[in]      Offset                     Offset of the SBI message
  @param[in]      Opcode                     Opcode
  @param[in]      Posted                     Posted message
  @param[in, out] Data32                     Read/Write data
  @param[out]     Response                   Response

  @retval         EFI_SUCCESS                Successfully completed.
  @retval         EFI_DEVICE_ERROR           Transaction fail
  @retval         EFI_INVALID_PARAMETER      Invalid parameter

**/
EFI_STATUS
EFIAPI
PchSbiExecution (
  IN     SC_SBI_PID                     Pid,
  IN     UINT64                         Offset,
  IN     PCH_SBI_OPCODE                 Opcode,
  IN     BOOLEAN                        Posted,
  IN OUT UINT32                         *Data32,
  OUT    UINT8                          *Response
  );

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

  @param[in]      Pid                        Port ID of the SBI message
  @param[in]      Offset                     Offset of the SBI message
  @param[in]      Opcode                     Opcode
  @param[in]      Posted                     Posted message
  @param[in]      Fbe                        First byte enable
  @param[in]      Bar                        Bar
  @param[in]      Fid                        Function ID
  @param[in, out] Data32                     Read/Write data
  @param[out]     Response                   Response

  @retval         EFI_SUCCESS                Successfully completed.
  @retval         EFI_DEVICE_ERROR           Transaction fail
  @retval         EFI_INVALID_PARAMETER      Invalid parameter

**/
EFI_STATUS
EFIAPI
PchSbiExecutionEx (
  IN     SC_SBI_PID                     Pid,
  IN     UINT64                         Offset,
  IN     PCH_SBI_OPCODE                 Opcode,
  IN     BOOLEAN                        Posted,
  IN     UINT16                         Fbe,
  IN     UINT16                         Bar,
  IN     UINT16                         Fid,
  IN OUT UINT32                         *Data32,
  OUT    UINT8                          *Response
  );

/**
  This function saves all PCH SBI registers.
  The save and restore operations must be done while using the PchSbiExecution inside SMM.
  It prevents the racing condition of PchSbiExecution re-entry between POST and SMI.
  Before using this function, make sure the P2SB is not hidden.

  @param[in, out] PchSbiRegister        Structure for saving the registers

  @retval         EFI_SUCCESS           Successfully completed.
  @retval         EFI_DEVICE_ERROR      Device is hidden.

**/
EFI_STATUS
EFIAPI
PchSbiRegisterSave (
  IN OUT PCH_SBI_REGISTER_STRUCT        *PchSbiRegister
  );

/**
  This function restores all PCH SBI registers
  The save and restore operations must be done while using the PchSbiExecution inside SMM.
  It prevents the racing condition of PchSbiExecution re-entry between POST and SMI.
  Before using this function, make sure the P2SB is not hidden.

  @param[in] PchSbiRegister               Structure for restoring the registers

  @retval    EFI_SUCCESS                  Successfully completed.
  @retval    EFI_DEVICE_ERROR             Device is hidden.

**/
EFI_STATUS
EFIAPI
PchSbiRegisterRestore (
  IN PCH_SBI_REGISTER_STRUCT            *PchSbiRegister
  );

#endif /* _SC_SBI_ACCESS_LIB_H_ */
