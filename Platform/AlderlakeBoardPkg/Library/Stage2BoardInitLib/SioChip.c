/** @file

  Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#include "SioChip.h"
#include <Library/PciSegmentLib.h>

LOCAL_IO_WRITE8    mIoWrite8         = IoWrite8;

//
// COM 1 UART Controller
//
ACPI_SIO_RESOURCES_IO_IRQ      mCom1Resources = {
  {
    {ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR},
    COM1_ADDRESS,
    8
  },
  {
    {ACPI_IRQ_NOFLAG_DESCRIPTOR},
    COM1_IRQ_BIT
  },
  {
    ACPI_END_TAG_DESCRIPTOR,
    0
  }
};

ACPI_SIO_RESOURCES_IO_IRQ      mCom2Resources = {
  {
    {ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR},
    COM2_ADDRESS,
    8
  },
  {
    {ACPI_IRQ_NOFLAG_DESCRIPTOR},
    COM2_IRQ_BIT
  },
  {
    ACPI_END_TAG_DESCRIPTOR,
    0
  }
};

ACPI_SIO_RESOURCES_IO            mHwMonResources = {
  {
    {ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR},
    HWMON_BASE_ADDRESS,
    8
  },
  {
    ACPI_END_TAG_DESCRIPTOR,
    0
  }
};
//
// Table of SIO Controllers. This table assumes COM1 (UART_A) will always
// be enabled. Compile error will happen if SIO_SUPPORT_UART_A is undefined.
//
DEVICE_INFO    mDeviceInfo[] = {
  {
    {
      EISA_PNP_ID(0x501),
      11
    },
    SIO_COM1,
    RESOURCE_IO | RESOURCE_IRQ,
    {(ACPI_SMALL_RESOURCE_HEADER *) &mCom1Resources},
    {(ACPI_SMALL_RESOURCE_HEADER *) &mCom1Resources}
  },  // COM 1 UART Controller
  {
    {
      EISA_PNP_ID(0x501),
      12
    },
    SIO_COM2,
    RESOURCE_IO | RESOURCE_IRQ,
    {(ACPI_SMALL_RESOURCE_HEADER *) &mCom2Resources},
    {(ACPI_SMALL_RESOURCE_HEADER *) &mCom2Resources}
  },  // COM 2 UART Controller
  {
    {
      EISA_PNP_ID(0xc08),
      0
    },
    0x4,
    RESOURCE_IO,
    {(ACPI_SMALL_RESOURCE_HEADER *) &mHwMonResources},
    {(ACPI_SMALL_RESOURCE_HEADER *) &mHwMonResources},
  }   // Environment Controller
};

//
// Using HWMON_REGISTER_PAIR for convinience, as it's already defined as
// a register/value pair. Just defining some register/value pairs that need
// to be programmed.
// This table accomplishes the following tasks: Programing multipurpose pins
// mostly to GPIO, except for those required by UART A; disabling all GPIO,
// floppy controller, LPT, and UART B to reduce power consumption; enabling
// PS2 controller. Though the PS2 controller is not physically connected, it
// is needed for legacy USB keyboard (DOS and when installing WINXP).
// Because there is no register 0xaa (0xaa is used to exit SIO configuration
// space), register 0xaa is used as a marker for end of table.
//
HWMON_REGISTER_PAIR     mGenericRegisters[] = {
  {0x07, 0x00, 0x01}, // COM1 RS232
  {0xF0, 0xFE, 0x01}, // Change to Shared IRQ
  {0x07, 0x00, 0x07}, // SIO GPIO
  {0x26, 0xCC, 0x00}, // Enable COM2 Pin
  {0x29, 0xFB, 0x04}, // Enable GP52 as WDT_RST
  {0x2C, 0xFE, 0x00}, // Disable monitor internal AVCC3
  //{0xF1, 0xBB, 0x44}, // Enable SMI due to WDT
  {0xF4, 0x00, 0x2A}, // Set WDT_RST pin mapping
  {0x72, 0x5F, 0x80}, // Set WDT Configure
  {0x71, 0xFE, 0x00}, // Disable WDT as default
  {0x07, 0x00, 0x02}, // COM2 RS485
  {0x61, 0x00, 0xF8},
  {0x60, 0x00, 0x02},
  {0x70, 0x00, 0X03},
  {0xF0, 0x7E, 0x81}, // Change to Shared IRQ and enable RS485
  {0x07, 0x00, 0x05}, // Disable KBC
  {0x30, 0x00, 0x00},
  {0x07, 0x00, 0X04},
  {0x70, 0x00, 0X00}, // Disable interrupt level select for Env Controller
  {0xaa, 0x00, 0xff}  // end of table.
};

HWMON_REGISTER_PAIR     mHwMointorReg[] = {
  {0x0A, 0x93, 0x64}, // Enable PECI
  {0x0B, 0x3F, 0x4F}, // Set PWM Smoothing Step Frequency
  {0x13, 0xCF, 0x10}, // Enable FAN_TAC1
  {0x14, 0x7F, 0x80}, // Set FAN to high active
  {0x30, 0x00, 0x7E}, // Set VIN0 High Limit
  {0x31, 0x00, 0xDE}, // Set VIN0 Low Limit
  {0x32, 0x00, 0x65},
  {0x33, 0x00, 0xEA},
  {0x34, 0x00, 0x68}, // Set VIN2 High Limit
  {0x35, 0x00, 0x96}, // Set VIN2 Low Limit
  {0x36, 0x00, 0x57}, // Set VIN3 High Limit
  {0x37, 0x00, 0xF7}, // Set VIN3 Low Limit
  {0x38, 0x00, 0xE1},
  {0x39, 0x00, 0xF2},
  {0x3A, 0x00, 0xE7},
  {0x3B, 0x00, 0xC2},
  {0x3C, 0x00, 0x1A},
  {0x3D, 0x00, 0xF3},
  {0x3E, 0x00, 0xAD}, // Set 3VSB High Limit
  {0x3F, 0x00, 0xA8}, // Set 3VSB Low Limit
  {0x40, 0x00, 0x5A}, // Set TMPIN1 High Limit
  {0x41, 0x00, 0x0A}, // Set TMPIN1 Low Limit
  {0x42, 0x00, 0xF3}, // Set TMPIN2 High Limit
  {0x43, 0x00, 0x51}, // Set TMPIN2 Low Limit
  {0x44, 0x00, 0xFF}, // Set TMPIN3 High Limit
  {0x45, 0x00, 0x91}, // Set TMPIN3 Low Limit
  {0x50, 0x73, 0x8C}, // Enable ADC Voltage Channel
  {0x51, 0x3F, 0x40}, // PCEI Host TMPIN mapping
  {0x5C, 0x7F, 0x80}, // Eanble Thermal Diode adjust regier write
  {0x56, 0x00, 0x64}, // Set Thermal Diode Zero Degree Adjust
  {0x5C, 0x7F, 0x00}, // Disable Thermal Diode adjust regier write
  {0x60, 0x00, 0x10}, // Set FAN1 OFF Temperature
  {0x61, 0x00, 0x22}, // Set FAN1 Fan Start Temperature
  {0x62, 0x00, 0x3D}, // Set FAN1 Full Speed Temperature
  {0x63, 0x00, 0xFF}, // Set FAN1 PWM start %
  {0x64, 0x80, 0x18}, // Set FAN1 PWM Slope
  {0x89, 0x00, 0x30}, // Set Host Target Address
  {0x8A, 0x00, 0x01}, // PECI Wrtie Length
  {0x8B, 0x00, 0x02}, // PECI Read Length
  {0x8C, 0x00, 0x01}, // PECI Host Command
  {0x8D, 0x00, 0x00}, // PECI Write Data
  {0x8E, 0x00, 0xE0}, // PECI Control
  {0x15, 0x00, 0x00}, // Enable FAN Automatic Operation
  {0X00, 0xFE, 0x01}, // Start Hardware Monitor
  {0xFF, 0x00, 0xff}  // end of table.
};

/* Here starts Hardware Monitor Specific Code */

