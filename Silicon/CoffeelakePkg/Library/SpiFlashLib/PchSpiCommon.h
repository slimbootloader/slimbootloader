/** @file

  Copyright (c) 2017 - 2019, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#ifndef _PCH_SPI_COMMON_LIB_H_
#define _PCH_SPI_COMMON_LIB_H_

//
// Maximum time allowed while waiting the SPI cycle to complete
//  Wait Time = 6 seconds = 6000000 microseconds
//  Wait Period = 10 microseconds
//
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
#define PCH_SPI_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('P', 'S', 'P', 'I')

typedef struct {
  UINT32                Signature;
  EFI_HANDLE            Handle;
  UINT16                PchAcpiBase;
  UINTN                 PchSpiBase;
  UINT8                 ReadPermission;
  UINT8                 WritePermission;
  UINT32                SfdpVscc0Value;
  UINT32                SfdpVscc1Value;
  UINT16                PchStrapBaseAddr;
  UINT16                PchStrapSize;
  UINT16                CpuStrapBaseAddr;
  UINT16                CpuStrapSize;
  UINT8                 NumberOfComponents;
  UINT32                Component1StartAddr;
  UINT32                TotalFlashSize;
} SPI_INSTANCE;

//
// Function prototypes used by the SPI protocol.
//

/**
  Initialize an SPI instance.

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
  Acquire pch spi mmio address.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

**/
UINTN
AcquireSpiBar0 (
  VOID
  );

/**
  Release pch spi mmio address.

  @param[in] SpiInstance          Pointer to SpiInstance to initialize

  @retval None
**/
VOID
ReleaseSpiBar0 (
  IN  SPI_INSTANCE                *SpiInstance
  );

#endif
