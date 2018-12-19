#!/usr/bin/env python
## @ BuildUtility.py
# Build bootloader main script
#
# Copyright (c) 2016 - 2018, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials are licensed and made available under
# the terms and conditions of the BSD License that accompanies this distribution.
# The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php.
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

##
# Import Modules
#
import os
import sys
import re
import glob
import struct
import shutil
import hashlib
import subprocess
import datetime
import zipfile
import ntpath
from   ctypes import *

sys.dont_write_bytecode = True
sys.path.append (os.path.join(os.path.dirname(__file__), '..', '..', 'IntelFsp2Pkg', 'Tools'))
from   SplitFspBin import RebaseFspBin, FirmwareDevice, EFI_SECTION_TYPE, FSP_INFORMATION_HEADER, PeTeImage, Bytes2Val


AUTO_GEN_DSC_HDR = """#
#  DO NOT EDIT
#  FILE auto-generated
#  Module name:
#    Platform.dsc
#  Abstract:       Auto-generated Platform.dsc to be included in primary DSC.
#
"""

gtools = {
	'FV_PATCH'   : 'BootloaderCorePkg/Tools/PatchFv.py',
	'GEN_CFG'    : 'BootloaderCorePkg/Tools/GenCfgData.py',
	'FSP_SPLIT'  : 'IntelFsp2Pkg/Tools/SplitFspBin.py',
	'IMG_REPORT' : 'BootloaderCorePkg/Tools/GenReport.py',
	'CFG_DATA'   : 'BootloaderCorePkg/Tools/CfgDataTool.py'
}

class STITCH_OPS:
	MODE_FILE_NOP   = 0x00
	MODE_FILE_ALIGN = 0x01
	MODE_FILE_PAD   = 0x02
	MODE_FILE_IGNOR = 0x80
	MODE_POS_TAIL   = 0
	MODE_POS_HEAD   = 1


class FLASH_REGION_TYPE:
	DESCRIPTOR   = 0x0
	BIOS         = 0x1
	ME           = 0x2
	GBE          = 0x3
	PLATFORMDATA = 0x4
	DER          = 0x5
	ALL          = 0x6
	MAX          = 0x7

class RsaSignature (Structure):
	_pack_ = 1
	_fields_ = [
		('Signature',  ARRAY(c_uint8, 256)),
		('Identifier', c_uint32),
		('PubKeyMod',  ARRAY(c_uint8, 256)),
		('PubKeyExp',  ARRAY(c_uint8, 4)),
		('Padding',    ARRAY(c_uint8, 8)),
		]

class UcodeHeader(Structure):
	_pack_ = 1
	_fields_ = [
		('header_version',  c_uint32),
		('update_revision',  c_uint32),
		('date',  c_uint32),
		('processor_signature',  c_uint32),
		('checksum',  c_uint32),
		('loader_revision',  c_uint32),
		('processor_flags',  c_uint32),
		('data_size',  c_uint32),
		('total_size',  c_uint32),
		('reserved',  ARRAY(c_uint32, 12)),
		]


class FitEntry(Structure):

	FIT_SIGNATURE = '_FIT_   '

	_pack_ = 1
	_fields_ = [
		('address',  c_uint64),
		('size',     c_uint32), # Bits[31:24] Reserved
		('version',  c_uint16),
		('type',     c_uint8), # Bit[7] = C_V
		('checksum', c_uint8),
		]

	def set_values(self, _address, _size, _version, _type, _checksum):
		self.address  = _address
		self.size     = _size
		self.version  = _version
		self.type     = _type
		self.checksum = _checksum


class HashStore(Structure):

	HASH_STORE_SIGNATURE    = '_HS_'
	HASH_STORE_MAX_IDX_NUM  = 8
	HASH_STORE_ENTRY_LEN    = 32

	_pack_ = 1
	_fields_ = [
		('Signature',         ARRAY(c_char, 4)),
		('Valid',             c_uint32),
		('Data',              ARRAY(c_uint8, HASH_STORE_ENTRY_LEN * HASH_STORE_MAX_IDX_NUM)),
		]

	def __init__(self):
		self.Signature = HashStore.HASH_STORE_SIGNATURE


class ImageVer(Structure):
	_pack_ = 1
	_fields_ = [
		('BuildNumber',       c_uint16),
		('ProjMinorVersion',  c_uint8),
		('ProjMajorVersion',  c_uint8),
		('CoreMinorVersion',  c_uint8),
		('CoreMajorVersion',  c_uint8),
		('SecureVerNum',      c_uint8),
		('Reserved',          c_uint8, 5),
		('BldDebug',          c_uint8, 1),
		('FspDebug',          c_uint8, 1),
		('Dirty',             c_uint8, 1),
		]


class VerInfo(Structure):
	_pack_ = 1
	_fields_ = [
		('Signature',         ARRAY(c_char, 4)),
		('HeaderLength',      c_uint16),
		('HeaderRevision',    c_uint8),
		('Reserved',          c_uint8),
		('ImageId',           c_uint64),
		('ImageVersion',      ImageVer),
		('SourceVersion',     c_uint64),
		]


