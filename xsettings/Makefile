CFLAGS= -Wall -g `gtk-config --cflags` -g -Wall
LIBS=`gtk-config --libs`

manager_objects = manager.o gtk-utils.o xsettings-common.o xsettings-manager.o

client_objects = client.o xsettings-common.o xsettings-client.o

all: xsettings-manager xsettings-client

xsettings-manager: $(manager_objects)
	$(CC) -o $@ $(manager_objects) $(LIBS)

xsettings-client: $(client_objects)
	$(CC) -o $@ $(client_objects) $(LIBS)

clean:
	rm -f *.o xsettings-manager xsettings-client

