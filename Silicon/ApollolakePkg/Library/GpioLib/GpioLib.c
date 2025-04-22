/** @file
  The platform GPIO library.

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <PiPei.h>
#include <Library/GpioLib.h>
#include <Library/DebugLib.h>
#include <RegAccess.h>
#include <GpioDefines.h>
#include <Library/ScSbiAccessLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Library/GpioLibApl.h>

//
// Structure for storing information about registers offset, community,
// maximal pad number for available groups
//
typedef struct {
  UINT8  Community;
  UINT16 SmiStsOffset;
  UINT16 SmiEnOffset;
  UINT16 PadCfgLockOffset;
  UINT16 PadCfgLockTxOffset;
  UINT8  PadPerGroup;
} GPIO_GROUP_INFO_STRUCT;

// GPIO Community Port ID definition
#define GPIO_SOUTHWEST_COMMUNITY     0xC0
#define GPIO_SOUTH_COMMUNITY         0xC2
#define GPIO_NORTHWEST_COMMUNITY     0xC4
#define GPIO_NORTH_COMMUNITY         0xC5
#define GPIO_WEST_COMMUNITY          0xC7

#define GPIO_GET_GROUP_FROM_PAD(Pad)  ((Pad) >> 16)
#define GPIO_GET_PAD_OFFSET(Pad)      ((Pad) & 0xFFFF)
#define GPIO_GET_PAD_NUMBER(Offset)   ((Offset - GPIO_PADBAR) / 8)

//
// If in GPIO_GROUP_INFO_STRUCT structure certain register doesn't exist
// it will have value equal to NO_REGISTER_FOR_PROPERTY
//
#define NO_REGISTER_FOR_PROPERTY 0xFFFF

//
// Possible registers to be accessed
//
typedef enum {
  GpioHostOwnershipRegister = 0,
  GpioGpeEnableRegister,
  GpioGpeStatusRegister,
  GpioSmiEnableRegister,
  GpioSmiStatusRegister,
  GpioNmiEnableRegister,
  GpioPadConfigLockRegister,
  GpioPadLockOutputRegister
} GPIO_REG;

typedef UINT32 GPIO_GROUP;

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_INFO_STRUCT mBxtpGpioGroupInfo[] = {
  {GPIO_SOUTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_SOUTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 11},
  {GPIO_WEST_COMMUNITY,      R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_WEST_COMMUNITY,      R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 15},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 32},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_2, R_PCR_GPIO_SMI_EN_2, R_PCR_GPIO_PADCFGLOCK_2, R_PCR_GPIO_PADCFGLOCKTX_2, 13},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 32},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_2, R_PCR_GPIO_SMI_EN_2, R_PCR_GPIO_PADCFGLOCK_2, R_PCR_GPIO_PADCFGLOCKTX_2, 13},
};

GLOBAL_REMOVE_IF_UNREFERENCED GPIO_GROUP_INFO_STRUCT mBxtGpioGroupInfo[] = {
  {GPIO_SOUTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 20},
  {GPIO_SOUTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 31},
  {GPIO_WEST_COMMUNITY,      R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_WEST_COMMUNITY,      R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 10},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 32},
  {GPIO_NORTHWEST_COMMUNITY, R_PCR_GPIO_SMI_STS_2, R_PCR_GPIO_SMI_EN_2, R_PCR_GPIO_PADCFGLOCK_2, R_PCR_GPIO_PADCFGLOCKTX_2, 8},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_0, R_PCR_GPIO_SMI_EN_0, R_PCR_GPIO_PADCFGLOCK_0, R_PCR_GPIO_PADCFGLOCKTX_0, 32},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_1, R_PCR_GPIO_SMI_EN_1, R_PCR_GPIO_PADCFGLOCK_1, R_PCR_GPIO_PADCFGLOCKTX_1, 32},
  {GPIO_NORTH_COMMUNITY,     R_PCR_GPIO_SMI_STS_2, R_PCR_GPIO_SMI_EN_2, R_PCR_GPIO_PADCFGLOCK_2, R_PCR_GPIO_PADCFGLOCKTX_2, 19},
};

///
/// BXT Series
///
typedef enum {
  Bxt          = 0x00,
  Bxt1,
  BxtX,
  BxtP,
  BxtSeriesMax = 0xFF
} BXT_SERIES;

/**
  Return SOC series type

  @retval  BXT_SERIES      SOC series type

**/
BXT_SERIES
EFIAPI
GetBxtSeries (
  VOID
  )
{
  UINTN   McD0Base;
  UINT16  VenId;
  UINT16  DevId;

  McD0Base = MM_PCI_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, 0);

  VenId = MmioRead16 (McD0Base + PCI_VENDOR_ID_OFFSET);
  DevId = MmioRead16 (McD0Base + PCI_DEVICE_ID_OFFSET);
  if (VenId == V_SA_MC_VID) {
    switch (DevId) {
      case V_SA_MC_DID0:
        return Bxt;
        break;
      case V_SA_MC_DID1:
        return Bxt1;
        break;
      case V_SA_MC_DID3:
        return BxtP;
        break;
      default:
        DEBUG ((DEBUG_ERROR, "Unsupported BXT Series.\n"));
        return BxtSeriesMax;
        break;
    }
  }
  return BxtSeriesMax;
}

