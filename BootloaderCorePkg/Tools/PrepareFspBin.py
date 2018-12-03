## @ PrepareFspBin.py
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
import shutil
import subprocess

def Fatal (msg):
    sys.stdout.flush()
    raise Exception (msg)


def CloneFspRepo (fsp_dir):
    if not os.path.exists(fsp_dir + '/.git'):
        print ('Cloning Intel FSP repo ...')
        cmd = 'git clone https://github.com/IntelFsp/FSP.git %s' % fsp_dir
        ret = subprocess.call(cmd.split(' '))
        if ret:
            Fatal ('Failed to clone FSP repo to directory %s !' % fsp_dir)
        print ('Done\n')

    print ('Checking out Intel FSP commit ...')
    cmd = 'git checkout 7431e4f3399a5081c956753b5fa3bcd764196723'
    ret = subprocess.call(cmd.split(' '), cwd=fsp_dir)
    if ret:
        Fatal ('Failed to check out branch !')
    print ('Done\n')


def CheckFileListExist (copy_list, sbl_dir):
    exists = True
    for src_path, dst_path in copy_list:
        dst_path = os.path.join (sbl_dir, dst_path)
        if not os.path.exists(dst_path):
            exists = False
            break
    return exists

def CopyFileList (copy_list, fsp_dir, sbl_dir):
    print ('Copy FSP into Slim Bootloader source tree ...')
    for src_path, dst_path in copy_list:
        src_path = os.path.join (fsp_dir, src_path)
        dst_path = os.path.join (sbl_dir, dst_path)
        if os.path.exists(dst_path):
            print ('Keep:  %s' % os.path.abspath(dst_path))
            continue
        if not os.path.exists(os.path.dirname(dst_path)):
            os.makedirs(os.path.dirname(dst_path))
        print ('Copy:  %s\n  To:  %s' % (os.path.abspath(src_path), os.path.abspath(dst_path)))
        shutil.copy (src_path, dst_path)
    print ('Done\n')


def CopyFspBins (fsp_dir, sbl_dir, platform):
    sys.stdout.flush()
    copy_list = []
    if platform == 'apl':
        copy_list.extend ([
          ('ApolloLakeFspBinPkg/FspBin/Fsp.fd',  'Silicon/ApollolakePkg/FspBin/FspDbg.bin'),
          ('ApolloLakeFspBinPkg/FspBin/Fsp.fd',  'Silicon/ApollolakePkg/FspBin/FspRel.bin'),
          ('ApolloLakeFspBinPkg/FspBin/Fsp.bsf', 'Silicon/ApollolakePkg/FspBin/Fsp.bsf'),
          ('ApolloLakeFspBinPkg/Vbt/Vbt.bin',    'Platform/ApollolakeBoardPkg/VbtBin/Vbt.dat'),
          ('ApolloLakeFspBinPkg/Vbt/Vbt.bsf',    'Platform/ApollolakeBoardPkg/VbtBin/Vbt.bsf')
        ])

    if CheckFileListExist(copy_list, sbl_dir):
        return

    CloneFspRepo (fsp_dir)

    CopyFileList (copy_list, fsp_dir, sbl_dir)

    return 0


