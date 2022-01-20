/** @file
  Header file for SerialIo

  Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _SERIAL_IO_H_
#define _SERIAL_IO_H_

/**
  Update Serial IO for FSP-S UPD

  @param  FspsConfig            The pointer to the FSP-S UPD to be updated.
**/
VOID
EFIAPI
SerialIoPostMemConfig (
  FSP_S_CONFIG  *FspsConfig
);

#endif // _SERIAL_IO_H
