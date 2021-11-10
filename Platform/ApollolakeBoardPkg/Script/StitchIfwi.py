## @ StitchIfwi.py
#  This is a python stitching script for Slim Bootloader APL build
#
# Copyright (c) 2019, Intel Corporation. All rights reserved. <BR>
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
from   subprocess   import check_output
from   collections  import OrderedDict

sys.dont_write_bytecode = True
from   StitchLoader import *

extra_usage_txt = \
"""This script creates a new Apollo Lake Slim Bootloader IFWI image using FIT
tool.  For the FIT tool and stitching ingredients listed in step 2 below, please
contact your Intel representative to get them.

Please follow the steps below for APL-I IFWI stitching.

  1. Create a stitching workspace. The paths mentioned below are all relative
     to it.

  2. Extract required tools and ingredients to stitching workspace.
     - FIT tool (APL-I Trusted Execution Engine firmware package)
       Copy 'fit.exe' or 'fit' and 'vsccommn.bin' to 'CSE/FIT' folder
       Copy 'meu.exe' or 'meu' to 'CSE/MEU' folder
       Copy 'cse_image.bin' to 'CSE/Silicon/cse_image.bin'
       For Linux, please ensure 'fit' and 'meu' have execution permission.

     - ISH firmware (APL Windows 10 Best Known Configuration package)
       Copy ISHC firmware image (EX: 'Production_ishC_4.1.0.3364.bin')
            to 'ISH/ISHC.bin'.
       Copy ISHC data image (EX: 'INTC_pdt_BXT_P_NS_BOM3_SENSORS')
            to 'ISH/INTC_pdt_APL_NS_BOM3_SENSORS'.

     - PMC firmware (APL Windows 10 Best Known Configuration package)
       Copy PMC firmware image (EX: 'v03_1a_00000000_prod_signed.bin')
            to 'PMC/pmcp.bin'.

     - IUnit and FIT template XML (Latest APL SOC FSP release package)
       Copy 'IUnit.bin' in BlStitch.7z to 'IUnit/IUnit.bin'
       Copy 'bxt_spi_8mb.xml' in BlStitch.7z to 'CSE/FIT/bxt_spi_8mb.xml'
       Please note, this XML is for Intel's APL LeafHill CRB platform.
       For other boards, plese use FIT to modify this XML accordingly.

  3. Extract additional public released ingredients to stitching workspace.
     - Audio firmware
       Please download the audio firmware from:
       https://downloadcenter.intel.com/download/26895
       After downloading, please extract and copy the DSP FW image into
       'Audio/dsp_fw.bin'

     - Microcode
       Please download the latest microcode package from:
       https://downloadcenter.intel.com/download/28087
       Current APL SOC needs microcode intel-ucode/06-5c-09 (APL D0) and 06-5c-0a (APL E0)
       After downloading, please extract and copy '06-5c-09' and '06-5c-0a 'into 'Patch'
       folder in stitching workspace.

     - Keys
       This is the private key for signing. Here, a test key is used for demonstration.
       Please download the test key TestSigningPrivateKey.pem from:
       https://github.com/slimbootloader/slimbootloader/tree/master/BootloaderCorePkg/Tools/Keys
       and copy it to 'Keys/bxt_dbg_priv_key.pem'

     - Openssl
       For Windows environment, please download required OpenSSL tool.
       For example, it can be downloaded from:
       https://sourceforge.net/projects/openssl
       Once downloaded, please extract the executable openssl.exe and configuration file
       openssl.cnf into 'Openssl' folder in stitching workspace.
       For Linux environment, please install openssl package directly.

  4. Stitch the final image
     Run this script from Slim Bootloader root directory:
     EX:
       Assume stitching workspace is at D:\Stitch
       To stitch IFWI with SPI QUAD mode and Boot Guard profile VM:
         python  Platform\ApollolakeBoardPkg\Script\StitchIfwi.py -b vm -q -w D:\Stitch
       To clean all generated files:
         python  Platform\ApollolakeBoardPkg\Script\StitchIfwi.py -c -w D:\Stitch

"""

