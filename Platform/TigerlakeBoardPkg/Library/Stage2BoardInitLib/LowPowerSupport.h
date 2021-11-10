/** @file
  Header file for LowPowerSupport.

  Copyright (c) 2019 - 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _LOW_POWER_SUPPORT_H_
#define _LOW_POWER_SUPPORT_H_

#include "PchLimits.h"

#define RootPort3             2
#define RootPort5             4
#define RootPort9             8

typedef struct {
  UINT8   PepSataContraints;
  UINT8   PepCpu;
  UINT8   PepGfx;
  UINT8   PepUart;
  UINT8   PepI2c0;
  UINT8   PepI2c1;
  UINT8   PepI2c2;
  UINT8   PepI2c3;
  UINT8   PepI2c4;
  UINT8   PepI2c5;
  UINT8   PepSpi;
  UINT8   PepXhci;
  UINT8   PepAudio;
  UINT8   PchHdAudio;
  UINT8   PepEmmc;
  UINT8   PepIpu;
  UINT8   PepGna;
  UINT8   PepCsme;
  UINT8   PepGbe;
  UINT8   PepThc0;
  UINT8   PepThc1;
  UINT8   PepTcss;
  UINT8   PepVmd;
  UINT8   PepPeg0;
  UINT8   PepOverride;
  UINT8   PepPeg0Storage;
  UINT8   PepPcieRp3;
  UINT8   PepPcieRp5;
  UINT8   PepPcieRp9;
  UINT8   PepPcieDg;
  UINT8   PepPcieStorage;
  UINT8   PepSerialIoI2c[8];
  UINT8   PepSerialIoSpi[7];
  UINT8   PepSerialIoUart[7];
  } PepConstraints;


/**
 Update Pep constraints.

 @param  Void
 @retval Updated Pep constraints Value
**/

UINT64
GetLowPowerS0IdleConstraint (
  VOID
  );

#endif //LOW_POWER_SUPPORT_H_
