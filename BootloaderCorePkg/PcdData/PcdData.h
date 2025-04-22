/** @file

  Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _PCD_DATA_H_
#define _PCD_DATA_H_

#define PCD_SERVICE_PEIM_VERSION            7

//
// PCD_PEI_SERVICE_DRIVER_VERSION is defined in Autogen.h.
//
#if (PCD_SERVICE_PEIM_VERSION != PCD_PEI_SERVICE_DRIVER_VERSION)
#error "Please make sure the version of PCD PEIM Service and the generated PCD PEI Database match."
#endif

#endif
