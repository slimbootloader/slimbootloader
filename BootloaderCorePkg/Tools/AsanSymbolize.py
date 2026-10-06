## @ AsanSymbolize.py
#  Add file:line to the Name+0x... addresses in an SBL AddressSanitizer report.
#
#  Copyright (c) 2026, Intel Corporation. All rights reserved.<BR>
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import argparse
import glob
import os
import re
import shutil
import subprocess
import sys

PC_RE = re.compile(r'\((\w+)\+0x([0-9a-fA-F]+)\)')
BACKTRACE_RE = re.compile(r'#\d+ ')


def find_image(sbl_dir, name):
    # Older reports print "image+0x..." for OsLoader.
    if name == 'image':
        name = 'OsLoader'
    pattern = os.path.join(sbl_dir, 'Build', 'BootloaderCorePkg', '*', '*', '**', name, 'DEBUG', name + '.dll')
    images = glob.glob(pattern, recursive=True)
    return max(images, key=os.path.getmtime) if images else None


def find_symbolizer():
    for candidate in [os.path.join(os.environ.get('CLANG_BIN', ''), 'llvm-symbolizer'),
                      shutil.which('llvm-symbolizer'),
                      r'C:\Program Files\LLVM\bin\llvm-symbolizer.exe']:
        if candidate and (os.path.isfile(candidate) or os.path.isfile(candidate + '.exe')):
            return candidate
    return None


def symbolize(symbolizer, image, offsets):
    # The image is linked at /BASE:0, so its offsets are also its virtual addresses.
    query = '\n'.join('0x%x' % offset for offset in offsets) + '\n'
    out = subprocess.run([symbolizer, '--obj=' + image, '--no-inlines', '--relative-address'],
                         input=query, capture_output=True, text=True, check=True).stdout
    blocks = [block.splitlines() for block in out.strip().split('\n\n')]
    return {offset: '%s %s' % (block[0], block[1]) if len(block) > 1 else '??'
            for offset, block in zip(offsets, blocks)}


def main():
    default_sbl = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
    parser = argparse.ArgumentParser(description=__doc__ or 'Symbolize an SBL ASAN report')
    parser.add_argument('log', nargs='?', help='serial log file (default: stdin)')
    parser.add_argument('-i', '--image', action='append', default=[], metavar='NAME=PATH',
                        help='linked image for a name in the report, such as OsLoader=path\\OsLoader.dll '
                             '(default: newest NAME.dll under Build); repeat for more images')
    parser.add_argument('-s', '--symbolizer', help='path to llvm-symbolizer')
    args = parser.parse_args()

    images = {}
    for item in args.image:
        name, sep, path = item.partition('=')
        if not sep:
            name, path = 'OsLoader', item
        images[name] = path
    images.setdefault('image', images.get('OsLoader'))

    symbolizer = args.symbolizer or find_symbolizer()
    if not symbolizer:
        sys.exit('Could not find llvm-symbolizer, pass it with --symbolizer')

    if args.log is None and sys.stdin.isatty():
        sys.exit('Pass the serial log file, or pipe it to stdin')
    with open(args.log, errors='replace') if args.log else sys.stdin as f:
        lines = f.read().splitlines()

    # Backtrace entries are return addresses, so look up the call instruction just before them.
    queries = {}
    for line in lines:
        match = PC_RE.search(line)
        if match:
            offset = int(match.group(2), 16)
            queries[line] = (match.group(1), offset - 1 if BACKTRACE_RE.search(line) else offset)

    if not queries:
        print('No "(Name+0x...)" addresses found in the log', file=sys.stderr)

    names = {}
    for name in sorted(set(name for name, _ in queries.values())):
        image = images.get(name) or find_image(default_sbl, name)
        if not image or not os.path.isfile(image):
            print('Could not find the %s image, pass it with --image %s=PATH' % (name, name), file=sys.stderr)
            continue
        # The image must be the one that produced the log, so say which one is used.
        print('Symbolizing %s with %s' % (name, image), file=sys.stderr)
        offsets = sorted(set(offset for n, offset in queries.values() if n == name))
        for offset, text in symbolize(symbolizer, image, offsets).items():
            names[(name, offset)] = text

    for line in lines:
        print('%s in %s' % (line, names[queries[line]]) if line in queries and queries[line] in names else line)


if __name__ == '__main__':
    main()
