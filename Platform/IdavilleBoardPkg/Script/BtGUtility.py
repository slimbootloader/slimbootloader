## @ StitchIfwi.py
#  This is a python stitching script for Slim Bootloader IDV build
#
# Copyright (c) 2020 - 2022, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import sys
import os
import re
import imp
import struct
import argparse
import zipfile
import shutil
import glob
import shlex
import subprocess
import xml.etree.ElementTree as ET
from   xml.dom import minidom
from   ctypes  import *
from   subprocess   import call

sys.dont_write_bytecode = True
sblopen_dir = os.path.join(os.path.abspath(os.path.dirname(__file__)), '../../../')
if not os.path.exists (sblopen_dir):
    sblopen_dir = os.getenv('SBL_SOURCE', '')
sys.path.append (os.path.join(sblopen_dir, "BootloaderCorePkg" , "Tools"))

try:
    from   IfwiUtility import *
except ImportError:
    err_msg  = "Cannot find IfwiUtility module!\n"
    err_msg += "Please make sure 'SBL_SOURCE' environment variable is set to open source SBL root folder."
    raise  ImportError(err_msg)

btg_profile_values = [\
                    "Boot Guard Profile 0 - No_FVME",\
                    "Not Applicable",\
                    "Not Applicable",\
                    "Boot Guard Profile 3 - VM",\
                    "Boot Guard Profile 4 - FVE",\
                    "Boot Guard Profile 5 - FVME"]

def is_wine_installed ():
    return True if shutil.which("wine") != None else False

def run_process_wrapper (cmds):
    if os.name == 'posix' and cmds[0].endswith('.exe'):
        if is_wine_installed ():
            cmds = ["wine"] + cmds
        else:
            print ("\n")
            print ("ERROR: Please install 'wine'.")
            print ("       To stitch IFWI in Linux, 'wine' is required")
            print ("\n")
            raise Exception ()
    run_process (cmds)

def gen_bpmgen2_params (stitch_cfg_file, InFile, OutFile):
    InFileptr = open(InFile, 'r', encoding='utf8')
    lines = InFileptr.readlines()
    InFileptr.close()

    params_change_list = stitch_cfg_file.get_bpmgen2_params_change_list()

    for variable, value in params_change_list:
        for linenumber, line in enumerate(lines):
            if line.split(':')[0].strip() == variable:
                lines[linenumber] = variable + ':    ' + value + '\n'
                break

    if OutFile == '':
        OutFile = Infile

    Outfileptr = open(OutFile, 'w')
    Outfileptr.write("".join(lines))
    Outfileptr.close()

def swap_ts_block(in_file, out_file, ts_size):
    print("Swapping Top Swap Blocks....")

    if not os.path.exists(in_file):
        raise Exception("%s not found !" % in_file)
        return 1

    in_file_size = os.path.getsize(in_file)

    if ts_size > in_file_size:
        raise Exception("TsSize %x is greater than the file size !" % ts_size)
        return 1

    in_fileptr = open(in_file, 'rb')
    in_file_data = bytearray(in_fileptr.read())
    in_fileptr.close()

    write_data = bytearray(0)
    out_file_ptr = open(out_file, 'wb')
    block_data = in_file_data[0:(in_file_size-(2*ts_size))]
    block_data += in_file_data[(in_file_size-ts_size):in_file_size]
    block_data += in_file_data[(in_file_size-(2*ts_size)):(in_file_size-ts_size)]
    out_file_ptr.write(block_data)
    out_file_ptr.close()

