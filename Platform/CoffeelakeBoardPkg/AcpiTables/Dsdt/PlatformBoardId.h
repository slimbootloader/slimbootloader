/** @file
  Defines Platform BoardIds

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
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

//
// CannonLake Board Id 0x00 - 0xF
//
//#define BoardIdCannonlakeX0Lpddr4         0x00
#define BoardIdCannonlakeUDdr4Rvp           0x01
#define BoardIdCannonlakeULpddr4Rvp         0x02
#define BoardIdCannonlakeULpdimmDvp         0x03
#define BoardIdCannonlakeUSds               0x05
#define BoardIdCannonLakeYLpddr3            0x06
#define BoardIdCannonLakeYLpddr4            0x07
//#define BoardIdCannonLakeLpdimmDoe        0x08
//#define BoardIdCannonLakeIdv1             0x09
#define BoardIdCannonLakeHDdr4              0x0B
#define BoardIdCannonLakeHLpddr4            0x0D
//#define BoardIdCannonlakeULpddr3Rvp5      0x0E

//
// CoffeeLake Board Id's 0x10 - 0x1F
//
#define BoardIdCoffeeLakeULpddr3CpuBrd      0x10 // This is KC PnP skew and has two board's one for CPU and one for Companion Card
#define BoardIdCoffeeLakeULpddr3CompBrd     0x11 // Companion Card PnP board Id
#define BoardIdCoffeeLakeUDdr4              0x12 // PI/merged power plane board
#define BoardIdCoffeeLakeULpddr3CpuBrdCrb   0x13 // This is KC PnP skew for crb board
#define BoardIdWhiskeyLakeErb               0x14 // WhiskeyLake ERB
#define BoardIdCoffeeLakeHDdr4              0x16
#define BoardIdCoffeeLakeHDdr4mDvpCfgC      0x117
#define BoardIdCoffeeLakeHDdr4mDvpCfgD      0x217
#define BoardIdCoffeeLakeSUdimm             0x18 // PI/merged power plane board
#define BoardIdCoffeeLakeSSodimm            0x19 // PnP board
#define BoardIdCoffeeLakeOCUdimm            0x1A // CFL-S OC board
#define BoardIdCoffeeLakeS82Udimm           0x1B // CFL-S 82
//0x1C reserved for Generic use
#define BoardIdGlacierFallsXErbEv           0x1D //GCF-X ERB-EV board
#define BoardIdGlacierFallsXCrb             0x1E //GCF-X CRB board
#define BoardIdGlacierFallsXPpv             0x1F //GCF-X PPV board

#define BoardIdCoffeeLakeHuSFF              0x116 //Virtual Board id used for uSFF

#define BoardIdUnknown1                     0xffff

#endif
