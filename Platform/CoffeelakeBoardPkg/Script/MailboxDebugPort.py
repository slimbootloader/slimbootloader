#!/usr/bin/env python3
## @ MailboxDebugPort.py
#
# Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import sys
import ipccli
import struct
import msvcrt

class MAILBOX_COMM:
    def __init__(self):
        self.ipc  = ipccli.baseaccess()
        self.portid    = 0x6e
        self.offset    = 0x120
        self.iosf_acc  = True
        self.flow_ctrl = True
        self.last_read = self.mailbox_rw (0)

    def mailbox_rw (self, value = None):
        if value is None:
            if self.iosf_acc:
                value = self.ipc.stateport.cnp_tpsb0.tap2iosfsb32(self.portid, self.offset, 0, 0, 6, 0)
                value = value.ToUInt32 ()
            else:
                value = self.ipc.stateport.cnp_tpsb0.sbreg(bar=0, fid=0, offsetset=self.offset, portid=self.portid, typecode=6)
                value = value.ToUInt64 () & 0xffffffff
            return value
        else:
            if self.iosf_acc:
                self.ipc.stateport.cnp_tpsb0.tap2iosfsb32(self.portid, self.offset, 0, 0, 7, 0, newValue=value)
            else:
                self.ipc.stateport.cnp_tpsb0.sbreg(bar=0, fid=0, offsetset=self.offset, portid=self.portid, typecode=7, newValue = value)
            return value

    def read (self):
        data32 = self.mailbox_rw ()
        if self.last_read != data32:
            self.last_read = data32
            if data32 & 0x01:
                val = (data32 >> 8) & 0xff
                msg = chr(val)
                sys.stdout.write (msg)
                if val == 0x0D:
                    sys.stdout.flush()
                if self.flow_ctrl:
                    data32 = data32 & 0xFFFFFFFE
                    self.mailbox_rw (data32)
                    self.last_read = data32
                return True
        else:
            if msvcrt.kbhit():
                key   = ord(msvcrt.getch())
                if key == 0x1b:
                    return None
                data32 = ((data32 | 0x02) & 0xff00ffff) + (key << 16)
                self.mailbox_rw (data32)
            return False

        return False

def run ():
  # Display SBL debug message through mailbox debug port communication
  dbg_port = MAILBOX_COMM()
  while True:
      ret = dbg_port.read ()
      if ret is None:
          break