def sign_binary(infile, stitch_dir, stitch_cfg_file):
    openssl_path = get_openssl_path()

    output_dir = os.path.join(stitch_dir, "Temp")
    shutil.copy(infile, os.path.join(output_dir,"sbl_sec_temp.bin"))

    print("Generating new keys....")

    bpm_gen2dir = os.path.join (stitch_dir, 'BpmGen2')
    bpm_key_dir = os.path.join (bpm_gen2dir, 'keys')
    if not os.path.exists(bpm_key_dir):
        os.mkdir(bpm_key_dir)

        cmd = '%s genrsa -F4 -out %s/oem_privkey_2048.pem 2048' % (openssl_path, bpm_key_dir)
        run_process_wrapper (cmd.split())

        cmd = '%s rsa -pubout -in %s/oem_privkey_2048.pem -out %s/oem_pubkey_2048.pem' % (openssl_path, bpm_key_dir, bpm_key_dir)
        run_process_wrapper (cmd.split())

        cmd = '%s genrsa -F4 -out %s/bpm_privkey_2048.pem 2048' % (openssl_path, bpm_key_dir)
        run_process_wrapper (cmd.split())

        cmd = '%s rsa -pubout -in %s/bpm_privkey_2048.pem -out %s/bpm_pubkey_2048.pem' % (openssl_path, bpm_key_dir, bpm_key_dir)
        run_process_wrapper (cmd.split())

    print ("Generating BPM GEN2 params file")
    gen_bpmgen2_params(stitch_cfg_file, os.path.join(bpm_gen2dir, "bpmgen2.params"), os.path.join(output_dir, "bpmgen2.params"))

    print("Generating Btg KeyManifest.bin....")
    run_process_wrapper ([os.path.join (bpm_gen2dir, 'BpmGen2.exe'),
        'KMGEN',
        '-KEY',        os.path.join (bpm_key_dir, 'bpm_pubkey_2048.pem'), 'BPM',
        '-KM',         os.path.join (output_dir,  'KeyManifest.bin'),
        '-SIGNKEY',    os.path.join (bpm_key_dir, 'oem_privkey_2048.pem'),
        '-SIGNPUBKEY', os.path.join (bpm_key_dir, 'oem_pubkey_2048.pem'),
        '-KMID',       '0x01',
        '-KMKHASH',    'SHA256',
        '-SCHEME',     'RSASSA',
        '-SVN',        '0',
        '-d:2'])

    print("Generating Btg Boot Policy Manifest (BPM).bin....")
    run_process_wrapper ([os.path.join (bpm_gen2dir, 'BpmGen2.exe'),
        'GEN',
        os.path.join (output_dir, 'sbl_sec_temp.bin'),
        os.path.join (output_dir, 'bpmgen2.params'),
        '-BPM',        os.path.join (output_dir, 'BpmManifest.bin'),
        '-U',          os.path.join (output_dir, 'sbl_sec.bin'),
        '-KM',         os.path.join (output_dir, 'KeyManifest.bin'),
        '-d:2'])

def gen_oem_key_hash(stitch_dir):

    output_dir = os.path.join(stitch_dir, "Temp")
    kmsigpubkeytxtfile  = os.path.join(output_dir, "kmsigpubkey.txt")
    kmsigpubkeybinfile  = os.path.join(output_dir, "kmsigpubkey.bin")

    with open(kmsigpubkeytxtfile, "w") as kmsigpubkeytxt_fh:
        run_process_wrapper ([get_openssl_path(), 'rsa' , '-in', 'BpmGen2/keys/oem_pubkey_2048.pem',
            '-pubin', '-modulus', '-out', 'Temp/kmsigpubkeytxt_fh'])
    shutil.copy(os.path.join(output_dir, "kmsigpubkeytxt_fh"), kmsigpubkeytxtfile)

    line=open(kmsigpubkeytxtfile,"r").readline()[8:8+256*2]
    if sys.hexversion >= 0x3000000:
        keybin = bytearray.fromhex(line)
    else:
        keybin = line.decode('hex')
    open(kmsigpubkeybinfile,"wb").write(keybin[::-1])

    # public exponent (i.e. 65537) is concatenated for OEM key hash calculation.
    line=open(kmsigpubkeytxtfile,"r").readline()[8:8+256*2]
    open(kmsigpubkeybinfile,"ab").write(bytearray.fromhex('01000100'))
    run_process_wrapper ([get_openssl_path(), 'dgst', '-sha256', '-binary',
        '-out', 'Temp/kmsigpubkey.hash', 'Temp/kmsigpubkey.bin'])

