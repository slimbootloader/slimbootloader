/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _HECI_LIB_H_
#define _HECI_LIB_H_

#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>
#include <RegAccess.h>


#define H_CB_WW                         0x00
#define H_CSR                           0x04

#define BIOS_FIXED_HOST_ADDR        0
#define BIOS_ASF_HOST_ADDR          1

// PCI access functions for SEC access
#define R_SEC_FW_STS0                   0x40


#define HeciPciRead8(reg) PciRead8 (PCI_LIB_ADDRESS(mCseBus,mCseDevice,mHeciFunction,reg))
#define HeciPciWrite8(reg, val)  PciWrite8 (PCI_LIB_ADDRESS(mCseBus,mCseDevice,mHeciFunction,reg), val)

#define HeciPciRead32(reg) PciRead32 (PCI_LIB_ADDRESS(mCseBus,mCseDevice,mHeciFunction,reg))
#define HeciPciWrite32(reg, val)  PciWrite32 (PCI_LIB_ADDRESS(mCseBus,mCseDevice,mHeciFunction,reg), val)

#define HeciPciOr16(reg, val)    PciOr16 (PCI_LIB_ADDRESS(mCseBus,mCseDevice,mHeciFunction,reg), val)

#define MKHI_BOOTLOADER_SEED_LEN              32
#define MKHI_BOOTLOADER_SEED_MAX_ENTRIES      4
#define MKHI_BOOTLOADER_PCR_LEN               32

#define MBP_APP_ID_KERNEL                     1
#define MBP_ITEM_ID_FW_VER_NAME               1

typedef struct {
  UINT32 Bus;
  UINT32 Device;
  UINT32 Function;
} HECI_INSTANCE;

//
// FWS - FirmWare Status
//
typedef union {
  UINT32  ul;
  struct {
    UINT32  CurrentState : 4;         // 0:3 - Current State
    UINT32  ManufacturingMode : 1;    // 4 Manufacturing Mode
    UINT32  FptBad : 1;               // 5 FPT(Flash Partition Table ) Bad
    UINT32  SeCOperationState : 3;    // 6:8 - SEC Operation State
    UINT32  FwInitComplete : 1;       // 9
    UINT32  FtBupLdFlr : 1;           // 10 - This bit is set when firmware is not able to load BRINGUP from the fault tolerant (FT) code.
    UINT32  FwUpdateInprogress : 1;   // 11
    UINT32  ErrorCode : 4;            // 12:15 - Error Code
    UINT32  SeCOperationMode : 4;     // 16:19 - Management Engine Current Operation Mode
    UINT32  Reserved2 : 4;            // 20:23
    UINT32  SeCBootOptionsPresent : 1;// 24 - If this bit is set, an Boot Options is present
    UINT32  AckData : 3;              // 25:27 Ack Data
    UINT32  BiosMessageAck : 4;       // 28:31 BIOS Message Ack
  } r;
} HeciFwsRegister;

//
// S_CSR - SEC Control Status
//
typedef union {
  UINT32 Data;
  struct {
    UINT32  SecIeHra : 1;    // 0 - SEC Interrupt Enable (Host Read Access)
    UINT32  SecIsHra : 1;    // 1 - SEC Interrupt Status (Host Read Access)
    UINT32  SecIgHra : 1;    // 2 - SEC Interrupt Generate (Host Read Access)
    UINT32  SecRdyHra : 1;   // 3 - SEC Ready (Host Read Access)
    UINT32  SecRstHra : 1;   // 4 - SEC Reset (Host Read Access)
    UINT32  Reserved : 3;     // 7:5
    UINT32  SecCbrpHra : 8;  // 15:8 - SEC CB Read Pointer (Host Read Access)
    UINT32  SecCbwpHra : 8;  // 23:16 - SEC CB Write Pointer (Host Read Access)
    UINT32  SecCbdHra : 8;   // 31:24 - SEC Circular Buffer Depth (Host Read Access)
  } r;
} CseControlRegister;

//
// H_CSR - Host Control Status
//
typedef union {
  UINT32  Data;
  struct {
    UINT32  Hie : 1;     // 0 - Host Interrupt Enable SEC
    UINT32  His : 1;     // 1 - Host Interrupt Status SEC
    UINT32  Hig : 1;     // 2 - Host Interrupt Generate
    UINT32  Hrdy : 1;    // 3 - Host Ready
    UINT32  Hrst : 1;    // 4 - Host Reset
    UINT32  Reserved : 3; // 7:5
    UINT32  Hcbrp : 8;   // 15:8 - Host CB Read Pointer
    UINT32  Hcbwp : 8;   // 23:16 - Host CB Write Pointer
    UINT32  Hcbd : 8;    // 31:24 - Host Circular Buffer Depth
  } r;
} HostControlRegister;

typedef union {
  UINT32  Data;
  struct {
    UINT32  SecAddress : 8;         // This is the logical address of the TXE client of the message
    UINT32  HostAddress : 8;        // This is the logical address of the Host client of the message.
    UINT32  Length : 9;             // This is the message length in bytes, not including the HECI_MESSAGE_HEADER
    UINT32  Reserved : 6;
    UINT32  MessageComplete : 1;    // This bit is used to indicate that this is the last message of a multi message HECI transfer of a client's message that is larger than HECI circular buffer.
  } Fields;
} HeciMessageHeader;


