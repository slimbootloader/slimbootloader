#!/usr/bin/env python

## @ CfgDataStitch.py
# Project template script to patch CFGDATA blob into IFWI image
#
# Copyright (c) 2018 - 2021, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys
import argparse
import subprocess

dlt_files = [] # TO BE PATCHED

def get_script_dir():
    return os.path.abspath(os.path.dirname(__file__))


def run_cmd(cmd_list):
    sys.stdout.flush()
    print (' '.join(cmd_list))
    ret = subprocess.call(cmd_list)
    if ret:
        sys.exit(1)


def check_file_exist (chk_files):
    for dir, files in chk_files:
        for file in files:
            path = os.path.join(dir, file)
            if not os.path.exists(path):
                return path
    return ''


def cfgdata_stitch(ifwi_file, ifwi_out_file, cfg_dir, key_file, script_dir, tool_dir, platform_id):
    if len (dlt_files) == 0:
        raise Exception("Please run the generated CfgDataStitch.py script instead of the original one in source tree !")

    fv_dir = get_script_dir()
    if not os.path.exists(fv_dir + '/ImgStitch.txt'):
        fv_dir = ''

    if ifwi_out_file == '':
        ifwi_out_file = os.path.join ('Output', os.path.basename(ifwi_file))

    out_dir = os.path.dirname(ifwi_out_file)
    if out_dir == '':
        out_dir = '.'

    if tool_dir == '':
        if fv_dir:
            tool_dir = os.path.realpath(os.path.join(fv_dir, '../../../../BaseTools/Bin/Win32'))
        else:
            tool_dir = get_script_dir()

    if script_dir == '':
        if fv_dir:
            script_dir = os.path.realpath(os.path.join(fv_dir, '../../../../BootloaderCorePkg/Tools'))
        else:
            script_dir = get_script_dir()

    if key_file == '':
        raise Exception("Key file is not specified!!")

    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    if cfg_dir == "":
        dlt_dir = out_dir
        cfg_dir = '.'
    else:
        dlt_dir = cfg_dir

    cfg_ext = 'yaml'
    # CfgDataDef needs to be under cfgdata_dir
    if not os.path.exists(os.path.join(cfg_dir ,'CfgDataDef.' + cfg_ext)):
        raise Exception("No CfgDataDef.%s file found under directory '%s' !" % (cfg_ext, cfg_dir))

    # ensure all required files exist
    chk_files = [
            (script_dir, ['GenCfgData.py', 'CfgDataTool.py']),
            (cfg_dir, ['CfgDataDef.' + cfg_ext]),
        ]
    # Check for KEY_ID in key file string
    if not key_file.startswith('KEY_ID'):
         chk_files.extend([(os.path.dirname(key_file), [os.path.basename(key_file)])])

    result = check_file_exist (chk_files)
    if result:
        raise Exception("Cannot find file '%s' !" % result)

    chk_files = [
        (dlt_dir, [file[1] for file in dlt_files]),
        (out_dir, ['CfgDataInt.bin'])
    ]

    result = check_file_exist (chk_files)
    if result:
        # generate all required bins and dlts
        name_id = []
        for pid, dlt in dlt_files:
          name_id.append('%d:%s' % (pid, os.path.splitext(dlt)[0]))
        name_str = ','.join(name_id)
        run_cmd([sys.executable, os.path.join(script_dir, 'CfgDataTool.py'), 'export',
                '-i', ifwi_file, '-t', tool_dir, '-o', out_dir, '-n', name_str])

        for pid, dlt_name in dlt_files:
          bin_name = os.path.splitext(dlt_name)[0] + '.bin'
          dlt_path = os.path.join(dlt_dir, dlt_name)
          bin_path = os.path.join(out_dir, bin_name)
          run_cmd([sys.executable, os.path.join(script_dir, 'GenCfgData.py'), 'GENDLT',
                 os.path.join(cfg_dir, 'CfgDataDef.%s;%s' % (cfg_ext, bin_path)), dlt_path])

    # generate indivisual CFGDATA for each board
    bin_files = []
    for pid, dlt in dlt_files:
        bin_file = os.path.splitext(dlt)[0] + '.bin'
        bin_file = os.path.join(out_dir, bin_file)
        run_cmd([sys.executable, os.path.join(script_dir, 'GenCfgData.py'), 'GENBIN',
                 os.path.join(cfg_dir, 'CfgDataDef.%s;') % cfg_ext + os.path.join(
                     dlt_dir, dlt), bin_file])
        bin_files.append(bin_file)

    # merge the CFGDATA
    cmd_list = [sys.executable, os.path.join(script_dir, 'CfgDataTool.py'), 'merge',
                '-o', os.path.join(out_dir, 'CfgDataExt.bin')]
    if platform_id:
        cmd_list.extend(['-p', platform_id])
    cmd_list.append(os.path.join(out_dir, 'CfgDataInt.bin*'))
    cmd_list.extend(bin_files)
    run_cmd(cmd_list)

    # sign the CFGDATA
    cmd_list = [sys.executable, os.path.join(script_dir, 'CfgDataTool.py'), 'sign',
                '-o', os.path.join(out_dir, 'CfgData.bin')]
    cmd_list.extend(['-k', key_file,
                     os.path.join(out_dir, 'CfgDataExt.bin')])
    run_cmd(cmd_list)

    # replace the CFGDATA in IFWI
    cmd_list = [sys.executable, os.path.join(script_dir, 'CfgDataTool.py'), 'replace',
                '-i', ifwi_file, os.path.join(out_dir, 'CfgData.bin')]
    cmd_list.extend(['-o', os.path.join(out_dir, os.path.basename(ifwi_out_file))])
    run_cmd(cmd_list)

    # clean intermediate files
    for each in ['CfgDataExt.bin', 'CfgData.bin', 'Stage1b.fd', 'Stage1b.lz', 'CfgData.bin.hash', 'CfgData.bin.hash.tmp']:
        bin_files.append (os.path.join(out_dir, each))
    for file in bin_files:
        if os.path.exists(file):
            os.remove(file)

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
                    '--key-file',
                    dest='key_file',
                    type=str,
                    default='',
                    help='Key Id or Signing key path')
    ap.add_argument('-s',
                    '--script-dir',
                    dest='script_dir',
                    type=str,
                    default='',
                    help='Scripts directory path')
    ap.add_argument('-t',
                    '--tool-dir',
                    dest='tool_dir',
                    type=str,
                    default='',
                    help='Compress binary tools directory path')
    ap.add_argument('-p',
                    '--platform-id',
                    dest='platform_id',
                    type=str,
                    default='',
                    help='Force a given platform ID')

    args = ap.parse_args()

    cfgdata_stitch(args.ifwi_image, args.output_file, args.cfgdata_dir,
                   args.key_file, args.script_dir, args.tool_dir, args.platform_id)


if __name__ == '__main__':
    sys.exit(main())
