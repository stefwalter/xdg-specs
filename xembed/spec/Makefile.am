EXTRA_DIST = xembed-spec.xml

html-build.stamp: xembed-spec.xml
	sdir=`cd $(srcdir) && pwd` &&           \
	rm -rf html && mkdir html &&		\
	cd html &&				\
	docbook2html $$sdir/xembed-spec.xml &&	\
	touch ../html-build.stamp

xembed-spec.ps: xembed-spec.xml
	docbook2ps $(srcdir)/xembed-spec.xml

all-local: html-build.stamp xembed-spec.ps

clean-local:
	rm -rf html-build.stamp html xembed-spec.ps

