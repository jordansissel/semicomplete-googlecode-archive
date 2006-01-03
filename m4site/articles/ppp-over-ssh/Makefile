
SUBTARGETS+=article
CLEANTARGETS+=cleanarticle

.PRECIOUS: index.in 

TOPDIR?=../..
include ../../Makefile

article: index.html
cleanarticle:
	rm index.html index.in index.xin 2> /dev/null; :
