# Drivers/Speech/eSpeak/Makefile.  Generated from Makefile.in by configure.
###############################################################################
# BRLTTY - A background process providing access to the console screen (when in
#          text mode) for a blind person using a refreshable braille display.
#
# Copyright (C) 1995-2012 by The BRLTTY Developers.
#
# BRLTTY comes with ABSOLUTELY NO WARRANTY.
#
# This is free software, placed under the terms of the
# GNU General Public License, as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any
# later version. Please see the file LICENSE-GPL for details.
#
# Web Page: http://mielke.cc/brltty/
#
# This software is maintained by Dave Mielke <dave@mielke.cc>.
###############################################################################

SRC_TOP = ../../../
SRC_DIR = .


BLD_TOP = ./../../../
BLD_DIR = .

include $(SRC_TOP)common.mk

###############################################################################
# BRLTTY - A background process providing access to the console screen (when in
#          text mode) for a blind person using a refreshable braille display.
#
# Copyright (C) 1995-2012 by The BRLTTY Developers.
#
# BRLTTY comes with ABSOLUTELY NO WARRANTY.
#
# This is free software, placed under the terms of the
# GNU General Public License, as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any
# later version. Please see the file LICENSE-GPL for details.
#
# Web Page: http://mielke.cc/brltty/
#
# This software is maintained by Dave Mielke <dave@mielke.cc>.
###############################################################################

DRIVER_CODE = es
DRIVER_NAME = eSpeak
DRIVER_COMMENT = text to speech engine 
DRIVER_VERSION = 0.2 
DRIVER_DEVELOPERS = Nicolas Pitre <nico@fluxnic.net>
SPK_OBJS = -L$(ESPEAK_ROOT)/lib -lespeak
include $(SRC_TOP)speech.mk

speech.$O:
	$(CC) $(SPK_CFLAGS) -I$(ESPEAK_ROOT)/include -c $(SRC_DIR)/speech.c

