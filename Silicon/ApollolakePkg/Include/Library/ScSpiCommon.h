/** @file
  Header file for the SC SPI Common Driver.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _SC_SPI_COMMON_LIB_H_
#define _SC_SPI_COMMON_LIB_H_

///
/// Maximum time allowed while waiting the SPI cycle to complete
///  Wait Time = 6 seconds = 6000000 microseconds
///  Wait Period = 10 microseconds
///
#define WAIT_TIME   6000000     ///< Wait Time = 6 seconds = 6000000 microseconds
#define WAIT_PERIOD 10          ///< Wait Period = 10 microseconds




///
/// Flash cycle Type
///
typedef enum {
  FlashCycleRead,
  FlashCycleWrite,
  FlashCycleErase,
  FlashCycleReadSfdp,
  FlashCycleReadJedecId,
  FlashCycleWriteStatus,
  FlashCycleReadStatus,
  FlashCycleMax
} FLASH_CYCLE_TYPE;

///
/// Flash Component Number
///
typedef enum {
  FlashComponent0,
  FlashComponent1,
  FlashComponentMax
} FLASH_COMPONENT_NUM;

///
/// Private data structure definitions for the driver
///
#define SC_SPI_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('P', 'S', 'P', 'I')

typedef struct {
  UINTN                 Signature;
  EFI_HANDLE            Handle;
  UINT16                PchAcpiBase;
  UINTN                 PchSpiBase;
  UINT16                RegionPermission;
  UINT32                SfdpVscc0Value;
  UINT32                SfdpVscc1Value;
  UINT32                StrapBaseAddress;
  UINT8                 NumberOfComponents;
  UINT32                Component1StartAddr;
} SPI_INSTANCE;

///
/// Function prototypes used by the SPI protocol.
///

/**
  Initialize an SPI protocol instance.

  @retval EFI_SUCCESS             The protocol instance was properly initialized
  @retval EFI_UNSUPPORTED         The SC is not supported by this module
**/
EFI_STATUS
SpiConstructor (
  VOID
  );

/**
  This function is a hook for Spi to disable BIOS Write Protect

  @retval EFI_SUCCESS             The protocol instance was properly initialized
  @retval EFI_ACCESS_DENIED       The BIOS Region can only be updated in SMM phase
**/
EFI_STATUS
EFIAPI
DisableBiosWriteProtect (
  VOID
  );

/**
  This function is a hook for Spi to enable BIOS Write Protect
**/
VOID
EFIAPI
EnableBiosWriteProtect (
  VOID
  );

/**
  Acquire SC spi mmio address.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval UINT32                  return SPI MMIO address
**/
UINT32
AcquireSpiBar0 (
  VOID
  );

/**
  Release SC spi mmio address.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize
**/
VOID
ReleaseSpiBar0 (
  VOID
  );

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
  );

/**
  Wait execution cycle to complete on the SPI interface.

  @param[in] PchSpiBar0           Spi MMIO base address
  @param[in] ErrorCheck           TRUE if the SpiCycle needs to do the error check

  @retval TRUE                    SPI cycle completed on the interface.
  @retval FALSE                   Time out while waiting the SPI cycle to complete.
                                  It's not safe to program the next command on the SPI interface.
**/
BOOLEAN
WaitForSpiCycleComplete (
  IN     UINT32             PchSpiBar0,
  IN     BOOLEAN            ErrorCheck
  );

#endif