class VariableRegionHeader(Structure):
	_pack_ = 1
	_fields_ = [
		('Signature',        ARRAY(c_char, 4)),
		('Size',             c_uint32),
		('Format',           c_uint8),
		('State',            c_uint8),
		('Reserved',         ARRAY(c_char, 6))
		]


class FlashMapDesc(Structure):
	_pack_ = 1
	_fields_ = [
		('sig',             ARRAY(c_char, 4)),
		('flags',           c_uint32),
		('offset',          c_uint32),
		('size',            c_uint32),
		]


class FlashMap(Structure):

	FLASH_MAP_SIGNATURE = 'FLMP'

	FLASH_MAP_COMPONENT_SIGNATURE = {
		"STAGE1A"       : "SG1A",
		"STAGE1B"       : "SG1B",
		"STAGE2"        : "SG02",
		"ACM"           : "ACM0",
		"UCODE"         : "UCOD",
		"MRCDATA"       : "MRCD",
		"VARIABLE"      : "VARS",
		"PAYLOAD"       : "PYLD",
		"EPAYLOAD"      : "EPLD",
		"SPI_IAS1"      : "IAS1",
		"SPI_IAS2"      : "IAS2",
		"FWUPDATE"      : "FWUP",
		"CFGDATA"       : "CNFG",
		"BPM"           : "_BPM",
		"OEMKEY"        : "OEMK",
		"SBLRSVD"       : "RSVD",
		"EMPTY"         : "EMTY",
		"UNKNOWN"       : "UNKN",
		}

	FLASH_MAP_ATTRIBUTES = {
	  "PRIMARY_REGION"  : 0x00000000,
	  "BACKUP_REGION"   : 0x00000001,
	}

	FLASH_MAP_DESC_FLAGS = {
	  "TOP_SWAP"      : 0x00000001,
	  "REDUNDANT"     : 0x00000002,
	  "NON_REDUNDANT" : 0x00000004,
	  "NON_VOLATILE"  : 0x00000008,
	  "COMPRESSED"    : 0x00000010,
	  "BACKUP"        : 0x00000040,
	}

	_pack_ = 1
	_fields_ = [
		('sig',              ARRAY(c_char, 4)),
		('version',          c_uint16),
		('length',           c_uint16),
		('attributes',       c_uint8),
		('reserved',         ARRAY(c_char, 3)),
		('romsize',          c_uint32),
		]

	def __init__(self):
		self.sig = FlashMap.FLASH_MAP_SIGNATURE
		self.version = 1
		self.romsize = 0
		self.attributes = 0
		self.length  = sizeof(self)
		self.descriptors = []

	def add(self, desc):
		self.descriptors.append(desc)

	def finalize (self):
		# Calculate size of the flash map
		self.romsize = sum ([x.size for x in self.descriptors])
		self.length  = sizeof(self) + len(self.descriptors) * sizeof(FlashMapDesc)


def split_fsp(path, out_dir):
	sys.stdout.flush()
	x = subprocess.call([
				"python",
				gtools['FSP_SPLIT'],
				"split",
				"-f", path,
				"-n", "FSP.bin",
				"-o", out_dir])
	if x: sys.exit(1)


def rebase_fsp(path, out_dir, base_t, base_m, base_s):
	sys.stdout.flush()
	x = subprocess.call([
		"python",
		gtools['FSP_SPLIT'],
		"rebase",
		"-f", path,
		"-b", "0x%x" % base_t, "0x%x" % base_m, "0x%x" % base_s,
		"-c", "t" , "m", "s",
		"-n", "Fsp.bin",
		"-o", out_dir])
	if x: sys.exit(1)


def patch_fv(fv_dir, fvs, *vargs):
	sys.stdout.flush()
	args = filter (lambda x: x != '', list(vargs))
	x = subprocess.call(["python", gtools['FV_PATCH'], fv_dir, fvs] + args)
	if x: sys.exit(1)


def gen_cfg_data (command, dscfile, outfile):
	sys.stdout.flush()
	x = subprocess.call([
			"python",
			gtools['GEN_CFG'],
			command,
			dscfile,
			outfile])
	if x: sys.exit(1)


def cfg_data_tool (command, infiles, outfile, extra = []):
	arg_list = ["python",	gtools['CFG_DATA'], command,	'-o', outfile]
	arg_list.extend (extra)
	arg_list.extend (infiles)
	print ' '.join(arg_list)
	sys.stdout.flush()
	x = subprocess.call(arg_list)
	if x: sys.exit(1)


def compress (path, alg="Lzma"):
	root_path = os.path.splitext(path)[0]
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
		shutil.copy(path, root_path+'.lz')
	else:
		cmdline = [
			"%sCompress" % alg,
			"-e",
			"-o", root_path+'.lz',
			path]
		sys.stdout.flush()
		x = subprocess.call(cmdline)
		if x: sys.exit(1)
	add_file_size(root_path+'.lz', path, sig)


def report_image_layout (fv_dir, stitch_file, report_file):
	sys.stdout.flush()
	rpt_file = open(os.path.join(fv_dir, report_file), "w")
	x = subprocess.call(["python", gtools['IMG_REPORT'], fv_dir, stitch_file, ""], stdout=rpt_file)
	rpt_file.close()
	if x: sys.exit(1)


