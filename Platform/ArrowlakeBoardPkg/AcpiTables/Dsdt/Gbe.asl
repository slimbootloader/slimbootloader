/**@file

  Copyright (c) 2024, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  Method (_DSM, 4, serialized) {
    if (PCIC (Arg0)) {
      return (PCID (Arg0, Arg1, Arg2, Arg3))
    };
    Return (Buffer () { 0 })
  }

  Method (_DSW, 3) {}
