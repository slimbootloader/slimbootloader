## @ StitchIfwi.py
#  This is a python stitching script for Slim Bootloader WHL/CFL build
#
# Copyright (c) 2019 - 2023, Intel Corporation. All rights reserved.<BR>
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
import subprocess
import xml.etree.ElementTree as ET
from   xml.dom import minidom
from   ctypes  import *
from   StitchLoader import *
from   subprocess   import call
sys.dont_write_bytecode = True

sblopen_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../'))
if not os.path.exists (sblopen_dir):
    sblopen_dir = os.getenv('SBL_SOURCE', '')
if not os.path.exists (sblopen_dir):
    raise  Exception("Please set env 'SBL_SOURCE' to SBL open source root folder")

btg_profile_values = [\
                    "Boot Guard Profile 0 - No_FVME",\
                    "Not Applicable",\
                    "Not Applicable",\
                    "Boot Guard Profile 3 - VM",\
                    "Boot Guard Profile 4 - FVE",\
                    "Boot Guard Profile 5 - FVME"]

extra_usage_txt = \
"""This is an IFWI stitch config script for Slim Bootloader For the FIT tool and
stitching ingredients listed in step 2 below, please contact your Intel representative.

  1. Create a stitching workspace directory. The paths mentioned below are all
     relative to it.

  2. Extract required tools and ingredients to stitching workspace.
     - FIT tool
       Copy 'fit.exe' or 'fit' and 'vsccommn.bin' to 'Fit' folder
     - BPMGEN2 Tool
       Copy the contents of the tool to Bpmgen2 folder
       Rename the bpmgen2 parameter to bpmgen2.params if its name is not this name.
      - Components
       Copy 'cse_image.bin'     to 'Input/cse_image.bin'
       Copy PMC firmware image  to 'Input/pmc.bin'.
       Copy EC firmware image   to 'Input/ec.bin'.
       copy ECregionpointer.bin to 'Input/ecregionpointer.bin'
       Copy GBE binary image    to 'Input/gbe.bin'.
       Copy ACM firmware image  to 'Input/acm.bin'.

  3. Openssl
       Openssl is required for stitch. the stitch tool will search evn OPENSSL_PATH,
       to find Openssl. If evn OPENSSL_PATH is not found, will find openssl from
       "C:\\Openssl\\Openssl"

  4. Stitch the final image
       EX:
       Assuming stitching workspace is at D:\Stitch and building ifwi for WHL platform
       To stitch IFWI with Boot Guard profile VM:
         StitchIfwi.py -b vm -p whl -w D:\Stitch -s Stitch_Components.zip -c StitchIfwiConfig.py

"""

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
            print ("       To stitch CFL IFWI in Linux, 'wine' is required")
            print ("\n")
            raise Exception ()
    run_process (cmds)

def gen_bpmgen2_params (stitch_cfg_file, InFile, OutFile):
    InFileptr = open(InFile, 'r', encoding='utf8')
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

    bpm_gen2dir = os.path.join (stitch_dir, 'bpmgen2')
    bpm_key_dir = os.path.join (bpm_gen2dir, 'keys')
    if not os.path.exists(bpm_key_dir):
        os.mkdir(bpm_key_dir)

        cmd = '%s genrsa -F4 -out %s/keyprivkey.pem 2048' % (openssl_path, bpm_key_dir)
        run_process (cmd.split())

        cmd = '%s rsa -pubout -in %s/keyprivkey.pem -out %s/keypubkey.pem' % (openssl_path, bpm_key_dir, bpm_key_dir)
        run_process (cmd.split())

        cmd = '%s genrsa -F4 -out %s/privkey.pem 2048' % (openssl_path, bpm_key_dir)
        run_process (cmd.split())

        cmd = '%s rsa -pubout -in %s/privkey.pem -out %s/pubkey.pem' % (openssl_path, bpm_key_dir, bpm_key_dir)
        run_process (cmd.split())

    print ("Generating BPM GEN2 params file")
    gen_bpmgen2_params(stitch_cfg_file, os.path.join(bpm_gen2dir, "Example.bpDef"), os.path.join(output_dir, "bpmgen2.params"))

    print("Generating Btg KeyManifest.bin....")
    run_process_wrapper ([get_path_wrapper (os.path.join (bpm_gen2dir, 'bpmgen2')),
        'KM1GEN',
        '-KEY',        os.path.join (bpm_key_dir, 'pubkey.pem'), 'BPM',
        '-KM',         os.path.join (output_dir,  'KeyManifest.bin'),
        '-SIGNKEY',    os.path.join (bpm_key_dir, 'keyprivkey.pem'),
        '-SIGNPUBKEY', os.path.join (bpm_key_dir, 'keypubkey.pem'),
        '-KMID',       '0x01',
        '-SVN',        '0',
        '-d:2'])

    print("Generating Btg Boot Policy Manifest (BPM).bin....")
    run_process_wrapper ([get_path_wrapper (os.path.join (bpm_gen2dir, 'bpmgen2')),
        'GEN',
        os.path.join (output_dir, 'sbl_sec_temp.bin'),
        os.path.join (output_dir, 'bpmgen2.params'),
        '-BPM',        os.path.join (output_dir, 'Manifest.bin'),
        '-U',          os.path.join (output_dir, 'sbl_sec.bin'),
        '-KM',         os.path.join (output_dir, 'KeyManifest.bin'),
        '-d:2'])

