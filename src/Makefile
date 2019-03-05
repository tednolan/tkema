#
#	$Id: Makefile,v 1.15 2004/04/12 17:23:31 ted Exp $
#
# This Makefile is intended to work on Solaris and MINGW
# Paths for Opnet and Tcl may need to be customized.
#

# always use gcc
CC=gcc

# check our OS
OS=$(shell uname -s)

ifeq ($(OS),SunOS)
	SHARED_SUFF=so
	OBJ_SUFF=o
	PACKAGE=solaris_sparc
	INSTALL=/usr/ucb/install
	OPDIR=/usr/local/cluster/opnet/10.0.A
	TCLDIR=/usr/local/cluster/tcl8.4.1
	TCLSH=tclsh8.4
	LIBOPEMA=$(OPDIR)/sys/sun_sparc_solaris/lib/libopema.so
	LIBOPEMA_IN=$(LIBOPEMA)

	CFLAGS=-Wall -fpic -I$(TCLDIR)/include \
	 	 -DTCL_USE_STUBS \
		 -DOPD_SUPPORT_PRIORITIES \
		 -DOPD_ALLOW_ODB \
		 -DHOST_SUN_SPARC_SOLAR \
		 -DSOLARIS \
		 -I/usr/openwin/include \
		 -I$(OPDIR)/sys/include

	LIBS=-R$(OPDIR)/sys/sun_sparc_solaris/lib \
		 -L$(OPDIR)/sys/sun_sparc_solaris/lib \
		-lopema -lopvos -lopmdutl -lopmdtab \
		-lopobj -lopprg -loptmm -lopxml \
		-loptim -loprel -lopgalaxy \
		-lX11 -lXext -lposix4 \
		-L$(TCLDIR)/lib -R$(TCLDIR)/lib \
		-ltclstub8.4 -lCrun

endif

ifeq ($(OS),MINGW32_NT-5.0)
	SHARED_SUFF=dll
	OBJ_SUFF=obj
	PACKAGE=win32_intel
	INSTALL=/bin/install
	OPDIR=/o/opnet/10.0.A
	TCLDIR=/usr/local/cluster/tcl845
	TCLSH=tclsh84
	LIBOPEMA=libopema.a
	LIBOPEMA_IN=$(OPDIR)/sys/pc_intel_win32/bin/opema.dll

	CFLAGS=-Wall -I$(TCLDIR)/include \
	 -DTCL_USE_STUBS \
	 -DOPD_SUPPORT_PRIORITIES \
	 -DOPD_ALLOW_ODB \
	 -DHOST_PC_INTEL_WIN32 \
	 -D_X86_=1 \
	 -DWIN32 \
	 -D_WIN32_ \
	 -D__WIN32__ \
	 -DWIN32=1 \
	 -D_MT \
	 -D_DLL \
	 -D_MSC_VER_ \
	 -I$(OPDIR)/sys/include

	LIBS=-L. -lopema \
       	 /o/opnet/10.0.A/sys/pc_intel_win32/lib/opvos.lib \
       	 /o/opnet/10.0.A/sys/pc_intel_win32/lib/opprg.lib \
       	 -L $(TCLDIR)/lib -ltclstub84 -ltcl84 
endif

#
# Carp if not Solaris or Mingw
#

ifneq ($(OS), SunOS)
ifneq ($(OS), MINGW32_NT-5.0)
$(error $(OS) Not supported!)
endif
endif

#
# We could use .o for MINGW as well, but then
# it could get confused with the Solaris .o
#
%.obj: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

all: libtkema.$(SHARED_SUFF) install

libtkema.$(SHARED_SUFF): tkema.$(OBJ_SUFF) $(LIBOPEMA)
	gcc -shared -o libtkema.$(SHARED_SUFF) tkema.$(OBJ_SUFF) $(LIBS)

$(LIBOPEMA): $(LIBOPEMA_IN)
	pexports $(LIBOPEMA_IN) > opema.def
	dlltool -d opema.def -l libopema.a

install: libtkema.$(SHARED_SUFF)
	-mkdir -p packages/solaris_sparc
	-mkdir -p packages/win32_intel
	$(INSTALL) -m 555 libtkema.$(SHARED_SUFF) packages/$(PACKAGE)
	echo "cd packages/$(PACKAGE); pkg_mkIndex -verbose . *.$(SHARED_SUFF)" | $(TCLSH)

clean:
	rm -f *.o *.so *.dll *.obj *.exe *.lib *.exp *.pch libopema.a *.def
	rm -rf packages
