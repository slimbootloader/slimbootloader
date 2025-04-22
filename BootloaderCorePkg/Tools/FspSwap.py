#!/usr/bin/env python3
## @ FspSwap.py
# This script swaps FSP binary from a SBL IFWI/BIOS image
#
# Copyright (c) 2021, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##


import os
import sys
import glob
import threading
import platform
import argparse
import hashlib
import subprocess
from   shutil import copy

from CommonUtility import *
from BuildUtility  import gen_hash_file, HashStoreData, HashStoreTable


script_dir = os.path.dirname(__file__)

sbl_stages = [
      ('IFWI/BIOS/TS0/SG1A', 'Stage1A', 't'),
      ('IFWI/BIOS/RD0/SG1B', 'Stage1B', 'm'),
      ('IFWI/BIOS/RD0/SG02', 'Stage2',  's')
    ]


def extract_comp (ifwi, comp_path, file):
    cmd = ['python', os.path.join(script_dir, 'IfwiUtility.py'), 'extract', '-i', ifwi, '-p', comp_path, '-o', file]
    run_process (cmd)


def decompress_comp (fsp, fspc, in_file, out_file, compress_tool_dir):
    data   = get_file_data(in_file)
    if data[0:4] == b'LZ4 ':
        fsp[fspc]['lz'] = 'Lz4'
    elif data[0:4] == b'LZMA':
        fsp[fspc]['lz'] = 'Lzma'
    else:
        fsp[fspc]['lz'] = ''
        shutil.copyfile (in_file, out_file)
        return

    decompress (in_file, out_file, compress_tool_dir)


def locate_fsp (fsp, fspc, stage_file):
    data   = get_file_data(stage_file)
    if fspc == 't':
        offset = 0
    else:
        offset = bytes_to_value (data[0x20:0x24])
    fsp[fspc]['data'] = bytearray(data)
    fsp[fspc]['offs'] = offset
    fsp[fspc]['base'] = bytes_to_value (data[offset+0xb0:offset+0xb4])
    fsp[fspc]['size'] = bytes_to_value (data[offset+0xac:offset+0xb0])


