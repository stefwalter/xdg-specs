PACKAGE=xsettings
VERSION=0.2

CFLAGS= -Wall -g `gtk-config --cflags` -g -Wall
LIBS=`gtk-config --libs`

manager_objects = manager.o gtk-utils.o xsettings-common.o xsettings-manager.o

client_objects = client.o xsettings-common.o xsettings-client.o

all: xsettings-manager xsettings-client xsettings.html

xsettings-manager: $(manager_objects)
	$(CC) -o $@ $(manager_objects) $(LIBS)

xsettings-client: $(client_objects)
	$(CC) -o $@ $(client_objects) $(LIBS)

xsettings.html: xsettings.xml
	xmlto html-nochunks $<

xsettings.ps: xsettings.xml
	docbook2ps xsettings.xml

clean:
	rm -f *.o xsettings-manager xsettings-client xsettings.html

dist:
	distdir=$(PACKAGE)-$(VERSION) ;					\
	tag=`echo $$distdir | tr a-z.- A-Z__` ;				\
	cvs tag -F $$tag &&						\
	cvs export -r $$tag -d $$distdir $(PACKAGE) &&			\
	tar cvf - $$distdir | gzip -c --best > $$distdir.tar.gz &&	\
	rm -rf $$distdir

distcheck: dist
	distdir=$(PACKAGE)-$(VERSION) ;		\
	tar xvfz $$distdir.tar.gz &&		\
	cd $$distdir &&				\
	make &&					\
	cd .. &&				\
	rm -rf $$distdir
