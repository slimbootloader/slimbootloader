/** @file
  SC SPI Common Driver implements the SPI Host Controller Compatibility Interface.

  Copyright (c) 2017-2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi/UefiBaseType.h>
#include <IndustryStandard/Pci30.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SpiFlashLib.h>
#include <Library/ScSpiCommon.h>
#include <RegAccess.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BootloaderCommonLib.h>
#include <Guid/OsBootOptionGuid.h>

const SPI_FLASH_SERVICE   mSpiFlashService = {
  .Header.Signature = SPI_FLASH_SERVICE_SIGNATURE,
  .Header.Version   = SPI_FLASH_SERVICE_VERSION,
  .SpiInit          = SpiConstructor,
  .SpiRead          = SpiFlashRead,
  .SpiWrite         = SpiFlashWrite,
  .SpiErase         = SpiFlashErase,
  .SpiGetRegion     = SpiGetRegionAddress
};

/**
  Get SPI Instance from library global data..

  @retval SpiInstance       Return SPI instance
**/
SPI_INSTANCE *
GetSpiInstance (
  VOID
)
{
  EFI_STATUS         Status;
  SPI_INSTANCE      *SpiInstance;

  Status = GetLibraryData (PcdGet8(PcdSpiFlashLibId), (VOID **)&SpiInstance);
  if (Status == EFI_NOT_FOUND) {
    SpiInstance  = AllocatePool (sizeof(SPI_INSTANCE));
    ZeroMem (SpiInstance, sizeof(SPI_INSTANCE));
    Status = SetLibraryData (PcdGet8(PcdSpiFlashLibId), SpiInstance, sizeof(SPI_INSTANCE));
  }

  if (EFI_ERROR(Status)) {
    SpiInstance = NULL;
  }

  return SpiInstance;
}



/**
  Initialize an SPI library.

  @retval EFI_SUCCESS             The protocol instance was properly initialized
  @exception EFI_UNSUPPORTED      The SC is not supported by this module
**/
EFI_STATUS
EFIAPI
SpiConstructor (
  VOID
  )
{
  UINT32          ScSpiBar0;
  UINT8           Comp0Density;
  SPI_INSTANCE   *SpiInstance;
  EFI_STATUS      Status;

  //
  // Initialize the SPI instance
  //
  SpiInstance = GetSpiInstance();
  ASSERT (SpiInstance != NULL);
  if (SpiInstance->Signature == SC_SPI_PRIVATE_DATA_SIGNATURE) {
    Status = RegisterService ((VOID *)&mSpiFlashService);
    return Status;
  }
  DEBUG ((DEBUG_INFO, "SpiInstance = %08X\n", SpiInstance));

  SpiInstance->Signature  = SC_SPI_PRIVATE_DATA_SIGNATURE;
  SpiInstance->Handle     = NULL;

  SpiInstance->PchSpiBase = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiInstance->PchSpiBase = TO_MM_PCI_ADDRESS (SpiInstance->PchSpiBase);

  SpiInstance->PchAcpiBase = ACPI_BASE_ADDRESS;
  ASSERT (SpiInstance->PchAcpiBase != 0);

  ScSpiBar0 = MmioRead32 (SpiInstance->PchSpiBase + R_SPI_BASE) & ~(B_SPI_BAR0_MASK);
  if (ScSpiBar0 == 0) {
    ASSERT (FALSE);
  }

  MmioOr32 (SpiInstance->PchSpiBase + PCI_COMMAND_OFFSET, EFI_PCI_COMMAND_MEMORY_SPACE);
  SpiInstance->RegionPermission = MmioRead16 (ScSpiBar0 + R_SPI_FRAP);
  SpiInstance->SfdpVscc0Value = MmioRead32 (ScSpiBar0 + R_SPI_LVSCC);
  SpiInstance->SfdpVscc1Value = MmioRead32 (ScSpiBar0 + R_SPI_UVSCC);

  ///
  /// Select to Flash Map 0 Register to get the number of flash Component
  ///
  MmioAndThenOr32 (
    ScSpiBar0 + R_SPI_FDOC,
    (UINT32) (~(B_SPI_FDOC_FDSS_MASK | B_SPI_FDOC_FDSI_MASK)),
    (UINT32) (V_SPI_FDOC_FDSS_FSDM | R_SPI_FDBAR_FLASH_MAP0)
    );

  ///
  /// Copy Zero based Number Of Components
  ///
  SpiInstance->NumberOfComponents = (UINT8) ((MmioRead16 (ScSpiBar0 + R_SPI_FDOD) & B_SPI_FDBAR_NC) >> N_SPI_FDBAR_NC);

  MmioAndThenOr32 (
    ScSpiBar0 + R_SPI_FDOC,
    (UINT32) (~(B_SPI_FDOC_FDSS_MASK | B_SPI_FDOC_FDSI_MASK)),
    (UINT32) (V_SPI_FDOC_FDSS_COMP | R_SPI_FCBA_FLCOMP)
    );

  ///
  /// Copy Component 0 Density
  ///
  Comp0Density = (UINT8) MmioRead32 (ScSpiBar0 + R_SPI_FDOD) & B_SPI_FLCOMP_COMP1_MASK;
  SpiInstance->Component1StartAddr = (UINT32) (V_SPI_FLCOMP_COMP1_512KB << Comp0Density);

  ///
  /// Select FLASH_MAP1 to get Flash SC Strap Base Address
  ///
  MmioAndThenOr32 (
    (ScSpiBar0 + R_SPI_FDOC),
    (UINT32) (~(B_SPI_FDOC_FDSS_MASK | B_SPI_FDOC_FDSI_MASK)),
    (UINT32) (V_SPI_FDOC_FDSS_FSDM | R_SPI_FDBAR_FLASH_MAP1)
    );

  SpiInstance->StrapBaseAddress = MmioRead32 (ScSpiBar0 + R_SPI_FDOD) & B_SPI_FDBAR_FPSBA;

  ///
  /// Align FPSBA with address bits for the SC Strap portion of flash descriptor
  ///
  SpiInstance->StrapBaseAddress &= B_SPI_FDBAR_FPSBA;

  Status = RegisterService ((VOID *)&mSpiFlashService);

  return Status;
}



