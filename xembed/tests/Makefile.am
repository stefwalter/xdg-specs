noinst_PROGRAMS = qt-client qt-embedder gtk-client gtk-embedder

INCLUDES = @all_includes@ @GTK_CFLAGS@ -DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED

#
# Qt Examples
#

qt_libs =  $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KFILE)  $(LIBSOCKET)

BUILT_SOURCES=moc_qt-client.cpp moc_qt-embedder.cpp moc_qt-common.cpp

moc_qt-client.cpp: qt-client.h
	$(MOC) $< -o $@

moc_qt-embedder.cpp: qt-embedder.h
	$(MOC) $< -o $@

moc_qt-common.cpp: qt-common.h
	$(MOC) $< -o $@

qt_client_SOURCES = qt-client.cpp qt-client.h qt-common.cpp qt-common.h
qt_client_LDADD = $(qt_libs) moc_qt-client.$(OBJEXT) moc_qt-common.$(OBJEXT)

qt_embedder_SOURCES = qt-embedder.cpp qt-embedder.h qt-common.cpp qt-common.h
qt_embedder_LDADD = $(qt_libs)  moc_qt-embedder.$(OBJEXT) moc_qt-common.$(OBJEXT)

CLEANFILES = moc_qt-client.cpp moc_qt-embedder.cpp moc_qt-common.cpp

#
# GTK+ examples
#

gtk_client_SOURCES = gtk-client.c gtk-common.c gtk-common.h
gtk_client_LDADD = $(GTK_LIBS)

gtk_embedder_SOURCES = gtk-embedder.c gtk-common.c gtk-common.h
gtk_embedder_LDADD =  $(GTK_LIBS)