VOID
SetHwMonRegister (
  IN  UINT8 Register,
  IN  UINT8 Mask,
  IN  UINT8 Value
  )
{
  UINT8 OriValue;
  if(Value != 0x00){
    It8659HwMonReadRegister(Register, &OriValue);
    DEBUG ((DEBUG_INFO, "[HW] Ori value: %x Mask: %x\n",OriValue,Mask));
    Value |= Mask & OriValue;
  }
  mIoWrite8 (HWMON_INDEX_ADDRESS, Register);
  mIoWrite8 (HWMON_DATA_ADDRESS, Value);
  DEBUG ((DEBUG_INFO, "[HW] Write value %x to offset %x\n",Value,Register));
}

/**
  Find the DEVICE_INFO for specified Device.

  @param[in]  Device        Pointer to the EFI_SIO_ACPI_DEVICE_ID.

  @retval     DEVICE_INFO*  Pointer to the DEVICE_INFO.
**/
DEVICE_INFO *
DeviceSearch (
  IN EFI_SIO_ACPI_DEVICE_ID *Device
  )
{
  UINTN       Index;

  for (Index = 0; Index < sizeof (mDeviceInfo) / sizeof (mDeviceInfo[0]); Index++) {
    if (CompareMem (Device, &mDeviceInfo[Index].Device, sizeof (*Device)) == 0) {
      return &mDeviceInfo[Index];
    }
  }

  ASSERT (FALSE);
  return NULL;
}