/**
  Delay for at least the request number of microseconds for Runtime usage.

  @param[in] ABase                Acpi base address
  @param[in] Microseconds         Number of microseconds to delay.

  @retval None
**/
VOID
EFIAPI
PchPmTimerStallRuntimeSafe (
  IN  UINT16  ABase,
  IN  UINTN   Microseconds
  )
{
  UINTN   Ticks;
  UINTN   Counts;
  UINTN   CurrentTick;
  UINTN   OriginalTick;
  UINTN   RemainingTick;

  if (Microseconds == 0) {
    return;
  }

  OriginalTick   = IoRead32 ((UINTN) (ABase + R_ACPI_PM1_TMR)) & B_ACPI_PM1_TMR_VAL;
  CurrentTick    = OriginalTick;

  ///
  /// The timer frequency is 3.579545 MHz, so 1 ms corresponds 3.58 clocks
  ///
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;

  ///
  /// The loops needed by timer overflow
  ///
  Counts = Ticks / V_ACPI_PM1_TMR_MAX_VAL;

  ///
  /// Remaining clocks within one loop
  ///
  RemainingTick = Ticks % V_ACPI_PM1_TMR_MAX_VAL;

  ///
  /// not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
  /// one I/O operation, and maybe generate SMI
  ///
  while ((Counts != 0) || (RemainingTick > CurrentTick)) {
    CurrentTick = IoRead32 ((UINTN) (ABase + R_ACPI_PM1_TMR)) & B_ACPI_PM1_TMR_VAL;
    ///
    /// Check if timer overflow
    ///
    if ((CurrentTick < OriginalTick)) {
      if (Counts != 0) {
        Counts--;
      } else {
        ///
        /// If timer overflow and Counts equal to 0, that means we already stalled more than
        /// RemainingTick, break the loop here
        ///
        break;
      }
    }

    OriginalTick = CurrentTick;
  }
}

