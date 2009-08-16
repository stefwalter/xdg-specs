
SPEC = org.freedesktop.Secrets.xml

all: html/index.html

html/index.html: reference.xml params-html.xsl specification.xml
	xmlto --skip-validation -o html -p params-html.xsl xhtml specification.xml

reference.xml: tools/spec-to-docbook.xsl $(SPEC)
	xsltproc tools/spec-to-docbook.xsl $(SPEC) > $@
