/** @file
  HOB definition to be used by iTBT during pre-boot enviornment.

  @copyright
  INTEL CONFIDENTIAL
  Copyright (C) 2014 Intel Corporation.

  This software and the related documents are Intel copyrighted materials,
  and your use of them is governed by the express license under which they
  were provided to you ("License"). Unless the License provides otherwise,
  you may not use, modify, copy, publish, distribute, disclose or transmit
  this software or the related documents without Intel's prior written
  permission.

  This software and the related documents are provided as is, with no
  express or implied warranties, other than those that are expressly stated
  in the License.

@par Specification Reference:
**/
#ifndef _ITBT_INFO_GUID_H_
#define _ITBT_INFO_GUID_H_

#include <PeiITbtGenericStructure.h>
#include <TcssInfo.h>
#include <MemInfoHob.h>

#pragma pack(1)
//
// ITBT Info HOB
//
typedef struct _ITBT_INFO_HOB {
  EFI_HOB_GUID_TYPE      EfiHobGuidType;
  ITBT_ROOTPORT_CONFIG   ITbtRootPortConfig [MAX_ITBT_PCIE_PORT];           ///< iTbt Root Port Configuration
  UINT16                 ITbtForcePowerOnTimeoutInMs;                       ///< Timeout value for forcing power iTBT controller on every boot/reboot/Sx exit as a precondition for execution of following mailbox communication.
  UINT16                 ITbtConnectTopologyTimeoutInMs;                    ///< Timeout value while sending connect topology mailbox command
  UINT8                  ITbtPcieTunnelingForUsb4;                          ///< Disable/Enable PCIe tunneling for USB4.
  UINT8                  Usb4CmMode;                                        ///< USB4 CM mode
  UINT8                  Reserved[2];                                       ///< Reserved for DWORD alignment
} ITBT_INFO_HOB;
#pragma pack()

#endif