def get_config ():
    # This dictionary defines path for stitching components
    cfg_var = {
      'wkspace'   :   'output',
      'fitinput'  :   'Output/input',
      'ifwiname'  :   'APL_SBL_IFWI.bin',
      'fitxml'    :   'CSE/FIT/bxt_spi_8mb.xml',
      'fit'       :   'CSE/FIT/fit%s' %     ('.exe' if os.name == 'nt' else ''),
      'meu'       :   'CSE/MEU/meu%s' %     ('.exe' if os.name == 'nt' else ''),
      'openssl'   :   'Openssl/openssl.exe' if os.name == 'nt' else '/usr/bin/openssl',
      'audio'     :   'Audio/dsp_fw.bin',
      'cseimg'    :   'CSE/Silicon/cse_image.bin',
      'ishdata'   :   'ISH/INTC_pdt_APL_NS_BOM3_SENSORS',
      'ishimg'    :   'ISH/ISHC.bin',
      'iunit'     :   'IUnit/IUnit.bin',
      'pmc'       :   'PMC/pmcp.bin',
      'patch1'    :   'Patch/06-5c-09',
      'patch2'    :   'Patch/06-5c-0a',
      'keys'      :   'Keys/bxt_dbg_priv_key.pem',
    }
    return cfg_var



def copy_primary_bpdt (ifwi_file, output_file = ''):
    fd = open(ifwi_file, "rb")
    ifwi_img_data = bytearray(fd.read())
    fd.close()

    ret = manipulate_ifwi  ('COPY', '', ifwi_img_data)
    if ret != 0:
        return -1

    if output_file == '':
        output_file = ifwi_file

    fd = open(output_file, "wb")
    fd.write(ifwi_img_data)
    fd.close()

    return 0


def patch_ibb_flash_map (ibbl_file, ifwi_file, platform_data, output_file = ''):
    fd = open(ifwi_file, "rb")
    ifwi_img_data = bytearray(fd.read())
    fd.close()
    ifwi = patch_flash_map (ifwi_img_data, platform_data)
    if ifwi:
        return -1

    if output_file == '':
        output_file = ibbl_file

    ifwi = IFWI_PARSER.parse_ifwi_binary (ifwi_img_data)
    if not ifwi:
        return -2

    # Locate IBBL in both BP0 and BP1
    for part in range(2):
        path = "IFWI/BIOS/BP%d/BPDT/BpdtIbb/IBBL" % part
        ibbl = IFWI_PARSER.locate_component (ifwi, path)
        if not ibbl:
            if part == 0:
                return -3
            continue

        ibbl_img_data = ifwi_img_data[ibbl.offset:ibbl.offset + ibbl.length]

        if part == 1:
            output_file = output_file[0:-4] + "_backup.bin"

        fd = open(output_file, "wb")
        fd.write(ibbl_img_data)
        fd.close()

    return 0

def replace_ibb_partition (bios_image, ifwi_image, output_image, bios_path, ifwi_path):
    fd = open(bios_image, "rb")
    bios_img_data = bytearray(fd.read())
    fd.close()
    bios = IFWI_PARSER.parse_ifwi_binary (bios_img_data)
    if not bios:
        return -1

    fd = open(ifwi_image, "rb")
    ifwi_img_data = bytearray(fd.read())
    fd.close()
    ifwi = IFWI_PARSER.parse_ifwi_binary (ifwi_img_data)
    if not ifwi:
        return -2

    bios_comp = IFWI_PARSER.locate_component (bios, bios_path)
    if not bios_comp:
        return -3

    ifwi_comp = IFWI_PARSER.locate_component (ifwi, ifwi_path)
    if not ifwi_comp:
        return -4

    if bios_comp.length != ifwi_comp.length:
        return -5

    ifwi_img_data[ifwi_comp.offset:ifwi_comp.offset + ifwi_comp.length] = \
        bios_img_data[bios_comp.offset:bios_comp.offset + bios_comp.length]

    if output_image == '':
        output_image = ifwi_image

    fd = open(ifwi_image, "wb")
    fd.write(ifwi_img_data)
    fd.close()

    return 0


