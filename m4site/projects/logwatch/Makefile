
SUBTARGETS+=article
CLEANTARGETS+=cleanarticle

.PRECIOUS: index.in 

TOPDIR	?=../..
include $(TOPDIR)/Makefile

article: index.html
cleanarticle:
	rm index.html index.in 2> /dev/null; :
