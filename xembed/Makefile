all: html-build.stamp xembed-spec.ps

html-build.stamp: xembed-spec.xml
	rm -rf html && mkdir html &&		\
	cd html &&				\
	docbook2html ../xembed-spec.xml &&	\
	touch ../html-build.stamp

xembed-spec.ps: xembed-spec.xml
	docbook2ps ./xembed-spec.xml

clean: rm -rf html-build.stamp html xembed-spec.ps