def padding_ias_image (ibbl_image):
    fd = open(ibbl_image, "rb")
    ibbl_img_data = bytearray(fd.read())
    fd.close()

    # Locate FlashMap offset
    fla_map_off = (bytes_to_value(ibbl_img_data[-8:-4]) + len(ibbl_img_data)) & 0xFFFFFFFF
    fla_map_str = FLASH_MAP.from_buffer (ibbl_img_data, fla_map_off)
    entry_num  = (fla_map_str.length - sizeof(FLASH_MAP)) // sizeof(FLASH_MAP_DESC)
    for idx in range (entry_num):
        desc  = FLASH_MAP_DESC.from_buffer (ibbl_img_data, fla_map_off + sizeof(FLASH_MAP) + idx * sizeof(FLASH_MAP_DESC))
        if desc.sig != 'IAS1' :
            continue
        ias1name  = os.path.join(os.path.dirname (ibbl_image), 'Stitch_FB.bin')
        if not os.path.exists(ias1name):
            ias1size = 0
            mode     = 'wb'
        else:
            ias1size = os.path.getsize(ias1name)
            mode     = 'r+b'
        if desc.size > ias1size:
            ias_fh = open (ias1name, mode)
            ias_fh.seek (ias1size)
            ias_fh.write ('\xff' * (desc.size - ias1size))
            ias_fh.close()
        if desc.size < ias1size:
            raise Exception("ERROR: IAS1 image 0x%x bigger than size 0x%x in FlashMap!" % (ias1size, desc.size))

    return 0


