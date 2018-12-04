#!/usr/bin/env python
## @ BuildLoader.py
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

tool_dir = os.path.join(os.path.dirname (os.path.realpath(__file__)), 'BootloaderCorePkg', 'Tools')
sys.dont_write_bytecode = True
sys.path.append (tool_dir)

import re
import imp
import errno
import shutil
import argparse
import subprocess
import multiprocessing
from   ctypes import *
from   BuildUtility import *

def rebuild_basetools ():

	ret = 0
	sblsource = os.environ['SBL_SOURCE']
	if os.name == 'posix':
		genffs_exe_path = os.path.join(sblsource, 'BaseTools', 'Source', 'C', 'bin', 'GenFfs')
		genffs_exist = os.path.exists(genffs_exe_path)
		if not genffs_exist:
			ret = subprocess.call(['make', '-C', 'BaseTools'])

	elif os.name == 'nt':
		if 'PYTHON_HOME' not in os.environ:
			os.environ['PYTHON_HOME'] = 'C:\\Python27'
		genffs_exe_path = os.path.join(sblsource, 'BaseTools', 'Bin', 'Win32', 'GenFfs.exe')
		genffs_exist = os.path.exists(genffs_exe_path)
		if not genffs_exist:
			print "Could not find pre-built BaseTools binaries, try to rebuild BaseTools ..."
			ret = subprocess.call(['BaseTools\\toolsetup.bat', 'forcerebuild'])

	if ret:
		print "Build BaseTools failed, please check required build environment and utilities !"
		sys.exit(1)

		genffs_exist = os.path.exists(genffs_exe_path)
		if not genffs_exist:
				print "Build python executables failed !"
				sys.exit(1)


def prep_env ():
	sblsource = os.path.dirname(os.path.realpath(__file__))
	os.chdir(sblsource)
	if os.name == 'posix':
		toolchain = 'GCC49'
		gcc_ver = subprocess.Popen(['gcc', '-dumpversion'], stdout=subprocess.PIPE)
		(gcc_ver, err) = subprocess.Popen(['sed', 's/\\..*//'], stdin=gcc_ver.stdout, stdout=subprocess.PIPE).communicate()
		if int(gcc_ver) > 4:
			toolchain = 'GCC5'

		os.environ['PATH'] = os.environ['PATH'] + ':' + os.path.join(sblsource, 'BaseTools/BinWrappers/PosixLike')
	elif os.name == 'nt':
		toolchain = ''
		os.environ['PATH'] = os.environ['PATH'] + ';' + os.path.join(sblsource, 'BaseTools\\Bin\\Win32')
		os.environ['PATH'] = os.environ['PATH'] + ';' + os.path.join(sblsource, 'BaseTools\\BinWrappers\\WindowsLike')
		os.environ['PYTHONPATH'] = os.path.join(sblsource, 'BaseTools', 'Source', 'Python')
		vs_ver_list = [
			('2015', 'VS140COMNTOOLS'),
			('2013', 'VS120COMNTOOLS'),
			('2012', 'VS110COMNTOOLS'),
			('2010', 'VS100COMNTOOLS'),
			('2008', 'VS90COMNTOOLS'),
			('2005', 'VS80COMNTOOLS')
		]
		for vs_ver, vs_tool in vs_ver_list:
			if vs_tool in os.environ:
				toolchain='VS%s%s' % (vs_ver, 'x86')
				toolchainprefix = 'VS%s_PREFIX' % (vs_ver)
				os.environ[toolchainprefix] = os.path.join(os.environ[vs_tool], '..//..//')
				break
		if not toolchain:
			print "Could not find supported Visual Studio version !"
			sys.exit(1)
		if 'NASM_PREFIX' not in os.environ:
			os.environ['NASM_PREFIX'] = "C:\\Nasm\\"
		if 'OPENSSL_PATH' not in os.environ:
			os.environ['OPENSSL_PATH'] = "C:\\Openssl\\"
		if 'IASL_PREFIX' not in os.environ:
			os.environ['IASL_PREFIX'] = "C:\\ASL\\"
	else:
		print "Unsupported operating system !"
		sys.exit(1)

	# Update Environment vars
	os.environ['SBL_SOURCE']     = sblsource
	os.environ['EDK_TOOLS_PATH'] = os.path.join(sblsource, 'BaseTools')
	os.environ['BASE_TOOLS_PATH'] = os.path.join(sblsource, 'BaseTools')
	if 'WORKSPACE' not in os.environ:
		os.environ['WORKSPACE'] = sblsource
	os.environ['CONF_PATH']     = os.path.join(os.environ['WORKSPACE'], 'Conf')
	os.environ['TOOL_CHAIN']    = toolchain

def get_board_config_file (check_dir, board_cfgs):
	platform_dir = os.path.join (check_dir, 'Platform')
	if not os.path.isdir (platform_dir):
		if os.path.basename(check_dir) == 'Platform':
			platform_dir = check_dir
		else:
			return

	board_pkgs = os.listdir(platform_dir)
	for pkg in board_pkgs:
		cfgfile = os.path.join(platform_dir, pkg, 'BoardConfig.py')
		if os.path.exists(cfgfile):
			board_cfgs.append(cfgfile)

