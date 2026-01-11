/** @file

  Serial IO SPI ACPI definitions

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define SPI0_MODE         SM00
#define SPI1_MODE         SM01
#define SPI2_MODE         SM02
#define SPI3_MODE         SM03
#define SPI4_MODE         SM04
#define SPI5_MODE         SM05
#define SPI6_MODE         SM06

#define SPI0_PCIE_BASE    SC00
#define SPI1_PCIE_BASE    SC01
#define SPI2_PCIE_BASE    SC02
#define SPI3_PCIE_BASE    SC03
#define SPI4_PCIE_BASE    SC04
#define SPI5_PCIE_BASE    SC05
#define SPI6_PCIE_BASE    SC06


#define SPI_DEVICE_ADR       SERIAL_IO_SPI0_ADR
#define SPI_DEVICE_MODE      SPI0_MODE
#define SPI_DEVICE_PCI_BASE  SPI0_PCIE_BASE
Device (SPI0) {
Include ("SerialIoSpiController.asl")
}

#undef SPI_DEVICE_ADR
#undef SPI_DEVICE_MODE
#undef SPI_DEVICE_PCI_BASE
#define SPI_DEVICE_ADR       SERIAL_IO_SPI1_ADR
#define SPI_DEVICE_MODE      SPI1_MODE
#define SPI_DEVICE_PCI_BASE  SPI1_PCIE_BASE
Device (SPI1) {
Include ("SerialIoSpiController.asl")
}

#undef SPI_DEVICE_ADR
#undef SPI_DEVICE_MODE
#undef SPI_DEVICE_PCI_BASE
#define SPI_DEVICE_ADR       SERIAL_IO_SPI2_ADR
#define SPI_DEVICE_MODE      SPI2_MODE
#define SPI_DEVICE_PCI_BASE  SPI2_PCIE_BASE
Device (SPI2) {
Include ("SerialIoSpiController.asl")
}
