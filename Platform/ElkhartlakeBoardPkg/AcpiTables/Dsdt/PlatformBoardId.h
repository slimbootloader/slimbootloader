/** @file
Defines Platform BoardIds

 Copyright (c) 2011 - 2018, Intel Corporation. All rights reserved.<BR>
 SPDX-License-Identifier: BSD-2-Clause-Patent
**/

#ifndef _PLATFORM_BOARD_ID_H_
#define _PLATFORM_BOARD_ID_H_

#define FlavorUnknown                       0x0
#define FlavorMobile                        0x1
#define FlavorDesktop                       0x2
#define FlavorWorkstation                   0x3
#define FlavorUpServer                      0x4
#define FlavorEmbedded                      0x5
#define FlavorPlatformMax                   0x6

#define TypeUnknown                         0x0
#define TypeTrad                            0x1
#define TypeUltUlx                          0x2

// BoardId Reserved for PCD Default SkuId usage
#define BoardIdReserved                     0x00


// ElkhartLake Board Id 0x1
#define BomIdEhlULp4xType4Main              0x00

// ElkhartLake Board Id 0x2
#define BomIdEhlULp4xType4Main              0x00


#define BoardIdUnknown1                     0xFFFF

#endif
