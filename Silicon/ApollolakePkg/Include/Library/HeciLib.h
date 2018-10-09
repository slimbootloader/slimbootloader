/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _HECI_LIB_H_
#define _HECI_LIB_H_
#include <Uefi/UefiBaseType.h>
#include <Library/BaseLib.h>

#define EOP_GROUP_ID 0xFF
#define EOP_CMD_ID   0xC

#define H_CB_WW                         0x00
#define H_CSR                           0x04
#define SEC_CB_RW                       0x08
#define SEC_CSR_HA                      0x0C

#define BIOS_FIXED_HOST_ADDR        0
#define PREBOOT_FIXED_SEC_ADDR      7
#define BIOS_ASF_HOST_ADDR          1

// PCI access functions for SEC access
#define R_HECIMBAR0                     0x10
#define R_HECIMBAR1                     0x14
#define R_SEC_FW_STS0                   0x40

//
// Management Engine Current Operation Mode
//
#define SEC_OPERATION_MODE_NORMAL            0x00
#define SEC_OPERATION_MODE_ALT_DISABLED      0x02
#define SEC_OPERATION_MODE_SOFT_TEMP_DISABLE 0x03
#define SEC_OPERATION_MODE_SECOVR_JMPR       0x04
#define SEC_OPERATION_MODE_SECOVR_HECI_MSG   0x05

//
// Abstract SEC Mode Definitions
//
#define SEC_MODE_NORMAL        0x00

#define SEC_DEBUG_MODE_ALT_DIS 0x02
#define SEC_MODE_TEMP_DISABLED 0x03
#define SEC_MODE_SECOVER       0x04
#define SEC_MODE_FAILED        0x06

#define HeciPciRead8(reg) PciRead8 (PCI_LIB_ADDRESS(mCseBus,mCseDevice,mHeciFunction,reg))
#define HeciPciWrite8(reg, val)  PciWrite8 (PCI_LIB_ADDRESS(mCseBus,mCseDevice,mHeciFunction,reg), val)

#define HeciPciRead32(reg) PciRead32 (PCI_LIB_ADDRESS(mCseBus,mCseDevice,mHeciFunction,reg))
#define HeciPciWrite32(reg, val)  PciWrite32 (PCI_LIB_ADDRESS(mCseBus,mCseDevice,mHeciFunction,reg), val)

#define HeciPciOr16(reg, val)    PciOr16 (PCI_LIB_ADDRESS(mCseBus,mCseDevice,mHeciFunction,reg), val)

#define MKHI_BOOTLOADER_SEED_LEN              32
#define MKHI_BOOTLOADER_SEED_MAX_ENTRIES      4
#define MKHI_BOOTLOADER_PCR_LEN               32

#define MBP_APP_ID_KERNEL                     1
#define MBP_APP_ID_NVM                        8
#define MBP_ITEM_ID_FW_VER_NAME               1
#define MBP_ITEM_ID_CURRENT_BOOT_MEDIA        1
#define MBP_APP_ABL_SIG                       0x9
#define MBP_ITEM_ID_IAFW_IBB_SIG              0x1
#define MKHI_IFWI_UPD_SIG                     0x20
#define MKHI_IFWI_UPD_SIG_CMD                 0x7
#define SEND_CLEAR_SUB_COMMAND                0x0
#define SEND_USR_CMD_SUB_COMMAND              0x1

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

typedef struct {
  UINT8 CseSvn;   // 0XFF indicates invalid entry / SVN
  UINT8 Reserved[3];
  UINT8 Seed[MKHI_BOOTLOADER_SEED_LEN];
} MKHI_BOOTLOADER_SEED_INFO_EX;

typedef struct
{
  UINT32                             NumOfSeeds;
  MKHI_BOOTLOADER_SEED_INFO_EX       List[MKHI_BOOTLOADER_SEED_MAX_ENTRIES];
} MKHI_BOOTLOADER_SEED_LIST;

typedef struct
{
  UINT8        AppID;
  UINT8        ItemID;
  UINT8        Length;
  UINT8        Flag;
} MBP_ITEM_HEADER;

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
  Function forces a reinit of the HECI interface by following the reset HECI interface via host algorithm
  in HPS 0.90 doc 4-17-06 njy

  @retval  EFI_STATUS

**/
EFI_STATUS
EFIAPI
ResetHeciInterface (
  VOID
);

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

#endif