def get_fsp_size (path):
	di = open(path,'rb').read()[0x20:0x24]
	return struct.unpack('I', di)[0]


def get_fsp_upd_size (path):
	di = open(path,'rb').read()[0xBC:0xC0]
	return ((struct.unpack('I', di)[0] + 0x10) & 0xFFFFFFF0)


def get_fsp_revision (path):
	di = open(path,'rb').read()[0xA0:0xA4]
	return struct.unpack('I', di)[0]


def get_fsp_image_id (path):
	di = open(path,'rb').read()[0xA4:0xAC]
	return struct.unpack('8s', di[:8])[0].rstrip('\x00')


def get_redundant_info (comp_name):
	comp_base = os.path.splitext(os.path.basename(comp_name))[0].upper()
	match = re.match('(\w+)_([AB])$', comp_base)
	if match:
		comp_name = match.group(1)
		part_name = match.group(2)
	else:
		comp_name = comp_base
		part_name = ''
	return comp_name, part_name


def get_payload_list (payloads):
	pld_tmp  = dict()
	pld_lst  = []
	pld_num  = len(payloads)

	for idx, pld in enumerate(payloads):
		items    = pld.split(':')
		item_cnt = len(items)
		pld_tmp['file'] = items[0]

		if item_cnt > 1 and items[1].strip():
			pld_tmp['name'] = ("%-4s" % items[1])[:4]
		else:
			pld_tmp['name'] = 'PLD%d' % idx if pld_num > 1 else ''

		if item_cnt > 2 and items[2].strip():
			pld_tmp['algo'] = items[2]
		else:
			pld_tmp['algo'] = 'Lz4'

		pld_lst.append(dict(pld_tmp))

	return pld_lst


def gen_file_from_object (file, object):
	open (file, 'wb').write(object)


def gen_file_with_size (file, size):
	open (file, 'wb').write('\xFF' * size);


def gen_ias_file (rel_file_path, file_space, out_file):
	bins = bytearray()
	file_path = os.path.join(os.environ['PLT_SOURCE'], rel_file_path)
	if os.path.exists(file_path):
		ias_fh   = open (file_path, 'rb')
		file_bin = ias_fh.read()
		ias_fh.close ()
	else:
		file_bin = ''
	file_size = len(file_bin)
	if file_size > file_space:
		raise Exception ("Insufficient region size 0x%X for file '%s', requires size 0x%X!" % (file_space, os.path.basename(file_path), file_size))
	bins.extend (file_bin + '\xff' * (file_space - file_size))
	open (out_file, 'wb').write (bins)

def gen_flash_map_bin (flash_map_file, comp_list):
	flash_map = FlashMap()
	for comp in reversed(comp_list):
		desc  = FlashMapDesc ()
		desc.sig    = FlashMap.FLASH_MAP_COMPONENT_SIGNATURE[comp['bname']]
		desc.flags  = comp['flag']
		desc.offset = comp['offset']
		desc.size   = comp['size']
		flash_map.add (desc)
	flash_map.finalize ()

	fd = open (flash_map_file, 'wb')
	fd.write(flash_map)
	for desc in flash_map.descriptors:
		fd.write(desc)
	fd.close()


