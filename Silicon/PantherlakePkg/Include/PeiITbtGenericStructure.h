/** @file
  ITBT Policy definition to be referred in both PEI and DXE phase.

  @copyright
  INTEL CONFIDENTIAL
  Copyright (C) 2017 Intel Corporation.

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

#ifndef _ITBT_POLICY_GENERIC_H_
#define _ITBT_POLICY_GENERIC_H_

#pragma pack(push, 1)

///
/// iTBT RootPort Data Structure
///
typedef struct _ITBT_ROOTPORT_CONFIG{
  UINT8  ITbtPcieRootPortEn;            ///< Disable/Enable iTBT PCIe Root Port
  UINT8  Reserved[3];                   ///< Reserved for DWORD alignment
} ITBT_ROOTPORT_CONFIG;

///
/// ITBT Controller Data Structure to be used cross to RP and controller to be shared by CONFIG_BLOCK and HOB
///
typedef struct _ITBT_GENERIC_CONFIG{
  /**
    Timeout value for forcing power iTBT controller on every boot/reboot/Sx exit as a precondition for execution of following
    mailbox communication.
    After applying Force Power Thunderbolt BIOS shall poll for iTBT readiness for mailbox communication
    If TBT cable is disconnected, iTBT microcontrollers are in lower power state.
    To ensure successful mailbox execution, independently on presence of TBT cable, TBT BIOS shall bring iTBT
    microcontrollers up by applying Force Power. iTBT microcontrollers will wake up either due to TBT cable presence or
    Force Power event.
    <b>(Test)</b>
    <b> default is 500 ms </b>
  **/
  UINT16  ITbtForcePowerOnTimeoutInMs;
  /**
    Timeout value while sending connect topology mailbox command in order to bring all connected TBT devices are available
    on PCIe before BIOS will enumerate them in BDS
    <b>(Test)</b>
    <b> default is 5000 ms </b>
  **/
  UINT16  ITbtConnectTopologyTimeoutInMs;
  UINT8   ITbtSecurityLevel;            ///< iTbt Security Level <b>Deprecated</b>
  UINT8   ITbtPcieTunnelingForUsb4;     ///< Disable/Enable PCIe tunneling for USB4. <b>default is enable</b>
  UINT8   Usb4CmMode;                   ///< USB4 CM mode
  UINT8   Reserved[1];                  ///< Reserved for DWORD alignment
} ITBT_GENERIC_CONFIG;

#pragma pack(pop)

#endif
