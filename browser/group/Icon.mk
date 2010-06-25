#/*
# * Name        : Icon.mk
# * Description : Application icon makefile
# * Project     : This file is part of OpenMAR, an Open Mobile Augmented Reality browser
# * Website     : http://OpenMAR.org
# *
# * Copyright (c) 2010 David Caabeiro
# *
# * All rights reserved. This program and the accompanying materials are made available 
# * under the terms of the Eclipse Public License v1.0 which accompanies this 
# * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
# *
# */

ifeq (WINS,$(findstring WINS, $(PLATFORM)))
ZDIR=$(EPOCROOT)epoc32\release\$(PLATFORM)\$(CFG)\Z
else
ZDIR=$(EPOCROOT)epoc32\data\z
endif

TARGETDIR=$(ZDIR)\resource\apps
ICONTARGETFILENAME=$(TARGETDIR)\OpenMAR_0x2002E1AB.mif

ICONDIR=..\gfx

do_nothing :
	@rem do_nothing

MAKMAKE : do_nothing

BLD : do_nothing

CLEAN : do_nothing

LIB : do_nothing

CLEANLIB : do_nothing

# ----------------------------------------------------------------------------
# NOTE: if you have JUSTINTIME enabled for your S60 3rd FP1 or newer SDK
# and this command crashes, consider adding "/X" to the command line.
# See <http://forum.nokia.com/document/Forum_Nokia_Technical_Library_v1_35/contents/FNTL/Build_process_fails_at_mif_file_creation_in_S60_3rd_Ed_FP1_SDK.htm>
# ----------------------------------------------------------------------------

RESOURCE : $(ICONTARGETFILENAME)

$(ICONTARGETFILENAME) : $(ICONDIR)\Application.svg
	mifconv $(ICONTARGETFILENAME)  \
		 /c32 $(ICONDIR)\Application.svg

FREEZE : do_nothing

SAVESPACE : do_nothing

RELEASABLES :
	@echo $(ICONTARGETFILENAME)

FINAL : do_nothing
