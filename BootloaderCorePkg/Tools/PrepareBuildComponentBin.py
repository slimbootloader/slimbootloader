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

def CloneRepo (clone_dir, driver_inf):
    repo, commit = GetRepoAndCommit (driver_inf)
    if repo == '' or commit == '':
        Fatal ('Failed to find repo and commit information!')

    if not os.path.exists(clone_dir + '/.git'):
        print ('Cloning the repo ... %s' % repo)
        cmd = 'git clone %s %s' % (repo, clone_dir)
        ret = subprocess.call(cmd.split(' '))
        if ret:
            Fatal ('Failed to clone repo to directory %s !' % clone_dir)
        print ('Done\n')
    else:
        print ('Update the repo ...')
        cmd = 'git fetch origin master'
        ret = subprocess.call(cmd.split(' '), cwd=clone_dir)
        if ret:
            Fatal ('Failed to update repo in directory %s !' % clone_dir)
        print ('Done\n')

    print ('Checking out specified version ... %s' % commit)

    cmd = 'git checkout %s' % commit
    ret = subprocess.call(cmd.split(' '), cwd=clone_dir)
    if ret:
        Fatal ('Failed to check out specified version !')
    print ('Done\n')


def CheckFileListExist (copy_list, sbl_dir):
    exists  = True
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

def GetCopyList (driver_inf):
    fd = open (driver_inf, 'r')
    lines = fd.readlines()
    fd.close ()

    have_copylist_section = False
    copy_list      = []
    for line in lines:
        line = line.strip ()
        if line.startswith('['):
            if line.startswith('[UserExtensions.SBL."CopyList"]'):
                have_copylist_section = True
            else:
                have_copylist_section = False

        if have_copylist_section:
            match = re.match("^(.+)\s*:\s*(.+)", line)
            if match:
                copy_list.append((match.group(1).strip(), match.group(2).strip()))

    return copy_list

def GetRepoAndCommit (driver_inf):
    fd = open (driver_inf, 'r')
    lines = fd.readlines()
    fd.close ()

    repo      = ''
    commit    = ''
    have_repo_section = False
    for line in lines:
        line = line.strip ()
        if line.startswith('['):
            if line.startswith('[UserExtensions.SBL."CloneRepo"]'):
                have_repo_section = True
            else:
                have_repo_section = False

        if have_repo_section:
            match = re.match("^REPO\s*=\s*(.*)", line)
            if match:
                repo = match.group(1)

            match = re.match("^TAG\s*=\s*(.*)", line)
            if match:
                commit = match.group(1)
            match = re.match("^COMMIT\s*=\s*(.*)", line)
            if match:
                commit = match.group(1)

    return repo, commit

def CopyBins (driver_dir, sbl_dir, driver_inf):
    if not os.path.exists(driver_inf):
        return

    sys.stdout.flush()
    copy_list = GetCopyList (driver_inf)
    if len(copy_list) == 0:
        return

    if CheckFileListExist(copy_list, sbl_dir):
        return

    CloneRepo (driver_dir, driver_inf)

    CopyFileList (copy_list, driver_dir, sbl_dir)

def BuildFspBins (fsp_dir, sbl_dir, silicon_pkg_name, flag):
    sys.stdout.flush()

    copy_list = []
    if silicon_pkg_name == 'QemuSocPkg':
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

    edk2_base_tag = 'edk2-stable201905'
    print ('Building QEMU FSP binaries from EDKII repo (Base Tag: %s)' % edk2_base_tag)
    if not os.path.exists(fsp_dir + '/.git'):
        print ('Cloning EDKII repo ...')
        cmd = 'git clone https://github.com/tianocore/edk2.git %s' % fsp_dir
        ret = subprocess.call(cmd.split(' '))
        if ret:
            Fatal ('Failed to clone FSP repo to directory %s !' % fsp_dir)
        print ('Done\n')
    else:
        output = subprocess.check_output(['git', 'tag', '-l'], cwd=fsp_dir).decode()
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

    cmd = 'git clean -xfd'
    ret = subprocess.call(cmd.split(' '), cwd=fsp_dir)
    if ret:
        Fatal ('Failed to clean repo in directory %s !' % fsp_dir)

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

    print(flags)
    for flag in flags:
        os.environ['WORKSPACE'] = ''
        os.environ['EDK_TOOLS_PATH'] = ''
        os.environ['EDK_TOOLS_BIN'] = ''
        os.environ['CONF_PATH'] = ''
        if os.name == 'nt':
            ret = subprocess.call([os.path.join(fsp_dir, 'BuildFsp.cmd'), flag], cwd=fsp_dir)
        else:
            ret = subprocess.call([sys.executable, os.path.join(fsp_dir, 'BuildFsp.py'), flag], cwd=fsp_dir)
        if ret:
            Fatal ('Failed to build QEMU FSP binary !')

    print ('Done\n')

    CopyFileList (copy_list, fsp_dir, sbl_dir)

def Main():

    if len(sys.argv) < 3:
        print ('Silicon directory and silicon package name are required!')
        return -1
    target = ''
    if len(sys.argv) > 3:
        target = sys.argv[3]

    sbl_dir          = sys.argv[1]
    silicon_pkg_name = sys.argv[2]

    workspace_dir  = os.path.join(sbl_dir, '../Download')
    fsp_repo_dir   = os.path.abspath (os.path.join(workspace_dir, 'IntelFsp'))
    qemu_repo_dir  = os.path.abspath (os.path.join(workspace_dir, 'QemuFsp'))
    ucode_repo_dir = os.path.abspath (os.path.join(workspace_dir, 'IntelUcode'))

    if silicon_pkg_name == 'QemuSocPkg':
        BuildFspBins (qemu_repo_dir, sbl_dir, silicon_pkg_name, target)
    else:
        fsp_inf = os.path.join(sbl_dir, 'Silicon', silicon_pkg_name, 'FspBin', 'FspBin.inf')
        CopyBins (fsp_repo_dir, sbl_dir, fsp_inf,)

    microcode_inf = os.path.join(sbl_dir, 'Silicon', silicon_pkg_name, 'Microcode', 'Microcode.inf')
    CopyBins (ucode_repo_dir, sbl_dir, microcode_inf)
    return 0

if __name__ == '__main__':
    sys.exit(Main())
