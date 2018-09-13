/** @file
  TPM Event Log library routines to provide TCG Event Log support.
  For more details, consult 'Event Logging' chapter in TCG  PC Client
  Platform Firmware Profile  specification.

  Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php.

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

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
  IN UINT8 StartupLocality
  );

#endif //_TPM_EVENT_LIB_H_
