/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __HECI_SERVICE_H__
#define __HECI_SERVICE_H__

#include <Guid/BootLoaderServiceGuid.h>

#define HECI_SERVICE_SIGNATURE  SIGNATURE_32 ('H', 'E', 'C', 'I')
#define HECI_SERVICE_VERSION    1
#define HECI_SERVICE_VERSION_2  2


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
  Function sends one message (of any length) through the HECI circular buffer.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the Host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_UNSUPPORTED      Current ME mode doesn't support send this message through this HECI
**/
typedef
EFI_STATUS
(EFIAPI *HECI_SEND) (
  IN UINT8        HeciDev,
  IN UINT32      *Message,
  IN UINT32       Length,
  IN UINT8        HostAddress,
  IN UINT8        MeAddress
  );

/**
  Reads a message from the ME across HECI. This function can only be used after invoking HeciSend() first.

  @param[in] HeciDev              The HECI device to be accessed.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[out] MessageBody         Pointer to a buffer used to receive a message.
  @param[in][out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this message through this HECI
**/
typedef
EFI_STATUS
(EFIAPI *HECI_RECEIVE) (
  IN      UINT8        HeciDev,
  IN      UINT32       Blocking,
  OUT     UINT32      *MessageBody,
  IN OUT  UINT32      *Length
  );

/**
  Function forces a reinit of the heci interface by following the reset heci interface via Host algorithm

  @param[in] HeciDev              The HECI device to be accessed.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Interface reset
**/
typedef
EFI_STATUS
(EFIAPI *HECI_RESET_INTERFACE) (
  IN UINT8        HeciDev
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
  HECI_SEND                          HeciSend;
  HECI_RECEIVE                       HeciReceive;
  HECI_RESET_INTERFACE               HeciResetInterface;
  DUMMY_FUNCTION                     Reserved[6];
} HECI_SERVICE;


/**
  Function sends one message (of any length) through the HECI circular buffer.

  @param[in] HeciDevs             The HECI device to be accessed.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the Host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_UNSUPPORTED      Current ME mode doesn't support send this message through this HECI
**/
typedef
EFI_STATUS
(EFIAPI *HECI_SEND_V2) (
  IN UINT32       HeciDevs,
  IN UINT32      *Message,
  IN UINT32       Length,
  IN UINT8        HostAddress,
  IN UINT8        MeAddress
  );

/**
  Reads a message from the ME across HECI. This function can only be used after invoking HeciSend() first.

  @param[in] HeciDevs             The HECI device to be accessed.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[out] MessageBody         Pointer to a buffer used to receive a message.
  @param[in][out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
  @retval EFI_UNSUPPORTED         Current ME mode doesn't support this message through this HECI
**/
typedef
EFI_STATUS
(EFIAPI *HECI_RECEIVE_V2) (
  IN      UINT32       HeciDevs,
  IN      UINT32       Blocking,
  OUT     UINT32      *MessageBody,
  IN OUT  UINT32      *Length
  );

/**
  Function forces a reinit of the heci interface by following the reset heci interface via Host algorithm

  @param[in] HeciDevs             The HECI device to be accessed.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Interface reset
**/
typedef
EFI_STATUS
(EFIAPI *HECI_RESET_INTERFACE_V2) (
  IN UINT32        HeciDevs
  );

typedef struct {
  SERVICE_COMMON_HEADER              Header;
  SIMPLE_HECI_CMD                    SimpleHeciCommand;
  HECI_USER_COMMAND                  HeciUserCommand;
  HECI_SEND_V2                       HeciSend;
  HECI_RECEIVE_V2                    HeciReceive;
  HECI_RESET_INTERFACE_V2            HeciResetInterface;
  DUMMY_FUNCTION                     Reserved[6];
} HECI_SERVICE_V2;

#endif
