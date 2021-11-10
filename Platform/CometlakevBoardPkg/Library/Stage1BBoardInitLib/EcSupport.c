/** @file

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PciCf8Lib.h>
#include <Library/PciLib.h>
#include <Library/TimerLib.h>
#include <PchAccess.h>
#include <Register/PchRegsLpc.h>

//
// The EC implements an embedded controller interface at ports 0x60/0x64 and a ACPI compliant
// system management controller at ports 0x62/0x66. Port 0x66 is the command and status port,
// port 0x62 is the data port.
//
#define EC_D_PORT        0x62
#define EC_C_PORT        0x66

#define EC_TIME_OUT      0x20000

#define EC_C_FAB_ID         0x0D    // Get the board fab ID in the lower 3 bits
#define EC_C_LOW_POWER_EXIT 0x2D    // Exit Low Power Mode

#define EC_S_IBF         0x02    // Input buffer is full/empty
#define EC_S_OBF         0x01    // Output buffer is full/empty

//
// BoardId format definition (Data came from EC_C_FAB_ID command)
//
typedef union {
  UINT16    Raw;
  struct {
    UINT16  BoardId    :5;   // [4:0]   - BOARD_IDx
    UINT16  BomId      :3;   // [7:5]   - BOM_IDx
    UINT16  FabId      :3;   // [10:8]  - REV_FAB_IDx
    UINT16  SpdPresent :1;   // [11]    - TP_SPD_PRSNT
    UINT16  Reserved   :4;   // [15:12] - Reserved
  } CnlFields;
} BOARD_ID_INFO;


/**
  Receives status from EC.

  @param[out] EcStatus       Status byte to receive

  @retval     EFI_SUCCESS
  @retval     EFI_DEVICE_ERROR
**/
EFI_STATUS
ReceiveEcStatus (
  OUT UINT8                 *EcStatus
  )
{
  //
  // Read and return the status
  //
  *EcStatus = IoRead8 (EC_C_PORT);

  return EFI_SUCCESS;
}

/**
  Sends command to EC.

  @param[in] Command           Command byte to send
  @param[in] Timeout           Timeout in microseonds

  @retval    EFI_SUCCESS       Command success
  @retval    EFI_DEVICE_ERROR  Command error
  @retval    EFI_TIMEOUT       Command timeout
**/
EFI_STATUS
SendEcCommandTimeout (
  IN UINT8                  Command,
  IN UINT32                 Timeout
  )
{
  UINTN         Index;
  UINT8         EcStatus;

  Index = 0;
  EcStatus = 0;
  //
  // Wait for EC to be ready (with a timeout)
  //
  ReceiveEcStatus (&EcStatus);
  //
  // Check if output buffer bit(OBF) is set.
  // Read and discard the output buffer data so that next BIOS-EC cmd is in sync
  // OBF is cleared by h/w after all data in output buffer is read by BIOS.
  //
  while (((EcStatus & EC_S_OBF) != 0) && (Index < Timeout)) {
    //
    // Read EC data
    //
    MicroSecondDelay (10 * STALL_ONE_MICRO_SECOND);
    IoRead8 (EC_D_PORT);
    ReceiveEcStatus (&EcStatus);
    Index+=10;
  }

  if (Index >= Timeout) {
    return EFI_TIMEOUT;
  }

  Index = 0;

  while (((EcStatus & EC_S_IBF) != 0) && (Index < Timeout)) {
    MicroSecondDelay (10 * STALL_ONE_MICRO_SECOND);
    ReceiveEcStatus (&EcStatus);
    Index+=10;
  }

  if (Index >= Timeout) {
    return EFI_TIMEOUT;
  }

  //Printing EC Command Sent
  DEBUG ((DEBUG_INFO, "Sending EC Command: %02X\n", Command));

  //
  // Send the EC command
  //
  IoWrite8 (EC_C_PORT, Command);

  DEBUG ((DEBUG_INFO, "SendEcDataTimeout sent %x \n", Command));

  return EFI_SUCCESS;
}

/**
  Receives data from EC.

  @param[out] Data              Data byte received
  @param[in]  Timeout           Timeout in microseonds

  @retval     EFI_SUCCESS       Read success
  @retval     EFI_DEVICE_ERROR  Read error
  @retval     EFI_TIMEOUT       Command timeout
**/
EFI_STATUS
ReceiveEcDataTimeout (
  OUT UINT8                  *Data,
  IN  UINT32                 Timeout
 )
{
  UINTN         Index;
  UINT8         EcStatus;

  Index = 0;

  //
  // Wait for EC to be ready (with a timeout)
  //
  ReceiveEcStatus (&EcStatus);
  while (((EcStatus & EC_S_OBF) == 0) && (Index < Timeout)) {
    MicroSecondDelay (15 * STALL_ONE_MICRO_SECOND);
    ReceiveEcStatus (&EcStatus);
    Index++;
  }
  DEBUG ((DEBUG_INFO, "ReceiveEcDataTimeout Read %x \n",EcStatus));

  if (Index >= Timeout) {
    return EFI_TIMEOUT;
  }
  //
  // Read EC data and return
  //
  *Data = IoRead8 (EC_D_PORT);

  //Printing EC Data Received
  DEBUG ((DEBUG_INFO, "Receiving EC Data: %02X\n", *Data));

  return EFI_SUCCESS;
}

/**
  Get the board ID from EC.

  @param[out] BoardId       Buffer to return the board ID

**/
VOID
GetBoardId (
  OUT UINT8                  *BoardId
)
{
  BOARD_ID_INFO BoardInfo;
  EFI_STATUS Status;
  UINTN   LpcBaseAddr;
  UINT16  Data16;
  UINT8   EcData;

  //
  // Set board ID to unknown initially
  // in case there is any issue encountered
  // below.
  //
  *BoardId = 0xFF;

  LpcBaseAddr = PCI_LIB_ADDRESS (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC,
                      0);

  Data16 = PciCf8Read16 (LpcBaseAddr + R_LPC_CFG_IOE);
  Data16 |= B_LPC_CFG_IOE_ME1;
  MmioWrite16 (PCH_PCR_ADDRESS (PID_DMI, R_PCH_DMI_PCR_LPCIOE), Data16);
  PciCf8Write16 (LpcBaseAddr + R_LPC_CFG_IOE, Data16);

  BoardInfo.Raw = 0;
  Status = SendEcCommandTimeout (EC_C_LOW_POWER_EXIT, EC_TIME_OUT);
  if (EFI_ERROR (Status)) {
    return;
  }
  Status = SendEcCommandTimeout (EC_C_FAB_ID, EC_TIME_OUT);
  if (EFI_ERROR (Status)) {
    return;
  }
  EcData = 0;
  Status = ReceiveEcDataTimeout (&EcData, EC_TIME_OUT);
  if (EFI_ERROR (Status)) {
    return;
  }
  BoardInfo.Raw = (EcData << 8);

  EcData = 0;
  Status = ReceiveEcDataTimeout (&EcData, EC_TIME_OUT);
  if (EFI_ERROR (Status)) {
    return;
  }
  BoardInfo.Raw |= (UINT16) EcData;

  *BoardId = (UINT8) BoardInfo.CnlFields.BoardId;
}