class BaseBoard(object):
	def __init__(self, *args, **kwargs):

		# NOTE: Variables starting with '_' will not be exported to Platform.dsc

		# Define default private key (PEM format) used to sign configuration data, firmware capsule and IAS image
		key_dir = os.path.join('BootloaderCorePkg', 'Tools', 'Keys')
		self._CFG_PRIVATE_KEY       = os.path.join(key_dir, 'TestSigningPrivateKey.pem')
		self._FWU_PRIVATE_KEY       = os.path.join(key_dir, 'TestSigningPrivateKey.pem')
		self._IAS_PRIVATE_KEY       = os.path.join(key_dir, 'TestSigningPrivateKey.pem')

		self.VERINFO_IMAGE_ID       = 'SB_???? '
		self.VERINFO_PROJ_ID        = 1
		self.VERINFO_CORE_MAJOR_VER = 0
		self.VERINFO_CORE_MINOR_VER = 5

		self.VERINFO_PROJ_MAJOR_VER = 0
		self.VERINFO_PROJ_MINOR_VER = 1
		self.VERINFO_SVN            = 1
		self.VERINFO_BUILD_DATE     = '01/01/2018'

		self.FLASH_BLOCK_SIZE       = 0x1000
		self.FLASH_LAYOUT_START     = 0x100000000
		self.FLASH_BASE             = 0
		self.FLASH_SIZE             = 0

		self.PCI_EXPRESS_BASE       = 0xE0000000
		self.ACPI_PM_TIMER_BASE     = 0x0408
		self.USB_KB_POLLING_TIMEOUT = 1

		self.VERIFIED_BOOT_HASH_MASK  = 0x00000000
		self.BOOT_MEDIA_SUPPORT_MASK  = 0xFFFFFFFF
		self.FILE_SYSTEM_SUPPORT_MASK = 0x00000003
		self.DEBUG_OUTPUT_DEVICE_MASK = 0x00000003
		self.CONSOLE_IN_DEVICE_MASK   = 0x00000001
		self.CONSOLE_OUT_DEVICE_MASK  = 0x00000001

		self.HAVE_VBT_BIN          = 0
		self.HAVE_FIT_TABLE        = 0
		self.HAVE_VERIFIED_BOOT    = 0
		self.HAVE_MEASURED_BOOT    = 0
		self.HAVE_FSP_BIN          = 1
		self.HAVE_ACPI_TABLE       = 1
		self.HAVE_PSD_TABLE        = 0
		self.HAVE_FLASH_MAP        = 1
		self.HAVE_SEED_LIST        = 0

		self.FIT_ENTRY_MAX_NUM     = 10

		self.ENABLE_PCI_ENUM       = 1
		self.ENABLE_SMP_INIT       = 1
		self.ENABLE_FSP_LOAD_IMAGE = 0
		self.ENABLE_SPLASH         = 0
		self.ENABLE_FRAMEBUFFER_INIT = 0
		self.ENABLE_CRYPTO_SHA_NI  = 0
		self.ENABLE_FWU            = 0
		self.ENABLE_SOURCE_DEBUG   = 0
		self.ENABLE_SMM_REBASE     = 0

		self.ACM_SIZE              = 0
		self.UCODE_SIZE            = 0
		self.CFGDATA_SIZE          = 0
		self.MRCDATA_SIZE          = 0
		self.VARIABLE_SIZE         = 0
		self.FWUPDATE_SIZE         = 0

		self.SPI_IAS1_SIZE         = 0
		self.SPI_IAS2_SIZE         = 0

		self.KM_SIZE               = 0x1000 # valid only if ACM_SIZE > 0
		self.BPM_SIZE              = 0x1000 # valid only if ACM_SIZE > 0
		self.CFG_DATABASE_SIZE     = 0

		self.STAGE1A_XIP           = 1
		self.STAGE1B_XIP           = 1
		self.STAGE2_XIP            = 0
		self.STAGE2_LOAD_HIGH      = 1
		self.PAYLOAD_LOAD_HIGH     = 0
		self.PAYLOAD_EXE_BASE      = 0x00800000

		#     0: Direct access from flash
		# other: Load image into memory address
		self.PAYLOAD_LOAD_BASE     = 0
		self.FWUPDATE_LOAD_BASE    = 0

		self.PLD_HEAP_SIZE         = 0x02000000
		self.PLD_STACK_SIZE        = 0x00010000
		self.PLD_RSVD_MEM_SIZE     = 0x00004000

		# These memory sizes need to be page aligned
		self.LOADER_RSVD_MEM_SIZE         = 0x0038C000
		self.LOADER_ACPI_NVS_MEM_SIZE     = 0x00008000
		self.LOADER_ACPI_RECLAIM_MEM_SIZE = 0x00068000

		self.CFGDATA_REGION_TYPE   = FLASH_REGION_TYPE.BIOS

		self.RELEASE_MODE          = 0
		self.FSPDEBUG_MODE         = 0
		self.MIN_FSP_REVISION      = 0
		self.FSP_IMAGE_ID          = ''

		self.TOP_SWAP_SIZE         = 0
		self.REDUNDANT_SIZE        = 0

		self._PAYLOAD_NAME         = ''
		self._FSP_PATH_NAME        = ''

		self._PLATFORM_ID          = None
		self._CFGDATA_INT_FILE     = []
		self._CFGDATA_EXT_FILE     = []

		for key, value in kwargs.items():
			setattr(self, '%s' % key, value)


