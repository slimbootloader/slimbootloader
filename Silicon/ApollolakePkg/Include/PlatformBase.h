/** @file

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PLATFORM_BASE_ADDRESSES_H
#define _PLATFORM_BASE_ADDRESSES_H

//
//  Base Addresses should be listed in this Section
//  Sizes are listed down below in next Section
//

// ACPI Base Address: PcdScAcpiIoPortBaseAddress

#define GMADR_BASE_ADDRESS                0xA0000000
#define GTTMM_BASE_ADDRESS                0xBF000000

#define XHCI_TEMP_MEM_BASE_ADDRESS        0xC2C00000
#define SW_BASE_ADDRESS                   0xC3800000
#define MTB_BASE_ADDRESS                  0xC3600000
#define HECI_BASE_ADDRESS                 0xC3708000
#define HECI2_BASE_ADDRESS                0xC3709000
#define RTIT_BASE_ADDRESS                 0xC3700000
#define FW_TEMP_BASE_ADDRESS              0xC3400000

#ifndef ISPMM_BASE_ADDRESS
#define ISPMM_BASE_ADDRESS                0xCF000000
#endif

#define P2SB_MMIO_BASE_ADDRESS            0xD0000000
#define SPI_TEMP_MEM_BASE_ADDRESS         0xD2000000
#define PWM_TEMP_MEM_BASE_ADDRESS         0xD3000000
#define PCIEX_BASE_ADDRESS                0xE0000000

#define FW_BASE_ADDRESS                   0xFE240000
#define LPSS_I2C0_TMP_BAR0                0xFE910000
#define MPHY_BASE_ADDRESS                 0xFEA00000
#define CRAB_ABORT                        0xFEB00000
#define LPSS_UART_TEMP_BASE_ADDRESS       0xFE800000

#define IO_APIC_BASE_ADDRESS              0xFEC00000
#define IO_APIC_INDEX_REGISTER            IO_APIC_BASE_ADDRESS
#define IO_APIC_DATA_REGISTER             (IO_APIC_BASE_ADDRESS+0x10)
#define IO_APIC_EOI                       (IO_APIC_BASE_ADDRESS+0x40)

#define HPET_BASE_ADDRESS                 0xFED00000
#define EFI_ACPI_EVENT_TIMER_BLOCK_ID     0x8086A201

#define SPI_BASE_ADDRESS                  0xFED01000
#define PBASE                             0xFED03000
#define PUNIT_BASE_ADDRESS                0xFED06000
#define ILB_BASE_ADDRESS                  0xFED08000
#define IOBASE                            0xFED0C000
#define MCH_BASE_ADDRESS                  0xFED10000

#define DMI_BASE_ADDRESS                  0xFED18000  // Direct Media Interface
#define RCBA_BASE_ADDRESS                 0xFED1C000  // Root Complex Base Address
#define TPM_BASE_ADDRESS                  0xFED40000  // Trusted Platform Module
#define XHCI_DBC                          0xFFD60000  // Reserved For P2P Traffic Targeting the xHCI DBC Endpoint
#define DEF_VTD_BASE_ADDRESS              0xFED65000  // VT-d Engine

#define PTT_HCI_BASE_ADDRESS              0xFED740000  // Platform Trust Technology HCI
#define IO_BASE_ADDRESS                   0xFED80000  // IO Memory
#define LOCAL_APIC_BASE_ADDRESS           0xFEE00000  // Local APIC

//
//  Sizes for Base Addresses should be put in this Section
//
#define GMADR_SIZE                        0x20000000   //512M

#define SPI_TEMP_MEM_SIZE                 0x1000       //4K
#define SW_SIZE                           0x800000     //8M
#define MTB_SIZE                          0x100000     //1M
#define HECI_SIZE                         0x1000       //4K
#define HECI2_SIZE                        0x1000       //4k
#define RTIT_SIZE                         0x200        //512B
#define FW_SIZE                           0x200000     //1M

#define ACPI_MMIO_SIZE                    0x10000000   //256M can be reduced to actual needs.
#define GTTMM_SIZE                        0x1000000    //16M
#define PCIEX_SIZE                        0x10000000   //256M
#define ISPMM_SIZE                        0x1000000    //16M

#define GCR_BASE_ADDRESS_SIZE             0x1000       //4K
#define PMC_BASE_ADDRESS_SIZE             0x1000       //4K
#define PMC_IPC1_BAR1_SIZE                0x2000       //8K
#define PMC_IPC1_BAR2_SIZE                0x1000       //4K
#define FW_SIZE                           0x200000     //1M
#define CRAB_ABORT_size                   0x100000     //1M

#define ACPI_BASE_ADDRESS                 0x400

#define IO_APIC_SIZE                      0x40         //64B
#define HPET_SIZE                         0x400        //1K

#define SPI_SIZE                          0x1000       //4K
#define MCH_SIZE                          0x8000       //32K
#define XHCI_DBC_SIZE                     0x1000       //4K
#define LOCAL_APIC_SIZE                   0x100000     //1M

#endif


