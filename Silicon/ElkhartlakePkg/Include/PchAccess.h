/** @file
  Macros that simplify accessing PCH devices's PCI registers.

 Copyright (c) 1999 - 2019, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_ACCESS_H_
#define _PCH_ACCESS_H_

#include <PlatformBase.h>
#include "PchLimits.h"
#include "PchReservedResources.h"
#include "Register/PchRegsSpi.h"

#define STALL_ONE_MICRO_SECOND 1
#define STALL_ONE_SECOND 1000000


///
/// The default PCH PCI segment and bus number
///
#define DEFAULT_PCI_SEGMENT_NUMBER_PCH  0
#define DEFAULT_PCI_BUS_NUMBER_PCH      0

//
// Default Vendor ID and Subsystem ID
//
#define V_INTEL_VENDOR_ID       0x8086
#define V_PCH_INTEL_VENDOR_ID   0x8086      ///< Default Intel PCH Vendor ID

//
// Include device register definitions
//
//#include "PcieRegs.h"
#include "Register/PchRegsPcr.h"
#include "Register/PchRegsP2sb.h"
#include "Register/PchRegsHda.h"
#include "Register/PchRegsHsio.h"
#include "Register/PchRegsLpc.h"
#include "Register/PchRegsPcie.h"
#include "Register/PchRegsSata.h"
#include "Register/PchRegsSmbus.h"
#include "Register/PchRegsSpi.h"
#include "Register/PchRegsUsb.h"
#include "Register/PchRegsGpio.h"
#include "Register/PchRegsSerialIo.h"
#include "Register/PchRegsTraceHub.h"
#include "Register/PchRegsIsh.h"
#include "Register/PchRegsFia.h"
#endif
