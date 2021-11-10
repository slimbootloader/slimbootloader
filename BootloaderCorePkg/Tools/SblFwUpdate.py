## @ SblFwUpdate.py
#
# Copyright (c) 2017-2020, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
import os
import re
import sys
import shutil
import subprocess

def Usage():
    print ("SblFwUpdate")
    print ("Usage:")
    print ("    SblFwUpdate  <Input Capsule File>")

def Main():
    #
    # Parse the options and args
    #
    argc = len(sys.argv)
    if argc != 2:
        Usage()
        return 1

    if not os.path.exists(sys.argv[1]) :
        raise Exception ('%s file not found !' % sys.argv[1])

    if not os.path.exists('/boot/efi/'):
        raise Exception ('/boot/efi/ path not found !')

    shutil.copy(sys.argv[1], '/boot/efi/FwuImage.bin')
    fwucmd = "/sys/bus/wmi/devices/44FADEB1-B204-40F2-8581-394BBDC1B651/firmware_update_request"
    ofile = open(fwucmd, "wb")
    change=subprocess.Popen(["echo","1"], stdout=ofile)
    ofile.close()
    output = subprocess.check_output(['cat',fwucmd]).decode()
    if output[0] is not '1':
        raise Exception ('Triggering firmware update failed !')

    return 0

if __name__ == '__main__':
    sys.exit(Main())
