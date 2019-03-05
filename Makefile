#
#	$Id: Makefile,v 1.1 2001/06/11 20:50:35 ted Exp $
#
all: makesrc makeman

install: srcinstall maninstall
	sh -c "cd src; make install"
	sh -c "cd man; make install"

makeman:
	sh -c "cd man; make"

makesrc:
	sh -c "cd src; make"

clean:
	sh -c "cd man; make clean"
	sh -c "cd src; make clean"
