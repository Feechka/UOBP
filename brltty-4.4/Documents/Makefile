# Documents/Makefile.  Generated from Makefile.in by configure.
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

SRC_TOP = ../
SRC_DIR = .


BLD_TOP = ./../
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

all clean distclean::
	for directory in $(MANUAL_DIRECTORIES); do (cd $(BLD_TOP)$$directory && $(MAKE) $@) || exit 1; done

all:: $(ALL_API)

all-api man3: BrlAPIref-no

BrlAPIref-no:
	@echo doxygen is not installed - BrlAPI reference documentation will not be made

BrlAPIref-yes: BrlAPIref.made
BrlAPIref.made: BrlAPIref $(BLD_TOP)$(PGM_DIR)/brlapi.h $(BLD_TOP)$(PGM_DIR)/brlapi_constants.h $(SRC_TOP)$(PGM_DIR)/brlapi_keycodes.h $(SRC_TOP)$(PGM_DIR)/brlapi_protocol.h BrlAPIref.doxy
	$(DOXYGEN) BrlAPIref.doxy
	touch $@

BrlAPIref:
	mkdir BrlAPIref

clean::
	-rm -f *.made
	-rm -f -r BrlAPIref

distclean::
	-rm -f brltty.conf brltty.1 BrlAPIref.doxy