def gen_config_file (fv_dir, brd_name, platform_id, pri_key, cfg_db_size, cfg_size, cfg_int, cfg_ext):
	# Remove previous generated files
	for file in glob.glob(os.path.join(fv_dir, "CfgData*.*")):
			os.remove(file)

	CfgIntLen = len(cfg_int)

	# Generate CFG data
	brd_name_dir      = os.path.join(os.environ['PLT_SOURCE'], 'Platform', brd_name)
	comm_brd_dir      = os.path.join(os.environ['SBL_SOURCE'], 'Platform', 'CommonBoardPkg')
	brd_cfg_dir       = os.path.join(brd_name_dir, 'CfgData')
	com_brd_cfg_dir   = os.path.join(comm_brd_dir, 'CfgData')
	cfg_hdr_file      = os.path.join(brd_name_dir, 'Include', 'ConfigDataStruct.h')
	cfg_com_hdr_file  = os.path.join(comm_brd_dir, 'Include', 'ConfigDataCommonStruct.h')
	cfg_inc_file      = os.path.join(brd_name_dir, 'Include', 'ConfigDataBlob.h')
	cfg_dsc_file      = os.path.join(brd_cfg_dir, 'CfgDataDef.dsc')
	cfg_hdr_dyn_file  = os.path.join(brd_name_dir, 'Include', 'ConfigDataDynamic.h')
	cfg_dsc_dyn_file  = os.path.join(brd_cfg_dir, 'CfgDataDynamic.dsc')
	cfg_pkl_file      = os.path.join(fv_dir, "CfgDataDef.pkl")
	cfg_bin_file      = os.path.join(fv_dir, "CfgDataDef.bin")  #default core dsc file cfg data
	cfg_bin_int_file  = os.path.join(fv_dir, "CfgDataInt.bin")  #_INT_CFG_DATA_FILE settings
	cfg_bin_ext_file  = os.path.join(fv_dir, "CfgDataExt.bin")  #_EXT_CFG_DATA_FILE settings
	cfg_comb_dsc_file = os.path.join(fv_dir, 'CfgDataDef.dsc')

	# Generate parsed result into pickle file to improve performance
	if os.path.exists(cfg_dsc_dyn_file):
			gen_cfg_data ("GENHDR", cfg_dsc_dyn_file, cfg_hdr_dyn_file)

	gen_cfg_data ("GENPKL", cfg_dsc_file, cfg_pkl_file)
	gen_cfg_data ("GENDSC", cfg_pkl_file, cfg_comb_dsc_file)
	gen_cfg_data ("GENHDR", cfg_pkl_file, ';'.join([cfg_hdr_file, cfg_com_hdr_file]))
	gen_cfg_data ("GENBIN", cfg_pkl_file, cfg_bin_file)

	cfg_base_file = None
	for cfg_file_list in [cfg_int, cfg_ext]:
		if cfg_file_list is cfg_int:
			cfg_merged_bin_file = cfg_bin_int_file
			cfg_file_list.insert(0, 'CfgDataDef.bin');
		else:
			cfg_merged_bin_file = cfg_bin_ext_file

		cfg_bin_list = []
		for dlt_file in cfg_file_list:
			cfg_dlt_file  = os.path.join(brd_cfg_dir, dlt_file)
			if not os.path.exists(cfg_dlt_file):
				test_file = os.path.join(fv_dir, dlt_file)
				if os.path.exists(test_file):
					cfg_dlt_file = test_file
			if dlt_file.lower().endswith('.dlt'):
				bas_path = os.path.join (fv_dir, os.path.basename(cfg_dlt_file))
				bas_path = os.path.splitext(bas_path)[0]
				cfg_brd_bin_file = bas_path + '.bin'
				gen_cfg_data ("GENBIN", '%s;%s' % (cfg_pkl_file, cfg_dlt_file), cfg_brd_bin_file)
			else:
				cfg_brd_bin_file = cfg_dlt_file if os.path.exists(cfg_dlt_file) else os.path.join(fv_dir, dlt_file)
			if (cfg_file_list is cfg_int) and (cfg_base_file is None):
				cfg_base_file = cfg_bin_int_file
			cfg_bin_list.append (cfg_brd_bin_file)

		if cfg_bin_list:
			extra = []
			if cfg_file_list is cfg_ext:
				cfg_bin_list.insert(0, cfg_base_file + '*')
			else:
				if platform_id is not None:
					extra = ['-p', '%d' % platform_id]
			cfg_data_tool ('merge', cfg_bin_list, cfg_merged_bin_file, extra)
			bin_file_size = os.path.getsize(cfg_merged_bin_file)
			cfg_db_size
			if cfg_file_list is cfg_int:
				cfg_rgn_size = cfg_db_size
				cfg_rgn_name = 'internal'
			else:
				cfg_rgn_size = cfg_size
				cfg_rgn_name = 'external'
			if bin_file_size >= cfg_rgn_size:
				raise Exception ('CFGDATA_SIZE is too small, requested 0x%X for %s CFGDATA !' % (bin_file_size, cfg_rgn_name))

			if cfg_file_list is cfg_int:
				gen_cfg_data ("GENINC", cfg_merged_bin_file, cfg_inc_file)

	if not os.path.exists(cfg_merged_bin_file):
		cfg_merged_bin_file = cfg_bin_int_file

	cfg_final_file = os.path.join(fv_dir, "CFGDATA.bin")
	if pri_key:
		cfg_data_tool ('sign', ['-k', pri_key, cfg_merged_bin_file], cfg_final_file)
	else:
		shutil.copy(cfg_merged_bin_file, cfg_final_file)

	# generate CfgDataStitch script
	tool_dir    = os.path.abspath(os.path.dirname(__file__))
	src_file    = os.path.join (tool_dir, 'CfgDataStitch.py')
	dst_file    = os.path.join (fv_dir,   'CfgDataStitch.py')

	fd = open(src_file, 'r')
	script_txt  = fd.read()
	fd.close ()

	def_dlt_file = 'CfgData_Default.dlt'
	dlt_list    = cfg_int[1:] + cfg_ext
	dlt_files   = str([def_dlt_file] + dlt_list)
	replace_txt = script_txt.replace ('dlt_files = [] # TO BE PATCHED', 'dlt_files = %s' % (dlt_files))
	if dlt_files not in replace_txt:
		raise Exception ('Failed to generate project CfgDataStitch.py script !')

	fd = open(dst_file, 'w')
	fd.write(replace_txt)
	fd.close()

	# copy delta files
	shutil.copy (os.path.join (com_brd_cfg_dir, def_dlt_file), os.path.join (fv_dir, def_dlt_file))
	for dlt_file in dlt_list:
		shutil.copy (os.path.join (brd_cfg_dir, dlt_file), os.path.join (fv_dir, dlt_file))

