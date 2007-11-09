ANNOTATE_DIR=/tmp/h3annotate
INTRO_DIR=/tmp/h3intro

all: intro annotate
	cat intro
	cat annotate
	make video.avi

#mencoder crashes unless I strace it? Sigh.
video.avi: 
	rm output.avi > /dev/null 2>&1 || true
	tmp=`mktemp`; \
	mencoder mf://$(INTRO_DIR)/*.jpg -mf w=602:h=250:fps=25:type=jpg -ovc copy -aspect 2.42:1 -oac copy -o $$tmp.1; \
	mencoder mf://$(ANNOTATE_DIR)/*.jpg -mf w=602:h=250:fps=25:type=jpg -ovc copy -aspect 2.42:1 -oac copy -o $$tmp.2; \
	strace -o /dev/null mencoder $$tmp.1 $$tmp.2 -ovc lavc -aspect 2.42:1 -oac copy -lavcopts vcodec=mpeg4:trell:mbd=2 -o output.avi; \


halo3logo.jpg: h3icon_1600.jpg
	convert h3icon_1600.jpg -crop 1600x664+0+310 -resize 602!x250! $@

intro: halo3logo.jpg Makefile
	mkdir $(INTRO_DIR) || true
	sh makeintro.sh $(INTRO_DIR)
	touch intro

annotate: Makefile
	mkdir $(ANNOTATE_DIR) || true
	perl annotate.pl $(ANNOTATE_DIR) 
	touch annotate

black.jpg:
	convert -size 602x250 xc:black $@

clean:
	rm halo3logo.jpg intro_frame_*.jpg \
	$(ANNOTATE_DIR) $(INTRO_DIR) \
	annotate intro \
	> /dev/null 2>&1 || true