class Build(object):

	def __init__(self, board):
		self._toolchain                    = os.environ['TOOL_CHAIN']
		self._workspace                    = os.environ['WORKSPACE']
		self._board                        = board
		self._image                        = "SlimBootloader.bin"
		self._target                       = 'RELEASE' if board.RELEASE_MODE  else 'DEBUG'
		self._fsp_basename                 = 'FspDbg'  if board.FSPDEBUG_MODE else 'FspRel'
		self._fv_dir                       = os.path.join(self._workspace, 'Build', 'BootloaderCorePkg', '%s_%s' % (self._target, self._toolchain), 'FV')
		self._img_list                     = board.GetImageLayout()
		self._pld_list                     = get_payload_list (board._PAYLOAD_NAME.split(';'))
		self._comp_list                    = []
		self._region_list                  = []

	def update_fit_table (self):

		if not self._board.HAVE_FIT_TABLE:
			return

		print('Updating FIT')

		img_file = os.path.join (self._fv_dir, self._image)
		fi = open(img_file,'rb')
		rom = bytearray(fi.read())
		fi.close()

		# Find FIT pointer @ 0xFFFFFFC0
		fit_address = c_uint32.from_buffer(rom, len(rom)-0x40)
		print('  FIT Address: 0x%08X' % fit_address.value)

		if self._board.ACM_SIZE > 0:
			# Check FIT address alignment for 64 bytes if ACM is used
			# because BIOS IBB segments base/size require 64 bytes alignment.
			if fit_address.value & ~0x3F != fit_address.value:
				raise Exception ('  FIT address (0x%08X) is not 64-byte aligned' % fit_address.value)

		# Check FIT address range
		base = 0x100000000 - len(rom);
		if (fit_address.value < base) or (fit_address.value > (base + len(rom))):
			raise Exception('  FIT address (0x%08X) out of range' % fit_address.value)

		# Check FIT signature
		fit_offset = fit_address.value - base
		fit_header = FitEntry.from_buffer(rom, fit_offset)
		if fit_header.address != Bytes2Val (bytearray(FitEntry.FIT_SIGNATURE)):
			raise Exception('  FIT signature not found')

		num_fit_entries = 0
		if self._board.UCODE_SIZE > 0:
			ucode_base = self._board.UCODE_BASE
			ucode_offset = ucode_base - base;
			if (ucode_offset < 0):
				raise Exception ('  UCODE %x\n  UCODE address (0x%08X) out of range' % (base, ucode_base))

			# Collect all CPU uCode images
			u_code_images = []
			while ucode_offset < len(rom):
				ucode_hdr = UcodeHeader.from_buffer(rom, ucode_offset)
				if ucode_hdr.header_version == 1:
					if ucode_hdr.total_size:
						ucode_size = ucode_hdr.total_size
					else:
						ucode_size = 0x0800
					u_code_images.append((ucode_offset, ucode_size))
					ucode_offset += ucode_size
					num_fit_entries += 1
				else:
					break

			# Patch FIT with addresses of uCode images
			for i in range(0, num_fit_entries):
				fit_entry = FitEntry.from_buffer(rom, fit_offset + (i+1)*16)
				# uCode Update
				if len(u_code_images) > 0:
					offset, size = u_code_images.pop(0)
					fit_entry.set_values(base + offset, 0, 0x100, 0x1, 0)
					print('  Patching entry %d with 0x%08X - uCode' % (i, fit_entry.address))
				else:
					print('  Nullifying unused uCode patch entry %d' % i)
					fit_entry.type      = 0x7f

			if len(u_code_images) > 0:
				raise Exception('  Insufficient uCode entries in FIT. Need %d more.' % len(u_code_images))

		if self._board.ACM_SIZE > 0:
			# ACM
			fit_entry = FitEntry.from_buffer(rom, fit_offset + (num_fit_entries+1)*16)
			fit_entry.set_values(self._board.ACM_BASE, 0, 0x100, 0x2, 0)
			print('  Patching entry %d with 0x%08X:0x%08X - ACM' % (num_fit_entries, fit_entry.address, fit_entry.size))
			num_fit_entries     += 1

			# BIOS Module (IBB segment 0): from FIT table end to 4GB
			# Record it now and update later since the FIT size is unknown yet
			patch_entry = num_fit_entries
			num_fit_entries     += 1

			# BIOS Module (IBB segment 1): from Stage1A base to FIT table start
			addr = self._board.STAGE1A_BASE
			module_size =  (fit_address.value - addr) >> 4
			fit_entry = FitEntry.from_buffer(rom, fit_offset + (num_fit_entries+1)*16)
			fit_entry.set_values(addr, module_size, 0x100, 0x7, 0)
			print('  Patching entry %d with 0x%08X:0x%08X - BIOS Module(Stage1A base to FIT table start)' % (num_fit_entries, fit_entry.address, fit_entry.size))
			num_fit_entries     += 1

			# BIOS Module (IBB segment 2): full Stage1B
			addr = self._board.STAGE1B_BASE
			module_size = self._board.STAGE1B_SIZE >> 4
			fit_entry = FitEntry.from_buffer(rom, fit_offset + (num_fit_entries+1)*16)
			fit_entry.set_values(addr, module_size, 0x100, 0x7, 0)
			print('  Patching entry %d with 0x%08X:0x%08X - BIOS Module(Stage1B)' % (num_fit_entries, fit_entry.address, fit_entry.size))
			num_fit_entries     += 1

			# Keep this temporary to compare BTG with BTG+VerifiedBoot
			if not self._board.HAVE_VERIFIED_BOOT:
				# Stage2
				addr = self._board.STAGE2_BASE
				src = 'STAGE2.lz'
				src_path = os.path.join(self._fv_dir, src)
				if not os.path.exists(src_path):
					module_size = (self._board.STAGE2_SIZE >> 4)
				else:
					module_size = os.path.getsize(src_path)
					module_size = ((module_size + 0x3F) & 0xFFFFFFC0) # 0x40 align
					module_size = module_size >> 4
				fit_entry = FitEntry.from_buffer(rom, fit_offset + (num_fit_entries+1)*16)
				fit_entry.set_values(addr, module_size, 0x100, 0x7, 0)
				print('  Patching entry %d with 0x%08X:0x%08X - BIOS Module(Stage2)' % (num_fit_entries, fit_entry.address, fit_entry.size))
				num_fit_entries     += 1

				# Payload
				addr = self._board.PAYLOAD_BASE
				src = 'PAYLOAD.lz'
				src_path = os.path.join(self._fv_dir, src)
				if not os.path.exists(src_path):
					module_size = (self._board.PAYLOAD_SIZE >> 4)
				else:
					module_size = os.path.getsize(src_path)
					module_size = ((module_size + 0x3F) & 0xFFFFFFC0) # 0x40 align
					module_size = module_size >> 4
				fit_entry = FitEntry.from_buffer(rom, fit_offset + (num_fit_entries+1)*16)
				fit_entry.set_values(addr, module_size, 0x100, 0x7, 0)
				print('  Patching entry %d with 0x%08X:0x%08X - BIOS Module(Payload)' % (num_fit_entries, fit_entry.address, fit_entry.size))
				num_fit_entries     += 1

			# KM
			addr = self._board.ACM_BASE + self._board.ACM_SIZE - (self._board.KM_SIZE + self._board.BPM_SIZE)
			fit_entry = FitEntry.from_buffer(rom, fit_offset + (num_fit_entries+1)*16)
			fit_entry.set_values(addr, self._board.KM_SIZE, 0x100, 0xb, 0)
			print('  Patching entry %d with 0x%08X:0x%08X - KM' % (num_fit_entries, fit_entry.address, fit_entry.size))
			num_fit_entries     += 1

			# BPM
			addr = self._board.ACM_BASE + self._board.ACM_SIZE - self._board.BPM_SIZE
			fit_entry = FitEntry.from_buffer(rom, fit_offset + (num_fit_entries+1)*16)
			fit_entry.set_values(addr, self._board.BPM_SIZE, 0x100, 0xc, 0)
			print('  Patching entry %d with 0x%08X:0x%08X - BPM' % (num_fit_entries, fit_entry.address, fit_entry.size))
			num_fit_entries     += 1

			# Patch the entry 'FIT table end to 4GB' since FIT table size is known now
			# The size of the FIT table end address needs to be adjusted to align with 64
			# bytes so that IBB segment start address is 64 byte aligned as per required.
			addr = fit_address.value + (num_fit_entries + 1) * 16
			addr = (addr + 0x3F) & 0xFFFFFFC0
			module_size = (0x100000000 - addr) >> 4
			fit_entry = FitEntry.from_buffer(rom, fit_offset + (patch_entry+1)*16)
			fit_entry.set_values(addr, module_size, 0x100, 0x7, 0)
			print('  Patching entry %d with 0x%08X:0x%08X - BIOS Module(FIT table end to 4GB)' % (patch_entry, fit_entry.address, fit_entry.size))

		else :
			addr = fit_address.value + (num_fit_entries + 1) * 16

		# Check FIT length
		spaceleft = addr - (fit_address.value + fit_header.size)
		if spaceleft > 0:
				raise Exception('  Insufficient FIT entries in FIT table, need %d more entries !' %  ((spaceleft + 15) // 16))

		print('  FIT %d entries added' % num_fit_entries)

		# Update FIT checksum
		print('  Updating Checksum')
		fit_header.size      = num_fit_entries + 1
		fit_header.type      = 0x80 # Valid checksum
		fit_header.version   = 0x0100
		fit_header.checksum  = 0
		fit_sum = sum(rom[fit_offset:fit_offset+fit_header.size*16])
		fit_header.checksum = (0 - fit_sum) & 0xff
		fit_data = rom[fit_offset:fit_offset+fit_header.size*16]

		fo  = open(img_file,'r+b')

		fo.seek(fit_offset)
		fo.write(fit_data)

		if self._board.REDUNDANT_SIZE != 0:
			# Update FIT table in STAGE1A_B
			print('Updating FIT in STAGE1A_B')
			fit_offset -= self._board.TOP_SWAP_SIZE
			rom[fit_offset:fit_offset+fit_header.size*16] = fit_data

			# Update components base in Fit table.
			fit_data = rom[fit_offset:fit_offset+fit_header.size*16]
			for i in range(0, num_fit_entries):
				fit_entry = FitEntry.from_buffer(fit_data, (i+1)*16)
				if (0x100000000 - fit_entry.address) > self._board.TOP_SWAP_SIZE * 2:
					fit_entry.address  -= self._board.REDUNDANT_SIZE
					print('  Patching entry %d from 0x%08X with 0x%08X size:0x%08X ' %
						(i, fit_entry.address + self._board.REDUNDANT_SIZE, fit_entry.address, fit_entry.size))
			fit_header = FitEntry.from_buffer(fit_data)
			fit_header.checksum  = 0
			fit_sum = sum(fit_data)
			fit_header.checksum = (0 - fit_sum) & 0xff
			fo.seek(fit_offset)
			fo.write(fit_data)

		fo.close()


	def update_hash_table (self, img_file):

		if not self._board.HAVE_VERIFIED_BOOT:
			return

		print 'Updating HashStore %s' % os.path.basename (img_file)

		fi = open(img_file,'rb')
		stage1_bins = bytearray(fi.read())
		fi.close()

		hs_offset = stage1_bins.find (HashStore.HASH_STORE_SIGNATURE)
		if hs_offset < 0:
			raise Exceptoin ("HashStore not found in '%s'!" % os.path.basename(img_file))

		comp_name, part_name = get_redundant_info (img_file)
		if part_name:
			part_name = '_' + part_name

		hash_file_list = [('STAGE1B%s.hash' % part_name, 0), ('STAGE2.hash', 1), ('PAYLOAD.hash', 2), ('FWUPDATE.hash', 3), ('CFGKEY.hash', 4), ('FWUKEY.hash', 5), ('OSKEY.hash', 6),  ('PLDDYN', 7)]
		if len(hash_file_list) > HashStore.HASH_STORE_MAX_IDX_NUM:
			raise Exception ('Insufficant hash entries !')

		hash_store = HashStore.from_buffer(stage1_bins, hs_offset)
		hash_len   = HashStore.HASH_STORE_ENTRY_LEN
		for hash_file, hash_idx in hash_file_list:
			# If the hash verification is not required for certain stage, skip it
			if ((1 << hash_idx) & self._board.VERIFIED_BOOT_HASH_MASK) == 0:
				continue

			if hash_file == 'PLDDYN':
				hash_data = bytearray('\x00' * hash_len)
			else:
				src_path = os.path.join(self._fv_dir, hash_file)
				if not os.path.exists(src_path):
					raise Exception ("Hash data file '%s' not found !" % hash_file )
				fh = open(src_path,'rb')
				hash_data = bytearray(fh.read())
				fh.close()
				if hash_len != len (hash_data):
					raise Exception ("Hash data file '%s' length is incorrect !" % hash_file )

			# update hash data
			start = hash_len * hash_idx
			hash_store.Data[start:start+hash_len] = hash_data

			# update valid bit
			hash_store.Valid |= 1 << hash_idx

			print ' Update HashStore entry %d at offset 0x%08X:0x%X with file %s' % (hash_idx, start, hash_len, hash_file)

		fo = open(img_file,'r+b')
		fo.seek(hs_offset)
		fo.write(hash_store)
		fo.close()


	def update_component_list (self):

		def process_image_list (idx, offset):

			region_name, part_name = get_redundant_info (img_list[idx][0])
			redundant   = True if part_name == 'B' else False

			flags  = 0
			if redundant:
				flags |= FlashMap.FLASH_MAP_DESC_FLAGS['BACKUP']
			if region_name in ['TOP_SWAP', 'REDUNDANT', 'NON_REDUNDANT', 'NON_VOLATILE']:
				flags |= FlashMap.FLASH_MAP_DESC_FLAGS[region_name]

			oldidx         = len (comp_list)
			parent_size    = getattr(self._board, '%s_SIZE' % region_name, 0)
			remaining_size = parent_size
			for comp in img_list[idx][1]:
				if comp[3] & STITCH_OPS.MODE_FILE_IGNOR:
					continue
				compress = FlashMap.FLASH_MAP_DESC_FLAGS['COMPRESSED'] if comp[1] else 0
				if comp[0] in region_name_list:
					idx = region_name_list.index (comp[0])
					region_size = process_image_list (idx, offset)
					region_list.append ({'name':comp[0], 'offset':offset, 'size':region_size})
					offset += region_size
				else:
					comp_list.append ({'name':comp[0], 'size':comp[2], 'flag':flags | compress})
				remaining_size -= comp[2]

			if remaining_size > 0:
				comp_node = find_component_in_image_list (img_list[idx][0], img_list)
				pos  = STITCH_OPS.MODE_POS_HEAD if comp_node is None else comp_node[4]
				comp = {'name':'EMPTY.bin', 'size':remaining_size, 'flag':flags}
				if pos == STITCH_OPS.MODE_POS_HEAD:
					comp_list.insert (oldidx, comp)
				else:
					comp_list.append (comp)
			elif remaining_size < 0:
				if parent_size == 0:
					parent_size = -remaining_size
				else:
					raise Exception ('Insufficant space, please adjust %s_SIZE (0x%X more is requried) !' % (region_name, -remaining_size))

			return parent_size

		# Create compoent list and update base and offset
		img_list         = self._img_list
		region_name_list = [img[0] for img in img_list]
		comp_list        = []
		region_list      = []

		try:
			master_name = self._image
			master_idx  = region_name_list.index(master_name)
			process_image_list (master_idx, 0)
			image_size = sum (comp['size'] for comp in comp_list)
			image_base = self._board.FLASH_LAYOUT_START - image_size
			image_offs = 0
			for comp in comp_list:
				comp['bname']  = get_redundant_info (comp['name'])[0]
				comp['offset'] = image_offs
				comp['base']   = image_base + image_offs
				image_offs    += comp['size']

			for rgn in region_list:
				rgn['base'] = image_base + rgn['offset']

		except ValueError:
			print "Warning: No '%s' component in image list !" % master_name

		#print_component_list (comp_list)

		self._comp_list   = comp_list
		self._region_list = region_list


	def patch_stages (self):

		print('Patching STAGE1A')
		extra_cmd = [
			"STAGE1A:STAGE1A",
			"0xFFFFFFFC,            _BASE_STAGE1A_,                    @Patch BFV",
			"_OFFS_STAGE1A_,        Stage1A:__ModuleEntryPoint,        @Patch Stage1A Entry",
			"_OFFS_STAGE1A_+4,      Stage1A:BASE,                      @Patch Module Base",
			"<Stage1A:__gPcd_BinaryPatch_PcdVerInfoBase>,  {3473A022-C3C2-4964-B309-22B3DFB0B6CA:0x1C}, @Patch VerInfo",
			"<Stage1A:__gPcd_BinaryPatch_PcdFileDataBase>, {EFAC3859-B680-4232-A159-F886F2AE0B83:0x1C}, @Patch PcdBase"
		]
		if self._board.HAVE_FLASH_MAP:
			extra_cmd.append (
				"0xFFFFFFF8, {3CEA8EF3-95FC-476F-ABA5-7EC5DFA1D77B:0x1C}, @Patch FlashMap",
			)
		if self._board.HAVE_FIT_TABLE:
			if self._board.ACM_SIZE > 0:
				extra_cmd.append (
					"0xFFFFFFC0,        ({CD17FF5E-7731-4D16-8441-FC7A113C392F:0x1C} + 0x3F) & ~0x3F,  @FIT table"
				)
			else:
				extra_cmd.append (
					"0xFFFFFFC0,        {CD17FF5E-7731-4D16-8441-FC7A113C392F:0x1C},                   @FIT table"
				)
			extra_cmd.extend ([
					"<[0xFFFFFFC0]>+0,  0x5449465F,                                                    @FIT Signature Low" ,
					"<[0xFFFFFFC0]>+4,  0x2020205F,                                                    @FIT Signature High",
					"<[0xFFFFFFC0]>+8,  [CD17FF5E-7731-4D16-8441-FC7A113C392F:0x18] & 0xFFFFFF ,       @FIT FFS section length",
					"<[0xFFFFFFC0]>+8,  {CD17FF5E-7731-4D16-8441-FC7A113C392F:0x1C} + [[0xFFFFFFC0] + 8] - [0xFFFFFFC0],  @FIT table max length",
				])
		if self._board.HAVE_VERIFIED_BOOT:
			extra_cmd.append (
				"<Stage1A:__gPcd_BinaryPatch_PcdHashStoreBase>, {18EDB1DF-1DBE-4EC5-8E26-C44808B546E1:0x1C}, @Patch HashStore",
			)
		patch_fv(self._fv_dir, *extra_cmd)

		print('Patching STAGE1B')
		patch_fv(
				self._fv_dir,
				"STAGE1B:STAGE1B",
				"_OFFS_STAGE1B_,        Stage1B:__ModuleEntryPoint,        @Patch Stage1B Entry",
				"_OFFS_STAGE1B_+4,      Stage1B:BASE,                      @Patch Stage1B Base",
				)

		print('Patching STAGE2')
		extra_cmd = []
		if self._board.HAVE_VBT_BIN:
			extra_cmd.append (
				"<Stage2:__gPcd_BinaryPatch_PcdGraphicsVbtAddress>, {E08CA6D5-8D02-43AE-ABB1-952CC787C933:0x1C}, @Patch VBT"
			)
		if self._board.HAVE_ACPI_TABLE:
			extra_cmd.append (
				"<Stage2:__gPcd_BinaryPatch_PcdAcpiTablesAddress>, {7E374E25-8E01-4FEE-87F2-390C23C606CD:0x1C}, @Patch ACPI",
			)
		if self._board.ENABLE_SPLASH:
			extra_cmd.append (
				"<Stage2:__gPcd_BinaryPatch_PcdSplashLogoAddress>, {5E2D3BE9-AD72-4D1D-AAD5-6B08AF921590:0x1C}, @Patch Logo",
			)
		patch_fv(
			self._fv_dir,
			"STAGE2:STAGE2",
			"_OFFS_STAGE2_,     Stage2:__ModuleEntryPoint, @Patch Stage2 Entry",
			"_OFFS_STAGE2_+4,   Stage2:BASE,               @Patch Stage2 Base",
			*extra_cmd
			)


	def create_dsc_inc_file (self, file):

		with open(file, 'w') as fh:
			fh.write('%s\n' % AUTO_GEN_DSC_HDR)
			fh.write('# Platform specific macro definitions\n')
			fh.write('[Defines]\n')

			for attr in sorted(vars(self._board)):
				if attr.startswith('_'):
					continue
				value = getattr(self._board, attr)
				if type(value) is not str:
					if value == 0 or value == 1:
						value = '0x%x' % value
					else:
						value = '0x%08X' % value
				fh.write('  DEFINE %-24s = %s\n'   % (attr, value))

			if getattr(self._board, "GetDscLibrarys", None):
				libsdict = self._board.GetDscLibrarys()
				for arch in libsdict:
					fh.write('\n# Platform specific libraries\n')
					fh.write('[LibraryClasses.%s]\n' % arch)
					for lib in libsdict[arch]:
						fh.write('  %s\n' % lib)
					fh.write('\n')


	def create_platform_vars (self):
		for comp in self._comp_list:
			if comp['flag'] & FlashMap.FLASH_MAP_DESC_FLAGS['BACKUP'] or comp['bname'] == 'EMPTY':
				continue
			setattr(self._board, '%s_BASE' % comp['bname'], comp['base'])

		image_base = self._board.FLASH_LAYOUT_START
		for idx, comp_name in enumerate(['STAGE1A', 'STAGE1B', 'STAGE2']):
			if not hasattr(self._board, '%s_BASE' % comp_name):
				image_base -= getattr(self._board, '%s_SIZE' % comp_name)
				if idx > 0:
					image_base &= ~0xFFFFF
				setattr(self._board, '%s_BASE' % comp_name, image_base)

			if getattr(self._board, '%s_XIP' % comp_name) or comp_name == 'STAGE1A':
				setattr(self._board, '%s_FD_SIZE' % comp_name, getattr(self._board, '%s_SIZE' % comp_name))
				setattr(self._board, '%s_FD_BASE' % comp_name, getattr(self._board, '%s_BASE' % comp_name))

			if getattr(self._board, '%s_XIP' % comp_name):
				setattr(self._board, '%s_LOAD_BASE' % comp_name, getattr(self._board, '%s_BASE' % comp_name))
			else:
				var_name = '%s_LOAD_BASE' % comp_name
				if not hasattr(self._board, var_name):
					setattr(self._board, var_name, getattr(self._board, '%s_BASE' % comp_name))
				for var in ['%s_FD_SIZE', '%s_FD_BASE', '%s_LOAD_BASE']:
					var_name = var % comp_name
					if not hasattr(self._board, var_name):
						raise Exception ('%s needs to be defined' % var_name)

			fd_size = getattr(self._board, '%s_FD_SIZE' % comp_name)
			setattr(self._board, '%s_FD_NUMBLK' % comp_name, fd_size // self._board.FLASH_BLOCK_SIZE)

		pld_list = ['PAYLOAD']
		if self._board.ENABLE_FWU:
			pld_list.append ('FWUPDATE')
		for pld in pld_list:
			if not hasattr(self._board, '%s_LOAD_BASE' % pld):
				if not hasattr(self._board, '%s_BASE' % pld):
					raise Exception ('%s_BASE or %s_LOAD_BASE needs to be defined !' % (pld, pld))
				setattr(self._board, '%s_LOAD_BASE' % pld, getattr(self._board, '%s_BASE' % pld))

		setattr(self._board, 'FSP_T_OFFSET'       , 0)
		setattr(self._board, 'STAGE1A_FV_OFFSET'  , getattr(self._board, 'FSP_T_OFFSET')      + getattr(self._board, 'FSP_T_SIZE'))
		setattr(self._board, 'STAGE1A_FV_SIZE'    , getattr(self._board, 'STAGE1A_FD_SIZE')   - getattr(self._board, 'FSP_T_SIZE'))
		setattr(self._board, 'STAGE1B_FV_OFFSET'  , 0)
		setattr(self._board, 'STAGE1B_FV_SIZE'    , getattr(self._board, 'STAGE1B_FD_SIZE')   - getattr(self._board, 'FSP_M_SIZE'))
		setattr(self._board, 'STAGE2_FV_OFFSET'   , 0)
		setattr(self._board, 'STAGE2_FV_SIZE'     , getattr(self._board, 'STAGE2_FD_SIZE')    - getattr(self._board, 'FSP_S_SIZE'))
		setattr(self._board, 'FSP_S_OFFSET'       , getattr(self._board, 'STAGE2_FV_OFFSET')  + getattr(self._board, 'STAGE2_FV_SIZE'))
		setattr(self._board, 'FSP_M_OFFSET'       , getattr(self._board, 'STAGE1B_FV_OFFSET') + getattr(self._board, 'STAGE1B_FV_SIZE'))
		setattr(self._board, 'FSP_T_BASE'         , getattr(self._board, 'STAGE1A_FD_BASE')   + getattr(self._board, 'FSP_T_OFFSET'))
		setattr(self._board, 'FSP_M_BASE'         , getattr(self._board, 'STAGE1B_FD_BASE')   + getattr(self._board, 'FSP_M_OFFSET'))
		setattr(self._board, 'FSP_S_BASE'         , getattr(self._board, 'STAGE2_FD_BASE')    + getattr(self._board, 'FSP_S_OFFSET'))

		if getattr(self._board, 'FLASH_SIZE') == 0:
			if not hasattr (self._board, 'SLIMBOOTLOADER_SIZE'):
				raise Exception ('FLASH_SIZE needs to be defined !')
			else:
				setattr(self._board, 'FLASH_SIZE' , getattr(self._board, 'SLIMBOOTLOADER_SIZE'))
		setattr(self._board, 'FLASH_BASE' , 0x100000000 - getattr(self._board, 'FLASH_SIZE'))

		if getattr(self._board, 'ACM_SIZE') > 0:
			acm_base = getattr(self._board, 'ACM_BASE')
			if acm_base & 0x7FFF:
				raise Exception ('ACM base[FSP-T+CAR:0x%x] must be 32KB aligned!' % acm_base)


	def create_redundant_components (self):
		if self._board.REDUNDANT_SIZE == 0:
			return

		print "Generating redundant components"

		shutil.copy(
			os.path.join(self._fv_dir, 'STAGE1A.fd'),
			os.path.join(self._fv_dir, 'STAGE1A_A.fd'))

		shutil.copy(
			os.path.join(self._fv_dir, 'STAGE1A.fd'),
			os.path.join(self._fv_dir, 'STAGE1A_B.fd'))

		# put a marker (@ 0xFFFFFFF4) for STAGE1A_B so that
		# boot partition can be detected easily
		fo = open(os.path.join(self._fv_dir, 'STAGE1A_B.fd'), 'r+b')
		fo.seek(-12, 2)
		nop = fo.read(1)
		if nop != '\x90':
			raise Exception ('Unexpected instructions at reset vector!')
		fo.seek(-12, 2)
		fo.write('\x80')
		fo.close()

		# Stage 1B_B will be created during rebasing
		shutil.copy(
			os.path.join(self._fv_dir, 'STAGE1B.fd'),
			os.path.join(self._fv_dir, 'STAGE1B_A.fd'))

		stage1b_path   = os.path.join(self._fv_dir, 'STAGE1B.fd')
		stage1b_b_path = os.path.join(self._fv_dir, 'STAGE1B_B.fd')

		if self._board.STAGE1B_XIP:
			# Rebase stage1b.fd
			print "Rebasing STAGE1B_B"
			rebase_stage (stage1b_path, stage1b_b_path, -self._board.REDUNDANT_SIZE)

			# rebase FSPM in Stage1B and update stage1B hash in key store
			if self._board.HAVE_FSP_BIN:
				fsp_path = os.path.join(self._fv_dir, 'Fsp.bin')
				rebase_fsp (fsp_path, self._fv_dir, self._board.FSP_T_BASE, self._board.FSP_M_BASE - self._board.REDUNDANT_SIZE, self._board.FSP_S_BASE)
				split_fsp (fsp_path, self._fv_dir)

				# write rebased fspm to second firmware
				di = open(os.path.join(self._fv_dir, 'FSP_M.bin'), 'rb').read()
				fo = open(stage1b_b_path,'r+b')
				fo.seek(self._board.FSP_M_OFFSET)
				fo.write(di)
				fo.close()
		else:
			shutil.copy(stage1b_path, stage1b_b_path)


	def create_bootloader_image (self, layout_name):

		layout_file = open(os.path.join(self._fv_dir, layout_name), 'w')
		layout_file.write("BOARD_INFO = ['%s']\n" % self._board.BOARD_NAME)

		rgn_name_list = [rgn['name'] for rgn in self._region_list]

		for idx, (comp_name, file_list)  in enumerate(self._img_list):
			if (self._board.ENABLE_FWU == 0) and (comp_name == 'Stitch_FWU.bin') :
				print "No firmware update payload specified, skip firmware update."
				continue
			out_file = comp_name
			out_path = os.path.join(self._fv_dir, out_file)
			bins = bytearray()
			new_list = []
			for src, algo, val, mode, pos in file_list:
				if mode & STITCH_OPS.MODE_FILE_IGNOR:
					continue
				new_list.append((src, algo, val, mode, pos))
				src_path = os.path.join(self._fv_dir, src)
				bas_path = os.path.splitext(src_path)[0]
				if not os.path.exists(src_path):
					raise Exception ("Component '%s' could not be found !" % src)

				if algo:
					compress(src_path, algo)
					src_path = bas_path + '.lz'
				else:
					if src == 'STAGE2.fd':
						raise Exception ("STAGE2.fd must be compressed, please change BoardConfig.py file !")
				if src not in rgn_name_list:
					gen_hash_file (src_path)

				if mode != STITCH_OPS.MODE_FILE_NOP:
					dst_path = bas_path + '.pad'
					align_pad_file(src_path, dst_path, val, mode, pos)
					src_path = dst_path
				else:
					if val and (val != os.path.getsize(src_path)):
						raise Exception ("Size of file '%s' does not match expected 0x%X !" % (src_path, val))
				if 'STAGE1A' in src :
					self.update_hash_table (src_path)

				fi = open(src_path, 'rb')
				bins.extend(bytearray(fi.read()))
				fi.close()

			if comp_name == self._image:
				bins = '\xff' * (self._board.SLIMBOOTLOADER_SIZE - len(bins)) + bins

			fo = open(out_path,'wb')
			fo.write(bins)
			fo.close()

			comp_file = out_file
			comp_node = find_component_in_image_list (comp_name, self._img_list)
			if comp_node:
				space = comp_node[2] - len(bins)
				if space > 0:
					empty = ('EMPTY', '', space, STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_HEAD)
					if comp_node[4] == STITCH_OPS.MODE_POS_HEAD:
						new_list.insert (0, empty)
					else:
						new_list.append (empty)
					comp_file = os.path.splitext(comp_name)[0] + '.pad'

			image_base = self._board.FLASH_LAYOUT_START
			comp_name  = comp_name.replace ('TOP_SWAP_B.', 'TOP_SWAP_A.')

			if comp_name in rgn_name_list:
				idx = rgn_name_list.index(comp_name)
				image_base = self._region_list[idx]['base'] + self._region_list[idx]['size']

			layout_file.write("IMAGE_INFO = ['%s', 0x%X, %d]\n" % (comp_file, image_base, True))
			layout_file.write("IMAGE_LIST = %s\n" % new_list)

		layout_file.close()

		self.update_fit_table ()

		# generate flash layout file
		layout_file = os.path.splitext(self._image)[0] + '.txt'
		report_image_layout (self._fv_dir, layout_name, layout_file)

		# copy files to staging directory for stitching
		if getattr(self._board, "GetOutputImages", None):
			extra_list = self._board.GetOutputImages()
		else:
			extra_list = []
		out_file = os.path.join("Outputs", self._board.BOARD_NAME, 'Stitch_Components.zip')
		copy_images_to_output (self._fv_dir, out_file, self._img_list, rgn_name_list, extra_list)


	def pre_build(self):
		# Check if BaseTools has been compiled
                rebuild_basetools ()

		# Update search path
		sbl_dir = os.environ['SBL_SOURCE']
		plt_dir = os.environ['PLT_SOURCE']
		os.environ['PACKAGES_PATH'] = plt_dir
		if plt_dir != sbl_dir:
			os.environ['PACKAGES_PATH'] += os.pathsep + sbl_dir

		# create conf and build folder if not exist
		if not os.path.exists(os.path.join(self._workspace, 'Conf')):
			os.makedirs(os.path.join(self._workspace, 'Conf'))
		for name in ['target', 'tools_def', 'build_rule']:
			txt_file = os.path.join(self._workspace, 'Conf/%s.txt' % name)
			if not os.path.exists(txt_file):
				shutil.copy (
					os.path.join(sbl_dir, 'BaseTools/Conf/%s.template' % name),
					os.path.join(self._workspace, 'Conf/%s.txt' % name))

		if not os.path.exists(self._fv_dir):
			os.makedirs(self._fv_dir)

		# check if FSP binary exists
		fsp_dir  = os.path.join(plt_dir, 'Silicon', self._board.SILICON_PKG_NAME, "FspBin", self._board._FSP_PATH_NAME)
		fsp_path = os.path.join(fsp_dir, self._fsp_basename + '.bin')

		check_fsp = os.path.join(tool_dir, 'PrepareFspBin.py')
		if os.path.exists(check_fsp):
			ret = subprocess.call(['python', check_fsp, plt_dir, self._board.BOARD_NAME, '/d' if self._board.FSPDEBUG_MODE else '/r'])
			if ret:
				raise Exception  ('Failed to checkout FSP binaries !')

		# create FSP size and UPD size can be known
		fsp_list = ['FSP_T', 'FSP_M', 'FSP_S']
		if self._board.HAVE_FSP_BIN:
			split_fsp (fsp_path, self._fv_dir)
		else:
			# create dummy FSP files
			for each in fsp_list:
				open(os.path.join(self._fv_dir, each + '.bin'),'wb').close()
		# generate size variables
		for each in fsp_list:
			fsp_bin  = os.path.join(self._fv_dir, "%s.bin" % each)
			if self._board.FSP_IMAGE_ID:
					imageid = get_fsp_image_id (fsp_bin)
					if self._board.FSP_IMAGE_ID != imageid:
							raise Exception ('Expecting FSP ImageId: %s, but got %s !' % (self._board.FSP_IMAGE_ID, imageid))
			revision = get_fsp_revision (fsp_bin)
			if revision < self._board.MIN_FSP_REVISION:
					raise Exception ('Required minimum FSP revision is 0x%08X, but current revision is 0x%08X !' %
								(self._board.MIN_FSP_REVISION, revision))
			setattr(self._board, '%s_SIZE' % each, get_fsp_size(fsp_bin) if self._board.HAVE_FSP_BIN else 0)
			setattr(self._board, '%s_UPD_SIZE' % each, get_fsp_upd_size(fsp_bin) if self._board.HAVE_FSP_BIN else 0)


		# create component base/size variables
		self.update_component_list ()
		self.create_platform_vars ()

		# generate a padding file
		gen_file_with_size (os.path.join(self._fv_dir, 'PADDING.bin'),  0)

		# create flashmap file
		if self._board.HAVE_FLASH_MAP:
			comp_list = self._comp_list
			if len(self._comp_list) == 0 and getattr(self._board, "GetFlashMapList", None):
				comp_list = self._board.GetFlashMapList()
			gen_flash_map_bin (os.path.join(self._fv_dir, 'FlashMap.bin'), comp_list)


		if not self._board.HAVE_VERIFIED_BOOT and self._board.HAVE_MEASURED_BOOT:
			raise Exception ('Verified Boot must also enabled to enable Measured Boot!')

		# create hashstore file
		if self._board.HAVE_VERIFIED_BOOT:
			gen_file_from_object (os.path.join(self._fv_dir, 'HashStore.bin'), HashStore())
		else:
			self._board.VERIFIED_BOOT_HASH_MASK = 0


		# create fit table
		if self._board.HAVE_FIT_TABLE:
			FitSize = sizeof(FitEntry) * (self._board.FIT_ENTRY_MAX_NUM - 1)
			if self._board.ACM_SIZE > 0:
				# Make sure FIT table start address and end address are 64 bytes aligned.
				FitSize = ((FitSize + 0x3F) & ~0x3F) + 0x3F
			gen_file_with_size (os.path.join(self._fv_dir, 'FitTable.bin'),  FitSize)


		# create bootloader version info file
		ver_info_name = 'VerInfo'
		ver_bin_file = os.path.join(self._fv_dir, ver_info_name + '.bin')
		ver_txt_file = os.path.join(os.environ['PLT_SOURCE'], 'Platform', self._board.BOARD_PKG_NAME, ver_info_name + '.txt')
		if self._board.USE_VERSION:
			ver_info = get_verinfo_via_file (ver_txt_file)
		else:
			keys = ['VERINFO_IMAGE_ID', 'VERINFO_BUILD_DATE', 'VERINFO_PROJ_MINOR_VER',
							'VERINFO_PROJ_MAJOR_VER', 'VERINFO_CORE_MINOR_VER', 'VERINFO_CORE_MAJOR_VER',
							'VERINFO_SVN', 'FSPDEBUG_MODE', 'RELEASE_MODE']
			ver_dict = {}
			for key in keys:
				ver_dict[key] = getattr (self._board, key)
			ver_info = get_verinfo_via_git  (ver_dict)
			gen_ver_info_txt (ver_txt_file, ver_info)
		gen_file_from_object (ver_bin_file, ver_info)


		# create platform include dsc file
		platform_dsc_path = os.path.join(sbl_dir, 'BootloaderCorePkg', 'Platform.dsc')
		self.create_dsc_inc_file (platform_dsc_path)


		# rebase FSP accordingly
		if self._board.HAVE_FSP_BIN:
			rebase_fsp(fsp_path, self._fv_dir, self._board.FSP_T_BASE, self._board.FSP_M_BASE, self._board.FSP_S_BASE)
			split_fsp(os.path.join(self._fv_dir, 'Fsp.bin'), self._fv_dir)


		# create configuration data
		if self._board.CFGDATA_SIZE > 0:
			# create config key hash
			if self._board.HAVE_VERIFIED_BOOT:
				cfg_priv_key     = self._board._CFG_PRIVATE_KEY
				cfg_pub_key_file = os.path.join(self._fv_dir, "CFGKEY.bin")
				gen_pub_key (cfg_priv_key, cfg_pub_key_file)
				gen_hash_file (cfg_pub_key_file, '', True)
			else:
				cfg_priv_key = None
			# create config data files
			gen_config_file (self._fv_dir, self._board.BOARD_PKG_NAME, self._board._PLATFORM_ID,
											 cfg_priv_key, self._board.CFG_DATABASE_SIZE, self._board.CFGDATA_SIZE,
											 self._board._CFGDATA_INT_FILE, self._board._CFGDATA_EXT_FILE)

		# rebuild reset vector
		vtf_dir = os.path.join('BootloaderCorePkg', 'Stage1A', 'Ia32', 'Vtf0')
		x = subprocess.call(['python', 'Build.py'],  cwd=vtf_dir)
		if x: raise Exception ('Failed to build reset vector !')


	def build(self):
		print "Build [%s] ..." % self._board.BOARD_NAME

		# Run pre-build
		self.pre_build()

		# Run build
		x = subprocess.call([
			"build" if os.name == 'posix' else "build.bat",
			"--platform", os.path.join('BootloaderCorePkg', 'BootloaderCorePkg.dsc'),
			"-b",         self._target,
			"--arch",     'IA32',
			"--tagname",  self._toolchain,
			"-n",         str(multiprocessing.cpu_count()),
			"-y",         "Report.log",
			"-Y",         "PCD",
			"-Y",         "FLASH",
			"-Y",         "LIBRARY"])
		if x: sys.exit(1)


		# Run post-build
		self.post_build()

		print "Done [%s] !" % self._board.BOARD_NAME


	def post_build(self):

		# create bootloader reserved binary of 4K size
		gen_file_with_size (os.path.join(self._fv_dir, 'SBLRSVD.bin'), 0x1000)


		# create ACM binary
		if self._board.ACM_SIZE > 0:
			gen_file_with_size (os.path.join(self._fv_dir, 'ACM.bin'), self._board.ACM_SIZE)


		# create MRC data
		if self._board.MRCDATA_SIZE:
			gen_file_with_size (os.path.join(self._fv_dir, 'MRCDATA.bin'), self._board.MRCDATA_SIZE)


		# create variable binary
		if self._board.VARIABLE_SIZE:
			varhdr = VariableRegionHeader.from_buffer(bytearray('\xFF' * sizeof(VariableRegionHeader)))
			varhdr.Signature = 'VARS'
			varhdr.Size      = self._board.VARIABLE_SIZE >> 1
			varhdr.State     = 0xFE
			varfile = open (os.path.join(self._fv_dir, "VARIABLE.bin"), "wb")
			varfile.write(varhdr)
			varfile.write('\xFF' * (self._board.VARIABLE_SIZE - sizeof(varhdr)));
			varfile.close()


		# create microcode binary
		if self._board.UCODE_SIZE > 0:
			shutil.copy (
				os.path.join(self._fv_dir, '../IA32/Microcode.bin'),
				os.path.join(self._fv_dir, "UCODE.bin"))


		# create OS public key hash
		if self._board.HAVE_VERIFIED_BOOT:
			ias_pub_key_file = os.path.join(self._fv_dir, "OSKEY.bin")
			gen_pub_key (self._board._IAS_PRIVATE_KEY, ias_pub_key_file)
			gen_hash_file (ias_pub_key_file, '', True)


		# generate payload
		gen_payload_bin (self._fv_dir, self._pld_list,
		                 os.path.join(self._fv_dir, "PAYLOAD.bin"),
		                 self._board._CFG_PRIVATE_KEY)


		# create firmware update key
		if self._board.ENABLE_FWU:
			srcfile = "../IA32/PayloadPkg/FirmwareUpdate/FirmwareUpdate/OUTPUT/FirmwareUpdate.efi"
			shutil.copyfile(
				os.path.join(self._fv_dir, srcfile),
				os.path.join(self._fv_dir, "FWUPDATE.bin"))
			fwup_key_file = os.path.join(self._fv_dir, "FWUKEY.bin")
			gen_pub_key (self._board._FWU_PRIVATE_KEY, fwup_key_file)
			gen_hash_file (fwup_key_file, '', True)


		# create SPI IAS image if required
		if self._board.SPI_IAS1_SIZE > 0 or self._board.SPI_IAS2_SIZE > 0:
			for idx in xrange (1, 3):
				file_path  = os.path.join('Platform', self._board.BOARD_PKG_NAME, 'SpiIasBin', 'iasimage%d.bin' % idx)
				file_space = getattr(self._board, 'SPI_IAS%d_SIZE' % idx)
				gen_ias_file (file_path, file_space, os.path.join(self._fv_dir, "SPI_IAS%d.bin" % idx))

		# patch stages
		self.patch_stages ()

		# create redundant components
		self.create_redundant_components ()


		# stitch all components
		layout_name = 'ImgStitch.txt'
		self.create_bootloader_image (layout_name)

		# print flash map
		if self._board.HAVE_FLASH_MAP and len(self._comp_list) > 0:
			print_addr = False if getattr(self._board, "GetFlashMapList", None) else True
			flash_map_text = decode_flash_map (os.path.join(self._fv_dir, 'FlashMap.bin'), print_addr)
			print '%s' % flash_map_text
			fd = open (os.path.join(self._fv_dir, 'FlashMap.txt'), 'w')
			fd.write (flash_map_text)
			fd.close ()


def main():
	prep_env ()
	board_cfgs   = []
	board_names  = []

	# Find all boards
	search_dir = os.environ['SBL_SOURCE']
	if 'PLT_SOURCE' in os.environ:
		search_dir = os.path.abspath(os.path.join(search_dir, os.path.pardir))
	board_pkgs = os.listdir (search_dir)
	for pkg in board_pkgs:
		get_board_config_file (os.path.join (search_dir, pkg), board_cfgs)

	for cfgfile in board_cfgs:
		brdcfg = imp.load_source('BoardConfig', cfgfile)
		board_names.append(brdcfg.Board().BOARD_NAME)


	ap = argparse.ArgumentParser()
	sp = ap.add_subparsers(help='command')

	def cmd_build(args):
		for index, name in enumerate(board_names):
			if args.board == name:
				brdcfg = imp.load_source('BoardConfig', board_cfgs[index])
				board  = brdcfg.Board(
										RELEASE_MODE      = args.release,     \
										FSPDEBUG_MODE     = args.fspdebug,    \
										USE_VERSION       = args.usever,      \
										_PAYLOAD_NAME     = args.payload,     \
										_FSP_PATH_NAME    = args.fsppath
										);
				os.environ['PLT_SOURCE']  = os.path.abspath (os.path.join (os.path.dirname (board_cfgs[index]), '../..'))
				Build(board).build()
				break

	buildp = sp.add_parser('build', help='build firmware')
	buildp.add_argument('-r',  '--release', action='store_true', help='Release build')
	buildp.add_argument('-v',  '--usever',  action='store_true', help='Use board version file')
	buildp.add_argument('-fp', dest='fsppath', type=str, help='FSP binary path relative to FspBin in Silicon folder', default='')
	buildp.add_argument('-fd', '--fspdebug', action='store_true', help='Use debug FSP binary')
	buildp.add_argument('-p',  '--payload' , dest='payload', type=str, help='Payload file name', default ='OsLoader.efi')
	buildp.add_argument('board', metavar='board', choices=board_names, help='Board Name (%s)' % ', '.join(board_names))
	buildp.set_defaults(func=cmd_build)

	def cmd_clean(args):
		workspace = os.environ['WORKSPACE']
		sbl_dir   = os.environ['SBL_SOURCE']
		dirs  = ['Build', 'Conf']
		files = [
			os.path.join (sbl_dir, 'BootloaderCorePkg/Stage1A/Ia32/Vtf0/Bin/ResetVector.ia32.raw'),
			os.path.join (sbl_dir, 'BootloaderCorePkg/Platform.dsc'),
			os.path.join (workspace, 'Report.log')
		]
		cmds  = []

		if args.distclean:
			dirs.extend ([
				'Outputs',
			])

			files.extend ([
			])

			cmds.extend ([
				'git clean -xdf BaseTools',
			])

		for dir in dirs:
			dirpath = os.path.join (workspace, dir)
			print 'Removing %s' % dirpath
			shutil.rmtree(dirpath, ignore_errors=True)

		for file in files:
			if os.path.exists(file):
				print 'Removing %s' % file
				os.remove(file)

		for cmd in cmds:
			x = subprocess.call(cmd.split(' '), cwd=sbl_dir)
			if x: raise Exception ('Failed to run clean-up commands !')

		print('Clean Done !')

	cleanp = sp.add_parser('clean', help='clean build dir')
	cleanp.add_argument('-d',  '--distclean', action='store_true', help='Distribution clean')
	cleanp.set_defaults(func=cmd_clean)

	args = ap.parse_args()
	args.func(args)

if __name__ == '__main__':
	main()

