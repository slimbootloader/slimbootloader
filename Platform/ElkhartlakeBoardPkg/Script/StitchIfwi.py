## @ StitchIfwi.py
#  This is a python stitching script for Slim Bootloader EHL build
#
# Copyright (c) 2019 - 2021, Intel Corporation. All rights reserved. <BR>
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
from   StitchLoader import *
from   security_stitch_help  import *
sys.dont_write_bytecode = True

# sign_bin_flag can be set to false to avoid signing process. Applicable for Btg profile 0
sign_bin_flag = True

def gen_xml_file(stitch_dir, stitch_cfg_file, btg_profile, plt_params_list, platform, tpm):
    print ("Generating xml file .........")

    if os.name == 'nt':
        fit_tool     = os.path.join (stitch_dir, 'Fit', 'fit.exe')
    else:
        fit_tool     = os.path.join (stitch_dir, 'Fit', 'fit')
    new_xml_file = os.path.join (stitch_dir, 'Temp', 'new.xml')
    updated_xml_file = os.path.join (stitch_dir, 'Temp', 'updated.xml')
    sku = stitch_cfg_file.get_platform_sku().get(platform)
    cmd = [fit_tool, '-sku', sku, '-save', new_xml_file, '-w', os.path.join (stitch_dir, 'Temp')]
    run_process (cmd)

    tree = ET.parse(new_xml_file)

    xml_change_list = stitch_cfg_file.get_xml_change_list (platform, plt_params_list, stitch_dir)
    for each in xml_change_list:
        for xml_path, value in each:
            node = tree.find('%s' % xml_path)
            node.set('value', value)
            print (value)

    tree.write(updated_xml_file)

def replace_component (ifwi_src_path, flash_path, file_path, comp_alg, pri_key, svn):
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
                    '-f', comp_file, '-c', comp_alg, '-k', pri_key, '-td', tool_bin_dir, '-s', '%d' % svn]
        run_process (cmd_line, True)
        comp_bin = bytearray (get_file_data (container_file))
    else:
        # replace directly in flash map
        comp_bin = bytearray (get_file_data (file_path))
    IFWI_PARSER.replace_component (ifwi_bin, comp_bin, flash_path)
    gen_file_from_object (ifwi_src_path, ifwi_bin)

def replace_components (ifwi_src_path, stitch_cfg_file, plt_params_list):
    print ("Replacing components.......")
    replace_list = stitch_cfg_file.get_component_replace_list (plt_params_list)
    for flash_path, file_path, comp_alg, pri_key, svn in replace_list:
        replace_component (ifwi_src_path, flash_path, file_path, comp_alg, pri_key, svn)

def stitch (stitch_dir, stitch_cfg_file, sbl_file, btg_profile, plt_params_list, platform_data, platform, tpm, full_rdundant = True):
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

    print("Replace components in both partitions....")
    replace_components (os.path.join(temp_dir, "SlimBootloader.bin"), stitch_cfg_file, plt_params_list)

    # Generate xml
    gen_xml_file(stitch_dir, stitch_cfg_file, btg_profile, plt_params_list, platform, tpm)

    if sign_bin_flag:
        update_btGuard_manifests(stitch_dir, stitch_cfg_file, btg_profile, tpm)
    else:
        shutil.copy(os.path.join(temp_dir, "SlimBootloader.bin"), os.path.join(temp_dir, "BiosRegion.bin"))

    print ("Run fit tool to generate ifwi.........")

    if os.name == 'nt':
        fit_tool     = os.path.join (stitch_dir, 'Fit', 'fit.exe')
    else:
        fit_tool     = os.path.join (stitch_dir, 'Fit', 'fit')
    run_process ([fit_tool, '-b', '-o', 'Temp/Ifwi.bin', '-f', os.path.join (temp_dir, 'updated.xml'),
        '-s', temp_dir, '-w', temp_dir, '-d', temp_dir])
    return 0

def get_para_list (plt_para):
    para_lst  = dict()
    for idx, para in enumerate(plt_para):
        items    = para.split(':')
        item_cnt = len(items)
        para_lst.update( {items[0] : None if (item_cnt == 1) else items[1].strip()})
    return para_lst

def softstrap_write (path,offset,val,act):
    with open(path, "r+b") as fh:
        #read from offset
        fh.seek(offset)
        data = fh.read(1)
        if act == 0:
            data_int = int.from_bytes(data, "big") & val
        else:
            data_int = int.from_bytes(data, "big") | val
        #write to the specific offset
        fh.seek(offset)
        fh.write(bytes([data_int]))
    fh.close()