/**
  This procedure will retrieve address and length of GPIO info table

  @param[out]  GpioGroupInfoTableLength   Length of GPIO group table

  @retval      Pointer to GPIO group table

**/
GPIO_GROUP_INFO_STRUCT*
EFIAPI
GpioGetGroupInfoTable (
  OUT UINT32 *GpioGroupInfoTableLength
  )
{
  BXT_SERIES  BxtSeries;

  BxtSeries = GetBxtSeries ();
  if (BxtSeries == BxtP) {
    *GpioGroupInfoTableLength = sizeof (mBxtpGpioGroupInfo) / sizeof (GPIO_GROUP_INFO_STRUCT);
    return mBxtpGpioGroupInfo;
  } else if (BxtSeries == Bxt || (BxtSeries == Bxt1)) {
    *GpioGroupInfoTableLength = sizeof (mBxtGpioGroupInfo) / sizeof (GPIO_GROUP_INFO_STRUCT);
    return mBxtGpioGroupInfo;
  } else {
    *GpioGroupInfoTableLength = 0;
    return NULL;
  }
}

STATIC
UINT32
EFIAPI
GetSideBandMmioAddress(
  IN UINT8 TargetPortId,
  IN UINT16 TargetRegister
  )
{
  UINT32 Temp;

  Temp  = MmioRead32 (MM_PCI_ADDRESS(0, PCI_DEVICE_NUMBER_P2SB, PCI_FUNCTION_NUMBER_P2SB, 0) + R_P2SB_BASE) & 0xff000000;
  Temp |= (UINT32)(TargetPortId << 16);
  Temp |= (UINT32)(TargetRegister);

  return Temp;
}


STATIC
UINT32
EFIAPI
SideBandRead32(
  IN UINT8  TargetPortId,
  IN UINT16 TargetRegister
  )
{
  UINT32 ReturnValue = 0;

  ReturnValue = MmioRead32(GetSideBandMmioAddress(TargetPortId, TargetRegister));

  return ReturnValue;
}

/**
  Determines whether to call a SideBand write via MMIO or SBMI (MMCFG) based on whether
  SBREG_REG is set up.

  This is the standard Write function that should be used for SideBand transactions.

  For MMIO Access:
    - In a single MMIO transaction, the "SBREG_BAR" is used
      to form a system address to access chipset registers.
    - Transactions using this method will write using Private Control Register opcode CRWr(07h).
    - All accesses using this method must be DWORD aligned DWORDs.

  For SBMI Access:  Refer to SideBandSbmiWrite32()

  @param  UINT8   Target port to write.
  @param  UINT16  Target register to write.
  @param  UINT32  The value to write.

  @retval None.
**/
STATIC
VOID
EFIAPI
SideBandWrite32(
  IN UINT8  TargetPortId,
  IN UINT16 TargetRegister,
  IN UINT32 Value
  )
{
  // MMIO access requires MMIO to be valid (P2SB BAR set and Memory Decode enabled) and the address to be 32-bit aligned
  MmioWrite32(GetSideBandMmioAddress(TargetPortId, TargetRegister), Value);
}


