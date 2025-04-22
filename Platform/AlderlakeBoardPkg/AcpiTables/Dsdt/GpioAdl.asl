/**@file
 GPIO ASL library

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#if FixedPcdGetBool(PcdAdlLpSupport) == 0
  Include ("GpioCommunitiesAdl.asl")
  Include ("GpioGroupsAdl.asl")
  Include ("GpioLibAdl.asl")
#else
  Include ("GpioCommunitiesTglLp.asl")
  Include ("GpioGroupsTglLp.asl")
  Include ("GpioLibTglLp.asl")
#endif
