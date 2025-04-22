/** @file

  The header file provides flash descriptor region access setting


  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/


#define MASTER_ACCESS_BIOS_SET_REGION        (B_READ_ACCESS_EX_REGION_0 \
                                                | B_READ_ACCESS_EX_REGION_1 \
                                                | B_READ_ACCESS_EX_REGION_2 \
                                                | B_READ_ACCESS_EX_REGION_3 \
                                                | B_WRITE_ACCESS_EX_REGION_1 \
                                                | B_WRITE_ACCESS_EX_REGION_3 )

#define MASTER_ACCESS_BIOS_CLEAR_REGION      (B_WRITE_ACCESS_EX_REGION_0 \
                                                | B_WRITE_ACCESS_EX_REGION_2 \
                                                | B_READ_ACCESS_EX_REGION_15 \
                                                | B_WRITE_ACCESS_EX_REGION_15 \
                                                | B_READ_ACCESS_EX_REGION_5 \
                                                | B_READ_ACCESS_EX_REGION_6 \
                                                | B_READ_ACCESS_EX_REGION_7 \
                                                | B_READ_ACCESS_EX_REGION_9 \
                                                | B_READ_ACCESS_EX_REGION_10 \
                                                | B_READ_ACCESS_EX_REGION_11 \
                                                | B_READ_ACCESS_EX_REGION_12 \
                                                | B_READ_ACCESS_EX_REGION_13 \
                                                | B_READ_ACCESS_EX_REGION_14 \
                                                | B_WRITE_ACCESS_EX_REGION_5 \
                                                | B_WRITE_ACCESS_EX_REGION_6 \
                                                | B_WRITE_ACCESS_EX_REGION_7 \
                                                | B_WRITE_ACCESS_EX_REGION_9 \
                                                | B_WRITE_ACCESS_EX_REGION_10 \
                                                | B_WRITE_ACCESS_EX_REGION_11 \
                                                | B_WRITE_ACCESS_EX_REGION_12 \
                                                | B_WRITE_ACCESS_EX_REGION_13 \
                                                | B_WRITE_ACCESS_EX_REGION_14 )

#define MASTER_ACCESS_ME_SET_REGION          (B_READ_ACCESS_EX_REGION_0 \
                                               | B_READ_ACCESS_EX_REGION_2 \
                                               | B_READ_ACCESS_EX_REGION_3 \
                                               | B_WRITE_ACCESS_EX_REGION_2 )


#define MASTER_ACCESS_ME_CLEAR_REGION        (B_WRITE_ACCESS_EX_REGION_0 \
                                                | B_WRITE_ACCESS_EX_REGION_1 \
                                                | B_WRITE_ACCESS_EX_REGION_3 \
                                                | B_WRITE_ACCESS_EX_REGION_4 \
                                                | B_WRITE_ACCESS_EX_REGION_8 \
                                                | B_READ_ACCESS_EX_REGION_1 \
                                                | B_READ_ACCESS_EX_REGION_4 \
                                                | B_READ_ACCESS_EX_REGION_8 \
                                                | B_READ_ACCESS_EX_REGION_15 \
                                                | B_WRITE_ACCESS_EX_REGION_15 \
                                                | B_READ_ACCESS_EX_REGION_5 \
                                                | B_READ_ACCESS_EX_REGION_6 \
                                                | B_READ_ACCESS_EX_REGION_7 \
                                                | B_READ_ACCESS_EX_REGION_9 \
                                                | B_READ_ACCESS_EX_REGION_10 \
                                                | B_READ_ACCESS_EX_REGION_11 \
                                                | B_READ_ACCESS_EX_REGION_12 \
                                                | B_READ_ACCESS_EX_REGION_13 \
                                                | B_READ_ACCESS_EX_REGION_14 \
                                                | B_WRITE_ACCESS_EX_REGION_5 \
                                                | B_WRITE_ACCESS_EX_REGION_6 \
                                                | B_WRITE_ACCESS_EX_REGION_7 \
                                                | B_WRITE_ACCESS_EX_REGION_9 \
                                                | B_WRITE_ACCESS_EX_REGION_10 \
                                                | B_WRITE_ACCESS_EX_REGION_11 \
                                                | B_WRITE_ACCESS_EX_REGION_12 \
                                                | B_WRITE_ACCESS_EX_REGION_13 \
                                                | B_WRITE_ACCESS_EX_REGION_14 )

#define MASTER_ACCESS_GBE_SET_REGION        (B_READ_ACCESS_EX_REGION_0 \
                                               | B_READ_ACCESS_EX_REGION_3 \
                                               | B_WRITE_ACCESS_EX_REGION_3)

#define MASTER_ACCESS_GBE_CLEAR_REGION      (B_WRITE_ACCESS_EX_REGION_0 \
                                                | B_WRITE_ACCESS_EX_REGION_1 \
                                                | B_WRITE_ACCESS_EX_REGION_2 \
                                                | B_WRITE_ACCESS_EX_REGION_4 \
                                                | B_WRITE_ACCESS_EX_REGION_8 \
                                                | B_READ_ACCESS_EX_REGION_1 \
                                                | B_READ_ACCESS_EX_REGION_2 \
                                                | B_READ_ACCESS_EX_REGION_4 \
                                                | B_READ_ACCESS_EX_REGION_8 \
                                                | B_READ_ACCESS_EX_REGION_15 \
                                                | B_WRITE_ACCESS_EX_REGION_15 \
                                                | B_READ_ACCESS_EX_REGION_5 \
                                                | B_READ_ACCESS_EX_REGION_6 \
                                                | B_READ_ACCESS_EX_REGION_7 \
                                                | B_READ_ACCESS_EX_REGION_9 \
                                                | B_READ_ACCESS_EX_REGION_10 \
                                                | B_READ_ACCESS_EX_REGION_11 \
                                                | B_READ_ACCESS_EX_REGION_12 \
                                                | B_READ_ACCESS_EX_REGION_13 \
                                                | B_READ_ACCESS_EX_REGION_14 \
                                                | B_WRITE_ACCESS_EX_REGION_5 \
                                                | B_WRITE_ACCESS_EX_REGION_6 \
                                                | B_WRITE_ACCESS_EX_REGION_7 \
                                                | B_WRITE_ACCESS_EX_REGION_9 \
                                                | B_WRITE_ACCESS_EX_REGION_10 \
                                                | B_WRITE_ACCESS_EX_REGION_11 \
                                                | B_WRITE_ACCESS_EX_REGION_12 \
                                                | B_WRITE_ACCESS_EX_REGION_13 \
                                                | B_WRITE_ACCESS_EX_REGION_14 )

#define MASTER_ACCESS_EC_SET_REGION         (B_READ_ACCESS_EX_REGION_0 \
                                               | B_READ_ACCESS_EX_REGION_8 \
                                               | B_WRITE_ACCESS_EX_REGION_8)

#define MASTER_ACCESS_EC_CLEAR_REGION       (B_WRITE_ACCESS_EX_REGION_0 \
                                                | B_WRITE_ACCESS_EX_REGION_1 \
                                                | B_WRITE_ACCESS_EX_REGION_2 \
                                                | B_WRITE_ACCESS_EX_REGION_3 \
                                                | B_WRITE_ACCESS_EX_REGION_4 \
                                                | B_READ_ACCESS_EX_REGION_2 \
                                                | B_READ_ACCESS_EX_REGION_3 \
                                                | B_READ_ACCESS_EX_REGION_4 \
                                                | B_READ_ACCESS_EX_REGION_15 \
                                                | B_WRITE_ACCESS_EX_REGION_15 \
                                                | B_READ_ACCESS_EX_REGION_5 \
                                                | B_READ_ACCESS_EX_REGION_6 \
                                                | B_READ_ACCESS_EX_REGION_7 \
                                                | B_READ_ACCESS_EX_REGION_9 \
                                                | B_READ_ACCESS_EX_REGION_10 \
                                                | B_READ_ACCESS_EX_REGION_11 \
                                                | B_READ_ACCESS_EX_REGION_12 \
                                                | B_READ_ACCESS_EX_REGION_13 \
                                                | B_READ_ACCESS_EX_REGION_14 \
                                                | B_WRITE_ACCESS_EX_REGION_5 \
                                                | B_WRITE_ACCESS_EX_REGION_6 \
                                                | B_WRITE_ACCESS_EX_REGION_7 \
                                                | B_WRITE_ACCESS_EX_REGION_9 \
                                                | B_WRITE_ACCESS_EX_REGION_10 \
                                                | B_WRITE_ACCESS_EX_REGION_11 \
                                                | B_WRITE_ACCESS_EX_REGION_12 \
                                                | B_WRITE_ACCESS_EX_REGION_13 \
                                                | B_WRITE_ACCESS_EX_REGION_14 )
