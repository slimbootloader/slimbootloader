## @ CorruptComponentUtility.py
#  This is a python IFWI/SBL component corruption utility.
#
#  It supports three corruption backends:
#    * SBL/IFWI component corruption (legacy)
#        Corrupts a component located by its IFWI path (e.g. IFWI/BIOS/TS0/SG1A)
#        by flipping a random non-padding byte.
#    * CSME (CSE) code region corruption
#        Automates the manual Intel Image Analyzer (IIA) flow of navigating into
#        the CSE region, into a logical boot partition / sub-partition (e.g. PCHC)
#        and corrupting a resolved target such as a manifest header, using a
#        deterministic, reproducible corruption rule.
#    * CSME (CSE) data region corruption
#        Locates the ME data partition (MFS) via the $FPT flash partition table
#        and overwrites its first bytes to exercise the CSME data-recovery flow.
#
# Copyright (c) 2022, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

from CommonUtility import *
from IfwiUtility import *

from ctypes import Structure, c_char, c_uint32, sizeof, ARRAY

import os
import random
import argparse
import sys

TOOL_VERSION = '1.2.0'

#
# CSE / CSME parsing constants and structures
#
CPD_SIGNATURE  = b'$CPD'
MANIFEST_TAG   = b'$MN2'
MANIFEST_TAG_OFFSET = 0x1C          # offset of the '$MN2' tag inside a manifest header

# Default corruption window for a manifest header target (bytes).
DEFAULT_CSME_LENGTH = 16


class MANIFEST_HEADER(Structure):
    _pack_ = 1
    _fields_ = [
        ('header_type',    c_uint32),
        ('header_length',  c_uint32),          # header size in dwords
        ('header_version', c_uint32),
        ('flags',          c_uint32),
        ('vendor',         c_uint32),
        ('date',           c_uint32),
        ('size',           c_uint32),          # total manifest size in dwords
        ('header_id',      ARRAY(c_char, 4)),  # '$MN2'
    ]


#
# CSME data region ($FPT / MFS) corruption constants
#
FPT_SIGNATURE            = b'$FPT'
DEFAULT_CSME_DATA_PART   = 'MFS'    # ME data partition holding the file system
DEFAULT_CSME_DATA_LENGTH = 4        # validation corrupts the first 4 bytes


CORRUPTION_MODES = ('ff', '00')


def apply_corruption(data, mode):
    '''Overwrite the target byte range with a fixed corruption value (0xFF or 0x00).'''
    n = len(data)
    if n == 0:
        raise ValueError('Cannot corrupt a zero-length target')

    if mode == 'ff':
        return bytearray(b'\xFF' * n)
    if mode == '00':
        return bytearray(n)

    raise ValueError("Unsupported corruption mode '%s'" % mode)


def get_cse_region_bounds(image_bin):
    '''Return (start, end) byte bounds of the CSE (TXE) region within the image.

    Falls back to the whole image when no SPI descriptor / TXE region is present.
    '''
    try:
        if len(image_bin) >= sizeof(SPI_DESCRIPTOR):
            spi_desc = SPI_DESCRIPTOR.from_buffer(image_bin, 0)
            if spi_desc.fl_val_sig == spi_desc.DESC_SIGNATURE:
                base, limit = IFWI_PARSER.find_ifwi_region(spi_desc, 'txe')
                if base is not None:
                    return base, limit + 1
    except Exception:
        pass
    return 0, len(image_bin)


