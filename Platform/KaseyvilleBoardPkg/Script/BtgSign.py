## @ BtgSign.py
#  This is a python stitching script for Boot Guard BPM signing
#
# Copyright (c) 2026, Intel Corporation. All rights reserved. <BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import sys
import os
import argparse
import shutil
from   ctypes  import *
from   CommonUtility import *
from   IfwiUtility   import *

SIGNING_KEY = {
    # Key Id                                | Key File Name |
    # =================================================================
    # KEY_ID_PRIV_OEM is used for OEM signing
    "KEY_ID_PRIV_OEM_RSA2048"          :    "oem_privkey_2048.pem",
    "KEY_ID_PRIV_OEM_RSA3072"          :    "oem_privkey_3072.pem",

    # KEY_ID_PRIV_BPM is used for BPM signing
    "KEY_ID_PRIV_BPM_RSA2048"          :    "bpm_privkey_2048.pem",
    "KEY_ID_PRIV_BPM_RSA3072"          :    "bpm_privkey_3072.pem",

    "KEY_ID_PUB_OEM_RSA2048"           :    "oem_pubkey_2048.pem",
    "KEY_ID_PUB_OEM_RSA3072"           :    "oem_pubkey_3072.pem",

    "KEY_ID_PUB_BPM_RSA2048"           :    "bpm_pubkey_2048.pem",
    "KEY_ID_PUB_BPM_RSA3072"           :    "bpm_pubkey_3072.pem",
    }

# Sample entries for OemKeyManifest
def get_oemkeymanifest_change_list():
    xml_change_list = []
    xml_change_list.append ([
        # Path                                                                            | value |
        # =========================================================================================
        ('./KeyManifestEntries/KeyManifestEntry/Usage',                                    'OemDebugManifest'),
        ('./KeyManifestEntries/KeyManifestEntry/HashBinary',                               'Temp/kmsigpubkey.hash'),
    ])

    return xml_change_list

def generate_keys(bpm_key_dir, key_sz):

    openssl_path = get_openssl_path()

    os.mkdir(bpm_key_dir)

    oem_priv_key = SIGNING_KEY['KEY_ID_PRIV_OEM_RSA' + key_sz]
    cmd = '%s genrsa -F4 -out %s/%s %s' % (openssl_path, bpm_key_dir, oem_priv_key, key_sz)
    run_process (cmd.split())

    oem_pub_key = SIGNING_KEY['KEY_ID_PUB_OEM_RSA' + key_sz]
    cmd = '%s rsa -pubout -in %s/%s -out %s/%s' % (openssl_path, bpm_key_dir, oem_priv_key, bpm_key_dir, oem_pub_key)
    run_process (cmd.split())

    bpm_priv_key = SIGNING_KEY['KEY_ID_PRIV_BPM_RSA' + key_sz]
    cmd = '%s genrsa -F4 -out %s/%s %s' % (openssl_path, bpm_key_dir, bpm_priv_key, key_sz)
    run_process (cmd.split())

    bpm_pub_key = SIGNING_KEY['KEY_ID_PUB_BPM_RSA' + key_sz]
    cmd = '%s rsa -pubout -in %s/%s -out %s/%s' % (openssl_path, bpm_key_dir, bpm_priv_key, bpm_key_dir, bpm_pub_key)
    run_process (cmd.split())


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
                    '-s', 'key=%s' % os.path.join (bpm_key_dir, 'oem_privkey_3072.pem'), 'fd0v_key=%s' % os.path.join (bpm_key_dir, 'oem_pubkey_3072.pem'), '-o', oem_bin_sign]
    run_process (cmd)


def gen_oem_key_hash(output_dir, oem_pub_key, oem_pub_key_hash_file, hash_type, key_size):

    kmsigpubkeytxtfile  = os.path.join(output_dir, "kmsigpubkey.txt")
    kmsigpubkeybinfile  = os.path.join(output_dir, "kmsigpubkey.bin")
    kmsigpubkeytxt_fh   = os.path.join(output_dir, "kmsigpubkeytxt_fh")

    key_sz_bytes = int(key_size)//8

    openssl_path = get_openssl_path()

    run_process ([openssl_path, 'rsa' , '-in', oem_pub_key,
            '-pubin', '-modulus', '-out', kmsigpubkeytxt_fh])
    shutil.copy(kmsigpubkeytxt_fh, kmsigpubkeytxtfile)

    line=open(kmsigpubkeytxtfile,"r").readline()[8:8+key_sz_bytes*2]
    if sys.hexversion >= 0x3000000:
        keybin = bytearray.fromhex(line)
    else:
        keybin = line.decode('hex')
    open(kmsigpubkeybinfile,"wb").write(keybin[::-1])

    # public exponent (i.e. 65537) is concatenated for OEM key hash calculation.
    line=open(kmsigpubkeytxtfile,"r").readline()[8:8+key_sz_bytes*2]
    open(kmsigpubkeybinfile,"ab").write(bytearray.fromhex('01000100'))
    run_process ([openssl_path, 'dgst', '-'+hash_type, '-binary',
        '-out', oem_pub_key_hash_file, 'Temp/kmsigpubkey.bin'])

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
            print ("       To stitch CML IFWI in Linux, 'wine' is required")
            print ("\n")
            raise Exception ()
    run_process (cmds)

