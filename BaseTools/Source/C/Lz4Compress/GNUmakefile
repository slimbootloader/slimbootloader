## @file
# GNU/Linux makefile for 'Lz4Compress' module build.
#
# Copyright (c) 2009 - 2018, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#

MAKEROOT ?= ..

APPNAME = Lz4Compress

SDK_C = Sdk

OBJECTS = \
  Lz4Compress.o \
  $(SDK_C)/lz4frame.o \
  $(SDK_C)/lz4hc.o \
  $(SDK_C)/lz4.o \
  $(SDK_C)/xxhash.o

include $(MAKEROOT)/Makefiles/app.makefile

