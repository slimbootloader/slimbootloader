#!/usr/bin/env python

## @ CfgDataStitch.py
# Project template script to patch CFGDATA blob into IFWI image
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License that accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php.
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

import os
import sys
import argparse
import subprocess

dlt_files = [] # TO BE PATCHED

def get_tool_dir():
    return os.path.abspath(os.path.dirname(__file__))


def run_cmd(cmd_list):
    sys.stdout.flush()
    print ' '.join(cmd_list)
    ret = subprocess.call(cmd_list)
    if ret:
        sys.exit(1)


def cfgdata_stitch(ifwi_file, ifwi_out_file, cfg_dir, key_dir, tool_dir, pid):
    fv_dir = get_tool_dir()
    if not os.path.exists(fv_dir + '/ImgStitch.txt'):
        fv_dir = ''

    if ifwi_out_file == '':
        ifwi_out_file = ifwi_file

    out_dir = os.path.dirname(ifwi_out_file)

    if len (dlt_files) == 0:
        raise Exception("No DLT files found under directory '%s' !" % cfg_dir)

    if cfg_dir == '' and fv_dir:
        cfg_dir = fv_dir

    if tool_dir == '':
        if fv_dir:
            tool_dir = os.path.realpath(os.path.join(fv_dir, '../../../../BootloaderCorePkg/Tools'))
        else:
            tool_dir = get_tool_dir()

    if key_dir == '':
        if fv_dir:
            key_dir = os.path.realpath(os.path.join(fv_dir, '../../../../BootloaderCorePkg/Tools/Keys'))
        else:
            key_dir = get_tool_dir()

    if os.path.isdir(key_dir):
        key_file = os.path.join(key_dir, 'TestSigningPrivateKey.pem')
    else:
        key_file = key_dir

    if os.name == 'nt' and 'OPENSSL_PATH' not in os.environ:
        os.environ['OPENSSL_PATH'] = "C:\\Openssl\\"

    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    # ensure all required files exist
    chk_files = [
        (tool_dir, ['GenCfgData.py', 'CfgDataTool.py']),
        (os.path.dirname(key_file), [os.path.basename(key_file)]),
        (cfg_dir, dlt_files),
        (cfg_dir, ['CfgDataDef.dsc', 'CfgDataInt.bin']),
    ]

    for dir, files in chk_files:
        for file in files:
            path = os.path.join(dir, file)
            if not os.path.exists(path):
                raise Exception("Cannot find file '%s' !" % path)

    # generate indivisual CFGDATA for each board
    bin_files = []
    for dlt in dlt_files:
        bin_file = os.path.splitext(dlt)[0] + '.bin'
        bin_file = os.path.join(out_dir, bin_file)
        run_cmd(['python', os.path.join(tool_dir, 'GenCfgData.py'), 'GENBIN',
                 os.path.join(cfg_dir, 'CfgDataDef.dsc;') + os.path.join(
                     cfg_dir, dlt), bin_file])
        bin_files.append(bin_file)

    # merge the CFGDATA
    cmd_list = ['python', os.path.join(tool_dir, 'CfgDataTool.py'), 'merge',
                '-o', os.path.join(out_dir, 'CfgDataExt.bin'), '-p', pid,
                os.path.join(cfg_dir, 'CfgDataInt.bin*')]
    cmd_list.extend(bin_files)
    run_cmd(cmd_list)

    # sign the CFGDATA
    cmd_list = ['python', os.path.join(tool_dir, 'CfgDataTool.py'), 'sign',
                '-o', os.path.join(out_dir, 'CfgData.bin')]
    cmd_list.extend(['-k', key_file,
                     os.path.join(out_dir, 'CfgDataExt.bin')])
    run_cmd(cmd_list)

    # replace the CFGDATA in IFWI
    cmd_list = ['python', os.path.join(tool_dir, 'CfgDataTool.py'), 'replace',
                '-i', ifwi_file, os.path.join(out_dir, 'CfgData.bin')]
    cmd_list.extend(['-o', os.path.join(out_dir, os.path.basename(ifwi_file))])
    run_cmd(cmd_list)

    # clean intermediate files
    for file in bin_files:
        os.remove(file)
    os.remove(os.path.join(out_dir, 'CfgDataExt.bin'))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('-i',
                    '--ifwi-image',
                    dest='ifwi_image',
                    type=str,
                    required=True,
                    help='Specify input IFWI image')
    ap.add_argument('-o',
                    '--output-file',
                    dest='output_file',
                    type=str,
                    default='',
                    help='Specify output IFWI image')
    ap.add_argument('-c',
                    '--cfgdata-dir',
                    dest='cfgdata_dir',
                    type=str,
                    default='',
                    help='CFGDATA directory path')
    ap.add_argument('-k',
                    '--key-dir',
                    dest='key_dir',
                    type=str,
                    default='',
                    help='Signing key directory path')
    ap.add_argument('-t',
                    '--script-dir',
                    dest='script_dir',
                    type=str,
                    default='',
                    help='Scripts directory path')
    ap.add_argument('-p',
                    '--platform-id',
                    dest='platform_id',
                    type=str,
                    default='',
                    help='Force a given platform ID')

    args = ap.parse_args()

    cfgdata_stitch(args.ifwi_image, args.output_file, args.cfgdata_dir,
                   args.key_dir, args.script_dir, args.platform_id)


if __name__ == '__main__':
    sys.exit(main())
