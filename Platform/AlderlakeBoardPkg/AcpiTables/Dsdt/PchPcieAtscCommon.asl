/** @file
  This file contains the PCIe Root Port Common configuration

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
Name(_DSD, Package () {
  // This informs driver that PTM ART is traceble to TSC
  ToUUID("E995B7C2-BD1F-45F5-A911-5BCD9269CCEB"),
  Package () {
  }
  }) // END Name(_DSD)
