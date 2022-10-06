/** @file

  Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef ASPEED_GFX_TYPES_H_
#define ASPEED_GFX_TYPES_H_

#include <Base.h>
#include <PiPei.h>
#include <IndustryStandard/Pci.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/AspeedGfxLib.h>

typedef INT8                    int8_t;
typedef INT16                   int16_t;
typedef INT32                   int32_t;
typedef INT64                   int64_t;

typedef UINT8                   uint8_t;
typedef UINT16                  uint16_t;
typedef UINT32                  uint32_t;
typedef UINT64                  uint64_t;

typedef INTN                    intptr_t;
typedef UINTN                   uintptr_t;

typedef UINT8                   u8;
typedef UINT16                  u16;
typedef UINT32                  u32;
typedef UINT64                  u64;

typedef UINTN                   resource_size_t;

#define bool                    BOOLEAN
#define true                    TRUE
#define false                   FALSE

#define udelay(x)               MicroSecondDelay(x)
#define mdelay(x)               MicroSecondDelay(x*1000)
#define msleep(x)               MicroSecondDelay(x*1000)
#define __iomem

#define dev_info(dev, ...)      DEBUG((DEBUG_VERBOSE, ##__VA_ARGS__))
#define dev_dbg(dev,  ...)      DEBUG((DEBUG_VERBOSE, ##__VA_ARGS__))
#define dev_err(dev,  ...)      DEBUG((DEBUG_ERROR,   ##__VA_ARGS__))
#define pr_info(...)            DEBUG((DEBUG_VERBOSE, ##__VA_ARGS__))
#define pr_debug(...)           DEBUG((DEBUG_VERBOSE, ##__VA_ARGS__))
#define pr_err(...)             DEBUG((DEBUG_ERROR,   ##__VA_ARGS__))
#define DRM_INFO                pr_info
#define DRM_ERROR               pr_err
#define DRM_DEBUG_KMS           pr_debug

#define GFP_KERNEL              0

#define kzalloc(size, flags)    AllocateZeroPool(size)
#define kfree(address)

#define EIO                     5
#define ENOMEM                  12
#define EINVAL                  22

#define container_of            BASE_CR
#define spinlock_t              UINTN

struct pci_dev {
  unsigned int    bus;
  unsigned int    devfn;
  unsigned short  device;
  unsigned char   revision;
};

#endif // ASPEED_GFX_TYPES_H_
