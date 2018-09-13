/** @file

  Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