/**
  Perform a 8-bit I/O write to SIO register.

  @param[in]  Index  The register index.
  @param[in]  Data   The value to write to register.
**/
VOID
WriteRegister (
  IN  UINT8            Index,
  IN  UINT8            Data
  )
{
  mIoWrite8 (SIO_INDEX_PORT, Index);
  mIoWrite8 (SIO_DATA_PORT, Data);
}


/**
  Perform a 8-bit I/O read from SIO register.

  @param[in]  Index  The register index.

  @retval     Value  The value written to the register.
**/
UINT8
ReadRegister (
  IN  UINT8            Index
  )
{
  IoWrite8 (SIO_INDEX_PORT, Index);
  return IoRead8 (SIO_DATA_PORT);
}


/**
  Program the SIO chip to enter the configure mode.
**/
VOID
EnterConfigMode (
  VOID
  )
{
  mIoWrite8 (SIO_CONFIG_PORT, 0x87);
  mIoWrite8 (SIO_CONFIG_PORT, 0x01);
  mIoWrite8 (SIO_CONFIG_PORT, 0x55);
  mIoWrite8 (SIO_CONFIG_PORT, 0xAA);

  //Test Connection
  IoWrite8 (SIO_INDEX_PORT, 0x20);
  DEBUG ((DEBUG_INFO, "Test SIO Connection\n"));
  DEBUG ((DEBUG_INFO, "EnterConfigMode - chip1 %x\n", IoRead8 (SIO_DATA_PORT)));
  IoWrite8 (SIO_INDEX_PORT, 0x21);
  DEBUG ((DEBUG_INFO, "EnterConfigMode - chip2 %x\n", IoRead8 (SIO_DATA_PORT)));
}


/**
  Program the SIO chip to exit the configure mode.
**/
VOID
ExitConfigMode (
  VOID
  )
{
  mIoWrite8 (SIO_INDEX_PORT, 0x02);
  mIoWrite8 (SIO_DATA_PORT, 0x02);
}


/**
  Program the SIO chip to select the specified device.

  @param[in]  Device       Pointer to EFI_SIO_ACPI_DEVICE_ID.

  @retval     DEVICE_INFO  Pointer to DEVICE_INFO.
**/
DEVICE_INFO *
DeviceSelect (
  IN EFI_SIO_ACPI_DEVICE_ID *Device
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);
  WriteRegister (REG_SIO_LOGICAL_DEVICE, DeviceInfo->DeviceId);
  return DeviceInfo;
}


/**
  Program the SIO chip to set the IO base address of the specified device.

  @param[in]  Device       Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[in]  BaseAddress  The base address to set.
**/
VOID
DeviceSetIo (
  IN EFI_SIO_ACPI_DEVICE_ID *Device,
  IN UINT16                 BaseAddress
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);

  if ((DeviceInfo->ResourceMask & RESOURCE_IO) == RESOURCE_IO) {
    WriteRegister (R_SIO_BASE_ADDRESS_HIGH, (UINT8) (BaseAddress >> 8));
    WriteRegister (R_SIO_BASE_ADDRESS_LOW,  (UINT8) BaseAddress);
  }
}


/**
  Program the SIO chip to set the IRQ of the specified device.

  @param[in]  Device      Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[in]  Irq         The IRQ to set.
**/
VOID
DeviceSetIrq (
  IN EFI_SIO_ACPI_DEVICE_ID *Device,
  IN UINT8                  Irq
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);

  if ((DeviceInfo->ResourceMask & RESOURCE_IRQ) == RESOURCE_IRQ) {
    WriteRegister (R_SIO_PRIMARY_INTERRUPT_SELECT, Irq);
  }
}


