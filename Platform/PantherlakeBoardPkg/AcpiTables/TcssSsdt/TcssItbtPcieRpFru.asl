/** @file
  This file contains the TCSS PCIe Root Port configuration

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope (TRP0) {
  #define GPE0_EN GPE_TC_PCI_EXP
  #define GPE1_EN GPE1_TC_PCI0_PCI_EXP
  Include ("TcssPrwWrapper.asl")

  Device (PXSX)
  {
    Name (_ADR, 0x00000000)
    Include ("TcssPrwWrapper.asl")
  }

  #undef GPE1_EN
  #undef GPE0_EN
}

Scope (TRP1) {
  #define GPE0_EN GPE_TC_PCI_EXP
  #define GPE1_EN GPE1_TC_PCI1_PCI_EXP
  Include ("TcssPrwWrapper.asl")

  Device (PXSX)
  {
    Name (_ADR, 0x00000000)
    Include ("TcssPrwWrapper.asl")
  }

  #undef GPE1_EN
  #undef GPE0_EN
}

Scope (TRP2) {
  #define GPE0_EN GPE_TC_PCI_EXP
  #define GPE1_EN GPE1_TC_PCI2_PCI_EXP
  Include ("TcssPrwWrapper.asl")

  Device (PXSX)
  {
    Name (_ADR, 0x00000000)
    Include ("TcssPrwWrapper.asl")
  }

  #undef GPE1_EN
  #undef GPE0_EN
}

Scope (TRP3) {
  #define GPE0_EN GPE_TC_PCI_EXP
  #define GPE1_EN GPE1_TC_PCI3_PCI_EXP
  Include ("TcssPrwWrapper.asl")

  Device (PXSX)
  {
    Name (_ADR, 0x00000000)
    Include ("TcssPrwWrapper.asl")
  }

  #undef GPE1_EN
  #undef GPE0_EN
}