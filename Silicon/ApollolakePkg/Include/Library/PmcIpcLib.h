/** @file
  Base IPC library implementation.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PMC_IPC_LIB_
#define _PMC_IPC_LIB_

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>

//
// IPC Register Offsets to PMC BAR0 (IPC1: 0x0000 ~ 0xFFF; GCR: 0x1000 ~ 0x1FFF)
//
#define R_IPC_CMD_OFFSET           0x00
#define R_IPC_STS_OFFSET           0x04
#define R_IPC_SPTR_OFFSET          0x08
#define R_IPC_DPTR_OFFSET          0x0C
#define R_IPC_WBUF_OFFSET          0x80
#define R_IPC_RBUF_OFFSET          0x90
#define S_IPC_RWBUF_SIZE           16

//
// IPC_CMD type
//
typedef enum {
  IPC_CMD_NONE,
  IPC_CMD_READ,
  IPC_CMD_WRITE,
  IPC_CMD_MAX
} IPC_CMD_TYPE;

//
// IPC_CMD register
//
typedef union {
  struct {
    UINT32  CommandId   :8;
    UINT32  Msi         :1;
    UINT32  Reserved1   :3;
    UINT32  SubCmdId    :4;
    UINT32  Size        :8;
    UINT32  Reserved2   :8;
  } Bits;
  UINT32  Uint32;
} IPC_COMMAND;

//
// IPC_STS register
//
typedef union {
  struct {
    UINT32  Busy        :1;
    UINT32  Error       :1;
    UINT32  Reserved1   :2;
    UINT32  CmdId       :4;
    UINT32  IndicatorId :8;
    UINT32  ErrCode     :8;
    UINT32  Reserved2   :8;
  } Bits;
  UINT32  Uint32;
} IPC_STATUS;

///
/// IPC command opcodes
///
/// @note: These commands need to double-confirm since FAS has conflicting definitions
///

#define IPC_CMD_ID_CAPSULE_UPDATE_RESET             0xE5  ///< PMC Capsule Update Reset
#define IPC_SUBCMD_ID_CAPSULE_UPDATE_RESET          0x00  ///<   Reserved, ignore by PMC

#define IPC_CMD_ID_PMC_VER                          0xE7  ///< PMC FW version
#define IPC_SUBCMD_ID_PMC_VER_READ                  0x00  ///<   Read PMC FW version

#define IPC_CMD_ID_EMI_RFI_SUPPORT                  0xE8  ///< PMC EMI/RFI Support, Note that this command is not valid on BXT-A
#define IPC_SUBCMD_ID_SSC_APPLY_NOW                 0x01  ///<   Apply the SSC setting immidiately
#define IPC_SUBCMD_ID_SSC_DO_NOT_APPLY_NOW          0x00  ///<   Apply the SSC setting on next cold reset or s0ix exit

#define IPC_CMD_ID_TELEMETRY                        0xEB  ///< Cmds used to read and write PMC telemetry registers
#define IPC_SUBID_TELEMETRY_EVENT_READ              0x00  ///<    Reads a TELEM_EVENT register
#define IPC_SUBID_TELEMETRY_EVENT_WRITE             0x01  ///<    Writes a TELEM_EVENT register

#define IPC_SUBID_TELEMETRY_INFO_READ               0x02  ///<  Reads the TELEM_INFO register

#define IPC_SUBID_TELEMETRY_TRACE_READ              0x03  ///<  Read MBB_TELEM_TRACE_MASK register
#define IPC_SUBID_TELEMETRY_TRACE_WRITE             0x04  ///<  Writes the MBB_TELEM_TRACE_MASK register

#define IPC_SUBID_TELEMETRY_TRACE_CTL_READ          0x05  ///<  Reads the MBB_TELEM_CONTROL register
#define IPC_SUBID_TELEMETRY_TRACE_CTL_WRITE         0x06  ///<  Writes the MBB_TELEM_CONTROL register

#define IPC_SUBID_TELEMETRY_EVENT_CTL_READ          0x07  ///<  Reads the TELEM_EVENT_CTL register
#define IPC_SUBID_TELEMETRY_EVENT_CTL_WRITE         0x08  ///<  Writes the TELEM_EVENT_CTL register

#define IPC_CMD_ID_SOUTH_IP_UNGATE                  0xE9  ///< Control South IP powergating for Visa Programming
#define IPC_SUBCMD_ID_SOUTH_IP_ALLOW_GATING         0x0   ///<   South IP allow gating
#define IPC_SUBCMD_ID_SOUTH_IP_UN_GATING            0x1   ///<   South IP Ungate

#define IPC_CMD_ID_FW_MSG_CTRL                      0xEA  ///< Read and write the PMC FW_MSG_CTL register
#define IPC_SUBCMD_ID_FW_MSG_CTRL_READ              0x0   ///<   Read PMC FW_MSG_CTL register (1 byte returned from PMC)
#define IPC_SUBCMD_ID_FW_MSG_CTRL_WRITE             0x1   ///<  Write PMC FW_MSG_CTL register (1 byte written to PMC)

#define IPC_CMD_ID_PM_DEBUG                         0xEC  ///< Cmds supported for PM debug
#define IPC_SUBCMD_ID_LPSS_DEBUG_UART_ENABLE        0X0B  ///<   LPSS Debug UART Enable
#define IPC_SUBCMD_ID_LPSS_DEBUG_UART_DISABLE       0X0C  ///<   LPSS Debug UART Disable
#define IPC_SUBCMD_ID_PM_DEBUG_LTR_IGNORE_REPORT    0x0D  ///<   LTR Report (IP to report is specified via IPC_WBUF)
#define IPC_SUBCMD_ID_PM_DEBUG_LTR_IGNORE_READ      0x0E  ///<   LTR ignore read
#define IPC_SUBCMD_ID_PM_DEBUG_LTR_IGNORE_WRITE     0x0F  ///<   LTR ignore write

#define IPC_CMD_ID_PHY_CFG                          0xEE  ///< Used by BIOS to communicate mphy configuration information, which MPHY is specified in IPC_WBUF
#define IPC_SUBCMD_ID_PHY_CFG_COMPLETE              0x00  ///<   PHY configuration is complete
#define IPC_SUBCMD_ID_PHY_WILL_NOT_CFG              0x01  ///<   PHY will not be configured

#define IPC_CMD_ID_USB3P3                           0xF0  ///< Cause PMC to turn on, off USB 3.3V rail
#define IPC_SUBCMD_ID_USB3P3_OFF                    0x00  ///<   Turn off USB 3.3V rail
#define IPC_SUBCMD_ID_USB3P3_ON                     0x01  ///<   Turn on USB 3.3V rail

#define IPC_CMD_ID_BLACKLIST_SEL                    0xEF  ///< PMIC Blacklist Select IPC1
#define IPC_SUBCMD_BLACKLIST_IAIns                  0x00  ///<   Use IA Insecure Blacklist to filter PMIC Accesses
#define IPC_SUBCMD_BLACKLIST_IASec                  0x02  ///<   Use IA Secure Blacklist to filter PMIC Accesses

#define IPC_CMD_ID_PMIC                             0xFF  ///< PMIC Register read and write
#define IPC_SUBCMD_ID_PMIC_READ                     0x00  ///<   PMIC Register write
#define IPC_SUBCMD_ID_PMIC_WRITE                    0x01  ///<   PMIC Register read
#define IPC_SUBCMD_ID_PMIC_READ_MOD_WRITE           0x02  ///<   PMIC Register read-modify-write

#define INVALID_IPC_CMD                             0x80000000

typedef enum
{
  ipcStsCode_none = 0,             ///< 0, No error
  ipcStsCode_cmdNotSupported,      ///< 1, IPC_CMD.CMD_ID is correct, but there is a problem in formatting of the other IPC_CMD fields.  Example: wrong IPC_CMD.SIZE.
  ipcStsCode_cmdNotServiced,       ///< 2, IPC_CMD is correct, but service you asked for is permanently disabled (fused off, blacklisted, etc).
  ipcStsCode_unableToServiceNow,   ///< 3, IPC_CMD is correct, but PMC cannot service at this time. Example: command which can only be called once, or can only happen at a particular time.
  ipcStsCode_cmdInvalid,           ///< 4, IPC_CMD.CMD_ID is invalid
  ipcStsCode_cmdFailed,            ///< 5, Attempted to service the command, but it failed for some reason!  Example: pmic access failed/timed out.
} ipcStsCode_t;

#pragma pack(1)
typedef struct {
  UINT16  Address;
  UINT8   Value;
} IPC_CMD_ID_PMIC_READ_WRITE_FORMAT;

typedef struct {
  UINT16  Address;
  UINT8   Value;
  UINT8   Mask;
} IPC_CMD_ID_PMIC_READ_MODIFY_WRITE_FORMAT;

typedef union {
  IPC_CMD_ID_PMIC_READ_WRITE_FORMAT         ReadWrite[5];
  IPC_CMD_ID_PMIC_READ_MODIFY_WRITE_FORMAT  ReadModifyWrite[4];
  UINT32                                    Uint32[4];
} IPC_CMD_ID_PMIC_BUFFER;
#pragma pack()

/**
  Checks the IPC channel is available for a new request

  @retval  EFI_SUCCESS    Ready for a new IPC
  @retval  EFI_NOT_READY  Not ready for a new IPC

**/
EFI_STATUS
EFIAPI
ReadyForNewIpc (
  VOID
  );