def gen_oem_key_hash(stitch_dir):

    output_dir = os.path.join(stitch_dir, "Temp")
    kmsigpubkeytxtfile  = os.path.join(output_dir, "kmsigpubkey.txt")
    kmsigpubkeybinfile  = os.path.join(output_dir, "kmsigpubkey.bin")

    with open(kmsigpubkeytxtfile, "w") as kmsigpubkeytxt_fh:
        run_process ([get_openssl_path(), 'rsa' , '-in', 'bpmgen2/keys/keypubkey.pem',
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
    print("Sigining Coffeelake....")

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

def gen_xml_file(stitch_dir, stitch_cfg_file, btg_profile, platform, tpm):

    print ("Generating xml file .........")

    fit_tool     = os.path.join (stitch_dir, 'Fit', 'fit')
    new_xml_file = os.path.join (stitch_dir, 'Temp', 'new.xml')
    updated_xml_file = os.path.join (stitch_dir, 'Temp', 'updated.xml')
    sku = stitch_cfg_file.get_platform_sku().get(platform)
    cmd = [get_path_wrapper(fit_tool), '-sku', sku, '-save', new_xml_file, '-w', os.path.join (stitch_dir, 'Temp')]
    run_process_wrapper (cmd)

    tree = ET.parse(new_xml_file)

    xml_change_list = stitch_cfg_file.get_xml_change_list (platform)
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

        if os.name == 'nt':
            tool_bin_dir  = os.path.join(sblopen_dir, "BaseTools", "Bin", "Win32")
        else:
            tool_bin_dir  = os.path.join(sblopen_dir, "BaseTools", "BinWrappers", "PosixLike")
        gen_container = os.path.join(sblopen_dir, "BootloaderCorePkg" , "Tools", "GenContainer.py")
        if not os.path.isabs(pri_key):
            pri_key = os.path.join (work_dir, pri_key)
        cmd_line = [sys.executable, gen_container, 'replace', '-i', container_file, '-o', container_file, '-n', comp_name,
                                    '-f', comp_file, '-c', comp_alg, '-k', pri_key, '-td', tool_bin_dir]
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

def stitch (stitch_dir, stitch_cfg_file, sbl_file, btg_profile, platform_data, platform, tpm, full_rdundant = True):

    temp_dir = os.path.abspath(os.path.join (stitch_dir, 'Temp'))
    if os.path.exists(temp_dir):
        shutil.rmtree(temp_dir, ignore_errors=True)
    shutil.copytree (os.path.join (stitch_dir, 'Input'), temp_dir)

    # Get bios region image ready
    sbl_image_ext = os.path.splitext(sbl_file)

    if sbl_image_ext[1] != ".zip":
        print ("\nCopy SBL image %s for stitch" % sbl_file)
        shutil.copy(sbl_file, os.path.join(temp_dir, "SlimBootloader.bin"))
    else:
        print ("\nUnpack files from zip file ...")
        zf = zipfile.ZipFile(sbl_file, 'r', zipfile.ZIP_DEFLATED)
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

    gen_xml_file(stitch_dir, stitch_cfg_file, btg_profile, platform, tpm)

    print ("Run fit tool to generate ifwi.........")
    run_process_wrapper ([get_path_wrapper ('./Fit/fit'), '-b', '-o', 'Temp/Ifwi.bin', '-f', os.path.join (temp_dir, 'updated.xml'),
        '-s', temp_dir, '-w', temp_dir, '-d', temp_dir])
    return 0
#    cmd = './fit -b -o Ifwi.bin -f Platform.xml'
#    run_cmd (cmd, os.path.join(stitch_dir, cfg_var['fitinput']))
#    return 0

def clean (stitch_dir, dist_mode):
    print ("Clean up workspace ...")

    cfg_var    = get_config ()
    for each in [cfg_var['wkspace']]:
        each = stitch_dir + '/' + each
        if os.path.exists(each):
            shutil.rmtree(each)

    fit_dir = stitch_dir + '/' + os.path.dirname(cfg_var['fit'])
    patterns = [
      fit_dir    + '/*.log',
      fit_dir    + '/*_sbl.xml',
    ]

    for pattern in patterns:
        for file in glob.glob(pattern):
            os.remove (file)

    return 0

def main():

    if len(sys.argv) == 1:
        print ('%s' % extra_usage_txt)

    hexstr = lambda x: int(x, 16)

    ap = argparse.ArgumentParser()
    ap.add_argument('-w',
                    '--stitch-work-dir',
                    dest='stitch_dir',
                    type=str,
                    required=True,
                    help='specify workspace directory for stitching')

    ap.add_argument('-s',
                    '--stitch-zip-file',
                    dest='sbl_file',
                    type=str,
                    default='Outputs/cfl/Stitch_Components.zip',
                    help='specify input stitching zip package file path')

    ap.add_argument('-b',
                    '--boot-guard-profile',
                    default = 'vm',
                    choices=['legacy', 'vm', 'fve', 'fvme'],
                    dest='btg_profile',
                    help='specify Boot Guard profile type')

    ap.add_argument('-q',
                    '--spi-quad-mode',
                    dest='quad_mode',
                    action = "store_true",
                    default = False,
                    help = "enable SPI QUAD mode")

    ap.add_argument('-c',
                    '--clean',
                    dest='clean',
                    action = "store_true",
                    default = False,
                    help = "clean stitching workspace")

    ap.add_argument('-a',
                    '--platform',
                    default = 'whl',
                    choices=['whl', 'cfls', 'cflh'],
                    dest='platform',
                    help='specify platform')

    ap.add_argument('-p',
                    '--platform-data',
                    dest='plat_data',
                    type=hexstr,
                    default=None,
                    help='Specify a platform specific data (HEX, DWORD) for customization')

    ap.add_argument('-t',
                    '--tpm-type',
                    dest='tpm',
                    default = 'ptt',
                    choices=['ptt', 'dtpm', 'none'],
                    help='specify TPM type')

    ap.add_argument('-cfg',
                    dest='config_file',
                    type=str, required=True,
                    help='specify the platform specific stitch config file')

    ap.add_argument('-op',
                     dest='outpath',
                     default = '',
                     help = "Specify path to write output IFIW and signed bin files")

    args = ap.parse_args()

    stitch_cfg_file = load_source('StitchIfwiConfig', args.config_file)
    if args.stitch_dir == '':
        print ("Please specify stitch work directory")
        print ('%s' % stitch_cfg_file.extra_usage_txt)
        return 1

    sku_dict = stitch_cfg_file.get_platform_sku()
    if len (sku_dict) == 1 and args.platform == '':
        for sku in sku_dict:
            args.platform =  sku
            print ("No sku is given, set to default sku value %s" % sku)
    if args.platform == '' or args.platform not in sku_dict:
        print ("Invalid sku (%s), Please provide valid sku:" % args.platform)
        for sku in sku_dict :
            print (" %s - 'For %s'" % (sku, sku_dict[sku]))
        return 1

    if args.btg_profile in ["vm","fvme"] and args.tpm == "none":
        print ("ERROR: Choose appropriate Tpm type for BootGuard profile 3 and 5")
        return 1

    if args.clean:
        shutil.rmtree(os.path.join(stitch_dir, 'Temp'), ignore_errors=True)
        print ("Cleaning completed successfully !\n")
        return 1

    print ("Executing stitch.......")
    curr_dir = os.getcwd()
    sbl_file = os.path.abspath(os.path.join (curr_dir, args.sbl_file))

    stitch_dir = os.path.abspath (args.stitch_dir)
    os.chdir(stitch_dir)
    if stitch (stitch_dir, stitch_cfg_file, sbl_file, args.btg_profile, args.plat_data, args.platform, args.tpm):
        raise Exception ('Stitching process failed !')
    os.chdir(curr_dir)

    generated_ifwi_file = os.path.join(stitch_dir, 'Temp', 'Ifwi.bin')
    ifwi_file_name = os.path.join(args.outpath,'sbl_ifwi_%s.bin' % (args.platform))
    shutil.copy(generated_ifwi_file, ifwi_file_name)

    generated_signed_sbl =  os.path.join(stitch_dir, 'Temp', 'SlimBootloader.bin')
    sbl_file_name = os.path.join(args.outpath,'SlimBootloader_%s.bin' % (args.platform))
    shutil.copy(generated_signed_sbl, sbl_file_name)

    print ("\nIFWI Stitching completed successfully !")
    print ("Boot Guard Profile: %s" % args.btg_profile.upper())
    print ("IFWI image: %s\n" % ifwi_file_name)

    return 0

if __name__ == '__main__':
    sys.exit(main())


