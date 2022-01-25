/**@file
  BUP Messages - HECI messages exchanged in pre-DID phase with CSME FW

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _BUP_MSGS_H
#define _BUP_MSGS_H

#include <MeBiosPayloadData.h>

#pragma pack(1)

///
/// BUP group IDs
///
#define BUP_COMMON_GROUP_ID 0xF0
#define BUP_MPHY_GROUP_ID   0xF2

///
/// Defines for Command
///
///
/// Defines for BUP_COMMON_GROUP_ID Command
///
#define DRAM_INIT_DONE_CMD       0x01
#define ME_ENABLE_CMD            0x03
#define HMRFPO_DISABLE_CMD       0x04
#define GET_ER_LOG_CMD           0x1B
#define GET_IP_FIRMWARE_CMD      0x21

///
/// Defines for BUP_MPHY_GROUP_ID Command
///
#define ICC_CMD  0x01


typedef struct {
  UINT32  BiosImrsBaLow;
  UINT32  BiosImrsBaHigh;
} DRAM_INIT_DONE_IMRS_REQ_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER          MkhiHeader;
  DRAM_INIT_DONE_IMRS_REQ_DATA ImrData;
  UINT32                       Flags;
  UINT8                        Reserved1[2];
  UINT8                        MemStatus;
  UINT8                        Reserved2;
} DRAM_INIT_DONE_CMD_REQ;


typedef struct {
  UINT32  AdrLow;
  UINT32  AdrHigh;
} PCI_2_PRIV_BASE;

typedef struct {
  MKHI_MESSAGE_HEADER          MkhiHeader;
  UINT32                       Reserved1[4];
  PCI_2_PRIV_BASE              Pci2PrivBase;
  UINT8                        Flags;
  UINT8                        BiosAction;
  UINT8                        Reserved2[6];
} DRAM_INIT_DONE_CMD_RESP;

typedef union {
  DRAM_INIT_DONE_CMD_REQ   Request;
  DRAM_INIT_DONE_CMD_RESP  Response;
} DRAM_INIT_DONE_CMD_BUFFER;

#define MAX_MBP_ITEMS_SIZE  125 * sizeof (UINT32)

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              SkipMbp;
} GET_MBP_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  MBP_HEADER          MbpHeader;
  UINT32              MbpItems[MAX_MBP_ITEMS_SIZE];
} GET_MBP_RESP;

typedef union {
  GET_MBP_REQ  Request;
  GET_MBP_RESP Response;
} GET_MBP_BUFFER;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} ME_ENABLE_CMD_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} ME_ENABLE_CMD_RESP;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} HMRFPO_DISABLE_CMD_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} HMRFPO_DISABLE_CMD_RESP;

typedef union {
  HMRFPO_DISABLE_CMD_REQ       Request;
  HMRFPO_DISABLE_CMD_RESP      Response;
} HMRFPO_DISABLE_CMD_BUFFER;

///
/// Get IMR size message
///
typedef struct {
  UINT32        BiosImrDisableMask0;
  UINT32        BiosImrDisableMask1;
  UINT32        Pci2PrivSize;
} GET_IMR_CMD_REQ_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
  GET_IMR_CMD_REQ_DATA  Data;
} GET_IMR_CMD_REQ;

typedef struct {
  UINT32        ImrsSize;
  UINT32        Alignment;
  UINT32        Flags;
} GET_IMR_CMD_RESP_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER    MkhiHeader;
  GET_IMR_CMD_RESP_DATA  Data;
} GET_IMR_CMD_RESP;

typedef union {
  GET_IMR_CMD_REQ   Request;
  GET_IMR_CMD_RESP  Response;
} GET_IMR_CMD_BUFFER;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MANUF_RESET_AND_HALT_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MANUF_RESET_AND_HALT_RESP;

typedef union {
  MANUF_RESET_AND_HALT_REQ  Request;
  MANUF_RESET_AND_HALT_RESP Response;
} MANUF_RESET_AND_HALT_BUFFER;

typedef struct {
  UINT32 FwType    : 3;
  UINT32 FwSubType : 8;
  UINT32 Reserved  : 21;
} FW_TYPE_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER    MkhiHeader;
  FW_TYPE_DATA           FwTypeData;
} GEN_GET_FW_TYPE_ACK;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} GEN_GET_FW_TYPE;

typedef union {
  GEN_GET_FW_TYPE     Request;
  GEN_GET_FW_TYPE_ACK Response;
} GEN_GET_FW_TYPE_BUFFER;

typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
  UINT8                 FirstLogEntryIndex;
  UINT16                Flags;
  UINT8                 Reserved;
} GET_ER_LOG_CMD_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
  UINT8                 MeasuredBootEnabled;
  UINT8                 FirstLogEntryIndex;
  UINT8                 LastLogEntryIndex;
  UINT8                 RemainingEntries;
  UINT16                DataLength;
  UINT8                 LogData[]; // Data buffer
} GET_ER_LOG_CMD_RESP;

typedef union {
  GET_ER_LOG_CMD_REQ     Request;
  GET_ER_LOG_CMD_RESP    Response;
} GET_ER_LOG_BUFFER;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              State;
} SET_EDEBUG_MODE_STATE_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} SET_EDEBUG_MODE_STATE_RESP;

typedef union {
  SET_EDEBUG_MODE_STATE_REQ  Request;
  SET_EDEBUG_MODE_STATE_RESP Response;
} SET_EDEBUG_MODE_STATE_BUFFER;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              Reserved;
} DATA_CLEAR_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} DATA_CLEAR_RESP;

typedef union {
  DATA_CLEAR_REQ      Request;
  DATA_CLEAR_RESP     Response;
} DATA_CLEAR_BUFFER;

#define MAX_DIGEST_SIZE                   64
#define IP_LOADING_CMD_VERSION            1

typedef struct {
  MKHI_MESSAGE_HEADER                     MkhiHeader;
  UINT32                                  Version;
  UINT32                                  Operation;
  UINT32                                  DramBaseLow;
  UINT32                                  DramBaseHigh;
  UINT32                                  MemorySize;
  UINT32                                  Reserved;
} IP_LOADING_CMD_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER                     MkhiHeader;
  UINT32                                  PayloadSize;
  UINT32                                  Reserved[2];
  UINT32                                  Status;
  UINT8                                   HashType;
  UINT8                                   Hash[];
} IP_LOADING_CMD_RESP;

typedef union {
  IP_LOADING_CMD_REQ   Request;
  IP_LOADING_CMD_RESP  Response;
} IP_LOADING_CMD_BUFFER;

/**
  Enable Debug Transports specifying Power Controls.
    0000: Use existing setup settings</b>
    0001: Enable DCI.OOB + DCI.DBC w/ power overrides.
    0010: Enable DCI.OOB-2 w/o power overrides.
    0011: Disable Debug.
    Everything else is reserved.
**/
typedef enum {
  SetupMenuDefault = 0x0,
  EnabledAll       = 0x1,
  EnabledLowPower  = 0x2,
  ForceDisabled    = 0x3
} STREAMING_TRACE_SINK;