/**
  Sends an IPC from the x86 CPU to the PMC and waits for the PMC to process the
  specified opcode.

  @param[in]   MessageId    The message identifier to send in the IPC packet.

**/
EFI_STATUS
EFIAPI
IpcSendCommand (
  IN UINT32  MessageId
  );

/**
  Sends an IPC from the x86 CPU to the PMC and waits for the PMC to process the
  specified opcode.

  @param[in]      Command                    The Command identifier to send in the IPC packet.
  @param[in]      SubCommand                 The SubCommand identifier to send in the IPC packet.
  @param[in, out] Buffer                     Pointer to buffer associated with MessageId.  This is an optional
                                             patrametert that may be NULL for some MessageId values.
  @param[in, out] BufferSize                 The size, in bytes, of Buffer.  Ignored if Buffer is NULL.

**/
EFI_STATUS
EFIAPI
IpcSendCommandEx (
  IN      UINT32  Command,
  IN      UINT8   SubCommand,
  IN OUT  VOID    *Buffer,    OPTIONAL
  IN OUT  UINTN   BufferSize
  );

/**
  Sends an IPC from the x86 CPU to the PMC and waits for the PMC to process the
  specified opcode.

  !! Use it only when you are sure that IPC PCI config space is no longer valid !!

  @param[in]      PciBar0                    PCI config BAR0 for IPC.
  @param[in]      Command                    The Command identifier to send in the IPC packet.
  @param[in]      SubCommand                 The SubCommand identifier to send in the IPC packet.
  @param[in, out] Buffer                     Pointer to buffer associated with MessageId.  This is an optional
                                             patrametert that may be NULL for some MessageId values.
  @param[in, out] BufferSize                 The size, in bytes, of Buffer.  Ignored if Buffer is NULL.

**/
EFI_STATUS
EFIAPI
IpcSendCommandBar0Ex (
  IN      UINT32  PciBar0,
  IN      UINT32  Command,
  IN      UINT8   SubCommand,
  IN OUT  VOID    *Buffer,    OPTIONAL
  IN OUT  UINTN   BufferSize
  );

#endif    // __BXT_PMC_IPC_LIB__

