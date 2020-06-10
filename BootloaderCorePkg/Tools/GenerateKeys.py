#!/usr/bin/env python
## @ GenerateKeys.py
#
# This script is an reference to generate test RSA private keys required by slimbootloader.
# This should be one time key generation for a project based on usecase needs and this would
# be pre-requiste to generate this before Slimboot build. These keys should be used for
# various components signing in Slimboot
#
#
# Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

##
# Import Modules
#
import os
import sys
import datetime
import ntpath
import argparse
import binascii
from ctypes import *
from CommonUtility import *


RSA_KEY_SIZE = {
        "RSA2048"      : '2048',
        "RSA3072"      : '3072',
    }

def signing_priv_keys():
    signing_keys_list = []
    signing_keys_list.append ([
      # Key ID                          | Key File Name start |
      # ===========================================================
        # MASTER_KEY_ID is used for signing Slimboot Key Hash Manifest (KEYH Component)
        ("MASTER_KEY_ID",               "MasterTestKey_Priv"),

        # CFGDATA_KEY_ID is used for signing external Config data blob)
        ("CFGDATA_KEY_ID",              "ConfigTestKey_Priv"),

        # FIRMWAREUPDATE_KEY_ID is used for signing capsule firmware update image)
        ("FIRMWAREUPDATE_KEY_ID",       "FirmwareUpdateTestKey_Priv"),

        # CONTAINER_KEY_ID is used for signing container header with mono signature
        ("CONTAINER_KEY_ID",            "ContainerTestKey_Priv"),

        # CONTAINER_COMP_KEY_ID is used for signing container components
        # One can add multiple component keys as needed.
        ("CONTAINER_COMP_KEY_ID",       "ContainerCompTestKey_Priv"),
        ])

    return signing_keys_list

def signing_pub_keys():
    signing_keys_list = []
    signing_keys_list.append ([
      # Key ID                          | Key File Name start |
      # ===========================================================
        # OS1_PUBLIC_KEY_ID is used for referencing Boot OS public keys
        ("OS1_PUBLIC_KEY_ID",           "OS1_TestKey_Pub"),

        ])

    return signing_keys_list

# Generate RSA Key based on required key size
def GenerateRsaPrivKeys (openssl_path, key_dir, key_size):
    key_list = signing_priv_keys()
    for item in key_list:
        for Key_name, Key_file_name in item:
            cmd = '%s genrsa -F4 -out %s/%s_RSA%s.pem %s' % (openssl_path, key_dir, Key_file_name, key_size, key_size)
            run_process (cmd.split())

    # Generate test public keys
    GenerateRsaPubKeys (openssl_path, key_dir, key_size)
    return

# Generate RSA Public Keys based on required key size
# User is required to replace the public keys with their original keys.
def GenerateRsaPubKeys (openssl_path, key_dir, key_size):
    key_list = signing_pub_keys()
    for item in key_list:
        for Key_name, Key_file_name in item:
            # Generate an test private key
            priv_key_temp = '%s/%s_RSA%s_priv.pem' % (key_dir, Key_file_name, key_size)
            print (priv_key_temp)
            cmd = '%s genrsa -F4 -out %s %s' % (openssl_path, priv_key_temp, key_size)
            run_process (cmd.split())

            # Extract public key from private key
            pub_key_name = '%s/%s_RSA%s.pem' % (key_dir, Key_file_name, key_size)
            print (pub_key_name)
            cmd = '%s rsa -pubout -in %s -out %s' % (openssl_path, priv_key_temp, pub_key_name)
            run_process (cmd.split())

    return

def main():
    parser = argparse.ArgumentParser()

    parser.add_argument('-k',  '--key_dir', dest='KeyDir', type=str, required=True, help='Key directory path')
    parser.add_argument('-s',  '--key_size', dest='KeySize', type=str, choices=['RSA2048', 'RSA3072', 'ALL'], default='ALL', help='key size for generation')

    #
    # Parse command line arguments
    #
    args = parser.parse_args()

    if not os.path.isdir(args.KeyDir):
        print ("Key directory path do not exists. Create directory %s !!" % args.KeyDir)
        os.mkdir(args.KeyDir)

    openssl_path = get_openssl_path()

    if args.KeySize is 'ALL':
        # Generate keys for key size 2048 and 3072
        GenerateRsaPrivKeys(openssl_path, args.KeyDir, RSA_KEY_SIZE["RSA2048"])
        GenerateRsaPrivKeys(openssl_path, args.KeyDir, RSA_KEY_SIZE["RSA3072"])
    else:
        # Generate keys for requested key size
        GenerateRsaPrivKeys(openssl_path, args.KeyDir, RSA_KEY_SIZE[args.KeySize])

    return

if __name__ == '__main__':
    sys.exit(main())
