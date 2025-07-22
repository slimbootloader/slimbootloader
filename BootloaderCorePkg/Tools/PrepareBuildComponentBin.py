## @ PrepareFspBin.py
#
# Copyright (c) 2018 - 2023, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys
import re
import shutil
import subprocess
import glob

def Fatal (msg):
    sys.stdout.flush()
    raise Exception (msg)

def CloneRepo (clone_dir, driver_inf):
    repo, commit = GetRepoAndCommit (driver_inf)
    if repo == '' or commit == '':
        Fatal ('Failed to find repo and commit information!')

    base_dir = os.path.basename(clone_dir)
    if base_dir == '$AUTO':
        repo_dir = os.path.basename(repo)
        if repo_dir.lower().endswith('.git'):
            repo_dir = repo_dir[:-4]
        clone_dir = os.path.join (os.path.dirname(clone_dir), repo_dir)

    if not os.path.exists(clone_dir + '/.git'):
        print ('Cloning the repo ... %s' % repo)
        cmd = 'git clone %s %s' % (repo, clone_dir)
        ret = subprocess.call(cmd.split(' '))
        if ret:
            Fatal ('Failed to clone repo to directory %s !' % clone_dir)
        print ('Done\n')
    else:
        # If the repository already exists, then try to check out the correct
        # revision without going to the network
        print ('Attempting to check out specified version ... %s' % commit)
        cmd = 'git checkout %s' % commit
        ret = subprocess.call(cmd.split(' '), cwd=clone_dir)
        if ret == 0:
            print ('Done\n')
            return clone_dir

        print ('Specified version not available. Update the repo ...')
        cmd = 'git fetch origin'
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
    return clone_dir


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
            match = re.match("^(.+)\\s*:\\s*(.+)", line)
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
            match = re.match("^REPO\\s*=\\s*(.*)", line)
            if match:
                repo = match.group(1)

            match = re.match("^TAG\\s*=\\s*(.*)", line)
            if match:
                commit = match.group(1)
            match = re.match("^COMMIT\\s*=\\s*(.*)", line)
            if match:
                commit = match.group(1)

    return repo, commit

def CopyBins (repo_dir, sbl_dir, driver_inf):
    if not os.path.exists(driver_inf):
        return

    sys.stdout.flush()
    copy_list = GetCopyList (driver_inf)
    if len(copy_list) == 0:
        return

    if CheckFileListExist(copy_list, sbl_dir):
        return

    repo_dir = CloneRepo (repo_dir, driver_inf)

    CopyFileList (copy_list, repo_dir, sbl_dir)

def BuildFspBins (fsp_dir, sbl_dir, fsp_inf, silicon_pkg_name, flag):
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

    if os.path.exists(fsp_dir + '/BuildFsp.py'):
        os.remove (fsp_dir + '/BuildFsp.py')

    if os.path.exists(fsp_dir + '/QemuFspPkg'):
        shutil.rmtree(fsp_dir + '/QemuFspPkg')

    print ('Cloning QEMU FSP from EDKII repo')
    sys.stdout.flush()
    CloneRepo (fsp_dir, fsp_inf)

    # skip submodule init since it takes a long time.
    # This will cause some build errors from missing include folders, so create dummy folders
    dep_dirs = [
        'MdeModulePkg/Library/BrotliCustomDecompressLib/brotli/c/include/',
        'MdePkg/Library/MipiSysTLib/mipisyst/library/include'
    ]
    for dir in dep_dirs:
        abs_dep_dir = os.path.join(fsp_dir, dir)
        if not os.path.exists(abs_dep_dir):
            os.makedirs(abs_dep_dir)

    print ('Applying QEMU FSP patch ...')
    patch_dir = os.path.join(sbl_dir, 'Silicon/QemuSocPkg/FspBin/Patches')
    cmd = 'git am --abort'
    with open(os.devnull, 'w') as fnull:
        ret = subprocess.call(cmd.split(' '), cwd=fsp_dir, stdout=fnull, stderr=subprocess.STDOUT)
    cmd = 'git am --keep-cr --whitespace=fix %s/0001-PATCH-Build-QEMU-FSP-2.0-binaries-with-edk2stable202.patch' % patch_dir
    ret = subprocess.call(cmd.split(' '), cwd=fsp_dir)
    if ret:
        Fatal ('Failed to apply QEMU FSP patch !')

    print ('Done\n')

    print ('Compiling QEMU FSP source ...')

    if flag == '':
        flags = ['/d', '/r']
    else:
        flags = [flag]

    for flag in flags:
        os.environ['WORKSPACE'] = ''
        os.environ['EDK_TOOLS_PATH'] = ''
        os.environ['EDK_TOOLS_BIN'] = ''
        os.environ['BASE_TOOLS_PATH'] = ''
        os.environ['CONF_PATH'] = ''
        ret = subprocess.call([sys.executable, os.path.join(fsp_dir, 'BuildFsp.py'), flag], cwd=fsp_dir)
        if ret:
            Fatal ('Failed to build QEMU FSP binary !')

    print ('Done\n')

    CopyFileList (copy_list, fsp_dir, sbl_dir)

def Main():

    if len(sys.argv) < 6:
        print ('Silicon directory, silicon package name, and target are required!')
        return -1

    sbl_dir          = sys.argv[1]
    silicon_pkg_name = sys.argv[2]
    fsp_inf          = sys.argv[3]
    microcode_inf    = sys.argv[4]
    target           = sys.argv[5]

    workspace_dir  = os.path.join(sbl_dir, '../Download', silicon_pkg_name)
    fsp_repo_dir   = os.path.abspath (os.path.join(workspace_dir, 'IntelFsp'))
    qemu_repo_dir  = os.path.abspath (os.path.join(workspace_dir, 'QemuFsp'))

    # Leave the final path node as '$AUTO' to allow to determine the repo dir automatically.
    ucode_repo_dir = os.path.abspath (os.path.join(workspace_dir, '$AUTO'))

    if silicon_pkg_name == 'QemuSocPkg':
        BuildFspBins (qemu_repo_dir, sbl_dir, fsp_inf, silicon_pkg_name, target)
    else:
        CopyBins (fsp_repo_dir, sbl_dir, fsp_inf)

    CopyBins (ucode_repo_dir, sbl_dir, microcode_inf)

    return 0

if __name__ == '__main__':
    sys.exit(Main())