/**
  Read data from the flash part.

  @param[in] FlashRegionType      The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.
  @param[out] Buffer              The Pointer to caller-allocated buffer containing the data received.
                                  It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashRead (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount,
  OUT    UINT8              *Buffer
  )
{
  EFI_STATUS        Status;

  ///
  /// Sends the command to the SPI interface to execute.
  ///
  Status = SendSpiCmd (
             FlashRegionType,
             FlashCycleRead,
             Address,
             ByteCount,
             Buffer
             );
  return Status;
}

/**
  Write data to the flash part.

  @param[in] FlashRegionType      The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.
  @param[in] Buffer               Pointer to caller-allocated buffer containing the data sent during the SPI cycle.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashWrite (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount,
  IN     UINT8              *Buffer
  )
{
  EFI_STATUS        Status;

  ///
  /// Sends the command to the SPI interface to execute.
  ///
  Status = SendSpiCmd (
             FlashRegionType,
             FlashCycleWrite,
             Address,
             ByteCount,
             Buffer
             );
  return Status;
}

/**
  Erase some area on the flash part.

  @param[in] FlashRegionType      The Flash Region type for flash cycle which is listed in the Descriptor.
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashErase (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount
  )
{
  EFI_STATUS        Status;

  ///
  /// Sends the command to the SPI interface to execute.
  ///
  Status = SendSpiCmd (
             FlashRegionType,
             FlashCycleErase,
             Address,
             ByteCount,
             NULL
             );
  return Status;
}

/**
  Read SFDP data from the flash part.

  @param[in] This                 Pointer to the SC_SPI_PROTOCOL instance.
  @param[in] ComponentNumber      The Component Number for chip select
  @param[in] ByteCount            Number of bytes in SFDP data portion of the SPI cycle, the max number is 64
  @param[out] SfdpData            The Pointer to caller-allocated buffer containing the SFDP data received
                                  It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashReadSfdp (
  IN     UINT8              ComponentNumber,
  IN     UINT32             ByteCount,
  OUT    UINT8              *SfdpData
  )
{
  EFI_STATUS        Status;
  UINT32            Address;
  SPI_INSTANCE      *SpiInstance;

  SpiInstance       = GetSpiInstance();
  Status            = EFI_SUCCESS;

  if ((ByteCount > 64) || (ComponentNumber > SpiInstance->NumberOfComponents)) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  Address = 0;
  if (ComponentNumber == FlashComponent1) {
    Address = SpiInstance->Component1StartAddr;
  }
  ///
  /// Sends the command to the SPI interface to execute.
  ///
  Status = SendSpiCmd (
             0,
             FlashCycleReadSfdp,
             Address,
             ByteCount,
             SfdpData
             );
  return Status;
}

/**
  Read Jedec Id from the flash part.

  @param[in] ComponentNumber      The Component Number for chip select
  @param[in] ByteCount            Number of bytes in JedecId data portion of the SPI cycle, the data size is 3 typically
  @param[out] JedecId             The Pointer to caller-allocated buffer containing JEDEC ID received
                                  It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashReadJedecId (
  IN     UINT8              ComponentNumber,
  IN     UINT32             ByteCount,
  OUT    UINT8              *JedecId
  )
{
  EFI_STATUS        Status;
  UINT32            Address;
  SPI_INSTANCE      *SpiInstance;

  SpiInstance       = GetSpiInstance();
  Status            = EFI_SUCCESS;

  if (ComponentNumber > SpiInstance->NumberOfComponents) {
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  Address = 0;
  if (ComponentNumber == FlashComponent1) {
    Address = SpiInstance->Component1StartAddr;
  }

  ///
  /// Sends the command to the SPI interface to execute.
  ///
  Status = SendSpiCmd (
             0,
             FlashCycleReadJedecId,
             Address,
             ByteCount,
             JedecId
             );
  return Status;
}

/**
  Write the status register in the flash part.

  @param[in] ByteCount            Number of bytes in Status data portion of the SPI cycle, the data size is 1 typically
  @param[in] StatusValue          The Pointer to caller-allocated buffer containing the value of Status register writing

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashWriteStatus (
  IN     UINT32             ByteCount,
  IN     UINT8              *StatusValue
  )
{
  EFI_STATUS        Status;

  ///
  /// Sends the command to the SPI interface to execute.
  ///
  Status = SendSpiCmd (
             0,
             FlashCycleWriteStatus,
             0,
             ByteCount,
             StatusValue
             );
  return Status;
}

/**
  Read status register in the flash part.

  @param[in] ByteCount            Number of bytes in Status data portion of the SPI cycle, the data size is 1 typically
  @param[out] StatusValue         The Pointer to caller-allocated buffer containing the value of Status register received.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiFlashReadStatus (
  IN     UINT32             ByteCount,
  OUT    UINT8              *StatusValue
  )
{
  EFI_STATUS        Status;

  ///
  /// Sends the command to the SPI interface to execute.
  ///
  Status = SendSpiCmd (
             0,
             FlashCycleReadStatus,
             0,
             ByteCount,
             StatusValue
             );
  return Status;
}

/**
  Read SC Soft Strap Values

  @param[in] SoftStrapAddr        SC Soft Strap address offset from FPSBA.
  @param[in] ByteCount            Number of bytes in SoftStrap data portion of the SPI cycle
  @param[out] SoftStrapValue      The Pointer to caller-allocated buffer containing SC Soft Strap Value.
                                  It is the caller's responsibility to make sure Buffer is large enough for the total number of bytes read.

  @retval EFI_SUCCESS             Command succeed.
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
EFI_STATUS
EFIAPI
SpiReadPchSoftStrap (
  IN     UINT32             SoftStrapAddr,
  IN     UINT32             ByteCount,
  OUT    UINT8              *SoftStrapValue
  )
{
  UINT32            StrapFlashAddr;
  EFI_STATUS        Status;
  SPI_INSTANCE      *SpiInstance;

  SpiInstance = GetSpiInstance();
  ASSERT (SpiInstance->StrapBaseAddress != 0);
  ///
  /// SC Strap Flash Address = FPSBA + RamAddr
  ///
  StrapFlashAddr = SpiInstance->StrapBaseAddress + SoftStrapAddr;

  ///
  /// Read SC Soft straps from using execute command
  ///
  Status = SendSpiCmd (
             FlashRegionDescriptor,
             FlashCycleRead,
             StrapFlashAddr,
             ByteCount,
             SoftStrapValue
             );
  return Status;
}

/**
  This function sends the programmed SPI command to the slave device.

  @param[in] SpiRegionType        The SPI Region type for flash cycle which is listed in the Descriptor
  @param[in] FlashCycleType       The Flash SPI cycle type list in HSFC (Hardware Sequencing Flash Control Register) register
  @param[in] Address              The Flash Linear Address must fall within a region for which BIOS has access permissions.
  @param[in] ByteCount            Number of bytes in the data portion of the SPI cycle.
  @param[in,out] Buffer           Pointer to caller-allocated buffer containing the data received or sent during the SPI cycle.

  @retval EFI_SUCCESS             SPI command completes successfully.
  @retval EFI_DEVICE_ERROR        Device error, the command aborts abnormally.
  @retval EFI_ACCESS_DENIED       Some unrecognized command encountered in hardware sequencing mode
  @retval EFI_INVALID_PARAMETER   The parameters specified are not valid.
**/
EFI_STATUS
SendSpiCmd (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  IN     FLASH_CYCLE_TYPE   FlashCycleType,
  IN     UINT32             Address,
  IN     UINT32             ByteCount,
  IN OUT UINT8              *Buffer
  )
{
  EFI_STATUS      Status;
  UINT32          Index;
  UINTN           SpiBaseAddress;
  UINT32          ScSpiBar0;
  UINT32          LimitAddress;
  UINT32          HardwareSpiAddr;
  UINT16          PermissionBit;
  UINT32          SpiDataCount;
  UINT32          FlashCycle;
  UINT8           BiosCtlSave;
  UINT32          SmiEnSave;
  UINT16          ABase;
  SPI_INSTANCE    *SpiInstance;

  SpiInstance       = GetSpiInstance();
  Status            = EFI_SUCCESS;
  ScSpiBar0         = AcquireSpiBar0 ();
  SpiBaseAddress    = SpiInstance->PchSpiBase;
  ABase             = SpiInstance->PchAcpiBase;

  ///
  /// Disable SMIs to make sure normal mode flash access is not interrupted by an SMI
  /// whose SMI handler accesses flash (e.g. for error logging)
  ///
  /// *** NOTE: if the SMI_LOCK bit is set (i.e., PMC PCI Offset A0h [4]='1'),
  /// clearing B_GBL_SMI_EN will not have effect. In this situation, some other
  /// synchronization methods must be applied here or in the consumer of the
  /// SendSpiCmd. An example method is disabling the specific SMI sources
  /// whose SMI handlers access flash before flash cycle and re-enabling the SMI
  /// sources after the flash cycle .
  ///
  SmiEnSave   = IoRead32 ((UINTN) (ABase + R_SMI_EN));
  IoWrite32 ((UINTN) (ABase + R_SMI_EN), SmiEnSave & (UINT32) (~B_SMI_EN_GBL_SMI));
  BiosCtlSave = MmioRead8 (SpiBaseAddress + R_SPI_BCR) & B_SPI_BCR_SRC;

  ///
  /// If it's write cycle, disable Prefetching, Caching and disable BIOS Write Protect
  ///
  if ((FlashCycleType == FlashCycleWrite) ||
      (FlashCycleType == FlashCycleErase)) {
    Status = DisableBiosWriteProtect ();
    if (EFI_ERROR (Status)) {
      goto SendSpiCmdEnd;
    }
    MmioAndThenOr32 (
      SpiBaseAddress + R_SPI_BCR,
      (UINT32) (~B_SPI_BCR_SRC),
      (UINT32) (V_SPI_BCR_SRC_PREF_DIS_CACHE_DIS <<  B_SPI_BCR_SRC)
      );
  }
  ///
  /// Make sure it's safe to program the command.
  ///
  if (!WaitForSpiCycleComplete (ScSpiBar0, FALSE)) {
    Status = EFI_DEVICE_ERROR;
    goto SendSpiCmdEnd;
  }

  HardwareSpiAddr = Address;
  if ((FlashCycleType == FlashCycleRead) ||
      (FlashCycleType == FlashCycleWrite) ||
      (FlashCycleType == FlashCycleErase)) {
    switch (FlashRegionType) {
    case FlashRegionDescriptor:
      if (FlashCycleType == FlashCycleRead) {
        PermissionBit = B_SPI_FRAP_BRRA_FLASHD;
      } else {
        PermissionBit = B_SPI_FRAP_BRRA_FLASHD;
      }
      HardwareSpiAddr += (MmioRead32 (ScSpiBar0 + R_SPI_FREG0_FLASHD) &
                          B_SPI_FREG0_BASE_MASK) << N_SPI_FREG0_BASE;
      LimitAddress = (MmioRead32 (ScSpiBar0 + R_SPI_FREG0_FLASHD) &
                      B_SPI_FREG0_LIMIT_MASK) >> N_SPI_FREG0_LIMIT;
      break;
    case FlashRegionBios:
      if (FlashCycleType == FlashCycleRead) {
        PermissionBit = B_SPI_FRAP_BRRA_BIOS;
      } else {
        PermissionBit = B_SPI_FRAP_BRRA_BIOS;
      }
      HardwareSpiAddr += (MmioRead32 (ScSpiBar0 + R_SPI_FREG1_BIOS) &
                          B_SPI_FREG1_BASE_MASK) << N_SPI_FREG1_BASE;
      LimitAddress = (MmioRead32 (ScSpiBar0 + R_SPI_FREG1_BIOS) &
                      B_SPI_FREG1_LIMIT_MASK) >> N_SPI_FREG1_LIMIT;
      break;
    case FlashRegionMe:
      if (FlashCycleType == FlashCycleRead) {
        PermissionBit = B_SPI_FRAP_BRRA_SEC;
      } else {
        PermissionBit = B_SPI_FRAP_BRWA_SEC;
      }
      HardwareSpiAddr += (MmioRead32 (ScSpiBar0 + R_SPI_FREG2_SEC) &
                          B_SPI_FREG2_BASE_MASK) << N_SPI_FREG2_BASE;
      LimitAddress = (MmioRead32 (ScSpiBar0 + R_SPI_FREG2_SEC) &
                      B_SPI_FREG2_LIMIT_MASK) >> N_SPI_FREG2_LIMIT;
      break;
    case FlashRegionGbE:
      if (FlashCycleType == FlashCycleRead) {
        PermissionBit = B_SPI_FRAP_BRRA_GBE;
      } else {
        PermissionBit = B_SPI_FRAP_BRWA_GBE;
      }
      HardwareSpiAddr += (MmioRead32 (ScSpiBar0 + R_SPI_FREG3_GBE) &
                          B_SPI_FREG3_BASE_MASK) << N_SPI_FREG3_BASE;
      LimitAddress = (MmioRead32 (ScSpiBar0 + R_SPI_FREG3_GBE) &
                      B_SPI_FREG3_LIMIT_MASK) >> N_SPI_FREG3_LIMIT;
      break;
    case FlashRegionPlatformData:
      if (FlashCycleType == FlashCycleRead) {
        PermissionBit = B_SPI_FRAP_BRRA_PLATFORM;
      } else {
        PermissionBit = B_SPI_FRAP_BRWA_PLATFORM;
      }
      HardwareSpiAddr += (MmioRead32 (ScSpiBar0 + R_SPI_FREG4_PLATFORM_DATA) &
                          B_SPI_FREG4_BASE_MASK) << N_SPI_FREG4_BASE;
      LimitAddress = (MmioRead32 (ScSpiBar0 + R_SPI_FREG4_PLATFORM_DATA) &
                      B_SPI_FREG4_LIMIT_MASK) >> N_SPI_FREG4_LIMIT;
      break;
    case FlashRegionAll:
      ///
      /// FlashRegionAll indicates address is relative to flash device (i.e., address is Flash Linear Address)
      /// No error checking for this case
      ///
      LimitAddress = 0;
      PermissionBit = 0;
      break;
    default:
      Status = EFI_UNSUPPORTED;
      goto SendSpiCmdEnd;
    }
    if ((LimitAddress != 0) && (Address > LimitAddress)) {
      Status = EFI_INVALID_PARAMETER;
      goto SendSpiCmdEnd;
    }
    ///
    /// If the operation is read, but the region attribute is not read allowed, return error.
    /// If the operation is write, but the region attribute is not write allowed, return error.
    ///
    if ((PermissionBit != 0) && ((SpiInstance->RegionPermission & PermissionBit) == 0)) {
      Status = EFI_ACCESS_DENIED;
      goto SendSpiCmdEnd;
    }
  }
  ///
  /// Check for SC SPI hardware sequencing required commands
  ///
  FlashCycle = 0;
  switch (FlashCycleType) {
  case FlashCycleRead:
    FlashCycle = (UINT32) (V_SPI_HSFS_CYCLE_READ << N_SPI_HSFS_CYCLE);
    break;
  case FlashCycleWrite:
    FlashCycle = (UINT32) (V_SPI_HSFS_CYCLE_WRITE << N_SPI_HSFS_CYCLE);
    break;
  case FlashCycleErase:
    if (((ByteCount % SIZE_4KB) != 0) ||
        ((HardwareSpiAddr % SIZE_4KB) != 0)) {
      DEBUG ((DEBUG_ERROR, "     Erase and erase size must be 4KB aligned. \n"));
      ASSERT (FALSE);
      Status = EFI_INVALID_PARAMETER;
      goto SendSpiCmdEnd;
    }
    break;
  case FlashCycleReadSfdp:
    FlashCycle = (UINT32) (V_SPI_HSFS_CYCLE_READ_SFDP << N_SPI_HSFS_CYCLE);
    break;
  case FlashCycleReadJedecId:
    FlashCycle = (UINT32) (V_SPI_HSFS_CYCLE_READ_JEDEC_ID << N_SPI_HSFS_CYCLE);
    break;
  case FlashCycleWriteStatus:
    FlashCycle = (UINT32) (V_SPI_HSFS_CYCLE_WRITE_STATUS << N_SPI_HSFS_CYCLE);
    break;
  case FlashCycleReadStatus:
    FlashCycle = (UINT32) (V_SPI_HSFS_CYCLE_READ_STATUS << N_SPI_HSFS_CYCLE);
    break;
  default:
    ///
    /// Unrecognized Operation
    ///
    ASSERT (FALSE);
    Status = EFI_INVALID_PARAMETER;
    goto SendSpiCmdEnd;
    break;
  }

  do {
    SpiDataCount = ByteCount;
    if ((FlashCycleType == FlashCycleRead) || (FlashCycleType == FlashCycleWrite)) {
      ///
      /// Trim at 256 byte boundary per operation,
      /// - SC SPI controller requires trimming at 4KB boundary
      /// - Some SPI chips require trimming at 256 byte boundary for write operation
      /// - Trimming has limited performance impact as we can read / write at most 64 byte
      ///   per operation
      ///
      if (HardwareSpiAddr + ByteCount > ((HardwareSpiAddr + BIT8) &~(BIT8 - 1))) {
        SpiDataCount = (((UINT32) (HardwareSpiAddr) + BIT8) &~(BIT8 - 1)) - (UINT32) (HardwareSpiAddr);
      }
      ///
      /// Calculate the number of bytes to shift in/out during the SPI data cycle.
      /// Valid settings for the number of bytes during each data portion of the
      /// SC SPI cycles are: 0, 1, 2, 3, 4, 5, 6, 7, 8, 16, 24, 32, 40, 48, 56, 64
      ///
      if (SpiDataCount >= 64) {
        SpiDataCount = 64;
      } else if ((SpiDataCount &~0x07) != 0) {
        SpiDataCount = SpiDataCount &~0x07;
      }
    }
    if (FlashCycleType == FlashCycleErase) {
      if (((ByteCount / SIZE_64KB) != 0) &&
          ((ByteCount % SIZE_64KB) == 0) &&
          ((HardwareSpiAddr % SIZE_64KB) == 0)) {
        if (HardwareSpiAddr < SpiInstance->Component1StartAddr) {
          //
          // Check whether Component0 support 64k Erase
          //
          if ((SpiInstance->SfdpVscc0Value & B_SPI_LVSCC_EO_64K) != 0) {
            SpiDataCount = SIZE_64KB;
          } else {
            SpiDataCount = SIZE_4KB;
          }
        } else {
          //
          // Check whether Component1 support 64k Erase
          //
          if ((SpiInstance->SfdpVscc1Value & B_SPI_LVSCC_EO_64K) != 0) {
            SpiDataCount = SIZE_64KB;
          } else {
            SpiDataCount = SIZE_4KB;
          }
        }
      } else {
        SpiDataCount = SIZE_4KB;
      }
      if (SpiDataCount == SIZE_4KB) {
        FlashCycle = (UINT32) (V_SPI_HSFS_CYCLE_4K_ERASE << N_SPI_HSFS_CYCLE);
      } else {
        FlashCycle = (UINT32) (V_SPI_HSFS_CYCLE_64K_ERASE << N_SPI_HSFS_CYCLE);
      }
    }
    ///
    /// If it's write cycle, load data into the SPI data buffer.
    ///
    if ((FlashCycleType == FlashCycleWrite) || (FlashCycleType == FlashCycleWriteStatus)) {
      if ((SpiDataCount & 0x07) != 0) {
        ///
        /// Use Byte write if Data Count is 0, 1, 2, 3, 4, 5, 6, 7
        ///
        for (Index = 0; Index < SpiDataCount; Index++) {
          MmioWrite8 (ScSpiBar0 + R_SPI_FDATA00 + Index, Buffer[Index]);
        }
      } else {
        ///
        /// Use Dword write if Data Count is 8, 16, 24, 32, 40, 48, 56, 64
        ///
        for (Index = 0; Index < SpiDataCount; Index += sizeof (UINT32)) {
          MmioWrite32 (ScSpiBar0 + R_SPI_FDATA00 + Index, *(UINT32 *) (Buffer + Index));
        }
      }
    }

    ///
    /// Set the Flash Address
    ///
    MmioWrite32 (
      (ScSpiBar0 + R_SPI_FADDR),
      (UINT32) (HardwareSpiAddr & B_SPI_FADDR_MASK)
      );

    ///
    /// Set Data count, Flash cycle, and Set Go bit to start a cycle
    ///
    MmioAndThenOr32 (
      ScSpiBar0 + R_SPI_HSFS,
      (UINT32) (~(B_SPI_HSFS_FDBC_MASK | B_SPI_HSFS_CYCLE_MASK)),
      (UINT32) (((SpiDataCount - 1) << N_SPI_HSFS_FDBC) | FlashCycle | B_SPI_HSFS_CYCLE_FGO)
      );

    ///
    /// end of command execution
    ///
    /// Wait the SPI cycle to complete.
    ///
    if (!WaitForSpiCycleComplete (ScSpiBar0, TRUE)) {
      Status = EFI_DEVICE_ERROR;
      goto SendSpiCmdEnd;
    }
    ///
    /// If it's read cycle, load data into the caller's buffer.
    ///
    if ((FlashCycleType == FlashCycleRead) ||
        (FlashCycleType == FlashCycleReadSfdp) ||
        (FlashCycleType == FlashCycleReadJedecId) ||
        (FlashCycleType == FlashCycleReadStatus)) {
      if ((SpiDataCount & 0x07) != 0) {
        ///
        /// Use Byte read if Data Count is 0, 1, 2, 3, 4, 5, 6, 7
        ///
        for (Index = 0; Index < SpiDataCount; Index++) {
          Buffer[Index] = MmioRead8 (ScSpiBar0 + R_SPI_FDATA00 + Index);
        }
      } else {
        ///
        /// Use Dword read if Data Count is 8, 16, 24, 32, 40, 48, 56, 64
        ///
        for (Index = 0; Index < SpiDataCount; Index += sizeof (UINT32)) {
          *(UINT32 *) (Buffer + Index) = MmioRead32 (ScSpiBar0 + R_SPI_FDATA00 + Index);
        }
      }
    }

    HardwareSpiAddr += SpiDataCount;
    Buffer += SpiDataCount;
    ByteCount -= SpiDataCount;
  } while (ByteCount > 0);

SendSpiCmdEnd:
  ///
  /// Restore the settings for SPI Prefetching and Caching and enable BIOS Write Protect
  ///
  if ((FlashCycleType == FlashCycleWrite) ||
      (FlashCycleType == FlashCycleErase)) {
    EnableBiosWriteProtect ();
    MmioAndThenOr8 (
      SpiBaseAddress + R_SPI_BCR,
      (UINT8) ~B_SPI_BCR_SRC,
      BiosCtlSave
      );
  }
  ///
  /// Restore SMIs.
  ///
  IoWrite32 ((UINTN) (ABase + R_SMI_EN), SmiEnSave);

  ReleaseSpiBar0 ();

  return Status;
}