def gen_payload_bin (fv_dir, pld_list, pld_bin, priv_key):
	fv_dir = os.path.dirname (pld_bin)
	for idx, pld in enumerate(pld_list):
		if pld['file'] in ['OsLoader.efi', 'FirmwareUpdate.efi']:
			pld_base_name = pld['file'].split('.')[0]
			src_file = "../IA32/PayloadPkg/%s/%s/OUTPUT/%s.efi" % (pld_base_name, pld_base_name, pld_base_name)
			src_file = os.path.join(fv_dir, src_file)
		else:
			src_file = os.path.join("PayloadPkg", "PayloadBins", pld['file'])
		if idx == 0:
			dst_path = pld_bin
		else :
			dst_path = os.path.join(fv_dir, os.path.basename(src_file))
		shutil.copy (src_file, dst_path)

	epld_bin   = os.path.join(os.path.dirname(pld_bin), 'E' + os.path.basename(pld_bin))
	ext_list   = pld_list[1:]
	if len(ext_list) == 0:
		# Create a empty EPAYLOAD.bin
		open (epld_bin, 'wb').close()
		return

	# Multiple payloads format
	entry_len  = 48
	entry_num  = len(ext_list)
	sig_len    = sizeof(RsaSignature)
	hdr_len    = entry_num * entry_len + sig_len + 16
	fout       = open (epld_bin, 'wb')
	fout.write (struct.pack('4sIII', 'LZDM', hdr_len, hdr_len, 0))
	fout.write (struct.pack('4sIII', '$PLD', entry_num, entry_len, 0))
	hdr_len    = 16 + hdr_len



	lz_list = []
	offset  = hdr_len
	for pld in ext_list:
		dst_path = os.path.join(fv_dir, os.path.basename(pld['file']))
		compress (dst_path, pld['algo'])
		base_path = os.path.splitext(dst_path)[0]
		comp_path = base_path  + '.lz'
		length = os.path.getsize(comp_path)
		fout.write(pld['name'])
		fout.write(struct.pack('III', offset, length, 0))
		with open (comp_path,'rb') as fin:
			fout.write (hashlib.sha256(bytearray(fin.read())).digest())
		newoff = (offset + length + 15) & ~0x0F
		lz_list.append((comp_path, newoff - offset - length))
		offset = newoff
	fout.close()

	pld_key_file = os.path.join(fv_dir, "EPLDKEY.bin")
	hdr_file = os.path.splitext(epld_bin)[0] + '_HDR.bin'
	rsa_sign_file (priv_key, pld_key_file, epld_bin, hdr_file, False, True)

	fd = open (hdr_file, 'rb')
	sig_bin = fd.read()
	fd.close()

	padding = sizeof(RsaSignature) - RsaSignature.Padding.offset
	if len(sig_bin) != sig_len - padding:
			raise Exception ("Unexpected signautre length !")

	fout       = open (epld_bin, 'ab')
	fout.write (sig_bin + '\x00' * padding)

	for lz in lz_list:
		with open(lz[0],'rb') as fin:
			fout.write (fin.read())
			fout.write ('\x00' * lz[1])

	fout.close()


def gen_hash_file (src_path, hash_path = '', is_key = False):
	if not hash_path:
		hash_path = os.path.splitext(src_path)[0] + '.hash'
	with open(src_path,'rb') as fi:
		di = bytearray(fi.read())
	if is_key:
		if len(di) != 0x108:
			raise Exception ("Invalid public key binary!")
		di = di[4:]
		di = di[:0x100][::-1] + di[0x100:][::-1]
	ho = hashlib.sha256(di)
	with open(hash_path,'wb') as fo:
		fo.write(ho.digest())


def add_file_size (dst_file, src_path, signature):
	fi = open(dst_file,'rb')
	di = fi.read()
	fi.close()
	fo = open(dst_file,'wb')
	fo.write(signature[0:4])
	fo.write(struct.pack('I', len(di)))
	fo.write(struct.pack('I', os.path.getsize(src_path)))
	fo.write(struct.pack('I', 0))
	fo.write(di)
	fo.close()


def align_pad_file (src, dst, val, mode = STITCH_OPS.MODE_FILE_ALIGN, pos = STITCH_OPS.MODE_POS_TAIL):
	fi = open(src, 'rb')
	di = fi.read()
	fi.close()
	srclen = len(di)
	if mode == STITCH_OPS.MODE_FILE_ALIGN:
		if not (((val & (val - 1)) == 0) and val != 0):
			raise Exception ("Invalid alignment %X for file '%s'!" % (val, os.path.basename(src)))
		val   -= 1
		newlen = (srclen + val) & ((~val) & 0xFFFFFFFF)
	elif mode == STITCH_OPS.MODE_FILE_PAD:
		if val < srclen:
			raise Exception ("File '%s' size 0x%X is greater than padding size 0x%X !" % \
					(os.path.basename(src), srclen, val))
		newlen = val
	elif mode == STITCH_OPS.MODE_FILE_NOP:
		return
	else:
		raise Exception ('Unsupported align mode %d !' % mode)
	padding = '\xff' * (newlen - srclen)
	if dst == '':
		dst = src
	fo = open(dst,'wb')
	if pos == STITCH_OPS.MODE_POS_HEAD:
		fo.write(padding)
	fo.write(di)
	if pos == STITCH_OPS.MODE_POS_TAIL:
		fo.write(padding)
	fo.close()


