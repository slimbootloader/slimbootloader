/** @file

  Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _REG_ACCESS_H_

#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <PchReservedResources.h>
#include <BlCommon.h>
#include <PchLimits.h>
#include <Register/PchRegsGpio.h>
#include <Register/PchRegsLpc.h>
#include <Register/RtcRegs.h>
#include <Register/PchRegsPcr.h>
#include <Register/PchRegsSerialIo.h>
#include <Register/PmcRegs.h>
#include <Register/PchRegsP2sb.h>
#include <Register/SaRegs.h>
#include <IndustryStandard/Pci.h>

#include <PlatformBase.h>

///
/// The default PCH PCI bus number
///
#define V_PCH_INTEL_VENDOR_ID   0x8086      ///< Default Intel PCH Vendor ID

#define DEFAULT_PCI_BUS_NUMBER_PCH  0

#define MM_PCI_OFFSET(Bus, Device, Function) \
    ( (UINTN)(Bus << 20) +    \
      (UINTN)(Device << 15) + \
      (UINTN)(Function << 12) )

#define MmPciBase(Bus, Device, Function) \
  ( (UINTN)PcdGet64(PcdPciExpressBaseAddress) + \
    (UINTN)(Bus << 20) +    \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) \
  )


#endif