typedef union {
  struct {
    UINT32 ExposeDebugMenu    :  1; ///< Expose Debug Menu. 0: Debug menu is hidden; 1: Expose debug menu if hidden
    /**
      Refer to definition of STREAMING_TRACE_SINK
    **/
    UINT32 StreamingTraceSink :  4;
    /**
      Force TraceHub "on" unconditionally.
        0: Normal. BIOS shall enable or disable TraceHub according to the value of BIT24 in scratchpad 0 register or BIOS menu configuration
        1: Force on. BIOS shall keep TraceHub enabled unconditionally
    **/
    UINT32 TraceHubForceOn    :  1;
    UINT32 TraceEnable        :  1; ///< Enable BIOS traces. 0: Normal behavior; 1: Enable Trace Messaging
    UINT32 JtagC10PgDis       :  1; ///< Disable JTAG C10 power gate 0: Normal behavior (Follow BIOS menu configuration); 1: Disable JTAG C10 power gate
    UINT32 UsbOverCurrentOvrd :  1; ///< Enable USB overcurrent override. 0: Normal behavior (Follow BIOS menu configuration); 1: Enable USB overcurrent override
    UINT32 IntelReserved      :  7; ///< Reserved for future Intel use
    UINT32 OemReserved        : 16; ///< Reserved for OEM use
  } Fields;
  UINT32  Data;
} DEBUG_TOKEN_KNOB_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER           MkhiHeader;
} GET_DEBUG_TOKEN_DATA_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER           MkhiHeader;
  UINT8                         TokenId;     ///< 1: Intel Token. 2: OEM Token
  UINT8                         Reserved[3];
  DEBUG_TOKEN_KNOB_DATA         KnobData;
} GET_DEBUG_TOKEN_DATA_RESP;

typedef union {
  GET_DEBUG_TOKEN_DATA_REQ      Request;
  GET_DEBUG_TOKEN_DATA_RESP     Response;
} GET_DEBUG_TOKEN_DATA_BUFFER;

///
/// ICC group defines
///
typedef struct {
  UINT32 FwNeedsWarmResetFlag:1;    ///< BIT0
  UINT32 Reserved:31;               ///< BIT1-31
} ICC_MESSAGE_RESP_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER  MkhiHeader;
} ICC_CMD_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
  ICC_MESSAGE_RESP_DATA Data;
} ICC_CMD_RESP;

typedef union {
  ICC_CMD_REQ         Request;
  ICC_CMD_RESP        Response;
} ICC_CMD_BUFFER;

///
/// MPHY group defines
///
typedef struct{
  MKHI_MESSAGE_HEADER                MkhiHeader;
  UINT32                             Reserved1;
  UINT32                             Offset;
  UINT32                             Reserved2;
  UINT32                             NumOfBytes;
} BUP_MPHY_READ_FROM_MPHY;

typedef struct {
  MKHI_MESSAGE_HEADER                MkhiHeader;
  UINT32                             Reserved1;
  UINT32                             FileSize;
  UINT32                             Reserved2;
  UINT32                             ReadSize;
  UINT32                             Reserved3;
  UINT8                              Data[];
} BUP_MPHY_READ_FROM_MPHY_ACK;

typedef union {
  BUP_MPHY_READ_FROM_MPHY     Request;
  BUP_MPHY_READ_FROM_MPHY_ACK Response;
} BUP_MPHY_READ_FROM_MPHY_BUFFER;

///
/// PM group defines
///
typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
} HR_NOTIFICATION_CMD;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               ResetRequested;
} HR_NOTIFICATION_CMD_RESP;

typedef union {
  HR_NOTIFICATION_CMD      Request;
  HR_NOTIFICATION_CMD_RESP Response;
} HR_NOTIFICATION_BUFFER;

#pragma pack()

#endif // _BUP_MSGS_H
