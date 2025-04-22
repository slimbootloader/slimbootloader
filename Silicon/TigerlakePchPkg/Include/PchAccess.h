/** @file
  Macros that simplify accessing PCH devices's PCI registers.

  Copyright (c) 1999 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PCH_ACCESS_H_
#define _PCH_ACCESS_H_

#include "PchLimits.h"
#include "PchReservedResources.h"
#include <GpioPinsVer2H.h>
#include <GpioPinsVer2Lp.h>

#define PCH_TGL                 1
#define PCH_ICL                 0
#define HPET_BASE_ADDRESS       0xFED00000

#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 1
#endif
#ifndef STALL_ONE_SECOND
#define STALL_ONE_SECOND 1000000
#endif

///
/// The default PCH PCI segment and bus number
///
#define DEFAULT_PCI_SEGMENT_NUMBER_PCH  0
#define DEFAULT_PCI_BUS_NUMBER_PCH      0

//
// Default Vendor ID and Subsystem ID
//
#define V_INTEL_VENDOR_ID       0x8086

//
// Include device register definitions
//
//#include "PcieRegs.h"
#include "Register/PchRegsPcr.h"
#include "Register/PchRegsP2sb.h"
#include "Register/PchRegsHda.h"
#include "Register/PchRegsHsio.h"
#include "Register/PchRegsLpc.h"
#include "Register/PchRegsPmc.h"
#include "Register/PchRegsPmcCnl.h"
#include "Register/PchRegsPcie.h"
#include "Register/PchRegsSata.h"
#include "Register/PchRegsSmbus.h"
#include "Register/PchRegsSpi.h"
#include "Register/PchRegsUsb.h"
#include "Register/PchRegsGpio.h"
#ifdef PCH_ICL
#else
#include "Register/PchRegsThermalCnl.h"

#endif
#include "Register/PchRegsSerialIo.h"
#include "Register/PchRegsCnvi.h"
#include "Register/PchRegsTraceHub.h"
#include "Register/PchRegsScs.h"
#include "Register/PchRegsIsh.h"
#include "Register/PchRegsDmi.h"
//#include "Register/PchRegsFia.h"
#include "Register/PmcRegs.h"
#include "Register/PchRegsSpi.h"
#include "Register/PchRegsSerialIoTgl.h"
#include "Register/RtcRegs.h"

#endif
