/** @file

  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _REG_ACCESS_H_

#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/PcdLib.h>

#include <PcieRegs.h>
#include <SocRegs.h>
#include <PlatformBase.h>

#define MmPciAddress( Segment, Bus, Device, Function, Register ) \
  ( (UINTN)PcdGet64(PcdPciExpressBaseAddress) + \
    (UINTN)(Bus << 20) + \
    (UINTN)(Device << 15) + \
    (UINTN)(Function << 12) + \
    (UINTN)(Register) \
  )

#define MmPciBase(Bus, Device, Function)   MmPciAddress(0, Bus, Device, Function, 0)

#endif
