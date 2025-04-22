/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#ifndef _PCH_REGS_H_
#define _PCH_REGS_H_

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

#endif //_PCH_REGS_H_