def main():
    hexstr = lambda x: int(x, 16)
    ap = argparse.ArgumentParser()
    ap.add_argument('-p', dest='platform', default = '', help='specify platform sku to stitch')
    ap.add_argument('-w', dest='work_dir', default = '', help='specify stitch workspace directory, CSME tools and ingredients should be here')
    ap.add_argument('-c', dest='config_file', type=str, required=True, help='specify the platform specific stitch config file')
    ap.add_argument('-s', dest='sbl_file', type=str, default='stitch_Components.zip', help='specify slim bootloader file or generate zip file')
    ap.add_argument('-b', dest='btg_profile', default = 'vm', choices=['legacy', 'vm', 'fve', 'fvme'], help='specify Boot Guard profile type')
    ap.add_argument('-d', dest='plat_data', type=hexstr, default=None, help='Specify a platform specific data (HEX, DWORD) for customization')
    ap.add_argument('-r', dest='remove', action = "store_true", default = False, help = "delete temporary files after stitch")
    ap.add_argument('-t', dest='tpm', default = 'ptt', choices=['ptt', 'dtpm', 'none'], help='specify TPM type')
    ap.add_argument('-k', dest='key_dir', type=str, required=True, help='specify the path to Sbl Keys directory')
    ap.add_argument('-o', dest='option', default = '', help = "Platform specific stitch option. Format: '-o option1;option2;...' For each option its format is 'parameter:data'. Try -o help for more information")
    ap.add_argument('-op', dest='outpath', default = '', help = "Specify path to write output IFIW and signed bin files")

    args = ap.parse_args()

    stitch_cfg_file = imp.load_source('StitchIfwiConfig', args.config_file)
    if args.work_dir == '':
        print ("Please specify stitch work directory")
        print ('%s' % stitch_cfg_file.extra_usage_txt)
        return 1

    sku_dict = stitch_cfg_file.get_platform_sku()
    if args.platform == '' or args.platform not in sku_dict:
        print ("Invalid sku (%s), Please provide valid sku:" % args.platform)
        for sku in sku_dict :
            print (" %s - 'For %s'" % (sku, sku_dict[sku]))
        return 1

    if args.btg_profile in ["vm","fvme"] and args.tpm == "none":
        print ("ERROR: Choose appropriate Tpm type for BootGuard profile 3 and 5")
        return 1

    plt_params_list = get_para_list (args.option.split(';'))
    if not stitch_cfg_file.check_parameter(plt_params_list):
        exit (0)

    print ("Executing stitch.......")
    curr_dir = os.getcwd()
    sbl_file = os.path.abspath(os.path.join (curr_dir, args.sbl_file))

    if args.key_dir:
        os.environ['SBL_KEY_DIR'] = os.path.abspath(args.key_dir)
    else:
        raise Exception ('SBL Keys dir is not set. Use -k to set directory!')

    work_dir = os.path.abspath (args.work_dir)
    os.chdir(work_dir)
    if stitch (work_dir, stitch_cfg_file, sbl_file, args.btg_profile, plt_params_list, args.plat_data, args.platform, args.tpm):
        raise Exception ('Stitching process failed !')
    os.chdir(curr_dir)

    generated_ifwi_file = os.path.join(work_dir, 'Temp', 'Ifwi.bin')
    ifwi_file_name = os.path.join(args.outpath,'sbl_ifwi_%s.bin' % (args.platform))
    shutil.copy(generated_ifwi_file, ifwi_file_name)

    generated_signed_sbl =  os.path.join(work_dir, 'Temp', 'SlimBootloader.bin')
    sbl_file_name = os.path.join(args.outpath,'SlimBootloader_%s.bin' % (args.platform))
    shutil.copy(generated_signed_sbl, sbl_file_name)

    list_val = [1,0,2,4,16,249,15] #List of values to override
    softstrap_write(ifwi_file_name,0xc18,list_val[0],1) #FUSA SOFTSTRAP due to CSME 2041 update
    ###################################################################################
    # Offset | Start Bit | Strap Size | Value | Comment                             |
    # 0x1ce    0x0          0x4         0x0     FIA/LOSL11
    # 0xc1c    0x1          0x1         0x1     pmc_smip/RSVD_2A_DIS_STRAP_PSEGBe0
    # 0xc1c    0x2          0x1         0x1     pmc_smip/RSVD_2A_DIS_STRAP_PSEGBe1
    # 0x1d5    0x4          0x4         0x1     MGBE/mgbe_soc_specific
    # 0x183    0x1          0x2         0x0     DMI/OPD_LVO
    # 0x1cc    0x4          0x4         0x0     FIA/LOSL8
    ###################################################################################
    softstrap_write(ifwi_file_name,0xc1c,list_val[2],1)
    softstrap_write(ifwi_file_name,0xc1c,list_val[3],1)
    softstrap_write(ifwi_file_name,0x183,list_val[5],0)
    if args.option == 'tsn':
        softstrap_write(ifwi_file_name,0x1ce,list_val[1],0)
        softstrap_write(ifwi_file_name,0x1d5,list_val[4],1)
        softstrap_write(ifwi_file_name,0x1cc,list_val[6],0)

    print ("\nIFWI Stitching completed successfully !")
    print ("Boot Guard Profile: %s" % args.btg_profile.upper())
    print ("IFWI image: %s\n" % ifwi_file_name)
    if args.remove:
        shutil.rmtree(os.path.join(work_dir, 'Temp'), ignore_errors=True)
    return 0

if __name__ == '__main__':
    sys.exit(main())
