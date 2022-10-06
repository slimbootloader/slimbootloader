/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "Stage1ABoardInitLib.h"

#define IPMI_DEFAULT_SMM_IO_BASE              0xCA2

#define REG_LOGICAL_DEVICE                    0x07
#define ACTIVATE                              0x30

#define ASPEED2500_SIO_INDEX_PORT             0x2E
#define ASPEED2500_SIO_DATA_PORT              (ASPEED2500_SIO_INDEX_PORT+1)

#define ASPEED2500_SIO_MAILBOX                0x0E

#define ASPEED2500_SIO_UNLOCK                 0xA5
#define ASPEED2500_SIO_LOCK                   0xAA

#define BASE_ADDRESS_HIGH0                    0x60
#define BASE_ADDRESS_LOW0                     0x61

/**
  Initialize Platform SIO chip.

**/
VOID
EFIAPI
PlatformSioEarlyInit (
  VOID
)
{
  //
  // Enable LPC decode at 0xCA2 for BMC, 0x600 for Mailbox
  //
  PchLpcGenIoRangeSet ((IPMI_DEFAULT_SMM_IO_BASE  & 0xFF0), 0x10);
  PchLpcGenIoRangeSet ((0x600  & 0xFF0), 0x10);

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_UNLOCK);

  //
  // Mailbox
  //
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, REG_LOGICAL_DEVICE);
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, ASPEED2500_SIO_MAILBOX);

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, BASE_ADDRESS_HIGH0);
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, (UINT8)(0x600 >> 8));

  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, BASE_ADDRESS_LOW0);
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, (UINT8)(0x600 & 0xFF));
  //
  // Active Mailbox
  //
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ACTIVATE);
  IoWrite8 (ASPEED2500_SIO_DATA_PORT, 1);
  IoWrite8 (ASPEED2500_SIO_INDEX_PORT, ASPEED2500_SIO_LOCK);
}
