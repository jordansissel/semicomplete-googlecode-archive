%: %.java
	@echo "* $@.java"
	@rm errors.$@ 2>&1 > /dev/null || true
	@javac $@.java 2> errors.$@ || true
	@echo "javac $@.java"
	@[ -r "errors.$@" ] && echo "Errors occurred. Output can be found in ./errors.$@"