/**
  Wait execution cycle to complete on the SPI interface.

  @param[in] ScSpiBar0            Spi MMIO base address
  @param[in] ErrorCheck           TRUE if the SpiCycle needs to do the error check

  @retval TRUE                    SPI cycle completed on the interface.
  @retval FALSE                   Time out while waiting the SPI cycle to complete.
                                  It's not safe to program the next command on the SPI interface.
**/
BOOLEAN
WaitForSpiCycleComplete (
  IN     UINT32             ScSpiBar0,
  IN     BOOLEAN            ErrorCheck
  )
{
  UINT64        WaitTicks;
  UINT64        WaitCount;
  UINT32        Data32;
  SPI_INSTANCE  *SpiInstance;

  SpiInstance = GetSpiInstance();
  ///
  /// Convert the wait period allowed into to tick count
  ///
  WaitCount = WAIT_TIME / WAIT_PERIOD;
  ///
  /// Wait for the SPI cycle to complete.
  ///
  for (WaitTicks = 0; WaitTicks < WaitCount; WaitTicks++) {
    Data32 = MmioRead32 (ScSpiBar0 + R_SPI_HSFS);
    if ((Data32 & B_SPI_HSFS_SCIP) == 0) {
      MmioWrite32 (ScSpiBar0 + R_SPI_HSFS, B_SPI_HSFS_FCERR | B_SPI_HSFS_FDONE);
      if ( ((Data32 & B_SPI_HSFS_FCERR) != 0) && (ErrorCheck == TRUE)) {
        return FALSE;
      } else {
        return TRUE;
      }
    }
    PchPmTimerStallRuntimeSafe (SpiInstance->PchAcpiBase, WAIT_PERIOD);
  }
  return FALSE;
}


