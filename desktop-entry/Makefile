all: desktop-entry-spec.html desktop-entry-spec.ps

desktop-entry-spec.html: desktop-entry-spec.sgml spec.dsl
	jade -t sgml -i html -d spec.dsl#html -V nochunks desktop-entry-spec.sgml > $@

desktop-entry-spec.ps: desktop-entry-spec.sgml spec.dsl
	docbook2ps -d spec.dsl#print desktop-entry-spec.sgml > $@
