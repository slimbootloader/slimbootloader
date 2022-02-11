## @ StitchIfwi.py
#  This is a python stitching script for Slim Bootloader CMLV build
#
# Copyright (c) 2020, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import sys
import os
import re
import struct
import argparse
import zipfile
import shutil
import glob
import shlex
import subprocess # nosec
import xml.etree.ElementTree as ET
from   xml.dom import minidom
from   ctypes  import *
from   subprocess   import call # nosec
from   StitchLoader import *
sys.dont_write_bytecode = True

btg_profile_values = [\
                    "Boot Guard Profile 0 - No_FVME",\
                    "Not Applicable",\
                    "Not Applicable",\
                    "Boot Guard Profile 3 - VM",\
                    "Boot Guard Profile 4 - FVE",\
                    "Boot Guard Profile 5 - FVME"]

def is_wine_installed ():
    return True if shutil.which("wine") != None else False

def get_path_wrapper (path):
    if os.name == 'posix' and is_wine_installed ():
        cmd = ['winepath', '-w', path]
        return run_process (cmd, capture_out=True).strip()
    else:
        return path

def run_process_wrapper (cmds):
    if os.name == 'posix':
        if is_wine_installed ():
            cmds = ["wine"] + cmds
        else:
            print ("\n")
            print ("ERROR: Please install 'wine'.")
            print ("       To stitch CMLV IFWI in Linux, 'wine' is required")
            print ("\n")
            raise Exception ()
    run_process (cmds)

def get_openssl_path_wrapper (stitch_dir):
    if os.name == 'posix' and is_wine_installed ():
        return get_path_wrapper (os.path.join (stitch_dir, 'OpenSSL/openssl.exe'))
    else:
        return get_openssl_path ()

def gen_bpmgen2_params (stitch_cfg_file, InFile, OutFile):
    InFileptr = open(InFile, 'r',encoding='iso-8859-15')
    lines = InFileptr.readlines()
    InFileptr.close()

    params_change_list = stitch_cfg_file.get_bpmgen2_params_change_list()

    for item in params_change_list:
        for variable, value in item:
            for linenumber, line in enumerate(lines):
                if line.split(':')[0].strip() == variable:
                    lines[linenumber] = variable + ':    ' + value + '\n'
                    break

    if OutFile == '':
        OutFile = Infile

    Outfileptr = open(OutFile, 'w',encoding='iso-8859-15')
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