/**
  Acquire SC spi mmio address.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval ScSpiBar0               Return SPI MMIO address
**/
UINT32
AcquireSpiBar0 (
  VOID
  )
{
  SPI_INSTANCE  *SpiInstance;

  SpiInstance = GetSpiInstance();
  return MmioRead32 (SpiInstance->PchSpiBase + R_SPI_BASE) & ~(B_SPI_BAR0_MASK);
}

/**
  Release SC spi mmio address. Do nothing.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval None
**/
VOID
ReleaseSpiBar0 (
  VOID
  )
{
}

/**
  This function is a hook for Spi to disable BIOS Write Protect

  @retval EFI_SUCCESS             The protocol instance was properly initialized
  @retval EFI_ACCESS_DENIED       The BIOS Region can only be updated in SMM phase

**/
EFI_STATUS
EFIAPI
DisableBiosWriteProtect (
  VOID
  )
{
  UINTN             SpiBaseAddress;

  SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

  if ((MmioRead8 (SpiBaseAddress + R_SPI_BCR) & B_SPI_BCR_SMM_BWP) != 0) {
    return EFI_ACCESS_DENIED;
  }
  //
  // Enable the access to the BIOS space for both read and write cycles
  //
  MmioOr8 (
    SpiBaseAddress + R_SPI_BCR,
    B_SPI_BCR_BIOSWE
    );

  return EFI_SUCCESS;
}

