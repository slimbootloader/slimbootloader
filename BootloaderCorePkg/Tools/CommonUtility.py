#!/usr/bin/env python
## @ CommonUtility.py
# Common utility script
#
# Copyright (c) 2016 - 2019, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

##
# Import Modules
#
import os
import sys
import re
import shutil
import subprocess
import struct
import hashlib
from   functools import reduce
from   ctypes import *

class LZ_HEADER(Structure):
	_pack_ = 1
	_fields_ = [
		('signature',       ARRAY(c_char, 4)),
		('compressed_len',  c_uint32),
		('length',          c_uint32),
		('reserved',        c_uint32)
	]
	_compress_alg = {
		'LZDM' : 'Dummy',
		'LZ4 ' : 'Lz4',
		'LZMA' : 'Lzma',
	}

# Hash values defined should match with cryptolib.h
HASH_TYPE_VALUE = {
# {   Hash_string:        Hash_Value}
			"SHA2_256"    : 0,
			"SHA2_384"    : 1,
			"SHA2_512"    : 2,
			"SM3_256"     : 3,
	}

# Sign values defined should match with cryptolib.h
SIGN_TYPE_VALUE = {
# {   Sign_string:        Sign_Value}
			"RSA2048"     : 0,
			"RSA3072 "    : 1,
			"SM2     "    : 2,
	}

AUTH_TYPE_HASH_VALUE = {
# {   Auth_type:        Hash_type}
			"SHA2_256"       : 0,
			"SHA2_384"       : 1,
			"SHA2_512"       : 2,
			"SM3_256"        : 3,
			"RSA2048SHA256"  : 0,
			"RSA3072SHA384"  : 1,
	}

def bytes_to_value (bytes):
	return reduce(lambda x,y: (x<<8)|y,  bytes[::-1] )

def value_to_bytes (value, length):
	return [(value>>(i*8) & 0xff) for i in range(length)]

def get_aligned_value (value, alignment = 4):
	if alignment != (1 << (alignment.bit_length() - 1)):
		raise Exception ('Alignment (0x%x) should to be power of 2 !' % alignment)
	value = (value + (alignment - 1)) & ~(alignment - 1)
	return value

def get_padding_length (data_len, alignment = 4):
	new_data_len = get_aligned_value (data_len, alignment)
	return new_data_len - data_len

def get_file_data (file, mode = 'rb'):
	return open(file, mode).read()

def gen_file_from_object (file, object):
	open (file, 'wb').write(object)

def gen_file_with_size (file, size):
	open (file, 'wb').write(b'\xFF' * size);

def check_files_exist (base_name_list, dir = '', ext = ''):
	for each in base_name_list:
		if not os.path.exists (os.path.join (dir, each + ext)):
			return False
	return True

def get_openssl_path ():
	if os.name == 'nt':
		if 'OPENSSL_PATH' not in os.environ:
			os.environ['OPENSSL_PATH'] = "C:\\Openssl\\"
		if 'OPENSSL_CONF' not in os.environ:
			openssl_cfg = "C:\\Openssl\\openssl.cfg"
			if os.path.exists(openssl_cfg):
				os.environ['OPENSSL_CONF'] = openssl_cfg
	openssl = os.path.join(os.environ.get ('OPENSSL_PATH', ''), 'openssl')
	return openssl

def run_process (arg_list, print_cmd = False, capture_out = False):
	sys.stdout.flush()
	if print_cmd:
		print (' '.join(arg_list))

	exc    = None
	result = 0
	output = ''
	try:
		if capture_out:
			output = subprocess.check_output(arg_list).decode()
		else:
			result = subprocess.call (arg_list)
	except Exception as ex:
		result = 1
		exc    = ex

	if result:
		if not print_cmd:
			print ('Error in running process:\n  %s' % ' '.join(arg_list))
		if exc is None:
			sys.exit(1)
		else:
			raise exc

	return output

def rsa_sign_file (priv_key, pub_key, hash_type, in_file, out_file, inc_dat = False, inc_key = False):

	_hash_type_string = {
			"SHA2_256"    : '-sha256',
			"SHA2_384"    : '-sha384',
			"SHA2_512"    : '-sha512',
	}

	cmdargs = [get_openssl_path(), 'dgst' , '%s' % _hash_type_string[hash_type], '-sign', '%s' % priv_key,
	           '-out', '%s' % out_file, '%s' % in_file]
	run_process (cmdargs)

	bins = bytearray()
	if inc_dat:
		bins.extend(get_file_data(in_file))
	out_data = get_file_data(out_file)
	bins.extend(out_data)
	if inc_key:
		key = gen_pub_key (priv_key, pub_key)
		bins.extend(key)
	if len(bins) != len(out_data):
		gen_file_from_object (out_file, bins)

