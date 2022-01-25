/** @file
  Macros that simplify accessing PCH devices's PCI registers.

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_ACCESS_H_
#define _PCH_ACCESS_H_

#include "PchLimits.h"
#include "PchReservedResources.h"

#ifndef SLE_FLAG
#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 1
#endif
#ifndef STALL_ONE_SECOND
#define STALL_ONE_SECOND 1000000
#endif
#else  // SLE_FLAG
#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 0
#endif
#ifndef STALL_ONE_SECOND
#define STALL_ONE_SECOND 0
#endif
#endif // SLE_FLAG

///
/// The default PCH PCI segment and bus number
///
#define DEFAULT_PCI_SEGMENT_NUMBER_PCH  0
#define DEFAULT_PCI_BUS_NUMBER_PCH      0

//
// Default Vendor ID and Subsystem ID
//
#define V_PCH_INTEL_VENDOR_ID   0x8086      ///< Default Intel PCH Vendor ID
#define V_PCH_DEFAULT_SID       0x7270      ///< Default Intel PCH Subsystem ID
#define V_PCH_DEFAULT_SVID_SID  (V_INTEL_VENDOR_ID + (V_PCH_DEFAULT_SID << 16))   ///< Default INTEL PCH Vendor ID and Subsystem ID

//
// Include device register definitions
//
//#include "PcieRegs.h"
#include <Register/PchRegs.h>
#include "Register/PchRegsPcr.h"
#include "Register/PchRegsP2sb.h"
#include "Register/PchRegsLpc.h"
#include "Register/PchRegsLpcAdl.h"
#include "Register/PchRegsPmc.h"
#include "Register/PchRegsPmcVer4.h"
#include "Register/PchRegsGpio.h"
#include "Register/PchRegsGpioVer4.h"
#include "Register/PchRegsDmi.h"
#include <Register/PchBdfAssignment.h>
#include <Register/SerialIoRegs.h>
#endif