/**
  This function is a hook for Spi to enable BIOS Write Protect

  @retval
**/
VOID
EFIAPI
EnableBiosWriteProtect (
  VOID
  )
{
  UINTN                           SpiBaseAddress;

  SpiBaseAddress = GetDeviceAddr (OsBootDeviceSpi, 0);
  SpiBaseAddress = TO_MM_PCI_ADDRESS (SpiBaseAddress);

  //
  // Disable the access to the BIOS space for write cycles
  //
  MmioAnd8 (
    SpiBaseAddress + R_SPI_BCR,
    (UINT8) (~B_SPI_BCR_BIOSWE)
    );
}

/**
  Get the SPI region base and size, based on the enum type

  @param[in] FlashRegionType      The Flash Region type for for the base address which is listed in the Descriptor.
  @param[out] BaseAddress         The Flash Linear Address for the Region 'n' Base
  @param[out] RegionSize          The size for the Region 'n'

  @retval EFI_SUCCESS             Read success
  @retval EFI_INVALID_PARAMETER   Invalid region type given
  @retval EFI_DEVICE_ERROR        The region is not used
**/
EFI_STATUS
EFIAPI
SpiGetRegionAddress (
  IN     FLASH_REGION_TYPE  FlashRegionType,
  OUT    UINT32             *BaseAddress, OPTIONAL
  OUT    UINT32             *RegionSize OPTIONAL
  )
{
  UINT32          ScSpiBar0;
  UINT32          RegOffset;
  UINT32          Base;

  switch (FlashRegionType) {
  case FlashRegionBios:
    RegOffset = R_SPI_FREG1_BIOS;
    break;
  case FlashRegionPlatformData:
    RegOffset = R_SPI_FREG4_PLATFORM_DATA;
    break;
  default:
    return EFI_UNSUPPORTED;
  }

  ScSpiBar0 = AcquireSpiBar0 ();
  Base = (MmioRead32 (ScSpiBar0 + RegOffset) & B_SPI_FREG1_BASE_MASK) << N_SPI_FREG1_BASE;
  if (BaseAddress != NULL) {
    *BaseAddress = Base;
  }
  if (RegionSize != NULL) {
    *RegionSize =  ((((MmioRead32 (ScSpiBar0 + RegOffset) & B_SPI_FREG1_LIMIT_MASK) >> N_SPI_FREGX_LIMIT)
                    + 1) << N_SPI_FREGX_LIMIT_REPR) - Base;
  }
  ReleaseSpiBar0 ();

  return EFI_SUCCESS;
}