// Common interface for GPIO register read
UINT32
EFIAPI
GpioRead(
  IN UINT8 Community,
  IN UINT16 Offset
)
{
  return SideBandRead32(Community, Offset);
}

// Common interface for GPIO register write
EFI_STATUS
EFIAPI
GpioWrite(
  IN UINT8 Community,
  IN UINT16 Offset,
  IN UINT32 Value
)
{
  SideBandWrite32(Community, Offset, Value);

  return EFI_SUCCESS;
}

/**
  Program GPIO wake event for direct IRQ GPIO pins at power on.
  Assume EVOUTEN_0 and EVMAP_0 are not initialized.

  @param[in]  Community       - GPIO community.
  @param[in]  Index           - GPIO pad index in its community.
**/
VOID
ConfigureDirectIrqWakeEvent (
  IN UINT8     Community,
  IN UINT16    Index
  )
{
  UINT64 d64 = 0;
  UINT32 d32;

  //
  // Both North and NorthWest community GPIO index are linear
  // But some of the GPIO pins need to be filtered due to not support Direct IRQ
  //

  if (Community == NORTH) {
    if (Index > 61) { // filter the GPIO that doesn't support Direct IRQ
      //DEBUG ((DEBUG_ERROR, "\tThere is configuration error on direct IRQ! index: %d\n", Index));
      return;
    }
  } else if (Community == NORTHWEST) {
    if ((Index < 33) || (Index == 52) || (Index == 60)) { // filter the GPIO that doesn't support Direct IRQ
      //DEBUG ((DEBUG_ERROR, "\tThere is configuration error on direct IRQ! index: %d\n", Index));
      return;
    }
  } else { // Direct IRQ wake event is only supported on North and Northwest communities.
    return;
  }

  // program Event Trigger Mapping
  d64 = (LShiftU64((UINT64)GpioRead(Community, EVMAP_0 + 4), 32)) | GpioRead(Community, EVMAP_0);
  d64 |= LShiftU64((UINT64)(Index / EVENT_MUX_SIZE), ((Index % EVENT_MUX_SIZE) << 2));
  GpioWrite(Community, EVMAP_0, (UINT32)(d64 & 0xFFFFFFFF));
  GpioWrite(Community, EVMAP_0 + 4, (UINT32)(RShiftU64 (d64, 32)));

  // program Event Trigger Output Enable
  d32 = GpioRead(Community, EVOUTEN_0);
  d32 |=  1 << (Index % WAKE_EVENT_NUM);
  GpioWrite(Community, EVOUTEN_0, d32);

  return;
}


/**
  Common interface to read an 32-bit specific GPIO pad register.

  It is recommand to use the definition of GPIO pad in GpioLib.h
  e.g. GpioPadRead (N_GPIO_20)

  @param[in]  CommAndOffset    It contains GPIO community and offset.

  @retval     UINT32           The 32-bit value read from GpioRead operation.

**/
UINT32
EFIAPI
GpioPadRead (
  IN UINT32    CommAndOffset
  )
{
  UINT8   Community;
  UINT16  GpioPadOffset;

  GpioPadOffset = (UINT16) (CommAndOffset & 0xFFFF);
  Community  = (UINT8) (CommAndOffset >> 16);

  return GpioRead (Community, GpioPadOffset);
}


