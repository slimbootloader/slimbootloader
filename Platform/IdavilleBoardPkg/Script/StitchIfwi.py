## @ StitchIfwi.py
#  This is a python stitching script for Slim Bootloader IDV build
#
# Copyright (c) 2020 - 2023, Intel Corporation. All rights reserved. <BR>
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
from   subprocess   import call
from   BtGUtility  import *
from   StitchLoader import *

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

def gen_xml_file(stitch_dir, stitch_cfg_file, btg_profile, platform, tpm, plt_params_list):
    print ("Generating xml file .........")

    temp_dir = os.path.abspath(os.path.join (stitch_dir, 'Temp'))
    fit_tool     = os.path.join (stitch_dir, 'Fit', 'spsFITc.exe')
    new_xml_file = os.path.join (stitch_dir, 'Temp', 'new.xml')
    updated_xml_file = os.path.join (stitch_dir, 'Temp', 'updated.xml')
    sku = stitch_cfg_file.get_sku()
    platform = stitch_cfg_file.get_platforms()[platform]
    cmd = [fit_tool, '-sku', sku, '-platform', platform, '-save', new_xml_file, '-w', os.path.join (stitch_dir, 'Temp')]
    run_process_wrapper (cmd)

    tree = ET.parse(new_xml_file)

    xml_change_list = stitch_cfg_file.get_xml_change_list (platform, plt_params_list)
    for xml_path, value in xml_change_list:
        node = tree.find('%s' % xml_path)
        print (xml_path, value)
        node.set('value', value)

    update_btGuard_manifests(stitch_dir, stitch_cfg_file)
    print ("Updating btGuard XML")
    update_btGuard_xml(btg_profile, stitch_dir, tree, stitch_cfg_file.get_btGuard_xml_change_dict())
    update_tpm_type(tpm, tree)

    tree.write(updated_xml_file)

def gen_igfw_xml_file(stitch_dir, stitch_cfg_file, btg_profile, platform):
    print ("Generating Ignition xml file .........")

    new_xml_file = os.path.join (stitch_dir, 'Igfw', 'Config', 'IGN_IVL_CDF_%s.xml' % platform)
    updated_xml_file = os.path.join (stitch_dir, 'Igfw', 'Config', 'IGN_IVL_CDF_%s_updated.xml' % platform)

    tree = ET.parse(new_xml_file)
    root = tree.getroot()

    update_btGuard_manifests(stitch_dir, stitch_cfg_file)
    print ("Updating Ignition btGuard XML")
    update_igfw_btGuard_xml(btg_profile, stitch_dir, root, stitch_cfg_file.get_igfw_btGuard_xml_change_dict())
    print ("Writing to an updated igfw xml %s" % updated_xml_file)
    tree.write(updated_xml_file)

def stitch (stitch_dir, stitch_cfg_file, sbl_file, btg_profile, platform, tpm, platform_data, plt_params_list, igfw):
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

    if igfw == '':
        gen_xml_file(stitch_dir, stitch_cfg_file, btg_profile, platform, tpm, plt_params_list)

        print ("Run fit tool to generate ifwi.........")
        sku = stitch_cfg_file.get_sku()
        platform = stitch_cfg_file.get_platforms()[platform]
        print (platform)
        fit_tool     = os.path.join (stitch_dir, 'Fit', 'spsFITc.exe')
        run_process_wrapper ([fit_tool, '-b', '-o', 'Temp/Ifwi.bin', '-f', os.path.join (temp_dir, 'updated.xml'),
            '-s', temp_dir, '-w', temp_dir, '-sku', sku, '-platform', platform])
        return 0
    else:
        gen_igfw_xml_file(stitch_dir, stitch_cfg_file, btg_profile, platform)

        print ("Run fitm tool to generate ignition ifwi.........")
        fitm_tool = os.path.join (stitch_dir, 'Igfw', 'FITm', 'fit_cmd.py')
        if not os.path.exists(fitm_tool):
            raise Exception ("FITm tool not present, cant create Ignition Ifwi")
        updated_xml_file = os.path.join (stitch_dir, 'Igfw', 'Config', 'IGN_IVL_CDF_%s_updated.xml' % platform)
        bios_region = os.path.join(stitch_dir, 'Temp', 'BiosRegion.bin')
        irc_region = os.path.join(stitch_dir, 'Input', 'irc_patch.bin')
        nac_region = os.path.join(stitch_dir, 'Input', 'img_nis_region.bin')
        me_region = os.path.join(stitch_dir, 'Igfw', 'MeRegion.bin')
        mphy_table = os.path.join(stitch_dir, 'Igfw', 'mphy', 'mphy_internal.bin')
        out_image = os.path.join(stitch_dir, 'Temp', 'Ifwi_igfw.bin')

        cmd = ['python', fitm_tool, '-b', '-i', updated_xml_file, '-s', updated_xml_file, '--layout',
                    'bios=%s' % bios_region, 'nac=%s' % nac_region,
                    '--params', 'me:input_file=%s' % me_region, 'mfs:mphy_table_internal=%s'% mphy_table,
                    'irc:input_file=%s enable_regions:irc=1' % irc_region,
                    'layout:output_file=%s' % out_image, '--skip_access_check' ]
        run_process_wrapper (cmd)

        return 0

