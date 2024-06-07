/** @file
  ACPI DSDT table for Serial Io I2C LTR settings

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

// _DSD: Device-Specific Data
//
//  - I2C LTR UUID: f87a6d23-2884-4fe4-a55f-633d9e339ce1
//  Designated for I2C Driver to configure its Latency Tolerance Reporting
//  - idle latency tolerance
//  - active latency for each Speed SS/FM/HS
//
//  Example:
//    "idle-latency-tolerance", 0xBB8
//    0xBB8 -> 3000 us = 3ms of IDLE LTR
//  Value of 0xFFFF will cause no LTR reporting/driver default used
//
  ToUUID ("f87a6d23-2884-4fe4-a55f-633d9e339ce1"), //I2C LTR reporting UUID
  Package () {
    Package (0x02) {
        "idle-latency-tolerance",
        0xFFFF
    },
    Package (0x02) {
        "SS-active-latency-tolerance",
        0xFFFF
    },
    Package (0x02) {
        "FM-active-latency-tolerance",
        0xFFFF
    },
    Package (0x02) {
        "FMP-active-latency-tolerance",
        0xFFFF
    }
  }
