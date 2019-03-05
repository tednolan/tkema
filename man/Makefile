#
#	$Id: Makefile,v 1.3 2001/06/07 21:28:35 ted Exp $
#
# Create html, PostScript and pdf from the manual pages.  Probably should be
# an install option
#
GS=/usr/local/cluster/gs5.10/bin/gs

all: makesrc

makesrc: setup
	sh -c "cd src;make"

setup:
	chmod +x bin/make_html bin/make_index
	mkdir -p html
clean:
	/bin/rm -rf html
	sh -c "cd src; make clean"

install: makesrc
	sh -c "cd src; make install"
