## @ GenExtKeyHashStore.py
# Tool to generate an public key hash store.
# Generated binary can be updated using capsule firmware update
#
#
# Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import argparse

from CommonUtility import *
from BuildUtility import gen_pub_key_hash_store, HASH_USAGE


#
#  HASH USAGE definitions supported
#
#  'PUBKEY_CFG_DATA'     : (1<<9)
#  'PUBKEY_FWU'          : (1<<10)
#  'PUBKEY_OS'           : (1<<11)
#  'PUBKEY_CONT_DEF'     : (1<<12)
#  'PUBKEY_OEM_0'        : (1<<24)
#  'PUBKEY_OEM_1'        : (1<<25)
#  'PUBKEY_OEM_2'        : (1<<26)
#  'PUBKEY_OEM_3'        : (1<<27)
#  'PUBKEY_OEM_4'        : (1<<28)
#  'PUBKEY_OEM_5'        : (1<<29)
#  'PUBKEY_OEM_6'        : (1<<30)
#  'PUBKEY_OEM_7'        : (1<<31)

#
#  Sample Key Hash Store Layout file
#
#      usage ,                                 KeyFile/Key_ID
# =========================================================================
#  (    HASH_USAGE['PUBKEY_CFG_DATA'],     'KEY_ID_CFGDATA_RSA2048' ),
#  (    HASH_USAGE['PUBKEY_FWU'],          'KEY_ID_FIRMWAREUPDATE_RSA2048'),
#  (    HASH_USAGE['PUBKEY_CONT_DEF'],     'KEY_ID_CONTAINER_RSA2048'  ),
#  (    HASH_USAGE['PUBKEY_OS'],           'KEY_ID_OS1_PUBLIC_RSA2048' ),
#
#

def main():

    #
    # Create command line argument parser object
    #

    parser = argparse.ArgumentParser()

    parser.add_argument('-l',  '--layout', dest='keyh_layout',   type=str, required=True, help='Key hash store layout input file')
    parser.add_argument('-i',  '--input_dir', dest='input_key_dir',   type=str, default='', help='Keys input directory')
    parser.add_argument('-k',  '--priv_key', dest='PrivKey', type=str, required=True, help='Key Id or Private RSA 2048/RSA3072 key in PEM format to sign image')
    parser.add_argument('-a',  '--alg_hash', dest='HashType', type=str, choices=['SHA2_256', 'SHA2_384', 'AUTO'], default='AUTO', help='Hash type for signing. For AUTO hash type will be choosen based on key length')
    parser.add_argument('-s',  '--sign_scheme', dest='SignScheme', type=str, choices=['RSA_PKCS1', 'RSA_PSS'], default='RSA_PSS', help='Signing Scheme types')
    parser.add_argument('-svn', dest='svn', type=int, default=0, help='Security version number for key hash store')
    parser.add_argument('-o',  '--output', dest='out_bin', type=str, required=True, help='Output file for signed image')

    #
    # Parse command line arguments
    #
    args = parser.parse_args()

    layout = get_file_data(args.keyh_layout, 'r')
    key_list = eval ('[%s]' % layout.replace('\\', '/'))

    if args.HashType == 'AUTO':
        HashType = adjust_hash_type(args.PrivKey)

    gen_pub_key_hash_store (args.PrivKey, key_list, HashType, args.SignScheme, args.svn, args.input_key_dir, args.out_bin)

    print('Successfully created key hash store binary')

if __name__ == '__main__':
    sys.exit(main())