def scan_cpd_directories(image_bin, start, end):
    '''Scan the given region for $CPD sub-partition directories.

    Returns a list (in image order) of dicts:
        { 'name': <sub-partition name>,
          'cpd_offset': <absolute offset of $CPD>,
          'entries': [ (entry_name, entry_offset, entry_size), ... ] }
    '''
    results = []
    idx = start
    while True:
        pos = image_bin.find(CPD_SIGNATURE, idx, end)
        if pos < 0:
            break
        idx = pos + 4
        if pos + sizeof(SUBPART_DIR_HEADER) > end:
            continue
        hdr = SUBPART_DIR_HEADER.from_buffer(
            bytearray(image_bin[pos:pos + sizeof(SUBPART_DIR_HEADER)]))
        if hdr.header_marker != CPD_SIGNATURE:
            continue
        if not (0 < hdr.num_of_entries < 256):
            continue

        # The $CPD header carries its own size in 'header_length'. Newer (v2)
        # directories append a 4-byte CRC32, making the header larger than the
        # fixed SUBPART_DIR_HEADER struct. Trust header_length to locate the
        # entry array so entries are not read a few bytes early.
        entries = []
        header_size = hdr.header_length
        if header_size < sizeof(SUBPART_DIR_HEADER):
            header_size = sizeof(SUBPART_DIR_HEADER)
        eidx = pos + header_size
        valid = True
        for _ in range(hdr.num_of_entries):
            if eidx + sizeof(SUBPART_DIR_ENTRY) > end:
                valid = False
                break
            ent = SUBPART_DIR_ENTRY.from_buffer(
                bytearray(image_bin[eidx:eidx + sizeof(SUBPART_DIR_ENTRY)]))
            name = ent.entry_name.decode('utf-8', 'ignore').rstrip('\x00')
            entries.append((name, ent.entry_offset, ent.entry_size))
            eidx += sizeof(SUBPART_DIR_ENTRY)
        if not valid:
            continue

        results.append({
            'name'      : hdr.sub_part_name.decode('utf-8', 'ignore').rstrip('\x00'),
            'cpd_offset': pos,
            'entries'   : entries,
        })
    return results


def _parse_csme_target(target):
    '''Resolve a 'manifest_header:<PART>[:<lbp>]' target to (partition, lbp_index).'''
    parts = target.split(':')
    if len(parts) >= 2 and parts[0].lower() == 'manifest_header':
        partition = parts[1]
        lbp_index = 0
        if len(parts) >= 3:
            lbp_txt = parts[2].lower().lstrip('lbp')
            if not lbp_txt.isdigit():
                raise ValueError("Invalid LBP selector in target '%s'" % target)
            lbp_index = max(0, int(lbp_txt) - 1)
        return partition, lbp_index

    raise ValueError(
        "Unknown target '%s'. Use the form 'manifest_header:<PART>[:<lbp>]'." % target)


def resolve_csme_target(image_bin, target):
    '''Resolve a CSME target name to an exact manifest-header byte range.

    Returns a dict describing the resolved target, or raises RuntimeError with an
    actionable message when the target cannot be resolved.
    '''
    partition, lbp_index = _parse_csme_target(target)

    start, end = get_cse_region_bounds(image_bin)
    cpds = scan_cpd_directories(image_bin, start, end)
    if not cpds:
        raise RuntimeError(
            'No CSE $CPD sub-partitions were found in the provided image')

    matches = [c for c in cpds if c['name'].upper() == partition.upper()]
    if not matches:
        raise RuntimeError(
            "Sub-partition '%s' could not be resolved in the CSE region of the "
            "provided IFWI" % partition)
    if lbp_index >= len(matches):
        raise RuntimeError(
            "Requested logical boot partition #%d for '%s' but only %d "
            "occurrence(s) were found" % (lbp_index + 1, partition, len(matches)))

    cpd = matches[lbp_index]

    man_entry = None
    for name, entry_offset, entry_size in cpd['entries']:
        if name.lower().endswith('.man'):
            man_entry = (name, entry_offset, entry_size)
            break
    if man_entry is None:
        raise RuntimeError(
            "No '.man' manifest entry found in sub-partition '%s'" % partition)

    entry_name, entry_offset, entry_size = man_entry
    man_offset = cpd['cpd_offset'] + entry_offset

    man_length = entry_size
    validated  = False
    if man_offset + sizeof(MANIFEST_HEADER) <= len(image_bin):
        man_hdr = MANIFEST_HEADER.from_buffer(
            bytearray(image_bin[man_offset:man_offset + sizeof(MANIFEST_HEADER)]))
        if man_hdr.header_id == MANIFEST_TAG:
            validated  = True
            man_length = man_hdr.header_length * 4

    return {
        'target'       : target,
        'partition'    : partition,
        'lbp_index'    : lbp_index,
        'entry_name'   : entry_name,
        'cpd_offset'   : cpd['cpd_offset'],
        'base_offset'  : man_offset,
        'header_length': man_length,
        'validated'    : validated,
    }