/**
  Program the SIO chip to set the DMA of the specified device.

  @param[in]  Device      Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[in]  Dma         The DMA to set.
**/
VOID
DeviceSetDma (
  IN EFI_SIO_ACPI_DEVICE_ID *Device,
  IN UINT8                  Dma
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);

  if ((DeviceInfo->ResourceMask & RESOURCE_DMA) == RESOURCE_DMA) {
    WriteRegister (R_SIO_DMA_CHANNEL_SELECT, Dma);
  }
}


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
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);

  *Resources = DeviceInfo->Resources;

  return EFI_SUCCESS;
}

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
  )
{
  DEVICE_INFO                                 *DeviceInfo;
  ACPI_RESOURCE_HEADER_PTR                    ResourcePtr;
  ACPI_RESOURCE_HEADER_PTR                    ResourcePtr2;
  EFI_ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR  *FixedIo;
  EFI_ACPI_IRQ_DESCRIPTOR                     *Irq;
  EFI_ACPI_DMA_DESCRIPTOR                     *Dma;

  ResourcePtr = Resources;
  DeviceInfo  = DeviceSearch (Device);

  EnterConfigMode ();
  DeviceSelect (Device);
  while (ResourcePtr.SmallHeader->Byte != ACPI_END_TAG_DESCRIPTOR) {

    switch (ResourcePtr.SmallHeader->Byte) {
      case ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR:
        FixedIo = (EFI_ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR *) ResourcePtr.SmallHeader;
        DeviceSetIo (Device, FixedIo->BaseAddress);
        break;

      case ACPI_IRQ_NOFLAG_DESCRIPTOR:
      case ACPI_IRQ_DESCRIPTOR:
        Irq = (EFI_ACPI_IRQ_DESCRIPTOR *) ResourcePtr.SmallHeader;
        DeviceSetIrq (Device, (UINT8) LowBitSet32 ((UINT32) Irq->Mask));
        break;

      case ACPI_DMA_DESCRIPTOR:
        Dma = (EFI_ACPI_DMA_DESCRIPTOR *) ResourcePtr.SmallHeader;
        DeviceSetDma (Device, (UINT8) LowBitSet32 ((UINT32) Dma->ChannelMask));
        break;
    }

    //
    // Update the current resource
    //
    ResourcePtr2 = DeviceInfo->Resources;
    while (ResourcePtr2.SmallHeader->Byte != ACPI_END_TAG_DESCRIPTOR) {
      if (ResourcePtr2.SmallHeader->Byte == ResourcePtr.SmallHeader->Byte) {
        if (ResourcePtr2.SmallHeader->Bits.Type == 0) {
          CopyMem (
            ResourcePtr2.SmallHeader,
            ResourcePtr.SmallHeader,
            ResourcePtr.SmallHeader->Bits.Length + sizeof (*ResourcePtr.SmallHeader)
            );
        } else {
          CopyMem (
            ResourcePtr2.LargeHeader,
            ResourcePtr.LargeHeader,
            ResourcePtr.LargeHeader->Length + sizeof (*ResourcePtr.LargeHeader)
            );
        }
        break;
      }

      if (ResourcePtr2.SmallHeader->Bits.Type == 0) {
        ResourcePtr2.SmallHeader = (ACPI_SMALL_RESOURCE_HEADER *) ((UINT8 *) ResourcePtr2.SmallHeader
                                + ResourcePtr2.SmallHeader->Bits.Length
                                + sizeof (*ResourcePtr2.SmallHeader));
      } else {
        ResourcePtr2.LargeHeader = (ACPI_LARGE_RESOURCE_HEADER *) ((UINT8 *) ResourcePtr2.LargeHeader
                                + ResourcePtr2.LargeHeader->Length
                                + sizeof (*ResourcePtr2.LargeHeader));
      }

    }
    //
    // Should find a match
    //
    ASSERT (ResourcePtr2.SmallHeader->Byte != ACPI_END_TAG_DESCRIPTOR);


    if (ResourcePtr.SmallHeader->Bits.Type == 0) {
      ResourcePtr.SmallHeader = (ACPI_SMALL_RESOURCE_HEADER *) ((UINT8 *) ResourcePtr.SmallHeader
                              + ResourcePtr.SmallHeader->Bits.Length
                              + sizeof (*ResourcePtr.SmallHeader));
    } else {
      ResourcePtr.LargeHeader = (ACPI_LARGE_RESOURCE_HEADER *) ((UINT8 *) ResourcePtr.LargeHeader
                              + ResourcePtr.LargeHeader->Length
                              + sizeof (*ResourcePtr.LargeHeader));
    }
  }

  ExitConfigMode ();

  return EFI_SUCCESS;
}

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
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);

  *Resources = DeviceInfo->PossibleResources;

  return EFI_SUCCESS;
}


