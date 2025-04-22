/** @file
  BUP Messages - HECI messages exchanged in pre-DID phase with CSME FW

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _BUP_MSGS_H
#define _BUP_MSGS_H

#include <Register/MeBiosPayloadData.h>

#pragma pack(1)

///
/// BUP group IDs
///
#define BUP_COMMON_GROUP_ID 0xF0
#define BUP_ICC_GROUP_ID    0xF1
#define BUP_HSIO_GROUP_ID   0xF2
#define BUP_PM_GROUP_ID     0xF3

///
/// Defines for Command
///
///
/// Defines for BUP_COMMON_GROUP_ID Command
///
#define DRAM_INIT_DONE_CMD       0x01
#define ME_ENABLE_CMD            0x03
#define HMRFPO_DISABLE_CMD       0x04
#define GET_IMR_SIZE_CMD         0x0C
///
/// Defines for BUP_ICC_GROUP_ID Command
///
#define ICC_CMD  0x01
///
/// Defines for BUP_HSIO_GROUP_ID Command
///
#define HSIO_CMD  0x1


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


typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              SkipMbp;
} MBP_CMD_REQ;

typedef struct {
  MKHI_MESSAGE_HEADER MkhiHeader;
  MBP_HEADER          MbpHeader;
  UINT32              MbpItems[1];
} MBP_CMD_RESP;

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


typedef struct {
  MKHI_MESSAGE_HEADER   MkhiHeader;
} HSIO_CMD_REQ;

typedef struct {
  UINT16 Crc;
  UINT8  Version;
  UINT8  Family;
} HSIO_MESSAGE_RESP_DATA;

typedef struct {
  MKHI_MESSAGE_HEADER    MkhiHeader;
  HSIO_MESSAGE_RESP_DATA Data;
} HSIO_CMD_RESP;

typedef union {
  HSIO_CMD_REQ  Request;
  HSIO_CMD_RESP Response;
} HSIO_CMD_BUFFER;

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

//
// Defines the HECI request buffer format for the ICC_HSIO_WRITE_SETTINGS_CMD.
//
typedef struct {
   ICC_HEADER  Header;      ///< Standard ICC HECI Header
   UINT32      Reserved;    ///< Reserved for future use
} HSIO_WRITE_SETTINGS_REQ;

//
// Defines the HECI request buffer format for the ICC_HSIO_READ_MPHY_CMD.
//
typedef struct {
  ICC_HEADER  Header;         ///< Standard ICC HECI Header
  UINT32      Reserved;       ///< Reserved for future use
  UINT32      Offset;         ///< Offset in the mPhy file to start reading from
  UINT32      Reserved2;      ///< Reserved for future use
  UINT32      NumBytesToRead; ///< How many bytes to read. For the entire file specifiy the mPhy max size
} HSIO_READ_MPHY_REQ;

//
// Defines the HECI request buffer format for reponse to ICC_HSIO_READ_MPHY_CMD.
//
typedef struct {
  ICC_HEADER  Header;               ///< Standard ICC HECI Header
  UINT32      Reserved;             ///< Reserved for future use
  UINT32      MphyFileSize;         ///< mPhy file size. This will be filled regardless of how many bytes the user asked to read (e.g. zero)
  UINT32      Reserved2;            ///< Reserved for future use
  UINT32      NumBytesActuallyRead; ///< How many bytes were actually read, having less than requested is NOT an error
  UINT32      Reserved3;            ///< Reserved for future use
  UINT8       Payload[];            ///< The binary content requested (or at least as many bytes as we could read)
} HSIO_READ_MPHY_ACK;

typedef union {
  HSIO_READ_MPHY_REQ Request;
  HSIO_READ_MPHY_ACK Response;
} HSIO_READ_MPHY_BUFFER;

#pragma pack()

#endif // _BUP_MSGS_H
