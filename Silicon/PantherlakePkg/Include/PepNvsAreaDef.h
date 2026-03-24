/**@file

  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

  //
  // Define PEP Constraint NVS Area operation region.
  //
#ifndef _PEP_NVS_H_
#define _PEP_NVS_H_

#pragma pack (push,1)
typedef struct {
  UINT8    PepPcieStorage;                          ///< Offset 0
  UINT8    PepPcieLan;                              ///< Offset 1
  UINT8    PepPcieWlan;                             ///< Offset 2
  UINT8    PepPcieGfx;                              ///< Offset 3
  UINT8    PepPcieOther;                            ///< Offset 4
  UINT8    PepAudio;                                ///< Offset 5
  UINT8    PepCpu;                                  ///< Offset 6
  UINT8    PepGfx;                                  ///< Offset 7
  UINT8    PepIpu;                                  ///< Offset 8
  UINT8    PepUart;                                 ///< Offset 9
  UINT8    PepI2c0;                                 ///< Offset 10
  UINT8    PepI2c1;                                 ///< Offset 11
  UINT8    PepI2c2;                                 ///< Offset 12
  UINT8    PepI2c3;                                 ///< Offset 13
  UINT8    PepI2c4;                                 ///< Offset 14
  UINT8    PepI2c5;                                 ///< Offset 15
  UINT8    PepI2c6;                                 ///< Offset 16
  UINT8    PepI2c7;                                 ///< Offset 17
  UINT8    PepSpi;                                  ///< Offset 18
  UINT8    PepXhci;                                 ///< Offset 19
  UINT8    PepCsme;                                 ///< Offset 20
  UINT8    PepHeci3;                                ///< Offset 21
  UINT8    PepGbe;                                  ///< Offset 22
  UINT8    PepThc0;                                 ///< Offset 23
  UINT8    PepThc1;                                 ///< Offset 24
  UINT8    PepUfs0;                                 ///< Offset 25
  UINT8    PepUfs1;                                 ///< Offset 26
  UINT8    PepTcss;                                 ///< Offset 27
  UINT8    PepNpu;                                  ///< Offset 28
  UINT8    PepIsh;                                  ///< Offset 29
  UINT8    PepGna;                                  ///< Offset 30
  UINT8    PepVmd;                                  ///< Offset 31
  UINT8    PepSata;                                 ///< Offset 32
  UINT8    PepSataEnum;                             ///< Offset 33
  UINT8    PepEmmc;                                 ///< Offset 34
  UINT8    PepSdxc;                                 ///< Offset 35
  UINT8    PepOsse;                                 ///< Offset 36
  UINT8    PepEse;                                  ///< Offset 37
} PEP_NVS_AREA;

#pragma pack(pop)
#endif