def run_cmd (cmd, cwd):
    sys.stdout.flush()
    try:
        cmd_args = shlex.split(cmd)
        if os.name == 'nt':
            shell = True
            if cmd_args[0].startswith('./'):
                cmd_args[0] = cmd_args[0][2:]
        else:
            shell = False
        proc = subprocess.Popen(cmd_args, shell = shell, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = proc.communicate()
        out = out.decode()
        err = err.decode()
        ret = proc.returncode
    except Exception as e:
        out, err = '', e
        ret = 1
    if ret:
        sys.stdout.flush()
        print('%s\n%s' % (out, err))
        print("Failed to run command:\n  PATH: %s\n   CMD: %s" % (cwd, cmd))
        sys.exit(1)


def rebuild_modules (node, mod_list):
    for child in list(node):
        node.remove (child)
    for mod, file_name in mod_list:
        child  = ET.SubElement(node, "DataModule")
        child.attrib['name'] = mod
        child2 = ET.SubElement(child, "InputFile")
        child2.attrib['value'] = "$SourceDir/Stitch_%s.bin" % file_name


def xml_write (tree, file):
    text = ET.tostring(tree.getroot(), 'utf-8')
    reparsed  = minidom.parseString(text)
    formatted = reparsed.toprettyxml(indent='    ', newl='\n', encoding="utf-8")
    fp = open (file, "w")
    for line in formatted.splitlines():
        if line.strip():
            fp.write ('%s\n' % (line.decode() if sys.hexversion >= 0x3000000 else line))
    fp.close()


def gen_meu_oem_key_xml (meu_dir, src, dst):
    cfg_var = get_config ()
    tree = ET.parse(meu_dir + '/%s' % src)
    node = tree.find('./KeyManifestEntries')
    for child in list(node):
        node.remove (child)

    manifest_list = [
      ('IfwiManifest | OemSmipManifest | OemDnxIfwiManifest | BootPolicyManifest' ,
        'bxt_dbg_pub_key_hash.bin'),
      ('cAvsImage0Manifest | cAvsImage1Manifest' ,
        'bxt_prod_pub_key_hash_Audio.bin'),
      ('iUnitBootLoaderManifest | iUnitMainFwManifest' ,
        'bxt_prod_pub_key_hash_iUnit.bin'),
      ('ish_manifest' ,
        'bxt_prod_pub_key_hash_ISH.bin')
    ]

    prefix = '../..'
    for manifest, file_name in manifest_list:
        child  = ET.SubElement(node, "KeyManifestEntry")
        child2 = ET.SubElement(child, "Usage")
        child2.attrib['value'] = manifest
        child3 = ET.SubElement(child, "HashBinary")
        child3.attrib['value'] = '%s/%s/%s' % (prefix, cfg_var['fitinput'], file_name)

    xml_write (tree, meu_dir + '/%s' % dst)


def gen_meu_cfg_xml (meu_dir, src, dst):
    cfg_var = get_config ()
    tree = ET.parse(meu_dir + '/%s' % src)
    node = tree.find("./SigningConfig/SigningToolPath")
    if cfg_var['openssl'].startswith('/'):
        node.attrib['value'] = cfg_var['openssl']
    else:
        node.attrib['value'] = '../../' + cfg_var['openssl']
    node = tree.find("./SigningConfig/PrivateKeyPath")
    node.attrib['value'] = '../../' + cfg_var['keys']
    xml_write (tree, meu_dir + '/%s' % dst)


def gen_fit_xml (fit_dir, src, dst, btg_profile, spi_quad):
    cfg_var = get_config ()
    tree = ET.parse(fit_dir + '/%s' % src)

    # Change tool path
    node = tree.find("./BuildSettings/BuildResults/MeuToolPath")
    if os.name == 'nt':
        node.attrib['value'] = "../MEU/meu.exe"
    else:
        node.attrib['value'] = "../MEU/meu"
    node = tree.find("./BuildSettings/BuildResults/SigningToolPath")
    if cfg_var['openssl'].startswith('/'):
        node.attrib['value'] = cfg_var['openssl']
    else:
        node.attrib['value'] = '../../' + cfg_var['openssl']

    # Change region order
    node = tree.find('./BuildSettings/BuildResults/RegionOrder')
    node.attrib['value'] = '415'

    # Enable PDR region
    node = tree.find('./FlashLayout/SubPartitions/PdrRegion/Enabled')
    node.attrib['value'] = 'Enabled'
    node = tree.find('./FlashLayout/SubPartitions/PdrRegion/InputFile')
    node.attrib['value'] = '$SourceDir\input\pdr.bin'

    # Change SPI mode settings
    if spi_quad:
        value = 'Yes'
    else:
        value = 'No'
    print("Set SPI QUAD mode: %s" % value)
    node = tree.find('./FlashSettings/FlashConfiguration/QuadIoReadEnable')
    node.attrib['value'] = value
    node = tree.find('./FlashSettings/FlashConfiguration/QuadOutReadEnable')
    node.attrib['value'] = value

    # Change boot guard settings
    node = tree.find('./PlatformProtection/BootGuardConfiguration/BtGuardProfileConfig')
    profiles = node.attrib['value_list'].split(',,')
    if len(profiles) != 3:
        print("Expected Boot Guard profile list in XML !")
        return -1
    print("Set Boot Guard profile: %s" % btg_profile)
    if btg_profile == 'legacy':
        node.attrib['value'] = profiles[0]
    elif btg_profile == 'v':
        node.attrib['value'] = profiles[1]
    else:
        node.attrib['value'] = profiles[2]

    xml_write (tree, fit_dir + '/%s' % dst)


def gen_meu_sbl_xml (meu_dir, src, dst):
    tree = ET.parse(meu_dir + '/%s' % src)
    node = tree.find('./IbbSubPartition/Modules')
    mod_list = [
      ("IPAD",  "IPAD"),
      ("IBBL",  "IBBL"),
      ("IBB",   "IBBM"),
      ("OBB",   "OBB"),
      ("FWUP",  "FWU"),
      ("CFGD",  "CFGDATA"),
      ("KEYH",  "KEYHASH"),
      ("PLD",   "PLD"),
      ("VAR",   "VAR"),
      ("MRCD",  "MRCDATA")
      ]
    rebuild_modules (node, mod_list)

    node = tree.find("./ObbSubPartition/Modules")
    mod_list = [
      ("OPAD",  "OPAD"),
      ("FB",  "FB"),
      ("EPLD",  "EPLD"),
      ("UVAR",  "UVAR")
      ]
    rebuild_modules (node, mod_list)
    xml_write (tree, meu_dir + '/%s' % dst)


def clean (stitch_dir, dist_mode):
    print ("Clean up workspace ...")

    cfg_var    = get_config ()
    for each in [cfg_var['wkspace']]:
        each = stitch_dir + '/' + each
        if os.path.exists(each):
            shutil.rmtree(each)

    fit_dir = stitch_dir + '/' + os.path.dirname(cfg_var['fit'])
    meu_dir = stitch_dir + '/' + os.path.dirname(cfg_var['meu'])
    patterns = [
      fit_dir    + '/*.log',
      meu_dir    + '/*.log',
      fit_dir    + '/*_sbl.xml',
      meu_dir    + '/*.xml',
    ]

    for pattern in patterns:
        for file in glob.glob(pattern):
            os.remove (file)

    return 0


def stitch (stitch_dir, stitch_zip, btg_profile, spi_quad_mode, platform_data, full_rdundant = True):

    cfg_var    = get_config ()

    print ("\nUnpack files from stitching zip file ...")
    zf = zipfile.ZipFile(stitch_zip, 'r', zipfile.ZIP_DEFLATED)
    zf.extractall(os.path.join(stitch_dir, cfg_var['fitinput']))
    zf.close()

    os.chdir(stitch_dir)

    print ("\nPadding IAS image size ...")
    ret = padding_ias_image ('%s/Stitch_IBBL.bin' % cfg_var['fitinput'])
    if ret:
        raise Exception ("Failed with error %d !" % ret)

    print ("\nChecking and copying components ...")
    copy_list = ['cseimg', 'ishdata', 'ishimg', 'iunit', 'pmc', 'audio']
    for each in ['fit', 'meu', 'openssl', 'fitxml'] + copy_list:
        if not os.path.exists(cfg_var[each]):
             raise Exception ("Could not find file '%s' !" % cfg_var[each])

    for each in copy_list:
        shutil.copy (cfg_var[each], cfg_var['fitinput'])
    shutil.copy (cfg_var['patch1'], cfg_var['fitinput'] + '/upatch1.bin')
    shutil.copy (cfg_var['patch2'], cfg_var['fitinput'] + '/upatch2.bin')
    key_dir = cfg_var['wkspace'] + '/keys'
    if not os.path.exists(key_dir):
        os.mkdir(key_dir)
    shutil.copy (cfg_var['keys'],   key_dir)

    # SMIP_IAFW is not used by SBL, create a dummy one for stitching.
    print ("\nGenerating smip_iafw.bin ...")
    output_dir = stitch_dir + '/' + cfg_var['fitinput']
    fp = open (output_dir + '/smip_iafw.bin', 'wb')
    fp.write (b'\xAF\xBE\xED\xDE' + b'\x00' * 0x380 + b'\xAA\xCC\xFF\xAA')
    fp.close()

    # PDR region is not used by SBL, here create one for stitching.
    input_pdr = stitch_dir + '/%s/Stitch_PDR.bin' % cfg_var['fitinput']
    output_pdr = output_dir  + '/pdr.bin'
    if not os.path.exists(input_pdr):
        print ("\nGenerating pdr.bin ...")
        fp = open (output_pdr, 'wb')
        fp.write (b'\xff' * 0x1000)
        fp.close()
    else:
        shutil.copy (input_pdr, output_pdr)

    prefix = '../..'
    meu_dir = stitch_dir + '/' + os.path.dirname(cfg_var['meu'])
    if not os.path.exists (meu_dir + '/meu_config.xml'):
        print ("\nGenerating meu_config.xml ...")
        cmd = './meu -gen meu_config'
        run_cmd (cmd, meu_dir)
        gen_meu_cfg_xml (meu_dir, 'meu_config.xml', 'meu_config.xml')

    print ("\nGenerating public key hash ...")
    cmd = './meu -keyhash %s -f %s' % (prefix + '/%s/bxt_prod_pub_key_hash_ISH' % cfg_var['fitinput'], prefix + '/%s' % cfg_var['ishimg'])
    run_cmd (cmd, meu_dir)
    cmd = './meu -keyhash %s -f %s' % (prefix + '/%s/bxt_prod_pub_key_hash_iUnit' % cfg_var['fitinput'], prefix + '/%s' % cfg_var['iunit'])
    run_cmd (cmd, meu_dir)
    cmd = './meu -keyhash %s -f %s' % (prefix + '/%s/bxt_prod_pub_key_hash_Audio' % cfg_var['fitinput'], prefix + '/%s' % cfg_var['audio'])
    run_cmd (cmd, meu_dir)
    cmd = './meu -keyhash %s -key %s' % (prefix + '/%s/bxt_dbg_pub_key_hash' % cfg_var['fitinput'], prefix + '/%s' % cfg_var['keys'])
    run_cmd (cmd, meu_dir)

    print ("\nGenerating sbl.xml ...")
    cmd = './meu -gen Bios -o sbl.xml'
    run_cmd (cmd, meu_dir)
    gen_meu_sbl_xml (meu_dir, 'sbl.xml', 'sbl.xml')

    print ("\nGenerating OEMKeyManifest.xml ...")
    cmd = './meu -gen OEMKeyManifest -o OEMKeyManifest.xml'
    run_cmd (cmd, meu_dir)
    gen_meu_oem_key_xml (meu_dir, 'OEMKeyManifest.xml', 'OEMKeyManifest.xml')

    print ("\nGenerating BIOS.bin ...")
    cmd  = './meu -f sbl.xml -o %s -u1 . -key %s -s %s' % (prefix + '/%s/bios.bin' % cfg_var['fitinput'],
           prefix + '/%s' % cfg_var['keys'], prefix + '/%s' % cfg_var['fitinput'])
    run_cmd (cmd, meu_dir)

    print ("\nGenerating OEM key manifest ...")
    cmd  = './meu -f OEMKeyManifest.xml -o %s' % (prefix + '/%s/oemkeymn2.bin' % cfg_var['fitinput'])
    run_cmd (cmd, meu_dir)

    fit_dir = stitch_dir + '/' + os.path.dirname(cfg_var['fit'])
    print ("\nGenerating bxt_spi_8mb_sbl.xml ...")
    gen_fit_xml (fit_dir, 'bxt_spi_8mb.xml', 'bxt_spi_8mb_sbl.xml', btg_profile, spi_quad_mode)

    print ("\nGernerating SPI Image ...")
    cmd = './fit -b -f bxt_spi_8mb_sbl.xml -o %s -w %s -s %s' % (prefix + '/%s/%s' % (cfg_var['wkspace'],
          cfg_var['ifwiname']),   prefix + '/%s' % cfg_var['wkspace'], prefix + '/%s' % cfg_var['wkspace'])

    if cfg_var['openssl'].startswith('/'):
        cmd = cmd + ' -st_path %s'   % (cfg_var['openssl'])
    else:
        cmd = cmd + ' -st_path %s'   % (prefix + '/' + cfg_var['openssl'])
    run_cmd (cmd, fit_dir)

    ifwi_file_name = '%s/%s' % (cfg_var['wkspace'], cfg_var['ifwiname'])
    if full_rdundant:
        print ("\nCopy BP0 BPDT into BP1 ...")
        ret = copy_primary_bpdt ('%s' % ifwi_file_name)

    print ("\nPatching IBBL Flash map ...")
    ret = patch_ibb_flash_map (cfg_var['fitinput'] + '/Stitch_IBBL.bin', ifwi_file_name, platform_data)
    if ret:
        raise Exception ("Failed with error %d !" % ret)

    print ("\nRe-gernerating BIOS.bin for BP0 ...")
    cmd  = './meu -f sbl.xml -o %s -u1 . -key %s -s %s' % (prefix + '/%s/bios.bin' % cfg_var['fitinput'], prefix + '/%s' % cfg_var['keys'], prefix + '/%s' % cfg_var['fitinput'])
    run_cmd (cmd, meu_dir)

    print ("\nUpdating IBB partition for BP0 in IFWI ...")
    ret = replace_ibb_partition (cfg_var['fitinput'] + '/bios.bin', ifwi_file_name,  ifwi_file_name, 'IFWI/BIOS/IBBP', "IFWI/BIOS/BP0/BPDT/BpdtIbb")
    if ret:
        raise Exception ("Failed with error %d !" % ret)

    if full_rdundant:
        print ("\nRe-gernerating BIOS.bin for BP1 ...")
        shutil.copy (cfg_var['fitinput'] + '/Stitch_IBBL_backup.bin', cfg_var['fitinput'] + '/Stitch_IBBL.bin')
        cmd  = './meu -f sbl.xml -o %s -u1 . -key %s -s %s' % (prefix + '/%s/bios_backup.bin' % cfg_var['fitinput'], prefix + '/%s' % cfg_var['keys'], prefix + '/%s' % cfg_var['fitinput'])
        run_cmd (cmd, meu_dir)

        print ("\nUpdating BIOS.bin for BP1 in IFWI ...")
        ret = replace_ibb_partition (cfg_var['fitinput'] + '/bios_backup.bin', ifwi_file_name, ifwi_file_name, 'IFWI/BIOS/IBBP', "IFWI/BIOS/BP1/BPDT/BpdtIbb")
        if ret:
            raise Exception ("Failed with error %d !" % ret)

    return 0


def main():
    hexstr = lambda x: int(x, 16)

    if len(sys.argv) == 1:
        print('%s' % extra_usage_txt)

    ap = argparse.ArgumentParser()
    ap.add_argument('-w',
                    '--stitch-work-dir',
                    dest='stitch_dir',
                    type=str,
                    required=True,
                    help='specify workspace directory for stitching')

    ap.add_argument('-s',
                    '--stitch-zip-file',
                    dest='stitch_zip',
                    type=str,
                    default='Outputs/apl/Stitch_Components.zip',
                    help='specify input stitching zip package file path')

    ap.add_argument('-b',
                    '--boot-guard-profile',
                    default = 'vm',
                    choices=['legacy', 'v', 'vm'],
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

    ap.add_argument('-p',
                    '--platform-data',
                    dest='plat_data',
                    type=hexstr,
                    default=0xFFFFFFFF,
                    help='specify a platform specific data (HEX, DWORD) for customization')

    args = ap.parse_args()

    stitch_dir = os.path.abspath (args.stitch_dir)
    if clean (stitch_dir, args.clean):
        raise Exception ('Stitching clean up failed !')

    if args.clean:
        print ("Cleaning completed successfully !\n")
        return 0

    if stitch (stitch_dir, args.stitch_zip, args.btg_profile, args.quad_mode, args.plat_data):
        raise Exception ('Stitching process failed !')

    cfg_var       = get_config ()
    ifwi_file_name = '%s/%s/%s' % (stitch_dir, cfg_var['wkspace'], cfg_var['ifwiname'])
    ifwi_file_name = os.path.normpath (ifwi_file_name)

    print ("")
    print ("IFWI Stitching completed successfully !")
    print ("Boot Guard Profile: %s" % args.btg_profile.upper())
    print ("IFWI image: %s\n" % ifwi_file_name)

    return 0


if __name__ == '__main__':
    sys.exit(main())
