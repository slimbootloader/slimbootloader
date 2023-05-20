/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PchPcrLib.h>
#include <Library/PciCf8Lib.h>
#include <Library/PciLib.h>
#include <Library/PchInfoLib.h>
#include <Library/TimerLib.h>
#include <PchAccess.h>
#include <Register/PchRegsLpc.h>
#include <Register/PchRegsPcr.h>
#include <PlatformBoardId.h>
#include <Library/SmbusLib.h>
#include <ConfigDataDefs.h>
#include <Library/ConfigDataLib.h>

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
// PCA9555 device slave address
//
#define SMBUS_IO_EXPANDER_SLAVE_ADDRESS    0x22

//
// PCA9555 command byte
// -------   ------------
// Command     Register
// -------   ------------
//   0x0     Input Port 0
//   0x1     Input Port 1
#define SMBUS_IO_EXPANDER_INPUT_PORT0_CMD   0x0
#define SMBUS_IO_EXPANDER_INPUT_PORT1_CMD   0x1

#define ADL_SMBUS_BOARDID_OFFSET            0x1

//
// BoardId format definition (Data came from EC_C_FAB_ID command)
//
typedef union {
  UINT16    Raw;
  struct {
    UINT16  BoardId        :6;   // [5:0]   - BOARD_IDx
    UINT16  BomId          :3;   // [8:6]   - BOM_IDx
    UINT16  FabId          :2;   // [10:9]  - REV_FAB_IDx
    UINT16  SpdPresent     :1;   // [11]    - SPD_PRSNT
    UINT16  VirtualBattery :1;   // [12]    - VIRTUAL_BATTERY
    UINT16  VirtualDock    :1;   // [13]    - VIRTUAL_DOCK_DET_N
    UINT16  RetimerBypass  :1;   // [14]    - RETIMER_BYPASS_STRAP
    UINT16  EspiTestCard   :1;   // [15]    - ESPI_TESTCARD_DET
  } AdlRvpFields;
} BOARD_ID_INFO;

typedef union {
  UINT8    InputPort0Raw;
  struct {
    UINT8  BomId          :1;   // [0]    - BOM_ID[2]
    UINT8  FabId          :2;   // [2:1]  - Fab_IDx
    UINT8  SpdPresent     :1;   // [3]    - SPD_PRSNT
    UINT8  DisplayId      :4;   // [7:4]  - DISPLAY_ID[3:0]
  } InputPort0Fields;
} SMBUS_INPUT_PORT0;
typedef union {
  UINT8    InputPort1Raw;
  struct {
    UINT8  BoardId        :6;   // [5:0]   - BOARD_ID[5:0]
    UINT8  BomId          :2;   // [7:6]   - BOM_ID[1:0]
  } InputPort1Fields;
} SMBUS_INPUT_PORT1;

/**
  Returns the BoardId ID of the platform from Smbus I/O port expander PCA9555PW.

  @param[in]  BoardId           BoardId ID as determined through the Smbus.
**/
VOID
EFIAPI
GetBoardIdFromSmbus (
  OUT UINT8          *PlatformId
  )
{
  UINT8                    BomId;
  UINT8                    BoardId;
  SMBUS_INPUT_PORT0        SmbusInputPort0Info;
  SMBUS_INPUT_PORT1        SmbusInputPort1Info;
  EFI_STATUS               Status0;
  EFI_STATUS               Status1;

  Status0 = EFI_DEVICE_ERROR;
  Status1 = EFI_DEVICE_ERROR;

  SmbusInputPort0Info.InputPort0Raw = SmBusReadDataByte (SMBUS_LIB_ADDRESS(SMBUS_IO_EXPANDER_SLAVE_ADDRESS, SMBUS_IO_EXPANDER_INPUT_PORT0_CMD, 0, 0), &Status0);
  SmbusInputPort1Info.InputPort1Raw = SmBusReadDataByte (SMBUS_LIB_ADDRESS(SMBUS_IO_EXPANDER_SLAVE_ADDRESS, SMBUS_IO_EXPANDER_INPUT_PORT1_CMD, 0, 0), &Status1);

  if ((Status0 == EFI_SUCCESS) && (Status1 == EFI_SUCCESS)) {
    BomId = (UINT8) ((SmbusInputPort0Info.InputPort0Fields.BomId << 2) | SmbusInputPort1Info.InputPort1Fields.BomId);

    DEBUG ((DEBUG_INFO, "Raw data from Input Port 0 is 0x%x\n", SmbusInputPort0Info.InputPort0Raw));
    DEBUG ((DEBUG_INFO, "Raw data from Input Port 1 is 0x%x\n", SmbusInputPort1Info.InputPort1Raw));
    DEBUG ((DEBUG_INFO, "Fields.BoardId from Smbus Io expander is 0x%x\n", SmbusInputPort1Info.InputPort1Fields.BoardId));
    DEBUG ((DEBUG_INFO, "Fields.BomId from Smbus Io expander is 0x%x\n", BomId));
    DEBUG ((DEBUG_INFO, "Fields.FabId from Smbus Io expander is 0x%x\n", (UINT16) (SmbusInputPort0Info.InputPort0Fields.FabId)));
    DEBUG ((DEBUG_INFO, "Fields.SpdPresent from Smbus Io expander is %x\n", (BOOLEAN) (SmbusInputPort0Info.InputPort0Fields.SpdPresent)));

    BoardId = SmbusInputPort1Info.InputPort1Fields.BoardId;
  }
  else {
    DEBUG ((DEBUG_ERROR, "Failed to get Board ID from Smbus Io expander\n"));
    return;
  }
  switch (BoardId) {
    case BoardIdAdlNDdr5Crb:
      *PlatformId = PLATFORM_ID_ADL_N_DDR5_CRB;
      break;
    case BoardIdAdlPSDdr5Crb:
      *PlatformId = PLATFORM_ID_ADL_PS_DDR5_CRB;
      break;
    default:
    break;
  }
}

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
Return Board ID from EC
@retval UINT8                   Board ID
**/
VOID
GetBoardIdFromEC (
  OUT UINT8                  *BoardId
  )
{
  BOARD_ID_INFO BoardInfo;
  EFI_STATUS Status;
  UINTN   eSPIBaseAddr;
  UINT16  Data16;
  UINT8   EcData;

  eSPIBaseAddr = PCI_LIB_ADDRESS (
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_ESPI,
                      PCI_FUNCTION_NUMBER_PCH_ESPI,
                      0);

  Data16 = PciCf8Read16 (eSPIBaseAddr + R_LPC_CFG_IOE);
  Data16 |= B_LPC_CFG_IOE_ME1;
  MmioWrite16 (PCH_PCR_ADDRESS (PID_DMI, R_PCH_DMI_PCR_LPCIOE), Data16);
  PciCf8Write16 (eSPIBaseAddr + R_LPC_CFG_IOE, Data16);

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

  *BoardId = (UINT8)BoardInfo.AdlRvpFields.BoardId;
}

