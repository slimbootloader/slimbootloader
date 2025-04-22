/**@file
  _DSD Methods for PCIe auxilary power budgeting support.
  Include it in the Pch PCIe root port scope that supports aux. power budgeting

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

Name (_DSD, Package () {
  //
  // This _DSD object informs Windows PCIe bus driver that
  // this root ports supports auxilary power budgeting.
  //
  ToUUID("6B4AD420-8FD3-4364-ACF8-EB94876FD9EB"),
  Package () {
  },
  //
  // Adding _DSD support for PCI root ports implementing fundamental device reset
  // The _DSD is queried by the OS driver to check the presence of a device reset pin on the Root Port
  //
  ToUUID("FDF06FAD-F744-4451-BB64-ECD792215B10"),
  Package () {
    Package (2) {"FundamentalDeviceResetTriggeredOnD3ToD0", 1},
  },
  // This informs driver that PTM ART is traceble to TSC
  ToUUID("E995B7C2-BD1F-45F5-A911-5BCD9269CCEB"),
  Package () {
  }
})