/**
  Program the SIO chip to enable the specified device using the default resource.

  @param[in]  Device          Pointer to EFI_SIO_ACPI_DEVICE_ID.
**/
VOID
DeviceEnable (
  IN EFI_SIO_ACPI_DEVICE_ID   *Device
  )
{

  DEBUG ((DEBUG_INFO, "SIO device enable - Device : %x\n", Device));
  ACPI_RESOURCE_HEADER_PTR    Resources;

  DeviceGetResources (Device, &Resources);
  DeviceSetResources (Device, Resources);

  EnterConfigMode ();
  DeviceSelect (Device);
  WriteRegister (R_SIO_ACTIVATE, 1);
  ExitConfigMode ();
}


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
  )
{
  EFI_SIO_ACPI_DEVICE_ID     *LocalDevices;
  UINTN                      LocalCount;
  UINTN                      Index;

  //
  // Allocate enough memory for simplicity
  //
  LocalDevices = AllocatePool (sizeof (EFI_SIO_ACPI_DEVICE_ID) * 3);
  ASSERT (LocalDevices != NULL);
  if (LocalDevices != NULL){
    LocalCount   = 0;

    for (Index = 0; Index < sizeof (mDeviceInfo) / sizeof (mDeviceInfo[0]); Index++) {
      CopyMem (&LocalDevices[LocalCount], &mDeviceInfo[Index].Device, sizeof (EFI_SIO_ACPI_DEVICE_ID));
      LocalCount ++;
    }

    *Devices = LocalDevices;
    *Count   = LocalCount;
  }
}