def get_verinfo_via_file (file):
	if not os.path.exists(file):
		raise Exception ("Version TXT file '%s' does not exist!" % file)
	hfile = open(file)
	lines = hfile.readlines()
	hfile.close()

	ver_dict = dict()
	for line in lines:
		elements = line.strip().split('=')
		if len(elements) == 2:
				ver_dict[elements[0].strip()] = elements[1].strip()
	image_id = '%-8s' % ver_dict['ImageId']
	image_id = image_id[0:8]

	ver_info = VerInfo ()
	ver_info.Signature      = '$SBH'
	ver_info.HeaderLength   = sizeof(ver_info)
	ver_info.HeaderRevision = 1
	ver_info.ImageId        = struct.unpack('Q', image_id)[0]
	try:
		ver_info.SourceVersion  = int(ver_dict['SourceVersion'], 16)
		ver_info.ImageVersion.ProjMinorVersion = int(ver_dict['ProjMinorVersion'])
		ver_info.ImageVersion.ProjMajorVersion = int(ver_dict['ProjMajorVersion'])
		ver_info.ImageVersion.CoreMinorVersion = int(ver_dict['CoreMinorVersion'])
		ver_info.ImageVersion.CoreMajorVersion = int(ver_dict['CoreMajorVersion'])
		ver_info.ImageVersion.BuildNumber  = int(ver_dict['BuildNumber'])
		ver_info.ImageVersion.SecureVerNum = int(ver_dict['SecureVerNum'])
		ver_info.ImageVersion.FspDebug     = int(ver_dict['FspDebug'])
		ver_info.ImageVersion.BldDebug     = int(ver_dict['BldDebug'])
		ver_info.ImageVersion.Dirty        = int(ver_dict['Dirty'])
	except KeyError:
		raise Exception ("Invalid version TXT file format!")

	return ver_info


def get_verinfo_via_git (ver_dict):
	gitcmd   = 'git describe --dirty --abbrev=16 --always'
	command  = subprocess.Popen(gitcmd, shell=True, stdout=subprocess.PIPE)
	line     = command.stdout.readline().strip()
	commitid = 0
	dirty    = 0
	if len(line) >= 16:
		if line.endswith('dirty'):
			dirty = 1
			line = line[:-6]
		try:
			commitid = int(line[-16:], 16)
		except ValueError:
			commitid = 0
	imgid = '%-8s' % ver_dict['VERINFO_IMAGE_ID']
	imgid = imgid[0:8]

	date_format = "%m/%d/%Y"
	base_date = datetime.datetime.strptime(ver_dict['VERINFO_BUILD_DATE'], date_format)
	delta     = datetime.datetime.now() - base_date

	ver_info = VerInfo ()
	ver_info.Signature      = '$SBH'
	ver_info.HeaderLength   = sizeof(ver_info)
	ver_info.HeaderRevision = 1
	if os.environ.get('BUILD_NUMBER'):
		build_number = int(os.environ['BUILD_NUMBER'])
		if build_number >= 65536:
			raise Exception ('BUILD_NUMBER is too large (<65536)')
	else:
		build_number  = int(delta.total_seconds()) // 3600
	ver_info.ImageVersion.BuildNumber  = build_number
	ver_info.ImageId                   = struct.unpack('Q', imgid)[0]
	ver_info.SourceVersion             = commitid
	ver_info.ImageVersion.ProjMinorVersion = ver_dict['VERINFO_PROJ_MINOR_VER']
	ver_info.ImageVersion.ProjMajorVersion = ver_dict['VERINFO_PROJ_MAJOR_VER']
	ver_info.ImageVersion.CoreMinorVersion = ver_dict['VERINFO_CORE_MINOR_VER']
	ver_info.ImageVersion.CoreMajorVersion = ver_dict['VERINFO_CORE_MAJOR_VER']
	ver_info.ImageVersion.SecureVerNum = ver_dict['VERINFO_SVN']
	ver_info.ImageVersion.FspDebug     = 1 if ver_dict['FSPDEBUG_MODE'] else 0;
	ver_info.ImageVersion.BldDebug     = 0 if ver_dict['RELEASE_MODE']  else 1;
	ver_info.ImageVersion.Dirty        = dirty

	return ver_info


