/** @file

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _SIO_H_
#define _SIO_H_

#include <IndustryStandard/Acpi10.h>
#include <IndustryStandard/Acpi20.h>
#include <IndustryStandard/Acpi30.h>
#include "Stage2BoardInitLib.h"
#include <Protocol/DevicePath.h>

// ALL
#define REG_CONFIG_CONTROL              0x02
#define REG_SIO_LOGICAL_DEVICE          0x07

#define REG_CHIP_ID1                    0x20
#define REG_CHIP_ID2                    0x21
#define REG_CHIP_VER                    0x22
#define REG_CLK_SEL                     0x23
//
// Define some fixed platform device location information
//
#define DESKTOP_LPC_SIO_CONFIG_DEFAULT_PORT 0x4E
#define DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT  0x4E
#define DESKTOP_LPC_SIO_DATA_DEFAULT_PORT   0x4F

//
// Port address for LPC
//
#define SIO_CONFIG_PORT    DESKTOP_LPC_SIO_CONFIG_DEFAULT_PORT
#define SIO_INDEX_PORT     DESKTOP_LPC_SIO_INDEX_DEFAULT_PORT
#define SIO_DATA_PORT      DESKTOP_LPC_SIO_DATA_DEFAULT_PORT

//
// logical device in LPC
//
#define SIO_COM1                        0x01
#define SIO_COM2                        0x02


// ALL
#define REG_SIO_LOGICAL_DEVICE          0x07


#define R_SIO_ACTIVATE                  0x30
#define R_SIO_BASE_ADDRESS_HIGH         0x60
#define R_SIO_BASE_ADDRESS_LOW          0x61
#define R_SIO_PRIMARY_INTERRUPT_SELECT  0x70
#define R_SIO_DMA_CHANNEL_SELECT        0x74


#define COM1_ADDRESS                    0x03F8
#define COM1_IRQ_BIT                    BIT4
#define COM2_ADDRESS                    0x02F8
#define COM2_IRQ_BIT                    BIT3
#define HWMON_BASE_ADDRESS              0x0290  // HWMON/EC uses +5 and +6.
#define HWMON_INDEX_ADDRESS             (HWMON_BASE_ADDRESS + 5)
#define HWMON_DATA_ADDRESS              (HWMON_BASE_ADDRESS + 6)

typedef struct {
  UINT8 Register;
  UINT8 Mask;
  UINT8 Value;
} HWMON_REGISTER_PAIR;

typedef
UINT8
(EFIAPI *LOCAL_IO_WRITE8) (
  IN      UINTN                     Port,
  IN      UINT8                     Value
  );

EFI_STATUS
It8659HwMonStart (
  VOID
  );

EFI_STATUS
It8659HwMonSetValues (
  IN  HWMON_REGISTER_PAIR  *RegisterPair,
  IN  UINT8                 RegisterCount
  );

EFI_STATUS
It8659HwMonReadRegister (
  IN  UINT8     Register,
  OUT UINT8    *Value
  );

#define RESOURCE_IO    BIT0
#define RESOURCE_IRQ   BIT1
#define RESOURCE_DMA   BIT2

#pragma pack(1)

typedef struct {
  EFI_ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR  Io;
  EFI_ACPI_IRQ_NOFLAG_DESCRIPTOR              Irq;
  EFI_ACPI_END_TAG_DESCRIPTOR                 End;
} ACPI_SIO_RESOURCES_IO_IRQ;

typedef struct {
  EFI_ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR  Io;
  EFI_ACPI_END_TAG_DESCRIPTOR                 End;
} ACPI_SIO_RESOURCES_IO;

#pragma pack()

typedef struct {
  UINT32                      HID;
  UINT32                      UID;
} EFI_SIO_ACPI_DEVICE_ID;

typedef union {
  ACPI_SMALL_RESOURCE_HEADER    *SmallHeader;
  ACPI_LARGE_RESOURCE_HEADER    *LargeHeader;
} ACPI_RESOURCE_HEADER_PTR;

typedef struct {
  EFI_SIO_ACPI_DEVICE_ID      Device;
  UINT8                       DeviceId;
  UINT8                       ResourceMask;
  ACPI_RESOURCE_HEADER_PTR    Resources;
  ACPI_RESOURCE_HEADER_PTR    PossibleResources;
} DEVICE_INFO;


/**
  Initialize the SIO chip and save for S3.
**/
VOID
SioInit (
  VOID
  );


/**
  Return the supported devices.

  @param[out] Devices         Pointer to pointer of EFI_SIO_ACPI_DEVICE_ID.
                              Caller is responsible to free the buffer.
  @param[out] Count           Pointer to UINTN holding the device count.
**/
VOID
DeviceGetList (
  OUT EFI_SIO_ACPI_DEVICE_ID **Devices,
  OUT UINTN                  *Count
  );


/**
  Program the SIO chip to enable the specified device using the default resource.

  @param[in] Device          Pointer to EFI_SIO_ACPI_DEVICE_ID.
**/
VOID
DeviceEnable (
  IN EFI_SIO_ACPI_DEVICE_ID   *Device
  );


/**
  Get the possible ACPI resources for specified device.

  @param[in]  Device          Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[out] Resources       Pointer to ACPI_RESOURCE_HEADER_PTR.

  @retval     EFI_SUCCESS     The resources are returned successfully.
**/
EFI_STATUS
DevicePossibleResources (
  IN  EFI_SIO_ACPI_DEVICE_ID   *Device,
  OUT ACPI_RESOURCE_HEADER_PTR *Resources
  );


/**
  Set the ACPI resources for specified device.

  The SIO chip is programmed to use the new resources and the
  resources setting are saved. The function assumes the resources
  are valid.

  @param[in] Device          Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[in] Resources       ACPI_RESOURCE_HEADER_PTR.

  @retval    EFI_SUCCESS     The resources are set successfully.
**/
EFI_STATUS
DeviceSetResources (
  IN EFI_SIO_ACPI_DEVICE_ID   *Device,
  IN ACPI_RESOURCE_HEADER_PTR Resources
  );


/**
  Get the ACPI resources for specified device.

  @param[in]  Device          Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[out] Resources       Pointer to ACPI_RESOURCE_HEADER_PTR.

  @retval     EFI_SUCCESS     The resources are returned successfully.
**/
EFI_STATUS
DeviceGetResources (
  IN  EFI_SIO_ACPI_DEVICE_ID   *Device,
  OUT ACPI_RESOURCE_HEADER_PTR *Resources
  );


/**
  Program the SIO chip to enter the configure mode.
**/
VOID
EnterConfigMode (
  VOID
  );


/**
  Program the SIO chip to exit the configure mode.
**/
VOID
ExitConfigMode (
  VOID
  );


/**
  Perform a 8-bit I/O write to SIO register.

  @param[in]  Index  The register index.
  @param[in]  Data   The value to write to register.
**/
VOID
WriteRegister (
  IN  UINT8            Index,
  IN  UINT8            Data
  );


/**
  Perform a 8-bit I/O read from SIO register.

  @param[in]  Index  The register index.

  @retval     Value  The value written to the register.
**/
UINT8
ReadRegister (
  IN  UINT8            Index
  );

#endif // _SIO_H_