def corrupt_csme(input_image, output_image, target, mode='ff',
                 length=DEFAULT_CSME_LENGTH):
    '''Resolve a CSME target and overwrite it with a fixed corruption value.'''
    image_bin = bytearray(get_file_data(input_image))

    info = resolve_csme_target(image_bin, target)

    patch_start = info['base_offset']
    patch_end   = patch_start + length

    if length <= 0:
        raise RuntimeError('Corruption length must be positive')
    if patch_end > len(image_bin):
        raise RuntimeError(
            'Resolved target (0x%08X-0x%08X) exceeds image length 0x%08X'
            % (patch_start, patch_end, len(image_bin)))

    original = bytearray(image_bin[patch_start:patch_end])
    image_bin[patch_start:patch_end] = apply_corruption(original, mode)
    gen_file_from_object(output_image, image_bin)

    print('Corrupted %s (%s) at 0x%08X-0x%08X with 0x%s -> %s'
          % (info['partition'], info['entry_name'], patch_start, patch_end,
             mode, output_image))
    return 0


def resolve_csme_data_target(image_bin, partition=DEFAULT_CSME_DATA_PART):
    '''Resolve a CSME data partition (default MFS) via the $FPT to an absolute offset.

    $FPT layout: header_length at +0x0A, num_entries at +0x04; each 0x20-byte
    entry holds the 4-char name at +0x00, a $FPT-relative offset at +0x04 and
    the partition length at +0x08.
    '''
    start, end = get_cse_region_bounds(image_bin)

    fpt = image_bin.find(FPT_SIGNATURE, start, end)
    if fpt < 0:
        raise RuntimeError('No $FPT flash partition table found in the CSE region')

    num_entries   = int.from_bytes(image_bin[fpt + 4:fpt + 8], 'little')
    header_length = image_bin[fpt + 0x0A] or 0x20
    entry         = fpt + header_length
    for _ in range(num_entries):
        if entry + 0x20 > end:
            break
        name = image_bin[entry:entry + 4].decode('utf-8', 'ignore').rstrip('\x00')
        if name.upper() == partition.upper():
            rel_offset = int.from_bytes(image_bin[entry + 4:entry + 8], 'little')
            return fpt + rel_offset          # $FPT offsets are relative to $FPT
        entry += 0x20

    raise RuntimeError(
        "Data partition '%s' was not found in the $FPT of the provided IFWI" % partition)


def corrupt_csme_data(input_image, output_image, partition=DEFAULT_CSME_DATA_PART,
                      mode='00', length=DEFAULT_CSME_DATA_LENGTH):
    '''Resolve the CSME data partition and overwrite its first bytes.'''
    if length <= 0:
        raise RuntimeError('Corruption length must be positive')

    image_bin   = bytearray(get_file_data(input_image))
    patch_start = resolve_csme_data_target(image_bin, partition)
    patch_end   = patch_start + length
    if patch_end > len(image_bin):
        raise RuntimeError(
            'Resolved target (0x%08X-0x%08X) exceeds image length 0x%08X'
            % (patch_start, patch_end, len(image_bin)))

    original = bytearray(image_bin[patch_start:patch_end])
    image_bin[patch_start:patch_end] = apply_corruption(original, mode)
    gen_file_from_object(output_image, image_bin)

    print('Corrupted %s data at 0x%08X-0x%08X (was %s) with 0x%s -> %s'
          % (partition, patch_start, patch_end,
             ' '.join('%02X' % b for b in original), mode, output_image))
    return 0


