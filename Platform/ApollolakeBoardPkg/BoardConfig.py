## @file
# This file is used to provide board specific image information.
#
#  Copyright (c) 2017 - 2018, Intel Corporation. All rights reserved.<BR>
#
#  This program and the accompanying materials
#  are licensed and made available under the terms and conditions of the BSD License
#  which accompanies this distribution.  The full text of the license may be found at
#  http://opensource.org/licenses/bsd-license.php
#
#  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

##
# Import Modules
#
import os
import sys

sys.dont_write_bytecode = True
sys.path.append (os.path.join('..', '..'))
from BuildLoader import FlashMap, BaseBoard, STITCH_OPS

class Board(BaseBoard):
	def __init__(self, *args, **kwargs):

		super(Board, self).__init__(*args, **kwargs)

		self.VERINFO_IMAGE_ID          = 'SB_APLI '
		self.VERINFO_PROJ_MAJOR_VER    = 1
		self.VERINFO_PROJ_MINOR_VER    = 0
		self.VERINFO_SVN               = 1
		self.VERINFO_BUILD_DATE        = '05/20/2018'

		self.BOARD_NAME           = 'apl'
		self.BOARD_PKG_NAME       = 'ApollolakeBoardPkg'
		self.SILICON_PKG_NAME     = 'ApollolakePkg'

		self.PCI_IO_BASE          = 0x00001000
		self.PCI_MEM32_BASE       = 0x80000000

		self.FLASH_SIZE           = 0x800000
		self.FLASH_BASE           = self.FLASH_LAYOUT_START - self.FLASH_SIZE

		self.HAVE_VBT_BIN         = 1
		self.HAVE_VERIFIED_BOOT   = 1
		self.HAVE_MEASURED_BOOT   = 0
		self.HAVE_FLASH_MAP       = 1
		self.HAVE_SEED_LIST       = 0
		self.HAVE_PSD_TABLE       = 1

		self.ENABLE_FSP_LOAD_IMAGE    = 0
		self.ENABLE_CRYPTO_SHA_NI     = 1
		self.ENABLE_VTD               = 1
		self.ENABLE_FWU               = 1
		self.ENABLE_SPLASH            = 1
		self.ENABLE_FRAMEBUFFER_INIT  = 1

		# To enable source debug, set 1 to self.ENABLE_SOURCE_DEBUG
		# self.ENABLE_SOURCE_DEBUG   = 1

		# Temporary skip Stage1A due to 32KB(IBBL) size limitation
                # until library size optimization has done.
		# If ENABLE_SOURCE_DEBUG is disabled, SKIP_STAGE1A_SOURCE_DEBUG will be ignored
		self.SKIP_STAGE1A_SOURCE_DEBUG  = 1

		# BIT0:Serial  BIT1:USB KB
		# Support serial port input console by default
		self.CONSOLE_IN_DEVICE_MASK  = 0x00000001

		# BIT0:Serial  BIT1:GFX
		self.CONSOLE_OUT_DEVICE_MASK = 0x00000001

		# Mem | NVMe | Usb | Spi | Ufs | eMMC | SD | Sata
		self.BOOT_MEDIA_SUPPORT_MASK  = 0xBF

		# EXT | FAT
		self.FILE_SYSTEM_SUPPORT_MASK  = 3

		# OS_PK | FWU_PK | CFG_PK | FWU_PLD | PLD | Stage2 | Stage1B
		# Stage1B is verified by CSE
		self.VERIFIED_BOOT_HASH_MASK  = 0x000000D6
		if self.ENABLE_FWU:
			self.VERIFIED_BOOT_HASH_MASK  |= 0x00000028

		# Verify required minimum FSP version
		self.MIN_FSP_REVISION     = 0x01040301
		# Verify FSP image ID. Empty string means skipping verification
		self.FSP_IMAGE_ID         = '$APLFSP$'

		self.STAGE1A_SIZE         = 0x00008000
		self.STAGE1B_SIZE         = 0x00035000
		if self.ENABLE_SOURCE_DEBUG:
			self.STAGE1B_SIZE += 0x2000
		self.STAGE2_SIZE          = 0x00032000
		self.PAYLOAD_SIZE         = 0x0001F000

		if len(self._PAYLOAD_NAME.split(';')) > 1:
			# EPAYLOAD is specified
			self.EPAYLOAD_SIZE      = 0x00120000
		else:
			# EPAYLOAD does not exist, create a dummy one
			self.EPAYLOAD_SIZE      = 0x1000

		if self.FSPDEBUG_MODE == 1:
			self.STAGE1B_SIZE += 0x00009000
			self.STAGE2_SIZE  += 0x0000F000

		self.STAGE1A_XIP          = 0
		self.STAGE1A_LOAD_BASE    = 0xFEF80000

		self.STAGE1B_XIP          = 0
		self.STAGE1B_LOAD_BASE    = 0xFEF40000
		self.STAGE1B_FD_BASE      = 0xFEF88000
		self.STAGE1B_FD_SIZE      = 0x0006B000
		if self.RELEASE_MODE == 0:
			self.STAGE1B_FD_SIZE += 0x00002000
			self.PAYLOAD_SIZE    += 0x00003000
		# For Stage2, it is always compressed.
		# if STAGE2_LOAD_HIGH is 1, STAGE2_FD_BASE will be ignored
		self.STAGE2_FD_BASE       = 0x01000000
		self.STAGE2_FD_SIZE       = 0x00080000
		self.STAGE2_LOAD_BASE     = 0x00100000

		self.STAGE1_STACK_SIZE    = 0x00002000
		self.STAGE1_DATA_SIZE     = 0x00006000

		# To support large payload such as UEFI
		self.LOADER_RSVD_MEM_SIZE = 0x00B8C000

		self.PLD_RSVD_MEM_SIZE    = 0x00500000
		self.PAYLOAD_LOAD_HIGH    = 1

		self.PLD_HEAP_SIZE        = 0x04000000

		self.FWUPDATE_SIZE        = 0x00030000
		self.CFGDATA_SIZE         = 0x00004000
		self.CFG_DATABASE_SIZE    = self.CFGDATA_SIZE
		self.MRCDATA_SIZE         = 0x00004000
		self.VARIABLE_SIZE        = 0x00002000
		self.SPI_IAS1_SIZE        = 0x00150000
		self.S3_DEBUG             = 0
		self.SBLRSVD_SIZE         = 0x00001000

		self._CFGDATA_INT_FILE = ['CfgData_Int_LeafHill.dlt']
		self._CFGDATA_EXT_FILE = ['CfgData_Ext_Gpmrb.dlt', 'CfgData_Ext_Up2.dlt','CfgData_Ext_OxbHill.dlt','CfgData_Ext_MB3.dlt','CfgData_Ext_JuniperHill.dlt']

	def GetDscLibrarys (self, BuildPkgName = "BootLoaderCorePkg"):
		dsc_libs = {}
		# These libraries will be added into the DSC files
		dsc_libs['IA32'] = [
			'LoaderLib|Platform/$(BOARD_PKG_NAME)/Library/LoaderLib/LoaderLib.inf',
			'SerialPortLib|Silicon/$(SILICON_PKG_NAME)/Library/SerialPortLib/SerialPortLib.inf',
			'SocInfoLib|Silicon/$(SILICON_PKG_NAME)/Library/SocInfoLib/SocInfoLib.inf',
			'PlatformHookLib|Silicon/$(SILICON_PKG_NAME)/Library/PlatformHookLib/PlatformHookLib.inf',
			'ScSbiAccessLib|Silicon/$(SILICON_PKG_NAME)/Library/ScSbiAccessLib/ScSbiAccessLib.inf',
			'GpioLib|Silicon/$(SILICON_PKG_NAME)/Library/GpioLib/GpioLib.inf',
			'SpiFlashLib|Silicon/$(SILICON_PKG_NAME)/Library/SpiFlashLib/SpiFlashLib.inf',
			'IgdOpRegionLib|Silicon/$(SILICON_PKG_NAME)/Library/IgdOpRegionLib/IgdOpRegionLib.inf',
			'IocIpcLib|Platform/$(BOARD_PKG_NAME)/Library/IocIpcLib/IocIpcLib.inf',
			'BootGuardLib|Silicon/$(SILICON_PKG_NAME)/Library/BootGuardLib20/BootGuardLib20.inf',
			'HeciLib|Silicon/ApollolakePkg/Library/HeciLib/HeciLib.inf',
			'PsdLib|Silicon/ApollolakePkg/Library/PsdLib/PsdLib.inf',
			'ShellExtensionLib|Platform/$(BOARD_PKG_NAME)/Library/ShellExtensionLib/ShellExtensionLib.inf',
			'BootMediaLib|Silicon/ApollolakePkg/Library/BootMediaLib/BootMediaLib.inf',
			'FlashDescriptorLib|Silicon/ApollolakePkg/Library/FlashDescriptorLib/FlashDescriptorLib.inf',
			'VtdLib|Silicon/$(SILICON_PKG_NAME)/Library/VtdLib/VtdLib.inf'
		]
		return dsc_libs

	def GetFlashMapList (self):
		img_list  = self.GetImageLayout ()
		comp_list = []
		offset    = 0

		# Skip Stitch_IPAD and Stitch_OPAD for flash map
		for img in img_list[2:][::-1]:
				child = img[1][0]
				if child[3] & STITCH_OPS.MODE_FILE_IGNOR:
					continue
				bname = os.path.splitext(child[0])[0]
				comp = {'name':child[0], 'bname':bname, 'offset':offset, 'size':child[2], 'flag': FlashMap.FLASH_MAP_DESC_FLAGS['COMPRESSED'] if child[1] else 0}
				if bname in  ['STAGE1A', 'STAGE1B', 'STAGE2', 'FWUPDATE', 'CFGDATA', 'MRCDATA', 'PAYLOAD', 'VARIABLE']:
					comp['flag'] |= FlashMap.FLASH_MAP_DESC_FLAGS['REDUNDANT']
				else:
					comp['flag'] |= FlashMap.FLASH_MAP_DESC_FLAGS['NON_REDUNDANT']
				comp_list.append (comp)
				offset += child[2]

		flag = FlashMap.FLASH_MAP_DESC_FLAGS['REDUNDANT']
		comp_list.append ({'name':'SBLRSVD.bin','bname':'SBLRSVD','offset':0, 'size':self.SBLRSVD_SIZE, 'flag': FlashMap.FLASH_MAP_DESC_FLAGS['NON_REDUNDANT']})
		comp_list.append ({'name':'BPM.bin',    'bname':'BPM',    'offset':0, 'size':0, 'flag': flag})
		comp_list.append ({'name':'OEMKEY.bin', 'bname':'OEMKEY', 'offset':0, 'size':0, 'flag': flag})
		return comp_list[::-1]

	def GetOutputImages (self):
		# define extra images that will be copied to output folder
		img_list = ['SlimBootloader.txt',
					'CfgDataStitch.py',
					'CfgDataDef.dsc',
					'CfgDataInt.bin'
					]
		return img_list

	def GetImageLayout (self):
		ias1_flag = 0 if self.SPI_IAS1_SIZE > 0 else STITCH_OPS.MODE_FILE_IGNOR
		fwu_flag = 0 if self.ENABLE_FWU else STITCH_OPS.MODE_FILE_IGNOR
		img_list  = []
		img_list.extend ([
					# Padding to ensure all other components in OBB partition will be aligned at 4KB boundary
					# 0xB00 assumes (IBBP.man, BPM.met) + (IPAD, IBBL, IBBM,  OBB, FWUP, CFGD, PLD, VAR, MRCD) in BpdtIBB
					# 0x180 assumes (OPAD,  PROV, EPLD) in BpdtOBB 
					# If more files are added, the offset needs to be adjusted accordingly
					('Stitch_IPAD.bin', [
						('PADDING.bin',  '',                0xB00,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
					),
					('Stitch_OPAD.bin', [
						('PADDING.bin',  '',                0x180,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
					),
					('Stitch_FWU.bin', [
						('FWUPDATE.bin' ,  'Lzma', self.FWUPDATE_SIZE, STITCH_OPS.MODE_FILE_PAD | fwu_flag, STITCH_OPS.MODE_POS_TAIL)]
					),
					('Stitch_FB.bin', [
						('SPI_IAS1.bin',  '',    self.SPI_IAS1_SIZE,  STITCH_OPS.MODE_FILE_PAD | ias1_flag, STITCH_OPS.MODE_POS_TAIL)]
					),
					('Stitch_PLD.bin', [
						('PAYLOAD.bin',  'Lz4', self.PAYLOAD_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
					),
					('Stitch_VAR.bin', [
						('VARIABLE.bin', '',     self.VARIABLE_SIZE,  STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL)]
 					),
					('Stitch_MRCDATA.bin', [
						('MRCDATA.bin', '',      self.MRCDATA_SIZE,   STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL)]
					),
 					('Stitch_CFGDATA.bin', [
						('CFGDATA.bin',  '',     self.CFGDATA_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
					),
					('Stitch_OBB.bin', [
						('STAGE2.fd',    'Lz4',  self.STAGE2_SIZE,    STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
					),
					('Stitch_IBBM.bin', [
						('STAGE1B.fd',   'Lz4',  self.STAGE1B_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
					),
					('Stitch_IBBL.bin', [
						('STAGE1A.fd',   '',     self.STAGE1A_SIZE,   STITCH_OPS.MODE_FILE_NOP, STITCH_OPS.MODE_POS_TAIL)]
					),			
					('Stitch_EPLD.bin', [
						('EPAYLOAD.bin', '',    self.EPAYLOAD_SIZE,   STITCH_OPS.MODE_FILE_PAD, STITCH_OPS.MODE_POS_TAIL)]
					),
					])

		return img_list