def swap_fsp (args):

    if os.name == 'nt':
        compress_tool_dir = os.path.realpath (os.path.join (script_dir, '../../BaseTools/Bin/Win32'))
    else:
        compress_tool_dir = os.path.realpath (os.path.join (script_dir, '../../BaseTools/BinWrappers/PosixLike'))

    # create output dir
    if not os.path.exists(args.out_dir):
        os.mkdir (args.out_dir)

    new_ifwi =  os.path.join (args.out_dir, os.path.basename(args.ifwi_image))
    shutil.copyfile (args.ifwi_image, new_ifwi)

    if args.non_redundant:
        part_num = 1
    else:
        part_num = 2

    for partition in range (part_num):

        print ('\nProcess Partition %d' % partition)
        print ('============================')


        fsp = {'t':{}, 'm':{}, 's':{}}
        print ('\nExtracting Stage1A/1B/2 ...')
        ifwi = args.ifwi_image
        for path, file, fspc in sbl_stages:
            if partition > 0:
                path = path.replace('0/SG', '1/SG')
            file = file + '_%s.bin' % partition
            extract_comp (ifwi, path, os.path.join(args.out_dir, file))


        print ('\nCollecting FSP info ...')
        for path, file, fspc in sbl_stages:
            if partition > 0:
                path = path.replace('0/SG', '1/SG')
            file = file + '_%s.bin' % partition
            src_file = os.path.join(args.out_dir, file)
            parts = os.path.splitext (src_file)
            dest_file = parts[0] + '.fd'
            decompress_comp (fsp, fspc, src_file, dest_file, compress_tool_dir)
            locate_fsp (fsp, fspc, dest_file)

        print ('FSP INFO:')
        for idx, fsp_comp in enumerate('tms'):
            lz = fsp[fsp_comp]['lz']
            if lz == '':
                lz = 'None'
            print ('  FSP-%s at 0x%08x in SBL %-7s (%s)' % (fsp_comp.upper(), fsp[fsp_comp]['base'], sbl_stages[idx][1], lz))

        fsp_rebase_name = 'FspRebase_%d.bin' % partition
        print ('\nRebasing FSP ...')
        fsp_tool_dir = os.path.realpath (os.path.join (script_dir, '../../IntelFsp2Pkg/Tools'))
        cmd = ['python', os.path.join(fsp_tool_dir, 'SplitFspBin.py'), 'rebase', '-f', args.fsp_bin,
               '-c', 't', 'm', 's', '-b',
               '0x%08x' % fsp['t']['base'],
               '0x%08x' % fsp['m']['base'],
               '0x%08x' % fsp['s']['base'],
               '-n', fsp_rebase_name,
               '-o', args.out_dir
              ]
        run_process (cmd)

        print ('\nSplitting FSP ...')
        cmd = ['python', os.path.join(fsp_tool_dir, 'SplitFspBin.py'), 'split',
               '-f', os.path.join (args.out_dir, fsp_rebase_name),
               '-n', fsp_rebase_name,
               '-o', args.out_dir
              ]
        run_process (cmd)

        print ('\nReplacing FSP ...')
        for idx, fsp_comp in enumerate('tms'):
            data = get_file_data(os.path.join (args.out_dir, '%s_%s.bin' % (os.path.splitext(fsp_rebase_name)[0], fsp_comp.upper())))
            base = bytes_to_value (data[0xb0:0xb4])
            size = bytes_to_value (data[0xac:0xb0])
            if base != fsp[fsp_comp]['base'] or size != fsp[fsp_comp]['size']:
                print ('New FSP-%s base: 0x%08x with size 0x%08x' % (fsp_comp.upper(), base, size))
                print ('Old FSP-%s base: 0x%08x with size 0x%08x' % (fsp_comp.upper(), fsp[fsp_comp]['base'], fsp[fsp_comp]['size']))
                raise Exception ('Unexpected FSP base or size for FSP-%s !' % fsp_comp.upper())
            offs = fsp[fsp_comp]['offs']
            fsp[fsp_comp]['data'][offs:offs+size] = data[0:size]
            print ('Replace FSP-%s at SBL %-7s offset 0x%x' % (fsp_comp.upper(), sbl_stages[idx][1], offs))

        print ('\nCompressing Stages ...')
        for path, file, fspc in sbl_stages:
            if partition > 0:
                path = path.replace('0/SG', '1/SG')
            base_name = os.path.join (args.out_dir, file + '_%s' % partition)
            if fsp[fspc]['lz']:
                inp_name = base_name + '_chg.fd'
                out_name = base_name + '.lz'
                gen_file_from_object (inp_name, fsp[fspc]['data'])
                print ('Compress SBL %-7s with %s' % (file, fsp[fspc]['lz']))
                compress (inp_name, fsp[fspc]['lz'], out_path = out_name, tool_dir = compress_tool_dir)
            else:
                gen_file_from_object (base_name + '.lz', fsp[fspc]['data'])

        print ('\nUpdating Hash Store ...')
        fsp_len = fsp['t']['size']
        data    = fsp['t']['data']
        hash_off = data[fsp_len:].find(b'_HS_')
        if hash_off < 0:
            raise Exception ('Failed to locate HASH store in Stage1A !')
        hash_off = hash_off + fsp_len
        hs = HashStoreTable.from_buffer(data, hash_off)

        offset = hash_off + hs.HeaderLength
        endoff = hash_off + hs.UsedLength
        while offset < endoff:
            entry = HashStoreData.from_buffer(data, offset)

            offset += sizeof(HashStoreData)
            if entry.Usage in [1, 2]:
                if entry.Usage == 1:
                    name = 'Stage1B'
                else:
                    name = 'Stage2'
                file_name = os.path.join (args.out_dir, name + '_%d' % partition + '.lz')
                new_hash  = gen_hash_file (file_name, HASH_VAL_STRING[entry.HashAlg])
                if len(new_hash) != entry.DigestLen:
                    raise Excpetion ('Unexpected hash size !')
                print ('Update SBL Hash Store for %s' % name)
                data[offset:offset+entry.DigestLen] = new_hash
            offset += entry.DigestLen
        base_name = os.path.join (args.out_dir, 'Stage1A_%s' % partition)
        gen_file_from_object (base_name + '.lz', fsp['t']['data'])

        print ('\nAssembling IFWI ...')
        for path, file, fspc in sbl_stages:
            if partition > 0:
                path = path.replace('0/SG', '1/SG')
            base_name = os.path.join (args.out_dir, file + '_%s' % partition)
            cmd = ['python', os.path.join(script_dir, 'IfwiUtility.py'), 'replace', '-i', new_ifwi, '-p', path, '-f', base_name + '.lz']
            run_process (cmd)

    print ('\nFSP has been swapped successfully !')
    print ('New IFWI image is generated at:')
    print ('  %s' % os.path.realpath(new_ifwi))


def main():
    parser     = argparse.ArgumentParser()
    parser.add_argument('-i',  '--ifwi'   , dest='ifwi_image',  type=str, help='IFWI image binary file path', required = True)
    parser.add_argument('-f',  '--fspbin' , dest='fsp_bin',     type=str, help='FSP binary file path',  required = True)
    parser.add_argument('-n',  '--non-redundant' , dest='non_redundant', action="store_true", help='Non-redundant flash map layout')
    parser.add_argument('-o',  '--outdir' , dest='out_dir',     type=str, help='Output directory path', default = 'Out')
    parser.set_defaults(func=swap_fsp)

    # Parse arguments and run sub-command
    args = parser.parse_args()
    try:
        func = args.func
    except AttributeError:
        parser.error("too few arguments")

    func(args)


if __name__ == '__main__':
    main()
