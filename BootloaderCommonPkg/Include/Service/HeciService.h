/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __HECI_SERVICE_H__
#define __HECI_SERVICE_H__

#include <Guid/BootLoaderServiceGuid.h>

#define HECI_SERVICE_SIGNATURE  SIGNATURE_32 ('H', 'E', 'C', 'I')
#define HECI_SERVICE_VERSION    1


#define  SUBCMD_ENTER_DNX_MODE   1
#define  SUBCMD_DUMP_CSE_STATUS  2

/**
  Wrapper function to for the HECI command without parameter.

  @retval EFI_SUCCESS         The sub command runs success
  @retval Others              The sub command doesn't run success

**/
typedef
EFI_STATUS
(EFIAPI *SIMPLE_HECI_CMD) (
  UINT32   SubCmd
);

/**
  HeciSendUserCommand

  @param[in,out]  Data
  @param[in]      Length
  @param[in]      Force

  @return EFI_STATUS

**/
typedef
EFI_STATUS
(EFIAPI *HECI_USER_COMMAND) (
  IN OUT UINT8 *Data,
  IN     UINT8 Length,
  IN     UINT8 Force
);

/**
  Dummy function

  @retval EFI_SUCCESS         The command runs success
  @retval Others              The command doesn't run success

**/
typedef
EFI_STATUS
(EFIAPI *DUMMY_FUNCTION) (
  VOID
);

typedef struct {
  SERVICE_COMMON_HEADER              Header;
  SIMPLE_HECI_CMD                    SimpleHeciCommand;
  HECI_USER_COMMAND                  HeciUserCommand;
  DUMMY_FUNCTION                     Reserved[9];
} HECI_SERVICE;

#endif
