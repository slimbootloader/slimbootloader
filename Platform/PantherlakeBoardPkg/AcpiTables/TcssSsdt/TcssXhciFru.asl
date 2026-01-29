/** @file
  This file contains the TCSS PCIe Root Port configuration

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Scope (TXHC) {
  #define GPE0_EN GPE_TC_PME_B0
  #define GPE1_EN GPE1_TC_IOM_PME_B0_EN
  Include ("TcssPrwWrapper.asl")
  #undef GPE1_EN
  #undef GPE0_EN
}