def corrupt_component(input_image, output_image, component_path):
    '''Legacy SBL/IFWI component corruption: flip one random non-padding byte.'''
    image_bin = bytearray(get_file_data(input_image))
    comp_bin = bytearray()

    # Extract the component from the IFWI/BIOS
    status = IFWI_PARSER.extract_component(image_bin, comp_bin, component_path)
    if status:
        sys.exit(1)

    # Perturb some random byte in the component, ensuring it's not
    # padding/null and different from what's currently there
    comp_offset = random.randrange(0, len(comp_bin))
    while comp_bin[comp_offset] == 0xff or comp_bin[comp_offset] == 0x00:
        comp_offset = random.randrange(0, len(comp_bin))
    comp_bin[comp_offset] = random.choice([i for i in range(0, 0xff + 1) if i != comp_bin[comp_offset]])

    # Replace the component in the IFWI/BIOS
    status = IFWI_PARSER.replace_component(image_bin, comp_bin, component_path)
    if status:
        sys.exit(1)

    # Create a new IFWI/BIOS binary
    gen_file_from_object(output_image, image_bin)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='IFWI/SBL and CSME (CSE) region corruption utility')
    parser.add_argument('-i', '--input-image', dest='input_image', type=str,
                    required=True, help='Specify input IFWI/SBL image file path')
    parser.add_argument('-o', '--output-image', dest='output_image', type=str,
                    help='Specify output IFWI/SBL image file path')
    parser.add_argument('-c', '--component', dest='component', type=str,
                    choices=['sbl', 'csme', 'csme-data'], default='sbl',
                    help="Corruption backend: 'sbl' (legacy component path), "
                    "'csme' (code/manifest) or 'csme-data' (MFS data region)")

    # Legacy SBL/IFWI component corruption
    parser.add_argument('-p', '--path', dest='component_path', type=str,
                    help='Specify path of component to corrupt in IFWI/SBL binary '
                    '(e.g. IFWI/BIOS/TS0/SG1A for BP0 Stage 1A of IFWI binary, '
                    'use IfwiUtility.py to see all available paths). Used with -c sbl.')

    # CSME corruption options
    parser.add_argument('-t', '--target', dest='target', type=str,
                    help="CSME code target 'manifest_header:<PART>[:<lbp>]' (used with -c csme)")
    parser.add_argument('-m', '--mode', dest='mode', type=str,
                    choices=list(CORRUPTION_MODES), default=None,
                    help='Corruption value: ff or 00 '
                    '(default: ff for csme, 00 for csme-data)')
    parser.add_argument('-l', '--length', dest='length', type=lambda x: int(x, 0),
                    default=None,
                    help='Number of bytes to corrupt '
                    '(default: 16 for csme, 4 for csme-data)')

    args = parser.parse_args()

    if args.component == 'csme':
        if not args.target:
            parser.error("-c csme requires -t/--target")
        if not args.output_image:
            parser.error('-o/--output-image is required')
        length = args.length if args.length is not None else DEFAULT_CSME_LENGTH
        try:
            corrupt_csme(args.input_image, args.output_image, args.target,
                         mode=(args.mode or 'ff'), length=length)
        except (RuntimeError, ValueError) as err:
            print('Error: %s' % err)
            sys.exit(1)
    elif args.component == 'csme-data':
        if not args.output_image:
            parser.error('-o/--output-image is required')
        length = args.length if args.length is not None else DEFAULT_CSME_DATA_LENGTH
        try:
            corrupt_csme_data(args.input_image, args.output_image,
                              mode=(args.mode or '00'), length=length)
        except (RuntimeError, ValueError) as err:
            print('Error: %s' % err)
            sys.exit(1)
    else:
        if not args.component_path:
            parser.error('-c sbl requires -p/--path')
        if not args.output_image:
            parser.error('-o/--output-image is required')
        corrupt_component(args.input_image, args.output_image, args.component_path)