/**
  Get the board ID from EC.

  @param[out] PlatformId       Buffer to return the Platform ID based on EC BaordId

**/
VOID
GetBoardId (
  OUT UINT8                  *PlatformId
)
{
  UINT8   BoardID = 0xFF;
  //
  // Set board ID to unknown initially
  // in case there is any issue encountered
  // below.
  //
  *PlatformId = 0xFF;

#if !defined(PLATFORM_ADLN) && !defined(PLATFORM_ASL)
  GetBoardIdFromEC(&BoardID);
#endif

  if (BoardID == 0xFF){
    //EC is not detected (timeout)
    GetBoardIdFromSmbus(PlatformId);
    if (*PlatformId == 0xFF)
      *PlatformId = PLATFORM_ID_RPL_P_DDR5_CRB;
    return;
  }

  switch (BoardID) {
    case BoardIdAdlSAdpSDdr4UDimm2DCrb:
      *PlatformId = PLATFORM_ID_ADL_S_ADP_S_CRB;
      break;
    case BoardIdAdlSAdpSDdr4SODimmCrb:
      *PlatformId = PLATFORM_ID_ADL_S_ADP_S_DDR4_SODIMM_CRB;
      break;
    case BoardIdAdlSAdpSDdr5SODimmCrb:
      *PlatformId = PLATFORM_ID_ADL_S_ADP_S_DDR5_SODIMM_CRB;
      break;
    case BoardIdAdlSAdpSDdr5UDimm1DCrb:
      *PlatformId = PLATFORM_ID_ADL_S_ADP_S_DDR5_UDIMM_1DC_CRB;
      break;
    case BoardIdAdlPLp4Rvp:
      *PlatformId = PLATFORM_ID_ADL_P_LP4_RVP;
      break;
    case BoardIdAdlPLp5Rvp:
    case BoardIdRplPLp5Rvp:
      *PlatformId = PLATFORM_ID_ADL_P_LP5_RVP;
      break;
    case BoardIdAdlPDdr5Rvp:
    case BoardIdRplPDdr5Rvp:
      *PlatformId = PLATFORM_ID_ADL_P_DDR5_RVP;
      break;
    case BoardIdAdlPSDdr5Rvp:
      *PlatformId = PLATFORM_ID_ADL_PS_DDR5_RVP;
      break;
    case BoardIdAdlNLp5Rvp:
      *PlatformId = PLATFORM_ID_ADL_N_LPDDR5_RVP;
      break;
    case BoardIdTestSDdr5UDimm1DRvp:
      *PlatformId = PLATFORM_ID_TEST_S_DDR5_UDIMM_RVP;
      break;
    case BoardIdTestSDdr5SODimmRvp:
      *PlatformId = PLATFORM_ID_TEST_S_DDR5_SODIMM_RVP;
      break;
    case BoardIdAdlpUpXtremei12:
      *PlatformId = PLATFORM_ID_ADL_P_UPXI12;
      break;
    default:
      DEBUG((DEBUG_INFO, "Unsupported board Id %x .....\n", *PlatformId));
      break;
  }

}