def gen_ver_info_txt (ver_file, ver_info):
	h_file = open (ver_file, 'w')
	h_file.write('#\n')
	h_file.write('# This file is automatically generated. Please do NOT modify !!!\n')
	h_file.write('#\n\n')
	h_file.write('ImageId       = %s\n'  % struct.pack('<Q', ver_info.ImageId))
	h_file.write('SourceVersion = %016x\n' % ver_info.SourceVersion)
	h_file.write('SecureVerNum  = %03d\n'  % ver_info.ImageVersion.SecureVerNum)
	h_file.write('ProjMajorVersion  = %03d\n'  % ver_info.ImageVersion.ProjMajorVersion)
	h_file.write('ProjMinorVersion  = %03d\n'  % ver_info.ImageVersion.ProjMinorVersion)
	h_file.write('CoreMajorVersion  = %03d\n'  % ver_info.ImageVersion.CoreMajorVersion)
	h_file.write('CoreMinorVersion  = %03d\n'  % ver_info.ImageVersion.CoreMinorVersion)
	h_file.write('BuildNumber   = %05d\n'  % ver_info.ImageVersion.BuildNumber)
	h_file.write('FspDebug      = %d\n'    % ver_info.ImageVersion.FspDebug)
	h_file.write('BldDebug      = %d\n'    % ver_info.ImageVersion.BldDebug)
	h_file.write('Dirty         = %d\n'    % ver_info.ImageVersion.Dirty)
	h_file.close()


def rsa_sign_file (priv_key, pub_key, in_file, out_file, inc_dat = False, inc_key = False):
	cmdline = os.path.join(os.environ.get ('OPENSSL_PATH', ''), 'openssl')
	cmdargs = [cmdline, 'dgst' , '-sha256', '-sign', '%s' % priv_key, '-out', '%s' % out_file, '%s' % in_file]
	x = subprocess.call(cmdargs)
	if x:
		raise Exception ('Failed to generate signature using openssl !')

	if inc_dat:
		bins = bytearray(open(in_file, 'rb').read())
	else:
		bins = bytearray()
	sign = open(out_file, 'rb').read()
	bins.extend(sign)

	key = gen_pub_key (priv_key, pub_key)

	if inc_key:
		bins.extend(key)
	open(out_file, 'wb').write(bins)


def gen_pub_key (priv_key, pub_key):
	cmdline = os.path.join(os.environ.get ('OPENSSL_PATH', ''), 'openssl')
	x = subprocess.call([cmdline, 'rsa', '-pubout', '-text', '-out', '%s' % pub_key, '-noout', '-in', '%s' % priv_key])
	if x:
		raise Exception ('Failed to generate public key using openssl !')

	data = open(pub_key, 'r').read()
	data = data.replace('\r', '')
	data = data.replace('\n', '')
	data = data.replace('  ', '')

	#
	# Extract the modulus
	#
	match = re.search('modulus(.*)publicExponent:\s+(\d+)\s+', data)
	if not match:
		raise Exception('Public key not found!')
	modulus  = match.group(1).replace(':', '')
	exponent = int(match.group(2))

	# Remove the '00' from the front if the MSB is 1
	if (len(modulus) != 512):
		modulus = modulus[2:]
	mod = bytearray.fromhex(modulus)
	exp = bytearray.fromhex('{:08x}'.format(exponent))

	key = "$IPP" + mod + exp
	open(pub_key, 'wb').write(key)

	return key


def copy_images_to_output (fv_dir, zip_file, img_list, rgn_name_list, out_list):
	zip_path_file = os.path.join (os.environ['WORKSPACE'], zip_file)
	output_dir    = os.path.dirname(zip_path_file)
	if not os.path.exists(output_dir):
		os.makedirs(output_dir)
	stitch_zip = zipfile.ZipFile(zip_path_file, 'w')

	zipped_list = []
	for out_file in out_list:
		src_file = os.path.join(fv_dir, out_file)
		for each_file in glob.glob(src_file):
			shutil.copy (each_file, output_dir)
			comp_file = ntpath.basename(each_file)
			stitch_zip.write (os.path.join(output_dir, comp_file), comp_file, compress_type = zipfile.ZIP_DEFLATED)
			zipped_list.append(comp_file)

	for idx, (out_file, file_list) in enumerate(img_list):
		if out_file in rgn_name_list:
			continue
		Ignore = True
		# Loop through the file list to see if all of them are ignored
		for src, algo, val, mode, pos in file_list:
			if mode & STITCH_OPS.MODE_FILE_IGNOR:
				continue
			# Found one file which is not ignored, so look for the file in build directory
			Ignore = False
			break
		# Out file is marked ignored, so ignore it.
		if Ignore == True:
			continue
		shutil.copy(os.path.join(fv_dir, out_file), output_dir)
		comp_file = ntpath.basename(out_file)
		if comp_file not in zipped_list:
			stitch_zip.write (os.path.join(output_dir, comp_file), comp_file, compress_type = zipfile.ZIP_DEFLATED)

	stitch_zip.close()

def rebase_stage (in_file, out_file, delta):

	if not os.path.exists(in_file):
		raise Exception("file '%s' not found !" % in_file)

	fd = FirmwareDevice(0, in_file)
	fd.ParseFd ()
	fd.ParseFsp ()

	# Data for the output file, this data will be modified below
	out_bins = fd.FdData

	# Base address for the stage1b FV is populated at offset 0 in Stage1b.fd
	old_entry = c_uint32.from_buffer(out_bins, 0)
	old_base  = c_uint32.from_buffer(out_bins, 4)

	# Calculate the delta between the old base and new base
	new_entry = old_entry.value + delta
	new_base  = old_base.value  + delta

	fsp_fv_idx_list = []
	for fsp in fd.FspList:
		fsp_fv_idx_list.extend(fsp.FvIdxList)

	for idx, fv in enumerate(fd.FvList):
		if idx in fsp_fv_idx_list:
			continue

		# Rebase stage1b redundant copy to the redundant stage1b base address
		rebase_fv (fv, out_bins, delta)

	# update the redundant stage1b fv base address at offset 0
	old_entry.value = new_entry
	old_base.value  = new_base

	# Open bios image and write rebased stage1b.fd to the redundant stage1b region
	open(out_file, 'wb').write(out_bins)


