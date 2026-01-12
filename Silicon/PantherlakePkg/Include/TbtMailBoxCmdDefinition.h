/**@file
  Header file of TBT MailBox Command Definition shared by C and ASL code.

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#define PCIE2TBT_VLD_B                               BIT0
#define TBT2PCIE_DON_R                               BIT0
#define TBT_MAILBOX_DELAY                            1000 ///< 1ms
#define SET_CM_TIMEOUT_IN_MS                         500   ///< 10 * TBT_MAILBOX_DELAY = 500ms

///
/// Mailbox command value to controller is generic on all types of TBT controllers
///
/**
  Mailbox command to link controller (LC) and Connection Manager (CM) to enter Sx with wake support
**/
#define PCIE2TBT_GO2SX                              0x04 //(0x02 << 1)
/**
  Mailbox command to link controller (LC) and Connection Manager (CM) to enter Sx without wake support
**/
#define PCIE2TBT_GO2SX_NO_WAKE                      0x06 //(0x03 << 1)
/**
  Mailbox command to controller to exit Sx and the system went to sleep with attached devices previously
**/
#define PCIE2TBT_SX_EXIT_TBT_CONNECTED              0x08 //(0x04 << 1)
#define PCIE2TBT_SX_EXIT_NO_TBT_CONNECTED           0x0A //(0x05 << 1)
#define PCIE2TBT_OS_UP                              0x0C //(0x06 << 1)
#define PCIE2TBT_SET_SECURITY_LEVEL                 0x10 //(0x08 << 1)
#define PCIE2TBT_GET_SECURITY_LEVEL                 0x12 //(0x09 << 1)
#define PCIE2TBT_CM_AUTH_MODE_ENTER                 0x20 //(0x10 << 1)
#define PCIE2TBT_CM_AUTH_MODE_EXIT                  0x22 //(0x11 << 1)
#define PCIE2TBT_BOOT_ON                            0x30 //(0x18 << 1)
#define PCIE2TBT_BOOT_OFF                           0x32 //(0x19 << 1)
#define PCIE2TBT_USB_ON                             0x32 //(0x19 << 1)
#define PCIE2TBT_GET_ENUMERATION_METHOD             0x34 //(0x1A << 1)
#define PCIE2TBT_SET_ENUMERATION_METHOD             0x36 //(0x1B << 1)
#define PCIE2TBT_POWER_CYCLE                        0x38 //(0x1C << 1)
/**
  Mailbox command to CM to ensure all connected TBT devices are available on PCIe
**/
#define PCIE2TBT_CONNECT_TOPOLOGY_COMMAND           0x3E //(0x1F << 1)
#define PCIE2TBT_FIRMWARE_CM_MODE                   0x44 //(0x22 << 1)
#define PCIE2TBT_PASS_THROUGH_MODE                  0x46 //(0x23 << 1)
#define PCIE2TBT_GO_2_SLEEP                         0x48 //(0x24 << 1)
/**
  Mailbox command to set class code of host router for USB4 Ver2 support
**/
#define PCIE2TBT_SET_CLASS_CODE                     0x62 //(0x31 << 1)

/**
  Extra data of mailbox command
**/
#define ENABLE_PCIE_TUNNELING_FOR_USB4              0x0
#define DISABLE_PCIE_TUNNELING_FOR_USB4             0x5

/**
  Mailbox command to set max payload size
**/
#define PCIE2TBT_MAX_PAYLOAD_SIZE_CMD               0x22 // (0x11 << 1)
#define PCIE2TBT_MAX_PAYLOAD_SIZE_DATA_128          0x0
#define PCIE2TBT_MAX_PAYLOAD_SIZE_DATA_256          0x1

///
/// Mailbox command to program data offset
///
#define PCIE2TBT_SSID_SVID_DATA_OFFSET              0xF4

