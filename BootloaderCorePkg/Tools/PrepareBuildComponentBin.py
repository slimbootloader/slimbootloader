## @ PrepareFspBin.py
#
# Copyright (c) 2018 - 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys
import re
import shutil
import subprocess
from   datetime import date

def Fatal (msg):
    sys.stdout.flush()
    raise Exception (msg)


def CloneFspRepo (fsp_dir, fsp_tag):
    if not os.path.exists(fsp_dir + '/.git'):
        print ('Cloning Intel FSP repo ...')
        cmd = 'git clone https://github.com/IntelFsp/FSP.git %s' % fsp_dir
        ret = subprocess.call(cmd.split(' '))
        if ret:
            Fatal ('Failed to clone FSP repo to directory %s !' % fsp_dir)
        print ('Done\n')
    else:
        print ('Update Intel FSP repo ...')
        cmd = 'git fetch origin master'
        ret = subprocess.call(cmd.split(' '), cwd=fsp_dir)
        if ret:
            Fatal ('Failed to update FSP repo to directory %s !' % fsp_dir)
        print ('Done\n')

    print ('Checking out Intel FSP commit ...')

    cmd = 'git checkout %s' % fsp_tag
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


def CopyFileList (copy_list, src_dir, sbl_dir):
    print ('Copy Files into Slim Bootloader source tree ...')
    for src_path, dst_path in copy_list:
        src_path = os.path.join (src_dir, src_path)
        dst_path = os.path.join (sbl_dir, dst_path)
        if os.path.exists(dst_path):
            print ('Keep:  %s' % os.path.abspath(dst_path))
            continue
        if not os.path.exists(os.path.dirname(dst_path)):
            os.makedirs(os.path.dirname(dst_path))
        print ('Copy:  %s\n  To:  %s' % (os.path.abspath(src_path), os.path.abspath(dst_path)))
        shutil.copy (src_path, dst_path)
    print ('Done\n')


def GetFspCopyList (platform):
    plat_pkg = GetPlatformPkgName (platform)
    fsp_inf  = os.path.join(os.environ['PLT_SOURCE'], 'Silicon/%s/FspBin/FspBin.inf' % plat_pkg)

    fd = open (fsp_inf, 'r')
    lines = fd.readlines()
    fd.close ()

    fsp_tag        = ''
    is_def_section = False
    is_lst_section = False
    copy_list      = []
    for line in lines:
        line = line.strip ()
        if line.startswith('['):
            if line.upper().startswith('[DEFINES]'):
                is_def_section = True
            else:
                is_def_section = False
            if line.upper().startswith('[COPYLIST]'):
                is_lst_section = True
            else:
                is_lst_section = False

        if is_def_section:
            match = re.match("^FSP_COMMIT_ID\s*=\s*([_\-\w]+)", line)
            if match:
                fsp_tag = match.group(1)

        if is_lst_section:
            match = re.match("^(.+)\s*:\s*(.+)", line)
            if match:
                copy_list.append((match.group(1).strip(), match.group(2).strip()))

    if fsp_tag == '':
        Fatal ('Failed to find FSP commit ID in file - %s' % fsp_inf)

    if len(copy_list) == 0:
        Fatal ('Failed to find FSP list files in file - %s' % fsp_inf)

    return copy_list, fsp_tag


def CopyFspBins (fsp_dir, sbl_dir, platform):
    if platform not in ['apl', 'cfl']:
        return

    sys.stdout.flush()
    copy_list, fsp_tag = GetFspCopyList (platform)

    if CheckFileListExist(copy_list, sbl_dir):
        return

    CloneFspRepo (fsp_dir, fsp_tag)

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

def GetPlatformPkgName (platform):
    PlatfromPkgNameDict = {
      'qemu' : 'QemuSocPkg',
      'apl'  : 'ApollolakePkg',
      'cfl'  : 'CoffeelakePkg'
    }
    return PlatfromPkgNameDict[platform]

def CloneUcodeRepo (ucode_dir, tag):
    if not os.path.exists(ucode_dir + '/.git'):
        print ('Cloning Intel Ucode repo ...')
        cmd = 'git clone https://github.com/intel/Intel-Linux-Processor-Microcode-Data-Files.git %s' % ucode_dir
        ret = subprocess.call(cmd.split(' '))
        if ret:
            Fatal ('Failed to clone Ucode repo to directory %s !' % ucode_dir)
        print ('Done\n')
    else:
        print ('Update Intel Ucode repo ...')
        cmd = 'git pull origin master'
        ret = subprocess.call(cmd.split(' '), cwd=ucode_dir)
        if ret:
            Fatal ('Failed to update Ucode repo to directory %s !' % ucode_dir)
        print ('Done\n')

    print ('Checking out Intel Ucode commit ...')
    cmd = 'git checkout %s' % tag
    ret = subprocess.call(cmd.split(' '), cwd=ucode_dir)
    if ret:
        Fatal ('Failed to check out tag %s !' % tag)

def GetUcodeList (platform):
    plat_pkg  = GetPlatformPkgName (platform)
    ucode_inf = os.path.join(os.environ['PLT_SOURCE'], 'Silicon/%s/Microcode/Microcode.inf' % plat_pkg)

    fd = open (ucode_inf, 'r')
    lines = fd.readlines()
    fd.close ()

    ucode_tag      = ''
    is_def_section = False
    is_src_section = False
    ucode_names    = []
    for line in lines:
        line = line.strip ()
        if line.startswith('['):
            if line.upper().startswith('[DEFINES]'):
                is_def_section = True
            else:
                is_def_section = False
            if line.upper().startswith('[SOURCES]'):
                is_src_section = True
            else:
                is_src_section = False

        if is_def_section:
            match = re.match("^MICROCODE_TAG\s*=\s*([_\-\w]+)", line)
            if match:
                ucode_tag = match.group(1)

        if is_src_section:
            match = re.match("^([_\-\w]+)\.mcb", line)
            if match:
                ucode_names.append(match.group(1))

    if ucode_tag == '':
        ucode_tag = 'microcode-20190514a'

    if len(ucode_names) == 0:
        Fatal ('Failed to find microcode files in file - %s' % ucode_inf)

    ucode_list = []
    for ucode in ucode_names:
        ucode_list.append (('intel-ucode/%s' % ucode, 'Silicon/%s/Microcode/%s.mcb' % (plat_pkg, ucode)))

    return ucode_list, ucode_tag

def CopyUcodeBins (ucode_dir, sbl_dir, platform):
    if platform not in ['cfl']:
        return

    copy_list, ucode_tag = GetUcodeList (platform)
    if CheckFileListExist(copy_list, sbl_dir):
        return

    CloneUcodeRepo (ucode_dir, ucode_tag)

    CopyFileList (copy_list, ucode_dir, sbl_dir)

    print ('Done\n')

def Main():
    curr_dir       = os.path.dirname (os.path.realpath(__file__))
    workspace_dir  = os.path.abspath (os.path.join(curr_dir, '../../..'))
    fsp_repo_dir   = os.path.abspath (os.path.join(workspace_dir, 'IntelFsp'))
    qemu_repo_dir  = os.path.abspath (os.path.join(workspace_dir, 'QemuFsp'))
    ucode_repo_dir = os.path.abspath (os.path.join(workspace_dir, 'IntelUcode'))

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

    CopyUcodeBins (ucode_repo_dir, plt_dir, platform)
    return 0

if __name__ == '__main__':
    sys.exit(Main())
