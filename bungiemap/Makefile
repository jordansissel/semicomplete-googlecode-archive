map.avi: 
	env - PATH="$PATH" mencoder mf://input_*.jpg -mf w=602:h=250:fps=25:type=jpg -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o output.avi

halo3logo.jpg: h3icon_1600.jpg
	convert h3icon_1600.jpg -crop 1600x664+0+310 -resize 602!x250! $@

intro: halo3logo.jpg
	sh makeintro.sh
	#mencoder mf://intro_frame_*.jpg -mf w=602:h=250:fps=25:type=jpg -ovc lavc -lavcopts vcodec=mpeg4:mbd=2:trell -oac copy -o intro.avi

black.jpg:
	convert -size 602x250 xc:black $@

clean:
	rm halo3logo.jpg intro_frame_*.jpg > /dev/null 2>&1 || true

