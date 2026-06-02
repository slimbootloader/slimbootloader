/** @file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage1ABoardInitLib.h"
#include "SioRegs.h"
#include "Register/EspiRegs.h"
#include <Include/IblSbPortIds.h>
#include <Include/PchReservedResources.h>
#include <Library/PciSegmentLib.h>
#include <Library/PchCycleDecodingLib.h>
#include "Register/PchRegsPcr.h"

typedef struct {
  UINT8   Index;
  UINT8   Data;
} SIO_REG_TABLE;

STATIC  SIO_REG_TABLE  mASPEED2500Table [] = {
  { REG_LOGICAL_DEVICE,       ASPEED2500_SIO_UART1 },
  { ACTIVATE,                 0x01 },
  { ASPEED2500_PRIMARY_INTERRUPT_SELECT, 0x04 },    // COMA IRQ routing
  { INTERRUPT_TYPE, 0x01 },    // COMA Interrupt Type
  { REG_LOGICAL_DEVICE,       ASPEED2500_SIO_UART2 },
  { ACTIVATE,                 0x01 },
  { ASPEED2500_PRIMARY_INTERRUPT_SELECT, 0x03 },    // COMB IRQ routing
  { INTERRUPT_TYPE, 0x01 }     // COMB Interrupt Type
};

STATIC  SIO_REG_TABLE  mASPEED2600Table [] = {
  { REG_LOGICAL_DEVICE,       ASPEED2500_SIO_UART1 },
  { ASPEED2500_SIOR2_F0, 0x02 },    // bit 1 (clock source - 24Mhz) to 1
  { ACTIVATE,                 0x01 },
  { ASPEED2500_PRIMARY_INTERRUPT_SELECT, 0x04 },    // COMA IRQ routing
  { INTERRUPT_TYPE, 0x01 },    // COMA Interrupt Type
  { REG_LOGICAL_DEVICE,       ASPEED2500_SIO_UART2 },
  { ASPEED2500_SIOR2_F0, 0x02 },    // bit 1 (clock source - 24Mhz) to 1
  { ACTIVATE,                 0x01 },
  { ASPEED2500_PRIMARY_INTERRUPT_SELECT, 0x03 },    // COMB IRQ routing
  { INTERRUPT_TYPE, 0x01 }     // COMB Interrupt Type
};

/**
 * Checks for the presence of ASPEED SIO
 * @return TRUE if its present. FALSE if not.
 */
BOOLEAN
IsAspeedPresent (
  VOID
 )
{
 BOOLEAN PresenceStatus = FALSE;
 UINT8  ChipID = 0;

 //
 //ASPEED AST2500/AST2600
 //
 IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);
 IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);
 IoWrite8 (ASPEED2500_SIO_INDEX_PORT, REG_LOGICAL_DEVICE);
 IoWrite8 (ASPEED2500_SIO_DATA_PORT, ASPEED2500_SIO_UART1);
 if (IoRead8 (ASPEED2500_SIO_DATA_PORT) == ASPEED2500_SIO_UART1) {
  #if !FixedPcdGetBool(PcdSimicsEnv)
    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, AST2600_CHIP_ID_REG);
    IoWrite8 (ASPEED2500_SIO_DATA_PORT, 0xf0);
    ChipID = IoRead8 (ASPEED2500_SIO_DATA_PORT);
  #endif
  if ((ChipID == 0) || (ChipID == ASPEED2600_CHIP_ID)) { //Aspeed2500 or Aspeed2600
    PresenceStatus = TRUE;
  }
 }
 IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_LOCK);
 return PresenceStatus;
}

/**
 * Checks for the presence of ASPEED SIO.
 *
 * @retval TRUE   ASPEED SIO is present.
 * @retval FALSE  ASPEED SIO is not present.
 */
BOOLEAN
IsSioExist (
  VOID
  )
{
  return IsAspeedPresent ();
}

