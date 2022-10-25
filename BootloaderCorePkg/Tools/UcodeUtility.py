#!/usr/bin/env python
## @ UcodeUtility.py
# Script to perform microcode operations
#
# Copyright (c) 2022, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent


##
# Import Modules
#
import sys
import argparse

def pad_item(bin, size):
    bin.extend(b'\xff' * (size - len(bin)))

def align_up(val, align):
    return (((val) + ((align) - 1)) & ~((align) - 1))

def auto_int(x):
    # Have interpreter figure out int base, allows for hex
    return int(x, 0)

def build_ucode_bin(input_file_names, output_file_name, slot_size):
    full_reg_bin = bytearray()

    # Pad individual uCode binaries and add them to combined uCode binary
    for input_file_name in input_file_names:
        with open(input_file_name, 'rb') as input_file:
            current_slot_bin = bytearray(input_file.read())
        if len(current_slot_bin) > slot_size:
            raise Exception('Individual uCode binary exceeds uCode slot size!')
        pad_item(current_slot_bin, slot_size)
        full_reg_bin.extend(current_slot_bin)


    # Pad combined uCode binary so that region size is a multiple of 4KB
    reg_size = align_up(len(full_reg_bin), 0x1000)
    pad_item(full_reg_bin, reg_size)

    # Write the result to a file
    with open(output_file_name, 'wb') as output_file:
        output_file.write(full_reg_bin)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('-s', '--slot-size', type=auto_int, required=True, help='Specify the ucode slot size (in bytes).')
    ap.add_argument('-i', '--input_file_names', nargs='+', type=str, required=True, help='Specify the ucode file names (*.pdb files).')
    ap.add_argument('-o', '--output_file_name', type=str, required=True, help='Specify an output file name.')
    args = ap.parse_args()

    build_ucode_bin(args.input_file_names, args.output_file_name, args.slot_size)

if __name__ == '__main__':
    sys.exit(main())