def gen_oem_key_hash(stitch_dir):

    output_dir = os.path.join(stitch_dir, "Temp")
    kmsigpubkeytxtfile  = os.path.join(output_dir, "kmsigpubkey.txt")
    kmsigpubkeybinfile  = os.path.join(output_dir, "kmsigpubkey.bin")

    with open(kmsigpubkeytxtfile, "w") as kmsigpubkeytxt_fh:
        run_process ([get_openssl_path(), 'rsa' , '-in', 'BpmGen2/keys/oem_pubkey_2048.pem',
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
    run_process ([get_openssl_path(), 'dgst', '-sha256', '-binary',
        '-out', 'Temp/kmsigpubkey.hash', 'Temp/kmsigpubkey.bin'])

def gen_sign_oem_key_manifest(stitch_dir, stitch_cfg_file):

    output_dir = os.path.join(stitch_dir, "Temp")
    meu_path = os.path.join (stitch_dir, 'Meu', 'meu')
    oem_bin_input  = os.path.join (stitch_dir, 'Input', 'OemExtInputFile.bin')
    oem_bin_sign   = os.path.join (output_dir, 'OemExtInputFile.bin')

    openssl_path = get_openssl_path_wrapper (stitch_dir)

    bpm_gen2dir = os.path.join (stitch_dir, 'BpmGen2')
    bpm_key_dir = os.path.join (bpm_gen2dir, 'keys')

    if not os.path.exists(oem_bin_input):

        print("Generate and sigm OemKeyManifest binary....")

        #create an dummy meu_config.xml
        run_process_wrapper ([get_path_wrapper (meu_path), '-gen', 'meu_config',
            '-save', get_path_wrapper (os.path.join(output_dir, 'meu_config.xml'))])

        #Generate default OEMKeyManifest config xml
        run_process_wrapper ([get_path_wrapper (meu_path), '-gen', 'OEMKeyManifest',
            '-save', get_path_wrapper (os.path.join(output_dir, 'oemkeymanifest_sample_config.xml'))])

        #Update OEMKeyManifest config xml for sample/test params
        tree = ET.parse(os.path.join(output_dir, 'oemkeymanifest_sample_config.xml'))

        xml_change_list = stitch_cfg_file.get_oemkeymanifest_change_list()
        for each in xml_change_list:
            for xml_path, value in each:
                node = tree.find('%s' % xml_path)
                node.set('value', value)

        tree.write(os.path.join(output_dir, 'oemkeymanifest_sample_config.xml'), xml_declaration=True, encoding='utf-8')

        #Generate signed OEMKeyManifest binary
        #meu.exe -f decomp_km1.xml -o OemExtInputFile.bin -key oem_privkey_2048.pem -stp C:\Openssl\openssl.exe

        run_process_wrapper ([get_path_wrapper (meu_path),
            '-f', get_path_wrapper (os.path.join(output_dir, 'oemkeymanifest_sample_config.xml')),
            '-o', get_path_wrapper (oem_bin_sign),
            '-key',  get_path_wrapper (os.path.join (bpm_key_dir, 'oem_privkey_2048.pem')),
            '-cfg', get_path_wrapper (os.path.join(output_dir, 'meu_config.xml')),
            '-stp', openssl_path])
    else:
        # OemKeyManifest binary re signing
        print("Resign Oem key manifest binary....")

        #create an dummy meu_config
        run_process_wrapper ([get_path_wrapper (meu_path), '-gen', 'meu_config',
            '-save', get_path_wrapper (os.path.join(output_dir, 'meu_config.xml'))])

        #Resign Oem Binary using Meu tool
        run_process_wrapper ([get_path_wrapper (meu_path), '-resign',
            '-f', get_path_wrapper (oem_bin_input),
            '-o', get_path_wrapper (oem_bin_sign),
            '-key', get_path_wrapper (os.path.join (bpm_key_dir, 'oem_privkey_2048.pem')),
            '-cfg', get_path_wrapper (os.path.join(output_dir, 'meu_config.xml')),
            '-stp', openssl_path])


def sign_binary(infile, stitch_dir, stitch_cfg_file):
    openssl_path = get_openssl_path ()

    output_dir = os.path.join(stitch_dir, "Temp")
    shutil.copy(infile, os.path.join(output_dir,"sbl_sec_temp.bin"))

    print("Generating new keys....")

    bpm_gen2dir = os.path.join (stitch_dir, 'BpmGen2')
    bpm_key_dir = os.path.join (bpm_gen2dir, 'keys')
    if not os.path.exists(bpm_key_dir):
        os.mkdir(bpm_key_dir)

        cmd = '%s genrsa -F4 -out %s/oem_privkey_2048.pem 2048' % (openssl_path, bpm_key_dir)
        run_process (cmd.split())

        cmd = '%s rsa -pubout -in %s/oem_privkey_2048.pem -out %s/oem_pubkey_2048.pem' % (openssl_path, bpm_key_dir, bpm_key_dir)
        run_process (cmd.split())

        cmd = '%s genrsa -F4 -out %s/bpm_privkey_2048.pem 2048' % (openssl_path, bpm_key_dir)
        run_process (cmd.split())

        cmd = '%s rsa -pubout -in %s/bpm_privkey_2048.pem -out %s/bpm_pubkey_2048.pem' % (openssl_path, bpm_key_dir, bpm_key_dir)
        run_process (cmd.split())

    print ("Generating BPM GEN2 params file")
    gen_bpmgen2_params(stitch_cfg_file, os.path.join(bpm_gen2dir, "sbl_bpmgen2.params"), os.path.join(output_dir, "sbl_bpmgen2.params"))

    print("Generating Btg KeyManifest.bin....")
    run_process_wrapper ([get_path_wrapper (os.path.join (bpm_gen2dir, 'BpmGen2.exe')),
        'KMGEN',
        '-KEY',        get_path_wrapper (os.path.join (bpm_key_dir, 'bpm_pubkey_2048.pem')), 'BPM',
        '-KM',         get_path_wrapper (os.path.join (output_dir,  'KeyManifest.bin')),
        '-SIGNKEY',    get_path_wrapper (os.path.join (bpm_key_dir, 'oem_privkey_2048.pem')),
        '-SIGNPUBKEY', get_path_wrapper (os.path.join (bpm_key_dir, 'oem_pubkey_2048.pem')),
        '-KMID',       '0x01',
        '-KMKHASH',    'SHA256',
        '-SVN',        '0',
        '-d:2'])

    print("Generating Btg Boot Policy Manifest (BPM).bin....")
    run_process_wrapper ([get_path_wrapper (os.path.join (bpm_gen2dir, 'BpmGen2.exe')),
        'GEN',
        get_path_wrapper (os.path.join (output_dir, 'sbl_sec_temp.bin')),
        get_path_wrapper (os.path.join (output_dir, 'sbl_bpmgen2.params')),
        '-BPM',        get_path_wrapper (os.path.join (output_dir, 'BpmManifest.bin')),
        '-U',          get_path_wrapper (os.path.join (output_dir, 'sbl_sec.bin')),
        '-KM',         get_path_wrapper (os.path.join (output_dir, 'KeyManifest.bin')),
        '-d:2'])

def update_tpm_type(tpm_type, tree):

  ptt_supported_xml_entry     = "./PlatformProtection/IntelPttConfiguration/PttSupported"
  ptt_pwrup_state_xml_entry   = "./PlatformProtection/IntelPttConfiguration/PttPwrUpState"
  ptt_supported_fpf_xml_entry = "./PlatformProtection/IntelPttConfiguration/PttSupportedFpf"
  spi_over_tpmbus_xml_entry   = "./PlatformProtection/TpmOverSpiBusConfiguration/SpiOverTpmBusEnable"

  tpm_config = {
    'ptt': {
              ptt_supported_xml_entry: 'Yes',
              ptt_pwrup_state_xml_entry: 'Enabled',
              ptt_supported_fpf_xml_entry: 'Yes',
              spi_over_tpmbus_xml_entry: 'No'
            },
    'dtpm': {
              ptt_supported_xml_entry: 'No',
              ptt_pwrup_state_xml_entry: 'Disabled',
              ptt_supported_fpf_xml_entry: 'Yes',
              spi_over_tpmbus_xml_entry:'Yes'
            },
    'none': {
              ptt_supported_xml_entry: 'No',
              ptt_pwrup_state_xml_entry: 'Disabled',
              ptt_supported_fpf_xml_entry: 'Yes',
              spi_over_tpmbus_xml_entry: 'No'
            },
    }

  #Enable TPM/PTT device
  for key in tpm_config[tpm_type]:
    node = tree.find(key)
    if node is not None:
      node.attrib['value'] = tpm_config[tpm_type][key]

  print("[INFO] TPM device type enabled for Measured Boot: %s" % tpm_type)

def update_btGuard_xml(btg_profile, stitch_dir, tree):
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

    # Convert OEM key hash into hexadecimal for Fit tool consumption
    with open(kmsigpubkeyhashfile,"rb") as kmsigpubkeyhash_fh:
        hash = bytearray (kmsigpubkeyhash_fh.read())
        oemkeyhash = ""
        for b in hash:
            oemkeyhash = oemkeyhash + "%02X " % b
    print("oemkeyhash:")
    print(oemkeyhash)

    node = tree.find('./PlatformProtection/BootGuardConfiguration/BtGuardKeyManifestId')
    node.attrib['value'] = '0x1'

    node = tree.find('./PlatformProtection/PlatformIntegrity/OemPublicKeyHash')
    node.attrib['value'] = oemkeyhash

    node = tree.find('./PlatformProtection/BootGuardConfiguration/BtGuardProfileConfig')
    node.attrib['value'] = btg_profile_values[btguardprofile]

def update_btGuard_manifests(stitch_dir, stitch_cfg_file):
    output_dir = os.path.join(stitch_dir, "Temp")
    sbl_file = os.path.join (output_dir, 'SlimBootloader.bin')

    #print("Replace components in both partitions....")
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
    gen_sign_oem_key_manifest(stitch_dir, stitch_cfg_file)

def gen_xml_file(stitch_dir, stitch_cfg_file, btg_profile, spi_quad, platform, tpm):
    print ("Generating xml file .........")

    fit_tool     = os.path.join (stitch_dir, 'Fit', 'fit')
    new_xml_file = os.path.join (stitch_dir, 'Temp', 'new.xml')
    updated_xml_file = os.path.join (stitch_dir, 'Temp', 'updated.xml')

    sku = stitch_cfg_file.get_platform_sku().get(platform)
    run_process_wrapper ([get_path_wrapper (fit_tool), '-sku', sku,
        '-save', get_path_wrapper (new_xml_file),
        '-w', get_path_wrapper (os.path.join (stitch_dir, 'Temp'))])

    tree = ET.parse(new_xml_file)

    xml_change_list = stitch_cfg_file.get_xml_change_list (platform, spi_quad)
    for each in xml_change_list:
        for xml_path, value in each:
            node = tree.find('%s' % xml_path)
            node.set('value', value)
            print (value)

    update_btGuard_manifests(stitch_dir, stitch_cfg_file)
    update_btGuard_xml(btg_profile, stitch_dir, tree)
    update_tpm_type(tpm, tree)
    tree.write(updated_xml_file)

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
        sblopen_dir = os.path.join(os.path.abspath(os.path.dirname(__file__)), '../../../../', 'SblOpen')
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
                                    '-f', comp_file, '-c', comp_alg, '-k', pri_key, '-od', work_dir, '-td', tool_bin_dir]
        run_process (cmd_line, True)
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

def stitch (stitch_dir, stitch_cfg_file, stitch_zip, btg_profile, spi_quad_mode, platform_data, platform, tpm, full_rdundant = True):
    temp_dir = os.path.abspath(os.path.join (stitch_dir, 'Temp'))
    if os.path.exists(temp_dir):
        shutil.rmtree(temp_dir, ignore_errors=True)
    shutil.copytree (os.path.join (stitch_dir, 'Input'), temp_dir)

    print ("\nUnpack files from stitching zip file ...")
    zf = zipfile.ZipFile(stitch_zip, 'r', zipfile.ZIP_DEFLATED)
    zf.extractall(temp_dir)
    zf.close()

    if platform_data:
        fd = open(os.path.join(temp_dir, "SlimBootloader.bin"), "rb")
        input_data = bytearray(fd.read())
        fd.close()
        print ("\n Adding platform data to Slimbootloader ...")
        data = add_platform_data(input_data, platform_data)
        fd = open(os.path.join(temp_dir, "SlimBootloader.bin"), "wb")
        fd.write(data)
        fd.close()

    gen_xml_file(stitch_dir, stitch_cfg_file, btg_profile, spi_quad_mode, platform, tpm)

    print ("Run fit tool to generate ifwi.........")
    run_process_wrapper ([get_path_wrapper ('./Fit/fit'), '-b', '-o', get_path_wrapper ('Temp/Ifwi.bin'),
        '-f', get_path_wrapper (os.path.join (temp_dir, 'updated.xml')),
        '-s', get_path_wrapper (temp_dir), '-w', get_path_wrapper (temp_dir), '-d', get_path_wrapper (temp_dir)])
    return 0

def main():
    hexstr = lambda x: int(x, 16)
    ap = argparse.ArgumentParser()
    ap.add_argument('-p', dest='platform', default = '', help='specify platform to stitch')
    ap.add_argument('-w', dest='work_dir', default = '', help='specify stitch workspace directory')
    ap.add_argument('-c', dest='config_file', type=str, required=True, help='specify the platform specific stitch config file')
    ap.add_argument('-s', dest='sbl_zip_file', type=str, default='stitch_Components.zip', help='specify slim bootloader zip file')
    ap.add_argument('-b', dest='btg_profile', default = 'vm', choices=['legacy', 'vm', 'fve', 'fvme'], help='specify Boot Guard profile type')
    ap.add_argument('-d', dest='plat_data', type=hexstr, default=None, help='Specify a platform specific data (HEX, DWORD) for customization')
    ap.add_argument('-q', dest='spi_quad_mode', action = "store_true", default = False, help = "enable SPI QUAD mode")
    ap.add_argument('-r', dest='remove', action = "store_true", default = False, help = "delete temporary files after stitch")
    ap.add_argument('-t', dest='tpm', default = 'ptt', choices=['ptt', 'dtpm', 'none'], help='specify TPM type')
    ap.add_argument('-fusa', dest='fusa', action = "store_true", default = False, help = "Patch IFWI to generate Fusa ifwi")
    ap.add_argument('-op', dest='outpath', default = '', help = "Specify path to write output IFIW and signed bin files")

    args = ap.parse_args()

    stitch_cfg_file = load_source('StitchIfwiConfig', args.config_file)
    if args.work_dir == '':
        print ("Please specify stitch work directory")
        print ('%s' % stitch_cfg_file.extra_usage_txt)
        return 1
    if args.platform == '' or args.platform not in stitch_cfg_file.get_platform_sku():
        print ("Please specify platform from the list: %s" % stitch_cfg_file.get_platform_sku().keys())
        return 1
    if args.btg_profile in ["vm","fvme"] and args.tpm == "none":
        print ("ERROR: Choose appropriate Tpm type for BootGuard profile 3 and 5")
        return 1

    print ("Executing stitch.......")
    curr_dir = os.getcwd()
    sbl_zip_file = os.path.abspath(os.path.join (curr_dir, args.sbl_zip_file))

    work_dir = os.path.abspath (args.work_dir)
    os.chdir(work_dir)
    if stitch (work_dir, stitch_cfg_file, sbl_zip_file, args.btg_profile, args.spi_quad_mode, args.plat_data, args.platform, args.tpm):
        raise Exception ('Stitching process failed !')
    os.chdir(curr_dir)

    generated_ifwi_file = os.path.join(work_dir, 'Temp', 'Ifwi.bin')
    ifwi_file_name = os.path.join(args.outpath,'sbl_ifwi_%s.bin' % (args.platform))
    shutil.copy(generated_ifwi_file, ifwi_file_name)

    generated_signed_sbl =  os.path.join(work_dir, 'Temp', 'SlimBootloader.bin')
    sbl_file_name = os.path.join(args.outpath,'SlimBootloader_%s.bin' % (args.platform))
    shutil.copy(generated_signed_sbl, sbl_file_name)

    if args.fusa:
        print ("patch IFWI to generate Fusa ifwi")
        with open(ifwi_file_name, "r+b") as fh:
            fh.seek(0xC76)
            fh.write(b'\x6a')
            fh.seek(0xC18)
            fh.write(b'\x01')

    print ("\nIFWI Stitching completed successfully !")
    print ("Boot Guard Profile: %s" % args.btg_profile.upper())
    print ("IFWI image: %s\n" % ifwi_file_name)
    if args.remove:
        shutil.rmtree(os.path.join(work_dir, 'Temp'), ignore_errors=True)
    return 0

if __name__ == '__main__':
    sys.exit(main())
