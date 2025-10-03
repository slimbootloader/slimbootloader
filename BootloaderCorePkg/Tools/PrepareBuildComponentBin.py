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

def CleanFileList (copy_list, sbl_dir):
    for _, dst_path in copy_list:
        dst_path = os.path.join (sbl_dir, dst_path)
        if os.path.exists(dst_path):
            print('Removing %s' % dst_path)
            os.remove(dst_path)


def SubstituteMacrosInPath (path, defines_dict):
    match = re.search(r'\$\(([^)]+)\)', path)
    if match:
        var_name = match.group(1)
        if var_name in defines_dict:
            return path.replace(f"$({var_name})", defines_dict[var_name])
    return path


def GetCopyList (driver_inf):
    fd = open (driver_inf, 'r')
    lines = fd.readlines()
    fd.close ()

    have_defines_section  = False
    have_copylist_section = False
    copy_list      = []
    defines_dict   = {}
    for line in lines:
        line = line.strip ()

        # Skip empty lines and comments
        if not line or line.startswith('#'):
            continue

        # Check for section headers
        if line.startswith('['):
            if line.startswith('[Defines]'):
                have_defines_section = True
                have_copylist_section = False
            elif line.startswith('[UserExtensions.SBL."CopyList"]'):
                have_copylist_section = True
                have_defines_section = False
            else:
                have_defines_section = False
                have_copylist_section = False
            continue

        # Parse DEFINE statements in [Defines] section
        if have_defines_section and line.startswith('DEFINE '):
            define_match = re.match(r'DEFINE\s+(\w+)\s*=\s*(.+)', line)
            if define_match:
                defines_dict[define_match.group(1)] = define_match.group(2).strip()

        # Parse copy list entries in [UserExtensions.SBL."CopyList"] section
        if have_copylist_section:
            match = re.match(r'^(.+)\s*:\s*(.+)', line)
            if match:
                copy_list.append((match.group(1).strip(), match.group(2).strip()))

    if defines_dict:
        # substitute macro DEFINES in the copy list
        while True:
            var_found = False
            for i, entry in enumerate (copy_list):
                src_file, dest_file = entry
                updated_src  = SubstituteMacrosInPath (src_file, defines_dict)
                updated_dest = SubstituteMacrosInPath (dest_file, defines_dict)

                if updated_src != src_file or updated_dest != dest_file:
                    var_found = True
                    copy_list[i] = (updated_src, updated_dest)

            if not var_found:
                break
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
        local_env = os.environ.copy()
        local_env['WORKSPACE'] = ''
        local_env['EDK_TOOLS_PATH'] = ''
        local_env['EDK_TOOLS_BIN'] = ''
        local_env['BASE_TOOLS_PATH'] = ''
        local_env['CONF_PATH'] = ''
        ret = subprocess.call([sys.executable, os.path.join(fsp_dir, 'BuildFsp.py'), flag], cwd=fsp_dir, env=local_env)
        if ret:
            Fatal ('Failed to build QEMU FSP binary !')

    print ('Done\n')

    CopyFileList (copy_list, fsp_dir, sbl_dir)

def GetInfFilesFromDsc (dsc_file_path):
    inf_files = []
    if not os.path.exists(dsc_file_path):
        return inf_files

    with open(dsc_file_path, 'r') as f:
        lines = f.readlines()

    defines_dict = {}
    in_defines_section = False
    for line in lines:
        line = line.strip()
        if not line or line.startswith('#'):
            continue

        # Check for [Defines] section
        if line.startswith('[Defines]'):
            in_defines_section = True
            continue
        elif line.startswith('[') and line.endswith(']'):
            in_defines_section = False
            continue

        # Extract DEFINE macros
        if in_defines_section and line.startswith('DEFINE '):
            define_match = re.match(r'DEFINE\s+(\w+)\s*=\s*(.+)', line)
            if define_match:
                defines_dict[define_match.group(1)] = define_match.group(2).strip()

        # Look for INF files
        if '.inf' in line:
            inf_match = re.search(r'([^\s|]+\.inf)', line)
            if inf_match:
                inf_file = inf_match.group(1)
                if inf_file not in inf_files:
                    inf_files.append(inf_file)

    # Resolve macros in INF file paths
    if defines_dict:
        while True:
            var_found = False
            for i, inf_file in enumerate (inf_files):
                updated_inf = SubstituteMacrosInPath (inf_file, defines_dict)
                if updated_inf != inf_file:
                    var_found = True
                    inf_files[i] = updated_inf

            if not var_found:
                break

    return inf_files


def ProcessInfFileCopyList (sbl_dir, ignore_file_list, clean_copy_list = False):
    dsc_file_path = os.path.join(sbl_dir, 'BootloaderCorePkg', 'Platform.dsc')
    if not os.path.exists(dsc_file_path):
        print ('DSC file not found!')
        return

    inf_files  = GetInfFilesFromDsc (dsc_file_path)
    for inf_file in inf_files:
        if inf_file in ignore_file_list:
            continue

        inf_file = os.path.join(sbl_dir, inf_file)
        if not os.path.exists(inf_file):
            continue

        copy_list = GetCopyList (inf_file)
        if len(copy_list) == 0:
            continue

        inf_dir = os.path.dirname(inf_file)
        updated_copy_list = []
        for src_f, dest_f in copy_list:
            # Make source and destination paths relative to INF file directory
            updated_copy_list.append((os.path.join(inf_dir, src_f), os.path.join(inf_dir, dest_f)))

        if clean_copy_list:
            CleanFileList (updated_copy_list, sbl_dir)
        else:
            CopyFileList (updated_copy_list, '', sbl_dir)

def ProcessFspAndMicrocodeInf (sbl_dir, silicon_pkg_name, fsp_inf, microcode_inf, target):
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


