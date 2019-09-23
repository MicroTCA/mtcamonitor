#
# File:         Makefile 
#
#		tools/mtcamonitor
#
#
#
# Author:       Gerhard Grygiel <grygiel@desy.de>
#               Copyright 1994-2008
#
#               This program is free software; you can redistribute it
#               and/or  modify it under  the terms of  the GNU General
#               Public  License as  published  by  the  Free  Software
#               Foundation;  either  version 2 of the License, or  (at
#               your option) any later version.
#


DOOCSROOT = ../../..

# to define DOOCSROOT as an absolute path
include $(DOOCSROOT)/$(DOOCSARCH)/DEFINEDOOCSROOT

OBJDIR = $(DOOCSROOT)/$(DOOCSARCH)/obj/tools/mtcamonitor

SRCDIR = $(DOOCSROOT)/source/tools/mtcamonitor

# to define the arch dependend things
include $(DOOCSROOT)/$(DOOCSARCH)/CONFIG

SOURCEOBJ = \
        $(OBJDIR)/device_rw.o\
        $(OBJDIR)/hex_dec.o


ALLPROGS = \
	$(OBJDIR)/device_rw\
	$(OBJDIR)/hex_dec\
	$(OBJDIR)/mtcamonitor\
	$(OBJDIR)/mtcamonitor_hp\
	$(OBJDIR)/pciewr\
	$(OBJDIR)/lspci_t
	

all: $(ALLPROGS)

$(OBJDIR)/.depend depend:
		@if [ ! -f $(OBJDIR) ] ; then \
		  echo ---------- create dir $(OBJDIR) --------------; \
		  mkdir -p $(OBJDIR) ; \
		fi
		for i in $(SRCDIR)/*.cc ;do $(CCDEP) $$i ;done > $(OBJDIR)/.depend_temp
		cat $(OBJDIR)/.depend_temp | sed -e "/:/s/^/\$$\(OBJDIR\)\//g" > $(OBJDIR)/.depend
		chmod g+w $(OBJDIR)/.depend*

include $(OBJDIR)/.depend

$(OBJDIR)/device_rw:    $(OBJDIR)/device_rw.o	
		$(CC) -o $(OBJDIR)/device_rw $(OBJDIR)/device_rw.o
		@chmod g+w $(OBJDIR)/device_rw
		@echo "---------------- device_rw done---------------"

$(OBJDIR)/hex_dec:    $(OBJDIR)/hex_dec.o	
		$(CC) -o $(OBJDIR)/hex_dec $(SRCDIR)/hex_dec.c
		@chmod g+w $(OBJDIR)/hex_dec
		@echo "---------------- hex_dec done---------------"

$(OBJDIR)/mtcamonitor:	
		cp $(SRCDIR)/mtcamonitor $(OBJDIR)/
		@chmod g+w $(OBJDIR)/mtcamonitor
		@echo "---------------- mtcamonitor done---------------"

$(OBJDIR)/mtcamonitor_hp:	
		cp $(SRCDIR)/mtcamonitor_hp $(OBJDIR)/
		@chmod g+w $(OBJDIR)/mtcamonitor_hp
		@echo "---------------- mtcamonitor_hp done---------------"

$(OBJDIR)/pciewr:	
		cp $(SRCDIR)/pciewr $(OBJDIR)/
		@chmod g+w $(OBJDIR)/pciewr
		@echo "---------------- pciemon done---------------"

$(OBJDIR)/lspci_t:	
		cp $(SRCDIR)/lspci_t $(OBJDIR)/
		@chmod g+w $(OBJDIR)/lspci_t
		@echo "---------------- lspci_t done---------------"




clean:
	rm -f $(SOURCEOBJ) $(ALLPROGS) $(OBJDIR)/.depend*
	
update:
	cvs update

commit:
	cvs commit
	
package: $(ALLPROGS)
	create_package
