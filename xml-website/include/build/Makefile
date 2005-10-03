BUILDDIR=/u9/psionic/public_html/new/build

default:
	$(BUILDDIR)/build.sh

clean:
	find ./ -name '.depends.*' -type f | xargs rm -rf
	find ./ -name '.generate' -type d | xargs rm -rf
	find ./ -name '*.html' -type f | xargs rm -f
	find ./ -name '*.xmli' -type f | xargs rm -f
	find ./ -name '.xmlbuild.*' -type f | xargs rm -f