def gen_sign_oem_key_manifest(stitch_dir):
    output_dir = os.path.join(stitch_dir, "Temp")
    ibst_path = os.path.join (stitch_dir, 'IbstTool')
    oem_bin_sign = os.path.join (output_dir, 'OemKeyManifest.bin')
    bpm_key_dir = os.path.join (stitch_dir, 'BpmGen2', 'keys')

    if os.path.exists(oem_bin_sign):
        print('Use prebuilt OemKeyManifest.bin!')
        return

    if not os.path.exists(ibst_path):
        raise Exception ("Ibst tool not present, cant sign OEM KeyManifest")

    cmd = ['python', os.path.join(ibst_path, 'ibst.py'), os.path.join(ibst_path, 'config', 'OemKeyManifest.xml'),
                    '-s', 'key=%s' % os.path.join (bpm_key_dir, 'oem_privkey_2048.pem'), 'fd0v_key=%s' % os.path.join (bpm_key_dir, 'oem_pubkey_2048.pem'), '-o', oem_bin_sign]
    run_process_wrapper (cmd)

def replace_component (ifwi_src_path, flash_path, file_path, comp_alg, pri_key):
    print ("Replacing components.......")
    work_dir = os.getcwd()
    ifwi_bin = bytearray (get_file_data (ifwi_src_path))
    ifwi = IFWI_PARSER.parse_ifwi_binary (ifwi_bin)

    # assume a flash map path first
    comp_name     = ''
    replace_comps = IFWI_PARSER.locate_components (ifwi, flash_path)
    if len(replace_comps) == 0:
        # assume a container path if not in flash map
        nodes      = flash_path.split('/')
        comp_name  = nodes[-1]
        flash_path = '/'.join(nodes[:-1])
        replace_comps = IFWI_PARSER.locate_components (ifwi, flash_path)

    if len(replace_comps) == 0:
        raise Exception ("Could not locate component '%s' !" % flash_path)

    if len(replace_comps) > 1:
        raise Exception ("Multiple components were located for '%s' !" % flash_path)

    replace_comp = replace_comps[0]
    if comp_name:
        # extract container image
        container_file = os.path.join(work_dir, 'CTN_%s.bin') % comp_name
        gen_file_from_object (container_file, ifwi_bin[replace_comp.offset:replace_comp.offset + replace_comp.length])
        comp_file     = os.path.join(work_dir, file_path)
        sblopen_dir = os.path.join(os.path.abspath(os.path.dirname(__file__)), '../../../')
        if not os.path.exists (sblopen_dir):
            sblopen_dir = os.getenv('SBL_SOURCE', '')

        if not os.path.exists (sblopen_dir):
           raise  Exception("Please set env 'SBL_SOURCE' to SBL open source root folder")

        if os.name == 'nt':
            tool_bin_dir  = os.path.join(sblopen_dir, "BaseTools", "Bin", "Win32")
        else:
            tool_bin_dir  = os.path.join(sblopen_dir, "BaseTools", "BinWrappers", "PosixLike")
        gen_container = os.path.join(sblopen_dir, "BootloaderCorePkg" , "Tools", "GenContainer.py")
        if not os.path.isabs(pri_key):
            pri_key = os.path.join (work_dir, pri_key)
        cmd_line = [sys.executable, gen_container, 'replace', '-i', container_file, '-o', container_file, '-n', comp_name,
                                    '-f', comp_file, '-c', comp_alg, '-k', pri_key, '-td', tool_bin_dir]
        run_process_wrapper (cmd_line)
        comp_bin = bytearray (get_file_data (container_file))
    else:
        # replace directly in flash map
        comp_bin = bytearray (get_file_data (file_path))
    IFWI_PARSER.replace_component (ifwi_bin, comp_bin, flash_path)
    gen_file_from_object (ifwi_src_path, ifwi_bin)