/**
  Common interface to write an 32-bit to a specific GPIO pad register.

  It is recommand to use the definition of GPIO pad in GpioLib.h
  e.g. GpioPadWrite (N_GPIO_20, 0xXXXXXXXX)

  @param[in]  CommAndOffset   It contains GPIO community and offset.
  @param[in]  Value           The value to write.

  @retval     EFI_SUCCESS     Write to a specific GPIO pad register successfully.
  @retval     Others          Status depends on GpioWrite operation.

**/
EFI_STATUS
EFIAPI
GpioPadWrite (
  IN UINT32    CommAndOffset,
  IN UINT32    Value
  )
{
  UINT8   Community;
  UINT16  GpioPadOffset;

  GpioPadOffset = (UINT16) (CommAndOffset & 0xFFFF);
  Community  = (UINT8) (CommAndOffset >> 16);

  return GpioWrite (Community, GpioPadOffset, Value);
}


STATIC
VOID
GpioPadConfigItem (
  IN BXT_GPIO_CONFIG    Gpio_Conf_Data
)
/*++

Routine Description:

  Set GPIO PAD DW 0 and 1 registers for S/W/SW/N/NW GPIO communities

Arguments:

  Gpio_Pin_Begin_Num        - Pin Begin Number to configure
  Gpio_Pin_Size             - Pin size to config for each GPIO communities.
  Gpio_Conf_Data            - GPIO_CONF_PAD_INIT data array for each GPIO communities.

--*/
{
  UINT8          community;
  UINT16         index;
  UINT32         pad_conf0, pad_conf1;
  UINT16         mmio_conf0, mmio_conf1;
  UINT32         D32;

  // Calculate the MMIO Address for specific GPIO pin CONF0 register pointed by index.
  mmio_conf0 = (UINT16)(Gpio_Conf_Data.CommAndOffset & 0xFFFF) + BXT_GPIO_PAD_CONF0_OFFSET;
  mmio_conf1 = (UINT16)(Gpio_Conf_Data.CommAndOffset & 0xFFFF) + BXT_GPIO_PAD_CONF1_OFFSET;
  community  = (UINT8) (Gpio_Conf_Data.CommAndOffset >> 16);
  index = (mmio_conf0 - GPIO_PADBAR) / 8;

  // check pad ownership before any programing, if it is not owned by host, skip it.
  D32 = GpioRead(community, PAD_OWNERSHIP_0 + (index/8)*4);
  D32 =  (D32 >> ((index % 8) << 2)) & V_PAD_OWNERSHIP_MASK;
  if (D32 != V_PAD_OWNERSHIP_HOST) { // check the ad_own_xx bit for each pin
    //DEBUG ((DEBUG_INFO, "Index=%2x is not owned by host! skip it. Ownership: %02X\n", index, D32));
    return;
  }

  pad_conf0 = GpioRead(community, mmio_conf0);
  pad_conf0 = (pad_conf0 & PAD_DW0_MASK) | Gpio_Conf_Data.padConfg0.padCnf0;
  GpioWrite(community, mmio_conf0, pad_conf0);

  pad_conf1 = GpioRead(community, mmio_conf1);
  pad_conf1 = (pad_conf1 & PAD_DW1_MASK) | Gpio_Conf_Data.padConfg1.padCnf1;
  GpioWrite(community, mmio_conf1, pad_conf1);

  // ACPI/GPIO driver ownership program
  if (Gpio_Conf_Data.HostSw == GPIO_D) {
    D32 = GpioRead(community, HOSTSW_OWNERSHIP_0 + (index/32)*4);
    D32 = D32 | (UINT32)(1 << (index%32));
    GpioWrite(community, HOSTSW_OWNERSHIP_0 + (index/32)*4, D32);
  } else {
    D32 = GpioRead(community, HOSTSW_OWNERSHIP_0 + (index/32)*4);
    D32 = D32 & (~(UINT32)(1 << (index%32)));
    GpioWrite(community, HOSTSW_OWNERSHIP_0 + (index/32)*4, D32);
  }

  switch (Gpio_Conf_Data.padConfg0.r.GPIRout) {
  case SCI: // SCI/GPE
    if (Gpio_Conf_Data.HostSw == ACPI_D) {
      //DEBUG ((DEBUG_INFO, "BXT-P configure GPIO to SCI/GPE. \n"));
      // clear GPE status
      D32 = GpioRead(community, GPI_GPE_STS_0 + (index/32)*4);
      //DEBUG ((DEBUG_INFO, "Original value of 0x%X: 0x%08X\n", GPI_GPE_STS_0 + (index/32)*4, D32));
      D32 = D32 | (UINT32)(1 << (index%32));
      GpioWrite(community, GPI_GPE_STS_0 + (index/32)*4, D32);
      // Set GPE enable
      D32 = GpioRead(community, GPI_GPE_EN_0 + (index/32)*4);
      //DEBUG ((DEBUG_INFO, "Original value of 0x%X: 0x%08X\n", GPI_GPE_EN_0 + (index/32)*4, D32));
      D32 = D32 | (UINT32)(1 << (index%32));
      GpioWrite(community, GPI_GPE_EN_0 + (index/32)*4, D32);
      D32 = GpioRead(community, GPI_GPE_EN_0 + (index/32)*4);
      //DEBUG ((DEBUG_INFO, "New value of 0x%X: 0x%08X\n", GPI_GPE_EN_0 + (index/32)*4, D32));
    }
    break;
  case SMI: // No SMI in SBL
    break;
  case IOAPIC: // Direct IRQ
    //DEBUG ((DEBUG_INFO, "BXT-P configure GPIO to direct IRQ. \n"));
    if (Gpio_Conf_Data.WakeEnabled == Wake_Enabled) {
      ConfigureDirectIrqWakeEvent(community, index);
    }
    break;
  case NMI: //No BXT pins are capable of NMI operation
    break;
  case NONE: // GPI(Shared interrupt) should be enabled by OS GPIO driver
    break;
  default:
    break;
  }
}