/**
  Initialize the SIO chip for S3.
**/
VOID
SioInit (
  VOID
  )
{
  EFI_SIO_ACPI_DEVICE_ID  *Devices;
  HWMON_REGISTER_PAIR     *RegisterPtr;
  UINTN                   Index;
  UINTN                   Count;
  UINT8                   RegOrgValue;
  UINT64                  LpcBaseAddr;
  UINT16                  LpcIoDecodeRanges;

  //
  // LPC device is connected to PCH LPC interface on the
  // platform. Most PCH initialization work is done in Platform Code
  // such as Platform PEIM and Platform DXE driver.
  // Please make sure the initialization works are ready
  // This driver (It8659F) assumes that all PCH initialization has
  // been done before this driver is started to manage the device.
  //

  //
  // Program and Enable Default Super IO Configuration Port Addresses and range
  //

  DEBUG((DEBUG_INFO, "SioInit - Entry\n"));
  // Enable LPC IO decode for LPC/eSPI Bridge communication
  // Enable 4e/4f Decode over eSPI CS1#.
  DEBUG((DEBUG_INFO, "SioInit - Enable 4E 4F\n"));
  LpcBaseAddr = LpcPciCfgBase();
  LpcIoDecodeRanges = PciSegmentRead16(LpcBaseAddr + R_LPC_CFG_IOE);
  LpcIoDecodeRanges |= (B_LPC_CFG_IOE_ME2 | B_LPC_CFG_IOE_CAE | B_LPC_CFG_IOE_CBE);
  DEBUG((DEBUG_INFO, "SioInit - LpcIoDecodeRanges: %x\n", LpcIoDecodeRanges));
  PciSegmentWrite16((UINTN)(LpcBaseAddr + R_LPC_CFG_IOE), (UINT16)LpcIoDecodeRanges);

  //Enter Config Mode
  IoWrite8(SIO_CONFIG_PORT, 0x87);
  IoWrite8(SIO_CONFIG_PORT, 0x01);
  IoWrite8(SIO_CONFIG_PORT, 0x55);
  IoWrite8(SIO_CONFIG_PORT, 0xAA);
  IoWrite8(SIO_CONFIG_PORT, 0x20);

  if ((IoRead8(SIO_DATA_PORT) & 0xFF) == 0x86)
  {
      IoWrite8(SIO_CONFIG_PORT, 0x21);
      if ((IoRead8(SIO_DATA_PORT) & 0xFF) != 0x59)
      {
          DEBUG((DEBUG_INFO, "SioInit - Return CHIPID2:Not ITE8659\n"));
          return;
      }
  }
  else
  {
      DEBUG((DEBUG_INFO, "SioInit - Return CHIPID1:Not ITE8659\n"));
      return;
  }

  //Exit Config Mode
  IoWrite8(SIO_CONFIG_PORT, 0x02);
  IoWrite8(SIO_DATA_PORT, 0x02);

  //-------------------------


  DEBUG ((DEBUG_INFO, "SioInit Entry\n"));
  //
  // Assume it is It8659 and try to open configuration space (by writing
  // 0x87 twice to the index port) and read the device ID. If it's not the
  // correct device ID, try to close the configuration space by writing 0xaa
  // to the index port (several SIO use the same sequence to open and close
  // configuration space, but some older SIO have the configuration space
  // always open) and return EFI_DEVICE_ERROR.
  //
  IoWrite8 (SIO_CONFIG_PORT, 0x87);
  IoWrite8 (SIO_CONFIG_PORT, 0x01);
  IoWrite8 (SIO_CONFIG_PORT, 0x55);
  IoWrite8 (SIO_CONFIG_PORT, 0xAA);
  IoWrite8 (SIO_INDEX_PORT, REG_CHIP_ID1);
  DEBUG ((DEBUG_INFO, "SioDriverEntryPoint - chip1 %x\n", IoRead8 (SIO_DATA_PORT)));

  if ((IoRead8 (SIO_DATA_PORT) & 0xFF) == 0x86) {
    IoWrite8 (SIO_INDEX_PORT, REG_CHIP_ID2);
    DEBUG ((DEBUG_INFO, "SioDriverEntryPoint - chip2 %x\n", IoRead8 (SIO_DATA_PORT)));
    if ((IoRead8 (SIO_DATA_PORT) & 0xFF) != 0x59) {
      IoWrite8 (SIO_CONFIG_PORT, 0x2);
      IoWrite8 (SIO_DATA_PORT, 0x2);
      DEBUG((DEBUG_INFO, "SioDriverEntryPoint Error !\n"));
    }
  } else {
      IoWrite8 (SIO_CONFIG_PORT, 0x2);
      IoWrite8 (SIO_DATA_PORT, 0x2);
      DEBUG((DEBUG_INFO, "SioDriverEntryPoint Error !\n"));
  }

  //
  // The procedure DeviceGetList has ben modified to only return the UART A.
  // However, UART A is actually initialized at PEI, so here UART A is only
  // saved to the SIO table, no actual IO is required.
  //
  DeviceGetList (&Devices, &Count);

  for (Index = 0; Index < Count; Index++) {
    DeviceEnable (&Devices[Index]);
  }

  FreePool (Devices);

  It8659HwMonStart();

  // Generic programming: SIO global registers, disable unused devices, force
  // all unused possible GPIO pins to input GPIO. This will both access the
  // IO and save it to the S3 table. This initialization was not done at PEI.
  //
  RegisterPtr = mHwMointorReg;

  while (RegisterPtr->Register != 0xFF) {
    SetHwMonRegister (RegisterPtr->Register, RegisterPtr->Mask, RegisterPtr->Value);
    RegisterPtr++;
  }


  RegisterPtr = mGenericRegisters;

  EnterConfigMode();
  while (RegisterPtr->Register != 0xaa) {
    if (RegisterPtr->Mask != 0x00){
        RegOrgValue = ReadRegister (RegisterPtr->Register);
        WriteRegister (RegisterPtr->Register,((RegOrgValue&RegisterPtr->Mask)|RegisterPtr->Value));
        RegisterPtr++;
    }
    else{
        WriteRegister (RegisterPtr->Register, RegisterPtr->Value);
        RegisterPtr++;
    }
  }
  ExitConfigMode();
  //
  // Return to simple IO access.
  //
  mIoWrite8        = IoWrite8;
}

EFI_STATUS
It8659HwMonStart(
  VOID
  )
{
  EFI_SIO_ACPI_DEVICE_ID  HwMonDevice;

  DEBUG ((DEBUG_INFO, "SIO It8659fHwMonStart\n"));

  //mIoWrite8 = S3IoWrite8;
  HwMonDevice.HID = EISA_PNP_ID(0xc08);
  HwMonDevice.UID = 0;
  DeviceEnable(&HwMonDevice);
  mIoWrite8 = IoWrite8;
  return EFI_SUCCESS;
}

EFI_STATUS
It8659HwMonReadRegister (
  IN  UINT8     Register,
  OUT UINT8    *Value
  )
{
  IoWrite8 (HWMON_INDEX_ADDRESS, Register);
  *Value = IoRead8 (HWMON_DATA_ADDRESS);
  return EFI_SUCCESS;
}