def rebase_fv (fv, out_bin, delta):
	if len(fv.FfsList) == 0:
		return

	# Loop through the ffslist to identify TE and PE images
	imglist = []
	for ffs in fv.FfsList:
		for sec in ffs.SecList:
			if sec.SecHdr.Type in [EFI_SECTION_TYPE.TE, EFI_SECTION_TYPE.PE32]:   # TE or PE32
				offset = fv.Offset + ffs.Offset + sec.Offset + sizeof(sec.SecHdr)
				imglist.append ((offset, len(sec.SecData) - sizeof(sec.SecHdr)))

	# Rebase all TE and PE images to new base address
	fcount  = 0
	pcount  = 0
	for (offset, length) in imglist:
		img = PeTeImage(offset, out_bin[offset:offset + length])
		img.ParseReloc()
		pcount += img.Rebase(delta, out_bin)
		fcount += 1

	print "Patched %d entries in %d TE/PE32 images." % (pcount, fcount)


def decode_flash_map (flash_map_file, print_address = True):

	if not os.path.exists(flash_map_file):
		raise Exception("No layout file '%s' found !" % flash_map_file)
		return

	fmap_bins = open (flash_map_file, 'rb')
	flash_map_data = bytearray(fmap_bins.read())
	fmap_bins.close()

	flash_map = FlashMap.from_buffer (flash_map_data)
	entry_num = (flash_map.length - sizeof(FlashMap)) // sizeof(FlashMapDesc)

	image_size = flash_map.romsize
	image_base = 0x100000000 - image_size

	flash_map_lines = [
			"\nFlash Map Information:\n" \
			"\t+------------------------------------------------------------------------+\n" \
			"\t|                              FLASH  MAP                                |\n" \
			"\t|                         (RomSize = 0x%08X)                         |\n"     \
			"\t+------------------------------------------------------------------------+\n" \
			"\t|   NAME   |     OFFSET  (BASE)     |    SIZE    |         FLAGS         |\n" \
			"\t+----------+------------------------+------------+-----------------------+\n"  % image_size]

	region   = '      '
	prev_rgn = 'TS'
	disp_rgn = ''

	for idx in xrange (entry_num):
		desc  = FlashMapDesc.from_buffer (flash_map_data, sizeof(FlashMap) + idx * sizeof(FlashMapDesc))
		flags = 'Compressed  '  if (desc.flags & FlashMap.FLASH_MAP_DESC_FLAGS['COMPRESSED']) else 'Uncompressed'
		for rgn_name, rgn_flag in FlashMap.FLASH_MAP_DESC_FLAGS.items():
			if rgn_flag == (desc.flags & 0x0F):
				if rgn_flag & (FlashMap.FLASH_MAP_DESC_FLAGS['NON_REDUNDANT'] | FlashMap.FLASH_MAP_DESC_FLAGS['NON_VOLATILE']):
					rgn_suf      = ''
					disp_rgn_suf = ''
				else:
					suffixes = 'B' if desc.flags & FlashMap.FLASH_MAP_DESC_FLAGS['BACKUP'] else 'A'
					rgn_suf      = '_' + suffixes
					disp_rgn_suf = ' ' + suffixes
				region   = ''.join([word[0] for word in rgn_name.split('_')]) + rgn_suf
				disp_rgn = rgn_name.replace('_', ' ') + disp_rgn_suf
				region   = region.center(4, ' ')
				disp_rgn = disp_rgn.center(23, ' ')
				break

		if region != '      ':
			if region != prev_rgn:
				prev_rgn = region
				flash_map_lines.append (
				  "\t+------------------------------------------------------------------------+\n" \
				  "\t|                        %s                         |\n"                      \
				  "\t+------------------------------------------------------------------------+\n" % disp_rgn)
			flags += ', '
		flags += region
		if print_address:
			address = '0x%08X' % (desc.offset + image_base)
		else:
			address = ' ???????? '
		flash_map_lines.append ("\t|   %s   |  0x%06x(%s)  |  0x%06x  |  %s   |\n" \
			% (desc.sig, desc.offset, address, desc.size, flags))

	flash_map_lines.append ("\t+----------+------------------------+------------+-----------------------+\n")

	return ''.join(flash_map_lines)


def find_component_in_image_list (comp_name, img_list):
	for (out_file, file_list) in img_list:
		for file in file_list:
			if comp_name == file[0]:
				return file
	return None


def print_component_list (comp_list):
	for comp in comp_list:
		print '%-20s BASE=0x%08X' % (comp['name'], comp['base'])