def bpm_sign_binary(infile, out_file, output_dir, bpm_gen2dir, key_dir, bpmgen2_params, key_sz, oem_pub_key_hash_file, hash_type):

    shutil.copy(infile, os.path.join(output_dir,"sbl_sec_temp.bin"))


    oem_priv_key = SIGNING_KEY['KEY_ID_PRIV_OEM_RSA' + key_sz]
    oem_pub_key  = SIGNING_KEY['KEY_ID_PUB_OEM_RSA'  + key_sz]
    bpm_priv_key = SIGNING_KEY['KEY_ID_PRIV_BPM_RSA' + key_sz]
    bpm_pub_key  = SIGNING_KEY['KEY_ID_PUB_BPM_RSA'  + key_sz]
    if os.name == 'nt':
        bpmgen2_tool = os.path.join (bpm_gen2dir, 'bpmgen2.exe')
    else:
        bpmgen2_tool = os.path.join (bpm_gen2dir, 'bpmgen2')

    print (key_dir)
    if not os.path.exists(key_dir):
        print("Generating new keys....")
        generate_keys (key_dir, key_sz)

    print("Generating Btg KeyManifest.bin....")
    run_process ([bpmgen2_tool,
        'KMGEN',
        '-KEY',        os.path.join (key_dir, bpm_pub_key), 'BPM',
        '-KM',         os.path.join (output_dir, 'KeyManifest.bin'),
        '-SIGNKEY',    os.path.join (key_dir,  oem_priv_key),
        '-SIGNPUBKEY', os.path.join (key_dir, oem_pub_key),
        '-KMID',       '0x01',
        '-KMKHASH',    hash_type,
        '-SCHEME',     'RSAPSS',
        '-SVN',        '0',
        '-d:2'])

    print("Generating Btg Boot Policy Manifest (BPM).bin....")
    run_process ([bpmgen2_tool,
        'GEN',
        os.path.join (output_dir, 'sbl_sec_temp.bin'),
        bpmgen2_params,
        '-BPM',        os.path.join (output_dir, 'BpmManifest.bin'),
        '-U',          out_file,
        '-KM',         os.path.join (output_dir, 'KeyManifest.bin'),
        '-d:2'])

    # Generate OemKeyHash
    if oem_pub_key_hash_file:
        gen_oem_key_hash(output_dir,  os.path.join (key_dir, oem_pub_key), oem_pub_key_hash_file, hash_type, key_sz)

def sign_slimboot_binary(infile, bpm_gen2dir, bpmgen2_params, key_dir,  key_sz, hash_type, out_file, output_dir, oem_pub_key_hash_file):
    print("Sign primary partition....")
    bpm_sign_binary(infile, os.path.join(output_dir, "sbl_sec.bin"), output_dir, bpm_gen2dir, key_dir, bpmgen2_params, key_sz, oem_pub_key_hash_file, hash_type)
    shutil.copy(os.path.join(output_dir, "sbl_sec.bin"), os.path.join(output_dir, "SlimBootloader.bin"))

def sign(self):
    output_dir = os.path.join(self.work_dir, "Temp")
    if not os.path.exists(output_dir):
        os.mkdir(output_dir)

    sign_slimboot_binary(self.in_file, self.tool_dir, self.bpmgen_params, self.keys_dir, self.key_size, self.hash_type, self.out_file, output_dir, self.oem_pub_key_hash)

def main():
    parser = argparse.ArgumentParser()
    sub_parser = parser.add_subparsers(help='command')

    ap = sub_parser.add_parser('sign', help='Btg sign a slimboot binary')
    ap.add_argument('-i',  dest='in_file',  type=str, required=True, help='Input bios file path')
    ap.add_argument('-o',  dest='out_file',  type=str, required=True, help='Signed bios output image path')
    ap.add_argument('-wd', dest='work_dir', type=str, required=True, help='Work directory')
    ap.add_argument('-td', dest='tool_dir', type=str, required=True, help='Tool directory')
    ap.add_argument('-kd', dest='keys_dir', type=str, required=True, help='Keys directory')
    ap.add_argument('-kz', dest='key_size', type=str, default='3072', help='Key size')
    ap.add_argument('-ht', dest='hash_type', type=str, choices=['sha256', 'sha384'], default='sha384', help='hash types for Btg signing')
    ap.add_argument('-b',  dest='bpmgen_params', type=str, help='BpmGen param file path')
    ap.add_argument('-kh', dest='oem_pub_key_hash', type=str, default='', help='Oem pub key hash file to be captured')
    ap.add_argument('-s', dest='svn', type=int,  default=0, help='Security version number for BPM')
    ap.set_defaults(func=sign)

    # Parse arguments and run sub-command
    args = parser.parse_args()
    try:
        func = args.func
    except AttributeError:
        parser.error("few arguments")

    func(args)

if __name__ == '__main__':
    sys.exit(main())