def replace_components (ifwi_src_path, stitch_cfg_file):
    print ("Replacing components.......")
    replace_list = stitch_cfg_file.get_component_replace_list ()
    for flash_path, file_path, comp_alg, pri_key in replace_list:
        replace_component (ifwi_src_path, flash_path, file_path, comp_alg, pri_key)

def update_btGuard_manifests(stitch_dir, stitch_cfg_file):
    output_dir = os.path.join(stitch_dir, "Temp")
    sbl_file = os.path.join (output_dir, 'SlimBootloader.bin')

    print("Replace components in both partitions....")
    replace_components (sbl_file, stitch_cfg_file)

    # get topswap size
    sbl_bin  = bytearray (get_file_data (sbl_file))
    ifwi     = IFWI_PARSER.parse_ifwi_binary (sbl_bin)
    ifwi_comps = IFWI_PARSER.locate_components (ifwi, 'IFWI/BIOS/TS0')
    if len(ifwi_comps) == 0:
        raise Exception("Cannot find path 'IFWI/BIOS/TS0' in ifwi image!")
    for ifwi_comp in ifwi_comps:
        top_swap_size = ifwi_comp.length

    print("Sign primary partition....")
    sign_binary(os.path.join(output_dir,"SlimBootloader.bin"), stitch_dir, stitch_cfg_file)

    print("Swap top swap block....")
    swap_ts_block(os.path.join(output_dir, "sbl_sec.bin"), os.path.join(output_dir, "SwappedA.bin"), top_swap_size)

    print("Sign backup partition....")
    sign_binary(os.path.join(output_dir,"SwappedA.bin"), stitch_dir, stitch_cfg_file)
    os.remove(os.path.join(output_dir,"SwappedA.bin"))

    print("Swap to original top swap block....")
    swap_ts_block(os.path.join(output_dir, "sbl_sec.bin"), os.path.join(output_dir, "SwappedA.bin"), top_swap_size)
    shutil.copy(os.path.join(output_dir, "SwappedA.bin"), os.path.join(output_dir, "SlimBootloader.bin"))
    shutil.copy(os.path.join(output_dir, "SlimBootloader.bin"), os.path.join(output_dir, "BiosRegion.bin"))
    os.remove(os.path.join(output_dir,"SwappedA.bin"))

    # Generate OemKeyHash
    gen_oem_key_hash(stitch_dir)

    # Generate OemKeyManifest bin if not available in inputs
    gen_sign_oem_key_manifest(stitch_dir)

def update_btGuard_xml(btg_profile, stitch_dir, tree, btg_xml_change_dict):
    output_dir = os.path.join(stitch_dir, "Temp")
    kmsigpubkeyhashfile = os.path.join(output_dir, "kmsigpubkey.hash")

    if btg_profile == 'vm':
        btguardprofile = 3
    elif btg_profile == 'fve':
        btguardprofile = 4
    elif btg_profile == 'fvme':
        btguardprofile = 5
    else:
       print ("Boot Guard is NOT enabled.....")
       btguardprofile = 0

    node = tree.find(btg_xml_change_dict['BtGuardKeyManifestId'])
    node.attrib['value'] = '0x1'

    node = tree.find(btg_xml_change_dict['BtGuardProfileConfig'])
    node.attrib['value'] = btg_profile_values[btguardprofile]

    # Convert OEM key hash into hexadecimal for Fit tool consumption
    with open(kmsigpubkeyhashfile,"rb") as kmsigpubkeyhash_fh:
        hash = bytearray (kmsigpubkeyhash_fh.read())
        oemkeyhash = ""
        for b in hash:
            oemkeyhash = oemkeyhash + "%02X " % b
    print("oemkeyhash:")
    print(oemkeyhash)

    node = tree.find(btg_xml_change_dict['OemPublicKeyHash'])
    node.attrib['value'] = oemkeyhash

    node = tree.find(btg_xml_change_dict['OemExtInputFile'])
    node.attrib['value'] = '$SourceDir\OemKeyManifest.bin'

    node = tree.find(btg_xml_change_dict['ME_Region_OEM_Key_Manifest_Present'])
    node.attrib['value'] = '1'

