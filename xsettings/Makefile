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
