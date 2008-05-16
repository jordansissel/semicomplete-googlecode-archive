ANNOTATE_DIR=/tmp/h3annotate
INTRO_DIR=/tmp/h3intro

all: intro annotate
	make video.avi

#mencoder crashes unless I strace it? Sigh.
video.avi: intro.rawvid annotate.rawvid
	rm output.avi > /dev/null 2>&1 || true
	mencoder intro.rawvid annotate.rawvid -ovc lavc -aspect 2.42:1 -oac copy -lavcopts threads=4:vcodec=mpeg4:trell:mbd=2 -o output.avi
	#strace -o /dev/null mencoder $$tmp.1 $$tmp.2 -ovc lavc -aspect 2.42:1 -oac copy -lavcopts vcodec=mpeg4:trell:mbd=2 -o output.avi; 

intro.rawvid: intro
	mencoder mf://$(INTRO_DIR)/*.jpg -mf w=602:h=250:fps=25:type=jpg -ovc copy -aspect 2.42:1 -oac copy -o $@

annotate.rawvid: annotate
	mencoder mf://$(ANNOTATE_DIR)/*.jpg -mf w=602:h=250:fps=25:type=jpg -ovc copy -aspect 2.42:1 -oac copy -o $@

halo3logo.jpg: h3icon_1600.jpg
	convert h3icon_1600.jpg -crop 1600x664+0+310 -resize 602!x250! $@

intro: .intro
.intro: halo3logo.jpg black.jpg Makefile
	mkdir $(INTRO_DIR) || true
	sh makeintro.sh $(INTRO_DIR)
	touch $@

annotate: .annotate

.annotate: Makefile
	mkdir $(ANNOTATE_DIR) || true
	COUNT=$$(ls mapdata/* | wc -l); \
	(seq $$COUNT || jot $$COUNT) \
	| xargs -P4 -n20 perl annotate.pl $(ANNOTATE_DIR)
	touch $@

black.jpg:
	convert -size 602x250 xc:black $@

clean:
	rm halo3logo.jpg intro_frame_*.jpg \
	annotate intro \
	> /dev/null 2>&1 || true
	rm -rf $(ANNOTATE_DIR) $(INTRO_DIR) \

