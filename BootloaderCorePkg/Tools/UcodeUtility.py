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

def build_ucode_bin(slot_size, reg_size, input_file_names, output_file_name):
    full_reg_bin = bytearray()

    # Pad individual uCode binaries, add to combined uCode binary
    for input_file_name in input_file_names:
        with open(input_file_name, 'rb') as input_file:
            current_slot_bin = bytearray(input_file.read())
        pad_item(current_slot_bin, slot_size)
        full_reg_bin.extend(current_slot_bin)

    # Pad combined uCode binary
    pad_item(full_reg_bin, reg_size)
    with open(output_file_name, 'wb') as output_file:
        output_file.write(full_reg_bin)

def auto_int(x):
    # Have interpreter figure out int base, allows for hex
    return int(x, 0)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('-s', '--slot-size', type=auto_int, required=True, help='Specify the ucode slot size (in bytes).')
    ap.add_argument('-r', '--reg-size', type=auto_int, required=True, help='Specify the ucode region size (in bytes).')
    ap.add_argument('-i', '--input_file_names', nargs='+', type=str, required=True, help='Specify the ucode file names (*.pdb files).')
    ap.add_argument('-o', '--output_file_name', type=str, required=True, help='Specify an output file name.')
    args = ap.parse_args()

    build_ucode_bin(args.slot_size, args.reg_size, args.input_file_names, args.output_file_name)

if __name__ == '__main__':
    sys.exit(main())
