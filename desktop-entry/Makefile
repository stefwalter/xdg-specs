all: desktop-entry-spec.html

desktop-entry-spec.html: desktop-entry-spec.sgml
	jade -t sgml -i html -d spec.dsl#html -V nochunks $^ > $@