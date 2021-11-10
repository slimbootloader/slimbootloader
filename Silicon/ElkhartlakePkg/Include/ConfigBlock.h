/** @file
  Si Config Block

  Copyright (c) 2020 Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent
**/

typedef enum {
  ProbeTypeDisabled    = 0x00,
  ProbeTypeDciOob      = 0x02,
  ProbeTypeUsb3Dbc     = 0x03,
  ProbeTypeXdp3        = 0x04,
  ProbeTypeUsb2Dbc     = 0x05,
  ProbeType2WireDciOob = 0x06,
  ProbeTypeManual      = 0x07,
  ProbeTypeMax
} PLATFORM_DEBUG_CONSENT_PROBE_TYPE;

typedef enum {
  TraceHubModeDisabled       = 0,
  TraceHubModeTargetDebugger = 1,
  TraceHubModeHostDebugger   = 2,
  TraceHubModeMax
} TRACE_HUB_ENABLE_MODE;

typedef enum {
  DciDbcDisabled       = 0x0,
  DciDbcUsb2           = 0x1,
  DciDbcUsb3           = 0x2,
  DciDbcBoth           = 0x3,
  DciDbcNoChange       = 0x4,
  DciDbcMax
} DCI_DBC_MODE;

///
/// The TRACE_BUFFER_SIZE describes the desired TraceHub buffer size
///
typedef enum {
  TraceBufferNone,
  TraceBuffer1M,
  TraceBuffer8M       =2,
  TraceBuffer64M,
  TraceBuffer128M,
  TraceBuffer256M,
  TraceBuffer512M,
  TraceBufferMax
} TRACE_BUFFER_SIZE;
