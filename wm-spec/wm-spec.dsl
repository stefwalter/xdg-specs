<!DOCTYPE style-sheet PUBLIC "-//James Clark//DTD DSSSL Style Sheet//EN" [
<!ENTITY db-html PUBLIC "-//Norman Walsh//DOCUMENT DocBook HTML Stylesheet//EN" CDATA dsssl>
<!ENTITY db-print PUBLIC "-//Norman Walsh//DOCUMENT DocBook Print Stylesheet//EN" CDATA dsssl>
]>
<style-sheet>
<style-specification id="html" use="common db-html">
<style-specification-body> 
;; html customization goes here

(define %root-filename% "index")
(define %html-ext% ".html")

</style-specification-body>
</style-specification>

<style-specification id="print" use="common db-print">
<style-specification-body> 
;; print customization goes here
</style-specification-body>
</style-specification>

<style-specification id="common">
<style-specification-body> 
;; common customization goes here

(define %section-autolabel% #t)
(define %quanda-inherit-numeration% #t)
(define %generate-article-toc% #t)
(define (article-titlepage-recto-elements)
  (list (normalize "title") 
	(normalize "subtitle") 
	(normalize "corpauthor") 
	(normalize "authorgroup") 
	(normalize "author") 
	(normalize "releaseinfo")
	(normalize "date")))

</style-specification-body>
</style-specification>

<external-specification id="db-html" document="db-html">
<external-specification id="db-print" document="db-print">
</style-sheet>

