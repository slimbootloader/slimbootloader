#!/usr/bin/env python
## @ test_base.py
#
# Provide common functions for test script
#
# Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#

import os
import sys
import struct
import signal
import subprocess
import zipfile
import urllib.request
from   threading import Timer

def get_tool_dir (sbl_dir):
    if os.name == 'nt':
        return os.path.join(sbl_dir, 'BaseTools', 'Bin', 'Win32')
    else:
        return os.path.join(sbl_dir, 'BaseTools', 'BinWrappers', 'PosixLike')

def get_key_dir (sbl_dir):
    return os.path.realpath (os.path.join(sbl_dir, '..', 'SblKeys'))

def get_file_data (file, mode = 'rb'):
    return open(file, mode).read()

def gen_file_from_object (file, object, mode='b'):
    open (file, 'w' + mode).write(object)

def unzip_file (zip_file, tgt_dir):
    with zipfile.ZipFile(zip_file, 'r') as zip_ref:
        zip_ref.extractall (tgt_dir)


def download_url (url, save_path):
    urllib.request.urlretrieve (url, save_path)


def create_dirs (dirs):
    # create dirs
    for dir_name in dirs:
        if not os.path.exists(dir_name):
            os.mkdir (dir_name)


def run_qemu (bios_img, fwu_path, fwu_mode=False, boot_order='', timeout=0):
    if os.name == 'nt':
        path = r"C:\Program Files\qemu\qemu-system-x86_64"
    else:
        path = r"qemu-system-x86_64"
    cmd_list = [
        path, "-nographic",  "-machine", "q35,accel=tcg",
        "-cpu", "max", "-serial", "mon:stdio",
        "-m", "256M", "-drive",
        "id=mydrive,if=none,format=raw,file=fat:rw:%s" % fwu_path, "-device",
        "ide-hd,drive=mydrive", "-boot", "order=d%s" % ('an' if fwu_mode else boot_order),
        "-no-reboot", "-drive", "file=%s,if=pflash,format=raw" % bios_img
    ]

    lines = run_process (cmd_list, timeout)
    return lines


def run_command (arg_list):
    sys.stdout.flush()
    try:
        result = subprocess.call (arg_list)
    except Exception as ex:
        result = 1
    if result:
        print ('Error in running process:\n  %s' % ' '.join(arg_list))
    return result


def run_process (cmd, timeout = 0):
    def timerout (p):
        timer.cancel()
        os.kill(p.pid, signal.SIGTERM)

    lines = []
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, bufsize=1, universal_newlines=True)
    if timeout:
      timer = Timer(timeout, timerout, args=[p])
      timer.start()
    for line in iter(p.stdout.readline, ''):
        line = line.rstrip()
        print (line)
        lines.append (line)
    p.stdout.close()
    retcode = p.wait()
    if timeout:
        timer.cancel()

    return lines


def check_result (output, check_lines):
    ret   = 0
    index = 0
    count = len (output)
    for line in check_lines:
        found = False
        while not found and index < count:
            if line in output[index]:
                found = True
                break
            else:
                index += 1
        if found:
            index += 1
            continue
        else:
            print ("Failed locating '%s' !" % (line))
            ret = -1
            break
    return  ret