typedef struct
{
  UINT32      BPDTOffset;
  UINT32      PhysicalData;
  UINT32      PhysicalDeviceArea;
  UINT32      LogicalData;
} MBP_CURRENT_BOOT_MEDIA;

typedef struct {
  UINT16  MajorVersion;
  UINT16  MinorVersion;
  UINT16  HotfixVersion;
  UINT16  BuildVersion;
} MBP_VERSION;

typedef struct
{
  UINT32  MKHIHeader;
  UINT32  Flag;
} MBP_CMD_REQ_DATA;

typedef struct
{
  UINT32  MKHIHeader;
  UINT8   Length;
  UINT8   ItemsNum;
  UINT8   Flags;
  UINT8   Reserved;
} MBP_CMD_RESP_DATA;

#pragma pack ()

/**
  Dump the CSE device status.

  @retval EFI_SUCCESS         Dumping the status succeeded
  @retval EFI_NOT_FOUND       The CSE device is not found

**/
EFI_STATUS
EFIAPI
DumpCseStatus (
  VOID
);

/**
  Set HECI to DNX mode.

  @return EFI_SUCCESS   Enter DNX mode success.
  @return Others        Enter DNX mode failed.

**/
EFI_STATUS
EFIAPI
EnterDnxMode (
  VOID
);

/**
  Send EOP message through HECI.

  @return EFI_SUCCESS   Send EOP message success.
  @return Others              Send EOP message failed.

**/
EFI_STATUS
EFIAPI
HeciEndOfPost (
  VOID
);

/**
  HeciSendUserCommand

  @param[in,out]  Data
  @param[in]      Length
  @param[in]      Force

  @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
HeciSendUserCommand (
  UINT8 *Data,
  UINT8 Length,
  UINT8 Force
);

/**
  Extend the PCR for measured boot support

  @param[in]  Pcr

  @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
PcrExtend (
  UINT8 *Pcr
);

/**
  Prepare CSE for firmware update

  @return EFI_STATUS
**/
EFI_STATUS
PrepareCseForFirmwareUpdate (
  VOID
  );

/**
Send Get Firmware Version Request to Sec

  @param[out] MsgGenGetFwVersionAck   Return themessage of FW version

  @exception EFI_UNSUPPORTED      Current Sec mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
EFIAPI
HeciGetFwVersionMsg (
  OUT UINT8   *MsgGenGetFwVersionAck
  );

/**
  Send Get Firmware SKU Request to Sec

  @param[in] MsgGenGetFwCapsSku      Message for which to Get Firmware Capability SKU
  @param[out] MsgGenGetFwCapsSkuAck  Return message for Get Firmware Capability SKU ACK

  @exception EFI_UNSUPPORTED        Current Sec mode doesn't support this function
  @retval EFI_SUCCESS               Command succeeded
  @retval EFI_DEVICE_ERROR          HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT               HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL      Message Buffer is too smallfor the Acknowledge
 **/
EFI_STATUS
EFIAPI
HeciGetFwCapsSkuMsg (
  IN UINT8   *MsgGenGetFwCapsSku,
  OUT UINT8   *MsgGenGetFwCapsSkuAck
 );

/**
  Return Heci FW Status

  @param[out]  FW Status pointer

  @retval  EFI_SUCCESS      The device returned its FW status register
  @retval  EFI_NOT_FOUND    The device is not supported

**/
EFI_STATUS
EFIAPI
ReadHeciFwStatus (
  OUT UINT32 *FwSts
);

/**
  Wrapper function to for the HECI command without parameter.

  @param[in] SubCmd           The HECI sub-command

  @retval EFI_SUCCESS         The sub command runs success
  @retval Others              The sub command doesn't run success

**/
EFI_STATUS
EFIAPI
SimpleHeciCommand (
  IN UINT32     SubCmd
);

/**
  Register HECI service

  @retval EFI_SUCCESS             HECI service is registered successfully.
  @retval Others                  There is failure when register HECI service
**/
EFI_STATUS
EFIAPI
RegisterHeciService (
  VOID
);

/**
  Reads a message from the ME across HECI. This function can only be used after invoking HeciSend() first.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in, out] MessageBody     Pointer to a buffer used to receive a message.
  @param[in, out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_NO_RESPONSE         Can't get data from ME FW.
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciReceive (
  IN      HECI_DEVICE             HeciDev,
  IN      UINT32                  Blocking,
  IN OUT  UINT32                  *MessageBody,
  IN OUT  UINT32                  *Length
  );

/**
  Function sends one message (of any length) through the HECI circular buffer.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support send this message through this HECI
**/
EFI_STATUS
EFIAPI
HeciSend (
  IN HECI_DEVICE                  HeciDev,
  IN UINT32                       *Message,
  IN UINT32                       Length,
  IN UINT8                        HostAddress,
  IN UINT8                        MeAddress
  );

/**
  Function forces a reinit of the heci interface by following the reset heci interface via host algorithm
  in HPS 0.90 doc 4-17-06

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Interface reset
**/
EFI_STATUS
EFIAPI
ResetHeciInterface (
  IN  HECI_DEVICE                 HeciDev
  );

#endif