def update_igfw_btGuard_xml(btg_profile, stitch_dir, root, igfw_btg_xml_change_dict):
    output_dir = os.path.join(stitch_dir, "Temp")
    kmsigpubkeyhashfile = os.path.join(output_dir, "kmsigpubkey.hash")

    if btg_profile == 'vm':
        btguardprofile = 3
    elif btg_profile == 'fve':
        btguardprofile = 4
    elif btg_profile == 'fvme':
        btguardprofile = 5
    else:
       print ("Boot Guard is NOT enabled.....")
       btguardprofile = 0

    node = root.find(igfw_btg_xml_change_dict['BtGuardKeyManifestId'])
    node.attrib['value'] = '0x1'

    node = root.find(igfw_btg_xml_change_dict['BtGuardProfileConfig'])
    node.attrib['value'] = '0x' + str(btguardprofile)

    # Convert OEM key hash into hexadecimal for Fit tool consumption
    with open(kmsigpubkeyhashfile,"rb") as kmsigpubkeyhash_fh:
        hash = bytearray (kmsigpubkeyhash_fh.read())
        oemkeyhash = ""
        for b in hash:
            oemkeyhash = oemkeyhash + "%02X" % b
    print("oemkeyhash:")
    print(oemkeyhash)

    node = root.find(igfw_btg_xml_change_dict['OemPublicKeyHash'])
    node.attrib['value'] = oemkeyhash

    #node = tree.find(btg_xml_change_dict['OemExtInputFile'])
    #node.attrib['value'] = '$SourceDir\OemKeyManifest.bin'

    node = root.find(igfw_btg_xml_change_dict['ME_Region_OEM_Key_Manifest_Present'])
    node.attrib['value'] = '1'

def update_tpm_type(tpm_type, tree):

    ptt_supported_xml_entry     = "PttSupported"
    ptt_pwrup_state_xml_entry   = "PttPwrUpState"
    ptt_supported_fpf_xml_entry = "PttSupportedHw"
    ptt_enabled_xml_entry       = "PttEnabled"

    #spi_over_tpmbus_xml_entry   = "./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable"

    tpm_config = {
        'ptt': {
              ptt_supported_xml_entry     : '1',
              ptt_pwrup_state_xml_entry   : '1',
              ptt_supported_fpf_xml_entry : '1',
              ptt_enabled_xml_entry       : 'true',
        },
        'dtpm': {
              ptt_supported_xml_entry     : '0',
              ptt_pwrup_state_xml_entry   : '0',
              ptt_supported_fpf_xml_entry : '1',
              ptt_enabled_xml_entry       : 'false',
        },
        'none': {
              ptt_supported_xml_entry     : '0',
              ptt_pwrup_state_xml_entry   : '0',
              ptt_supported_fpf_xml_entry : '1',
              ptt_enabled_xml_entry       : 'false',
        },
    }

    # Enable TPM/PTT device
    # List variables for file paths for ./SpsFiles/file/variable
    root = tree.getroot()
    node = root.findall("./SpsFiles/file/variable")
    length = len(node)
    for i in range(0, length-1):
        if node[i].attrib['name'] in tpm_config[tpm_type]:
            node[i].attrib['value'] = tpm_config[tpm_type][node[i].attrib['name']]

    print("[INFO] TPM device type enabled for Measured Boot: %s" % tpm_type)

    # TBD: Move EOM functionality to separatefunction
    # Get EOM variable file and set EOM to OFF
    for file in root.findall('./SpsFiles/file'):
        if file.attrib['name'] == 'EOM':
            eom = file.find('variable')
            if eom.attrib['name'] == 'Status':
                eom.attrib['value'] = '0'


