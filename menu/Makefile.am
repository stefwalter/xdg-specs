HTML_FILES=	menu-spec.html

XML_FILES=	menu-spec.xml

EXTRA_DIST=   	$(HTML_FILES) $(XML_FILES)

if XML_DOCS_ENABLED
all-local: $(HTML_FILES)
endif

%.html: %.xml
	$(XMLTO) html-nochunks $<

maintainer-clean-local:
	rm -f $(HTML_FILES)

