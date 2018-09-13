## @ GenCapsuleFirmware.py
#
# Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License that accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php.
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

import os
import argparse
import subprocess
import sys
from ctypes import *
import re
import struct
import uuid
import ctypes

sys.dont_write_bytecode = True
from BuildUtility import rsa_sign_file, gen_pub_key

#
# GUID for firmware update image file
#
FIRMWARE_UPDATE_IMAGE_FILE_GUID = uuid.UUID('{1A3EAE58-B580-4fef-ACA3-A16D9E00DF5F}')
#0x1a3eae58, 0xb580, 0x4fef, 0xac, 0xa3, 0xa1, 0x6d, 0x9e, 0x0, 0xdf, 0x5f);

class Firmware_Update_Header(Structure):
  _fields_ = [
    ('FileGuid',           ARRAY(c_uint8, 16)),
    ('HeaderSize',         c_uint32),
    ('FirmwreVersion',     c_uint32),
    ('CapsuleFlags',       c_uint32),
    ('PubKeyOffset',       c_uint32),
    ('PubKeySize',         c_uint32),
    ('ImageOffset',        c_uint32),
    ('ImageSize',          c_uint32),
    ('SignatureOffset',    c_uint32),
    ('SignatureSize',      c_uint32),
    ('Reserved',           ARRAY(c_uint32, 3))
  ]

def sign_image (Infile, OutFile, priv_key, img_type):
	#
	# Generate the new image layout
	# 1. write firmware update header
	#
	unsigned_image            = bytearray ( sizeof(Firmware_Update_Header) )
	header                    = Firmware_Update_Header.from_buffer(unsigned_image, 0)

	file_size_alignment       = 0x1000
	file_size                 = os.path.getsize(Infile)
	aligned_size              = (file_size + file_size_alignment - 1) & ~(file_size_alignment - 1)
	append_size               = aligned_size - file_size

	header.FileGuid           = (ctypes.c_ubyte * 16).from_buffer_copy(FIRMWARE_UPDATE_IMAGE_FILE_GUID.get_bytes_le())
	header.HeaderSize         = sizeof (Firmware_Update_Header)
	header.FirmwreVersion     = 1
	header.CapsuleFlags       = 0 if img_type == 'b' else 1
	header.ImageOffset        = header.HeaderSize
	header.ImageSize          = aligned_size
	header.SignatureOffset    = header.ImageOffset + header.ImageSize
	header.SignatureSize      = 256
	header.PubKeyOffset       = header.SignatureOffset + header.SignatureSize

	pubkey_file = 'fwu_public_key.bin'

	gen_pub_key(priv_key, pubkey_file)

	header.PubKeySize = os.path.getsize(pubkey_file)

	InfileData = open(Infile, 'rb').read()
	if img_type == 'b':
		# for BIOS image, adding padding from head
		unsigned_image.extend('\xff' * append_size + InfileData)
	else:
		# for CFGDATA image, adding padding to tail
		unsigned_image.extend(InfileData + '\xff' * append_size)

	fwupdate_bin_file = 'fwupdate_unsigned.bin'
	open(fwupdate_bin_file, 'wb').write(unsigned_image)

	rsa_sign_file(priv_key, pubkey_file, fwupdate_bin_file, OutFile, True, True)

	os.remove(pubkey_file)
	os.remove(fwupdate_bin_file)

def main():

  if os.name == 'nt':
    os.environ['OPENSSL_PATH'] = "C:\\Openssl\\"

  #
  # Create command line argument parser object
  #
  parser = argparse.ArgumentParser()
  parser.add_argument('-b',  '--image', dest='Image', type=str, help='Raw image into firmware update capsule image', required = True)
  parser.add_argument('-t',  '--type', dest='Type', choices=['b','c'], help='Image type b:Bootloader c:CfgData', default = 'b')
  parser.add_argument('-k',  '--priv_key', dest='PrivKey', type=str, help='Private RSA 2048 key in PEM format to sign image', required = True)
  parser.add_argument('-o',  '--output', dest='NewImage', type=str, help='Output file for signed image', required = True)
  parser.add_argument("-q", "--quiet", dest='Quiet', action="store_true", help="without output messages or temp files")

  #
  # Parse command line arguments
  #
  args = parser.parse_args()

  sign_image(args.Image, args.NewImage, args.PrivKey, args.Type)
  image_type = 'Bootloader' if args.Type == 'b' else 'CfgData'
  print "Successfully signed %s image!" % image_type
  return 0;

if __name__ == '__main__':
	sys.exit(main())