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
        # KEY_ID_MASTER is used for signing Slimboot Key Hash Manifest (KEYH Component)
        ("KEY_ID_MASTER",               "MasterTestKey_Priv"),

        # KEY_ID_CFGDATA is used for signing external Config data blob)
        ("KEY_ID_CFGDATA",              "ConfigTestKey_Priv"),

        # KEY_ID_FIRMWAREUPDATE is used for signing capsule firmware update image)
        ("KEY_ID_FIRMWAREUPDATE",       "FirmwareUpdateTestKey_Priv"),

        # KEY_ID_CONTAINER is used for signing container header with mono signature
        ("KEY_ID_CONTAINER",            "ContainerTestKey_Priv"),

        # KEY_ID_CONTAINER_COMP is used for signing container components
        # One can add multiple component keys as needed.
        ("KEY_ID_CONTAINER_COMP",       "ContainerCompTestKey_Priv"),
        ])

    return signing_keys_list

def signing_pub_keys():
    signing_keys_list = []
    signing_keys_list.append ([
      # Key ID                          | Key File Name start |
      # ===========================================================
        # KEY_ID_OS1 is used for referencing Boot OS public keys
        ("KEY_ID_OS1",           "OS1_TestKey"),
        ])

    return signing_keys_list

# Generate RSA Key based on required key size
def generate_rsa_priv_keys (openssl_path, key_dir, key_size):

    if key_size == 'ALL':
        key_size_list = ['2048','3072']
    elif key_size == 'RSA2048':
        key_size_list = ['2048']
    elif key_size == 'RSA3072':
        key_size_list = ['3072']

    replace_all = 'no'

    # For required sizes
    for key_sz in key_size_list:
        key_list = signing_priv_keys()
        # Signing key list
        for item in key_list:
            for key_name, key_file_name in item:
                priv_key_name = '%s/%s_RSA%s.pem' % (key_dir, key_file_name, key_sz)
                # Check if key already exists and get user confirmation for replacing keys
                if os.path.exists(priv_key_name) and replace_all == 'no':
                    print("Key %s already exists!!" % priv_key_name)
                    user_confirm = input("Create and Replace Existing Key yes/no/all:   ")
                    print('\n' * 1)
                    if user_confirm.lower() == 'n' or user_confirm.lower() == 'no':
                        continue
                    # If use selects ALL replace remaining keys without checking for user inputs.
                    elif user_confirm.lower() == 'all':
                        replace_all = 'yes'

                print("Generating private key %s" % priv_key_name)
                cmd = '%s genrsa -F4 -out %s %s' % (openssl_path, priv_key_name, key_sz)
                run_process (cmd.split())
                print('\n' * 3)

        # Generate test public keys
        generate_rsa_pub_keys (openssl_path, key_dir, key_sz, replace_all)
    return

# Generate RSA Public Keys based on required key size
# User is required to replace the public keys with their original keys.
def generate_rsa_pub_keys (openssl_path, key_dir, key_size, replace_key):
    key_list = signing_pub_keys()
    for item in key_list:
        for key_name, key_file_name in item:
            # Generate an test private key
            priv_key_name = '%s/%s_Priv_RSA%s.pem' % (key_dir, key_file_name, key_size)
            if os.path.exists(priv_key_name) and replace_key == 'no':
                print("OS Signing Key %s already exists!!" % priv_key_name)
                user_confirm = input("Create and Replace Existing Key yes/no:  ")
                print("")
                if user_confirm.lower() != 'y' and user_confirm.lower() != 'yes':
                    continue

            print("Generating OS Private Key %s" % priv_key_name)
            cmd = '%s genrsa -F4 -out %s %s' % (openssl_path, priv_key_name, key_size)
            run_process (cmd.split())

            # Extract public key from private key
            pub_key_name = '%s/%s_Pub_RSA%s.pem' % (key_dir, key_file_name, key_size)
            print("Generating OS Public Key %s" % pub_key_name)
            cmd = '%s rsa -pubout -in %s -out %s' % (openssl_path, priv_key_name, pub_key_name)
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

    generate_rsa_priv_keys(openssl_path, args.KeyDir, args.KeySize)

    return

if __name__ == '__main__':
    sys.exit(main())
