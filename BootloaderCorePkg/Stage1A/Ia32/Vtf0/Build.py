## @file
#  Automate the process of building the various reset vector types
#
#  Copyright (c) 2009 - 2017, Intel Corporation. All rights reserved.<BR>
#
#  SPDX-License-Identifier: BSD-2-Clause-Patent
#

import glob
import os
import subprocess
import sys

def RunCommand(commandLine):
    return subprocess.call(commandLine)

def FixupForRawSection(sectionFile):
    d = open(sectionFile, 'rb').read()
    c = ((len(d) + 4 + 7) & ~7) - 4
    if c > len(d):
        c -= len(d)
        f = open(sectionFile, 'wb')
        f.write(b'\x90' * c)
        f.write(d)
        f.close()

for filename in glob.glob(os.path.join('Bin', '*.raw')):
    os.remove(filename)

# It takes one argument to indicate build arch 'ia32' or 'x64'
if len(sys.argv) < 2:
    arch = 'ia32'
else:
    arch = sys.argv[1]
debugType = None
output = os.path.join('Bin', 'ResetVector')
output += '.' + arch
if debugType is not None:
    output += '.' + debugType
output += '.raw'
nasmpath = os.path.join(os.environ.get ('NASM_PREFIX', ''), 'nasm')
commandLine = (
    nasmpath,
    '-D', 'ARCH_%s' % arch.upper(),
    '-D', 'DEBUG_%s' % str(debugType).upper(),
    '-o', output,
    'Vtf0.nasmb',
    )
ret = RunCommand(commandLine)
print ('\tASM\t' + output)
if ret != 0: sys.exit(ret)

FixupForRawSection(output)
print ('\tFIXUP\t' + output)


