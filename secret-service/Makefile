
SPEC = org.freedesktop.Secrets.xml

all: html/index.html

html/index.html: reference.xml docbook-params.xsl specification.xml
	xmlto --skip-validation -o html/ -x docbook-params.xsl xhtml specification.xml

reference.xml: tools/spec-to-docbook.xsl $(SPEC)
	xsltproc tools/spec-to-docbook.xsl $(SPEC) > $@

clean:
	rm -f reference.xml
	rm -f html/*.html

upload: all
	rsync -Hvax html/./ specs.freedesktop.org:/srv/specifications.freedesktop.org/www/secret-service/./
