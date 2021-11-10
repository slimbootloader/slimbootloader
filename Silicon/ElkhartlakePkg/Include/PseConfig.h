/** @file
  PSE policy

  Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/
#ifndef _PSE_CONFIG_H_
#define _PSE_CONFIG_H_

#define PSE_PREMEM_CONFIG_REVISION 1
#define PSE_CONFIG_REVISION 1

#pragma pack (push,1)

typedef enum {
  NONTGPIO,
  TOP20,
  LOWER20,
  MID20
} PSE_TGPIO_PIN_ALLOCATION;

#pragma pack (pop)

#endif // _PSE_CONFIG_H_