/**
  This procedure will write GPIO Lock/LockTx register using SBI.

  @param[in] RegType              GPIO register (Lock or LockTx)
  @param[in] Group                GPIO group number
  @param[in] PadNumber            Pad number of Group.

  @retval    EFI_SUCCESS          The function completed successfully
  @retval    EFI_UNSUPPORTED      Feature is not supported for this group or pad

**/
STATIC
EFI_STATUS
GpioWriteLockReg (
  IN GPIO_REG     RegType,
  IN GPIO_GROUP   Group,
  IN UINT32       PadNumber
  )
{
  UINT8            Response;
  EFI_STATUS       Status;
  GPIO_GROUP_INFO_STRUCT  *GpioGroupInfo;
  UINT32           GpioGroupInfoLength;
  UINT32           RegOffset;
  UINT32           OldPadCfgLockRegVal;
  UINT32           NewPadCfgLockRegVal;
  UINT32           GroupIndex;

  RegOffset = NO_REGISTER_FOR_PROPERTY;

  GpioGroupInfo = GpioGetGroupInfoTable (&GpioGroupInfoLength);
  if (GpioGroupInfo == NULL) {
    return EFI_UNSUPPORTED;
  }
  for (GroupIndex = 0; GroupIndex < GpioGroupInfoLength; GroupIndex++) {
    if (Group == GpioGroupInfo[GroupIndex].Community) {
      if (PadNumber < GpioGroupInfo[GroupIndex].PadPerGroup) {
        //
        // Found group and pad number
        //
        break;
      }
      PadNumber = PadNumber - GpioGroupInfo[GroupIndex].PadPerGroup;
    }
  }

  switch (RegType) {
    case GpioPadConfigLockRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockOffset;
      break;
    case GpioPadLockOutputRegister:
      RegOffset = GpioGroupInfo[GroupIndex].PadCfgLockTxOffset;
      break;
    default:
      ASSERT (FALSE);
      break;
  }

  //
  // Check if selected register exists
  //
  if (RegOffset == NO_REGISTER_FOR_PROPERTY) {
    return EFI_UNSUPPORTED;
  }

  OldPadCfgLockRegVal = MmioRead32 (SC_PCR_ADDRESS (GpioGroupInfo[GroupIndex].Community, RegOffset));
  NewPadCfgLockRegVal = OldPadCfgLockRegVal | (1 << PadNumber);

  Status = PchSbiExecution (
             GpioGroupInfo[GroupIndex].Community,
             RegOffset,
             GpioLockUnlock,
             FALSE,
             &NewPadCfgLockRegVal,
             &Response
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  This procedure will set PadCfgLock for selected pad

  @param[in] GpioPad                 GPIO pad

  @retval    EFI_SUCCESS             The function completed successfully
  @retval    EFI_INVALID_PARAMETER   Invalid group or pad number

**/
EFI_STATUS
EFIAPI
GpioLockPadCfg (
  IN UINT32    GpioPad
  )
{
  EFI_STATUS  Status;
  UINT8       Community;
  UINT16      Offset;
  UINT32      PadNumber;

  DEBUG ((DEBUG_VERBOSE, "GpioLockPadCfg: Pad == 0x%X\n", GpioPad));
  Community = (UINT8) GPIO_GET_GROUP_FROM_PAD (GpioPad);
  Offset    = GPIO_GET_PAD_OFFSET (GpioPad);
  PadNumber = GPIO_GET_PAD_NUMBER (Offset);

  Status = GpioWriteLockReg (
             GpioPadConfigLockRegister,
             Community,
             PadNumber
             );

  ASSERT_EFI_ERROR (Status);

  return Status;
}


/**
  This procedure will set PadCfgLockTx for selected pad

  @param[in] GpioPad                 GPIO pad

  @retval    EFI_SUCCESS             The function completed successfully
  @retval    EFI_INVALID_PARAMETER   Invalid group or pad number

**/
EFI_STATUS
EFIAPI
GpioLockPadCfgTx (
  IN UINT32    GpioPad
  )
{
  EFI_STATUS  Status;
  UINT8       Community;
  UINT16      Offset;
  UINT32      PadNumber;

  Community = (UINT8) GPIO_GET_GROUP_FROM_PAD (GpioPad);
  Offset    = GPIO_GET_PAD_OFFSET (GpioPad);
  PadNumber = GPIO_GET_PAD_NUMBER (Offset);

  Status = GpioWriteLockReg (
             GpioPadLockOutputRegister,
             Community,
             PadNumber
             );

  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
EFIAPI
GpioConfigurePads (
  IN UINT32                    NumberOfItems,
  IN VOID                     *GpioInitTableAddress
  )
/*++

Routine Description:

  Set GPIO PAD DW 0 and 1 registers for S/W/SW/N/NW GPIO communities

Arguments:

  NumberOfItems               - GPIO Pin Number to configure
  GpioInitTableAddress        - GPIO_CONF_PAD_INIT data array for each GPIO communities.

--*/
{
  UINT32           index;
  BXT_GPIO_PAD_INIT*  Gpio_Conf_Data;
  BXT_GPIO_CONFIG  mGpioConfigItem = {0};

  Gpio_Conf_Data = (BXT_GPIO_PAD_INIT *) (UINTN) GpioInitTableAddress;
  for (index = 0; index < NumberOfItems; index++) {
    mGpioConfigItem.CommAndOffset = (((UINT32)Gpio_Conf_Data[index].Community)<<16)+Gpio_Conf_Data[index].MMIO_ADDRESS;
    mGpioConfigItem.padConfg0.padCnf0 = Gpio_Conf_Data[index].padConfg0.padCnf0;
    mGpioConfigItem.padConfg1.padCnf1 = Gpio_Conf_Data[index].padConfg1.padCnf1;
    mGpioConfigItem.HostSw = Gpio_Conf_Data[index].HostSw;
    mGpioConfigItem.WakeEnabled = Gpio_Conf_Data[index].WakeEnabled;
    GpioPadConfigItem(mGpioConfigItem);
  }

  return EFI_SUCCESS;
}

/**
  Program SMIP GPIO PAD DW 0 and 1 registers for South/West/SouthWest/North/NorthWest GPIO communities.

  @param[in]  Gpio_Pin_Num         - GPIO Pin Number to configure.
  @param[in]  Gpio_Smip_Conf_Data  - Data array from SMIP setting for each GPIO communities.
**/
VOID
GpioSmipPadConfigTable (
  IN UINT32                    Gpio_Pin_Num,
  IN GPIO_CONFIG_SMIP          *Gpio_Smip_Conf_Data
)
{
  UINT32           index = 0;
  BXT_GPIO_CONFIG  mGpioConfigItem = {0};
  UINT8            Community = 0;

  for (index = 0; index < Gpio_Pin_Num; index++) {
    Community = (UINT8)Gpio_Smip_Conf_Data[index].Half0.r.Community;
    //
    // Skip null entry in SMIP GPIO table
    //
    if ((Community != SOUTH) && (Community != WEST) && (Community != NORTHWEST) && \
        (Community != SOUTHWEST) && (Community != NORTH)) {
      continue;
    }
    mGpioConfigItem.CommAndOffset = (UINT32)Gpio_Smip_Conf_Data[index].Half0.r.Community << 16 | Gpio_Smip_Conf_Data[index].Half0.r.Offset;
    mGpioConfigItem.padConfg0.padCnf0 = SMIP_Config0_Convert(Gpio_Smip_Conf_Data[index].Half1.r.padConfig0);
    mGpioConfigItem.padConfg1.padCnf1 = SMIP_Config1_Convert(Gpio_Smip_Conf_Data[index].Half1.r.padConfig1);
    mGpioConfigItem.HostSw = (UINT8)Gpio_Smip_Conf_Data[index].Half0.r.HostSw;
    mGpioConfigItem.WakeEnabled = (UINT8)Gpio_Smip_Conf_Data[index].Half0.r.DirectIrqWake;
    GpioPadConfigItem (mGpioConfigItem);
  }
}

/**
  This procedure will get GPIO input level

  @param[in]  GpioPad             GPIO pad
  @param[in]  Term                Termination
  @param[out] InputVal            GPIO Input value
                                  0: InputLow, 1: InputHigh

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Invalid GpioPad
**/
EFI_STATUS
EFIAPI
GpioGetInputValueWithTerm (
  IN  UINT32       GpioPad,
  IN  UINT32       Term,
  OUT UINT32      *InputVal
  )
{
  UINT8           Community;
  UINT16          PadNumber;
  BXT_CONF_PAD0   PadConfg0;
  BXT_CONF_PAD1   PadConfg1;
  UINT32          Data32;

  Community = GpioPad & 0xFF;
  if ((Community != SOUTH) && (Community != WEST) && (Community != NORTHWEST) && \
      (Community != SOUTHWEST) && (Community != NORTH)) {
    return EFI_INVALID_PARAMETER;
  }

  PadNumber = (UINT16)(GpioPad >> 8);
  if (PadNumber >= 0x300) {
    return EFI_INVALID_PARAMETER;
  }

  PadNumber += GPIO_PADBAR;
  PadConfg0.padCnf0 = GpioRead (Community, PadNumber + BXT_GPIO_PAD_CONF0_OFFSET);
  PadConfg0.r.PMode = 0;
  PadConfg0.r.GPIORxTxDis = 0x1;
  GpioWrite (Community, PadNumber + BXT_GPIO_PAD_CONF0_OFFSET, PadConfg0.padCnf0);
  PadConfg1.padCnf1 = GpioRead (Community, PadNumber + BXT_GPIO_PAD_CONF1_OFFSET);
  PadConfg1.r.Term  = Term & 0x0F;
  GpioWrite (Community, PadNumber + BXT_GPIO_PAD_CONF1_OFFSET, PadConfg1.padCnf1);
  Data32 = GpioRead (Community, PadNumber + BXT_GPIO_PAD_CONF0_OFFSET);

  if (InputVal != NULL) {
    *InputVal = (Data32 & BIT1) >> 1;
  }

  return EFI_SUCCESS;
}
