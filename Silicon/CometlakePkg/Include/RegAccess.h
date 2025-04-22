/** @file

  Copyright (c) 2016 - 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _REG_ACCESS_H_

#include <PlatformBase.h>
#include <IndustryStandard/Pci.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <BlCommon.h>

//
// Include device register definitions
//
#include <PcieRegs.h>
#include <Register/CpuRegs.h>
#include <Register/SaRegs.h>
#include <Register/MeChipset.h>
#include <Register/HeciRegs.h>
#include <Register/MkhiMsgs.h>
#include <Register/MeState.h>
#include <Register/BupMsgs.h>
#include <PchLimits.h>
#include <PchReservedResources.h>
#include <Register/PchRegsSpi.h>
#include <Register/PchRegsGpio.h>
#include <Register/PchRegsGpioCml.h>
#include <Register/PchRegsSerialIo.h>
#include <Register/PchRegsSerialIoCml.h>
#include <Register/PchRegsLpc.h>
#include <Register/PchRegsLpcCml.h>
#include <Register/PchRegsP2sb.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsPmc.h>
#include <Register/PchRegsPmcCml.h>
#include <Register/PchRegsPcie.h>
#include <Register/PchRegsDmi.h>
#include <Register/PchRegsScsSd.h>
#include <Register/RegsUsb.h>
#include <Register/RtcRegs.h>

//
// Default Vendor ID and Subsystem ID
//
#define V_INTEL_VENDOR_ID         0x8086
#define V_PCH_INTEL_VENDOR_ID     0x8086      ///< Default Intel PCH Vendor ID
#define V_PCH_DEFAULT_SID         0x7270      ///< Default Intel PCH Subsystem ID
#define V_PCH_DEFAULT_SVID_SID    (V_INTEL_VENDOR_ID + (V_PCH_DEFAULT_SID << 16))   ///< Default INTEL PCH Vendor ID and Subsystem ID

///
/// The default PCH PCI segment and bus number
///
#define DEFAULT_PCI_BUS_NUMBER_PCH      0

#define MM_PCI_OFFSET(Bus, Device, Function) \
    ( (UINTN)(Bus << 20) +    \
      (UINTN)(Device << 15) + \
      (UINTN)(Function << 12) )

#define MmPciBase(Bus, Device, Function) \
  ( (UINTN)PcdGet64(PcdPciExpressBaseAddress) + \
    MM_PCI_OFFSET(Bus, Device, Function) \
  )

#endif
