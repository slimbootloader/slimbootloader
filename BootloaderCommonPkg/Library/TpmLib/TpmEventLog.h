/** @file
  TPM Event Log library routines to provide TCG Event Log support.
  For more details, consult 'Event Logging' chapter in TCG  PC Client
  Platform Firmware Profile  specification.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef _TPM_EVENT_LIB_H_
#define _TPM_EVENT_LIB_H_


/**
  Allocate and initialize TCG Event Log.

  @retval RETURN_SUCCESS     Operation completed successfully.
  @retval Others             Unable to create TCG event log.
**/
RETURN_STATUS
TpmTcgLogInit (
  VOID
  );


/**
  Log the Specification ID Version event in TCG 2.0 Event Log.
  This event is not extended in PCR's.

  @param[in] ActivePcr  Currently Active PCR banks in TPM.

  @retval RETURN_SUCCESS     Operation completed successfully.
  @retval Others             Unable to log Spec ID event in TCG Event log.
**/
RETURN_STATUS
TpmLogSpecIDEvent (
  IN UINT32 ActivePcr
  );


/**
  Log Startup Locality event in TCG event log.

  @param[in] StartupLocality  Locality from which TPM_Startup was performed.
**/
VOID
TpmLogLocalityEvent (
  IN UINT8 StartupLocality,
  IN UINT32 ActivePcrBank
  );

#endif //_TPM_EVENT_LIB_H_