/**
  Initialize Platform SIO chip.

  @param None

  @retval None

**/
VOID
EFIAPI
PlatformSioEarlyInit (
  VOID
)
{

  UINT32  Index;
  UINT16  Decode;
  UINT16  Enable;
  UINT8   ChipID;

  //
  // Enable LPC decode
  // Set COMA/COMB base
  //
  Decode =  ((V_ESPI_CFG_ESPI_IOD_IOE_CA_3F8 << N_ESPI_CFG_ESPI_IOD_IOE_CA) | (V_ESPI_CFG_ESPI_IOD_IOE_CB_2F8 << N_ESPI_CFG_ESPI_IOD_IOE_CB));
  Enable =  ( V_ESPI_CFG_ESPI_IOE_ME2 | V_ESPI_CFG_ESPI_IOE_SE | V_ESPI_CFG_ESPI_IOE_ME1 \
              | V_ESPI_CFG_ESPI_IOE_CBE | V_ESPI_CFG_ESPI_IOE_CAE);

  PchLpcIoDecodeRangesSet (Decode);
  PchLpcIoEnableDecodingSet (Enable);

  //
  // ASPEED AST2500/AST2600 UART init.
  //
  if (IsSioExist ()) {
    //
    // Unlock SIO
    //
    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);
    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);

    //
    // COM1 & COM2
    //
    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, AST2600_CHIP_ID_REG);
    IoWrite8 (ASPEED2500_SIO_DATA_PORT, 0xf0);
    ChipID = IoRead8 (ASPEED2500_SIO_DATA_PORT);
    if (ChipID == 0) { // Aspeed2500
      for (Index = 0; Index < sizeof (mASPEED2500Table)/sizeof (SIO_REG_TABLE); Index++) {
        IoWrite8 (ASPEED2500_SIO_INDEX_PORT, mASPEED2500Table[Index].Index);
        IoWrite8 (ASPEED2500_SIO_DATA_PORT, mASPEED2500Table[Index].Data);
      }
    } else if (ChipID == ASPEED2600_CHIP_ID) { // Aspeed2600
      for (Index = 0; Index < sizeof (mASPEED2600Table)/sizeof (SIO_REG_TABLE); Index++) {
        IoWrite8 (ASPEED2500_SIO_INDEX_PORT, mASPEED2600Table[Index].Index);
        IoWrite8 (ASPEED2500_SIO_DATA_PORT, mASPEED2600Table[Index].Data);
      }
    }

    //
    // Lock SIO
    //
    IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_LOCK);
  }
}

/**
  Checks if eSPI is available.

  @retval TRUE          Espi is available
  @retval FALSE         Espi is not available
**/
BOOLEAN
IsEspiAvailable (
  VOID
  )
{
  UINT32   eSPINcStat;

  eSPINcStat = MmioRead32 (PCH_PCR_ADDRESS (IBL_PID_ESPISPI, R_ESPI_PCR_SLV_CFG_REG_CTL));
  eSPINcStat = (eSPINcStat & B_ESPI_PCR_SLV_CFG_REG_CTL_eSPINC) >> N_ESPI_PCR_SLV_CFG_REG_CTL_eSPINC;

  return (eSPINcStat != V_ESPI_PCR_SLV_CFG_REG_CTL_eSPINC_DIS);
}

/**
  AND then OR PCH LPC and eSPI CS0# IO enable decoding.
  Setup I/O Enables in DMI to the same value program in LPC/eSPI PCI offset 82h.
  Note: Bit[15:10] of the source decode register is Read-Only. The IO range indicated by the Enables field
  in LPC/eSPI PCI offset 82h[13:10] is always forwarded by DMI to subtractive agent for handling.

  @param[in] AndValue        LPC IO enable decoding "AND" value.
  @param[in] OrValue         LPC IO enable decoding "OR" value.

  @retval EFI_SUCCESS                   Successfully completed.
  @retval EFI_UNSUPPORTED               DMIC.SRL is set.
**/
EFI_STATUS
PchLpcIoEnableDecodingAndThenOr (
  IN  UINT16        AndValue,
  IN  UINT16        OrValue
  )
{
  UINT64      LpcBaseAddr;
  UINT16      IoEnableDecoding;

  LpcBaseAddr = LpcPciCfgBase ();

  IoEnableDecoding = (PciSegmentRead16 (LpcBaseAddr + R_ESPI_CFG_ESPI_IOE) & AndValue) | OrValue;

  //
  // program PCI offset 82h for LPC/eSPI.
  //
  PciSegmentWrite16 (LpcBaseAddr + R_ESPI_CFG_ESPI_IOE, IoEnableDecoding);
  return EFI_SUCCESS;
}

/**

  Enable Decode ranges of Sio 2E ad 2F EspiConfigSpace

**/
VOID
SioDecodeEnable (
  VOID
  )
{
  PchLpcIoEnableDecodingAndThenOr ((UINT16)~0, V_ESPI_CFG_ESPI_IOE_SE);
}

/**
  Checks if BMC is available.

  @retval TRUE          BMC is available
  @retval FALSE         BMC is not available
**/
BOOLEAN
EFIAPI
IsBMCPresent (
  VOID
  )
{
  if (IsEspiAvailable ()) {
    // It is required to enable decoding of I/O locations 2Eh and 2Fh i.e.
    // Super I/O Enable(BIT12) of I/O Enables configuration register of eSPI.
    // This needs to be done before we check presence of aspeed.
    SioDecodeEnable ();
    // Now, as decoding of 2Eh and 2Fh is enabled, check if the Sio exists.
    return IsSioExist ();
  }
  return FALSE;
}