def get_para_list (plt_para):
    para_lst  = dict()
    for idx, para in enumerate(plt_para):
        items    = para.split(':')
        item_cnt = len(items)
        para_lst.update( {items[0] : None if (item_cnt == 1) else items[1].strip()})
    return para_lst

def main():
    hexstr = lambda x: int(x, 16)
    ap = argparse.ArgumentParser()
    ap.add_argument('-w', dest='work_dir', default = '', help='specify stitch workspace directory, CSME tools and ingredients should be here')
    ap.add_argument('-c', dest='config_file', type=str, required=True, help='specify the platform specific stitch config file')
    ap.add_argument('-s', dest='sbl_file', type=str, default='stitch_Components.zip', help='specify slim bootloader file or generate zip file')
    ap.add_argument('-p', dest='platform', choices = ['BC', 'MC'], default = 'BC', help='specify platform sku to stitch')
    ap.add_argument('-b', dest='btg_profile', default = 'legacy', choices=['legacy', 'vm', 'fve', 'fvme'], help='specify Boot Guard profile type')
    ap.add_argument('-r', dest='remove', action = "store_true", default = False, help = "delete temporary files after stitch")
    ap.add_argument('-t', dest='tpm', default = 'dtpm', choices=['ptt', 'dtpm', 'none'], help='specify TPM type')
    ap.add_argument('-k', dest='key_dir', type=str, required=False, help='specify the path to Sbl Keys directory')
    ap.add_argument('-d', dest='plat_data', type=hexstr, default=None, help='Specify a platform specific data (HEX, DWORD) for customization')
    ap.add_argument('-o', dest='option', default = '', help = "Platform specific stitch option. Format: '-o option1;option2;...' For each option its format is 'parameter:data'. Try -o help for more information")
    ap.add_argument('-i', dest='igfw', default = '', help = "Stitch IGN FW flag. Give '-i igfw' if stitching Ignition Firmware")

    args = ap.parse_args()

    module = load_source('StitchIfwiConfigBase', args.config_file)
    stitch_cfg_file = module.get_stitch_ifwi_config_instance()
    if args.work_dir == '':
        print ("Please specify stitch work directory")
        print ('%s' % stitch_cfg_file.get_usage_txt())
        return 0

    if args.btg_profile in ["vm","fvme"] and args.tpm == "none":
        print ("ERROR: Choose appropriate Tpm type for BootGuard profile 3 and 5")
        return 0

    # get additional stitching options
    plt_params_list = None
    if args.option != '':
        plt_params_list = get_para_list(args.option.split(';'))

    print ("Executing stitch.......")
    curr_dir = os.getcwd()
    sbl_file = os.path.abspath(os.path.join (curr_dir, args.sbl_file))

    if args.key_dir:
        os.environ['SBL_KEY_DIR'] = os.path.abspath(args.key_dir)
    else:
        raise Exception ('SBL Keys dir is not set. Use -k to set directory!')

    work_dir = os.path.abspath (args.work_dir)
    os.chdir(work_dir)
    if stitch (work_dir, stitch_cfg_file, sbl_file, args.btg_profile, args.platform, args.tpm, args.plat_data, plt_params_list, args.igfw):
        raise Exception ('Stitching process failed !')
    os.chdir(curr_dir)

    if args.igfw == '':
        generated_ifwi_file = os.path.join(work_dir, 'Temp', 'Ifwi.bin')
    else:
        generated_ifwi_file = os.path.join(work_dir, 'Temp', 'Ifwi_igfw.bin')

    ifwi_file_name = 'sbl_idv.bin' if args.platform == 'BC' else 'sbl_idvh.bin'
    shutil.copy(generated_ifwi_file, ifwi_file_name)

    print ("\nIFWI Stitching completed successfully !")
    print ("Boot Guard Profile: %s" % args.btg_profile.upper())
    print ("Additional Options: %s" % plt_params_list)
    print ("IFWI image: %s\n" % ifwi_file_name)
    if args.remove:
        shutil.rmtree(os.path.join(work_dir, 'Temp'), ignore_errors=True)
    return 0

if __name__ == '__main__':
    sys.exit(main())