def gen_pub_key (priv_key, pub_key = None):
	cmdline = [get_openssl_path(), 'rsa', '-pubout', '-text', '-noout', '-in', '%s' % priv_key]
	if pub_key:
		cmdline.extend (['-out', '%s' % pub_key])
		capture = False
	else:
		capture = True

	output = run_process (cmdline, capture_out = capture)
	if not capture:
		output = get_file_data(pub_key, 'r')
	data	 = output.replace('\r', '')
	data	 = data.replace('\n', '')
	data   = data.replace('  ', '')

	# Extract the modulus
	match = re.search('modulus(.*)publicExponent:\s+(\d+)\s+', data)
	if not match:
		raise Exception('Public key not found!')
	modulus	 = match.group(1).replace(':', '')
	exponent = int(match.group(2))

	# Remove the '00' from the front if the MSB is 1
	if (len(modulus) != 512):
		modulus = modulus[2:]
	mod = bytearray.fromhex(modulus)
	exp = bytearray.fromhex('{:08x}'.format(exponent))
	key = b"$IPP" + mod + exp
	if pub_key:
		gen_file_from_object (pub_key, key)

	return key

def decompress (in_file, out_file, tool_dir = ''):
	# Remove the Lz Header
	fi = open(in_file,'rb')
	di = bytearray(fi.read())
	fi.close()

	lz_hdr = LZ_HEADER.from_buffer (di)
	offset = sizeof (lz_hdr)
	if lz_hdr.signature == "LZDM":
		fo = open(out_file,'wb')
		fo.write(di[offset:offset + lz_hdr.compressed_len])
		fo.close()
		return

	temp   = 	os.path.splitext(out_file)[0] + '.tmp'
	if lz_hdr.signature == "LZMA":
		alg = "Lzma"
	elif lz_hdr.signature == "LZ4 ":
		alg = "Lz4"
	else:
		raise Exception ("Unsupported compression '%s' !" % alg)
	fo = open(temp, 'wb')
	fo.write(di[offset:offset + lz_hdr.compressed_len])
	fo.close()

	compress_tool = "%sCompress" % alg
	cmdline = [
		os.path.join (tool_dir, compress_tool),
		"-d",
		"-o", out_file,
		temp]
	run_process (cmdline, False, True)
	os.remove(temp)

def compress (in_file, alg, out_path = '', tool_dir = ''):
	basename, ext = os.path.splitext(os.path.basename (in_file))
	if out_path:
		if os.path.isdir (out_path):
			out_file = os.path.join(out_path, basename + '.lz')
		else:
			out_file = os.path.join(out_path)
	else:
		out_file = os.path.splitext(in_file)[0] + '.lz'

	if alg == "Lzma":
		sig = "LZMA"
	elif alg == "Tiano":
		sig = "LZUF"
	elif alg == "Lz4":
		sig = "LZ4 "
	elif alg == "Dummy":
		sig = "LZDM"
	else:
		raise Exception ("Unsupported compression '%s' !" % alg)
	if sig == "LZDM":
		shutil.copy(in_file, out_file)
	else:
		compress_tool = "%sCompress" % alg
		cmdline = [
			os.path.join (tool_dir, compress_tool),
			"-e",
			"-o", out_file,
			in_file]
		run_process (cmdline, False, True)

	compress_data = get_file_data(out_file)
	lz_hdr = LZ_HEADER ()
	lz_hdr.signature = sig.encode()
	lz_hdr.compressed_len = len(compress_data)
	lz_hdr.length = os.path.getsize(in_file)
	data = bytearray ()
	data.extend (lz_hdr)
	data.extend (compress_data)
	gen_file_from_object (out_file, data)

	return out_file

def get_priv_key_type (priv_key, openssl_path):

	print ("get_priv_key_type")
	Output = subprocess.check_output(
            [openssl_path, 'rsa', '-pubout', '-text', '-noout', '-in',
             '%s' % priv_key],
            stderr=subprocess.STDOUT).decode()

	Match = re.match(r'Private-Key: (.*)\n', Output, re.M|re.I)
	temp = re.findall(r'\d+', Match.group(1))
	key_type = list(map(int, temp))

	print ("Key Type: RSA%s" % key_type[0])

	return key_type
