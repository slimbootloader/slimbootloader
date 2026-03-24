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
  Mailbox command to CM to ensure all connected TBT devices are available on PCIe
**/
#define PCIE2TBT_CONNECT_TOPOLOGY_COMMAND           0x3E //(0x1F << 1)
#define PCIE2TBT_FIRMWARE_CM_MODE                   0x44 //(0x22 << 1)
#define PCIE2TBT_PASS_THROUGH_MODE                  0x46 //(0x23 << 1)
#define PCIE2TBT_GO_2_SLEEP                         0x48 //(0x24 << 1)