def BuildFspBins (fsp_dir, sbl_dir, platform, flag):
    sys.stdout.flush()

    copy_list = []
    if platform == 'qemu':
        copy_list.extend ([
          ('BuildFsp/QEMU_FSP.bsf',        'Silicon/QemuSocPkg/FspBin/Fsp.bsf'),
          ('BuildFsp/QEMU_FSP_DEBUG.fd',   'Silicon/QemuSocPkg/FspBin/FspDbg.bin'),
          ('BuildFsp/QEMU_FSP_RELEASE.fd', 'Silicon/QemuSocPkg/FspBin/FspRel.bin')
        ])
        if flag == '/r':
          del copy_list[1]
        elif flag == '/d':
          del copy_list[-1]
    else:
        return

    if CheckFileListExist(copy_list, sbl_dir):
        return

    edk2_base_tag = 'edk2-stable201811'
    print ('Building QEMU FSP binaries from EDKII repo (Base Tag: %s)' % edk2_base_tag)
    if not os.path.exists(fsp_dir + '/.git'):
        print ('Cloning EDKII repo ...')
        cmd = 'git clone https://github.com/tianocore/edk2.git %s' % fsp_dir
        ret = subprocess.call(cmd.split(' '))
        if ret:
            Fatal ('Failed to clone FSP repo to directory %s !' % fsp_dir)
        print ('Done\n')
    else:
        output = subprocess.check_output(['git', 'tag', '-l'], cwd=fsp_dir)
        if edk2_base_tag not in output:
            ret = subprocess.call(['git', 'fetch', '--all'], cwd=fsp_dir)
            if ret:
                Fatal ('Failed to fetch all tags !')

    print ('Checking out EDKII stable tag (%s)...' % edk2_base_tag)

    if os.path.exists(fsp_dir + '/BuildFsp.cmd'):
        os.remove (fsp_dir + '/BuildFsp.cmd')

    if os.path.exists(fsp_dir + '/BuildFsp.py'):
        os.remove (fsp_dir + '/BuildFsp.py')

    if os.path.exists(fsp_dir + '/QemuFspPkg'):
        shutil.rmtree(fsp_dir + '/QemuFspPkg')

    cmd = 'git checkout -f ' + edk2_base_tag
    ret = subprocess.call(cmd.split(' '), cwd=fsp_dir)
    if ret:
        Fatal ('Failed to check out branch !')
    print ('Done\n')

    print ('Applying QEMU FSP patch ...')
    patch_dir = os.path.join(sbl_dir, 'Silicon/QemuSocPkg/FspBin/Patches')
    cmd = 'git am --abort'
    with open(os.devnull, 'w') as fnull:
        ret = subprocess.call(cmd.split(' '), cwd=fsp_dir, stdout=fnull, stderr=subprocess.STDOUT)
    cmd = 'git am --whitespace=nowarn %s/0001-Build-QEMU-FSP-2.0-binaries.patch' % patch_dir
    ret = subprocess.call(cmd.split(' '), cwd=fsp_dir)
    if ret:
        Fatal ('Failed to apply QEMU FSP patch !')
    print ('Done\n')

    print ('Compiling QEMU FSP source ...')

    if flag == '':
        flags = ['/d', '/r']
    else:
        flags = [flag]

    print flags
    for flag in flags:
        os.environ['WORKSPACE'] = ''
        os.environ['EDK_TOOLS_PATH'] = ''
        os.environ['EDK_TOOLS_BIN'] = ''
        os.environ['CONF_PATH'] = ''
        if os.name == 'nt':
            ret = subprocess.call([os.path.join(fsp_dir, 'BuildFsp.cmd'), flag], cwd=fsp_dir)
        else:
            ret = subprocess.call(['python', os.path.join(fsp_dir, 'BuildFsp.py'), flag], cwd=fsp_dir)
        if ret:
            Fatal ('Failed to build QEMU FSP binary !')

    print ('Done\n')

    CopyFileList (copy_list, fsp_dir, sbl_dir)


def Main():
    curr_dir      = os.path.dirname (os.path.realpath(__file__))
    workspace_dir = os.path.abspath (os.path.join(curr_dir, '../../..'))
    fsp_repo_dir  = os.path.abspath (os.path.join(workspace_dir, 'IntelFsp'))
    qemu_repo_dir = os.path.abspath (os.path.join(workspace_dir, 'QemuFsp'))

    if len(sys.argv) < 3:
        print ('Platform argument is required!')
        return -1

    plt_dir   = sys.argv[1]
    platform  = sys.argv[2]

    target = ''
    if len(sys.argv) > 3:
        target = sys.argv[3]

    if platform == 'qemu':
        BuildFspBins (qemu_repo_dir, plt_dir, platform, target)
    else:
        CopyFspBins (fsp_repo_dir, plt_dir, platform)

    return 0


if __name__ == '__main__':
    sys.exit(Main())
