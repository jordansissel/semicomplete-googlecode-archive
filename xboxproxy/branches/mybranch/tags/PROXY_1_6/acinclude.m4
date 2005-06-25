# Calling:
# AC_CHECK_PATHS([/path1 /path2 ... /pathN],
#                [ test dohickey ],
#                [ if found ],
#                [ if not found ])


# Because AC_PATH_FOUND can be called with no arguments or parens,
# tell autoconf that this token is ok 
m4_pattern_allow([AC_PATH_FOUND])

AC_DEFUN([AC_PATH_FOUND], [
			CHECK_PATH_FOUND=1
			break
			])

AC_DEFUN([AC_CHECK_PATHS], 
			[
			CHECK_PATH_FOUND=0
			CHECK_PATHS="$1"
			for dirname in $CHECK_PATHS; do
				$2
			done
			if test $CHECK_PATH_FOUND -eq 0; then
				:
				$4
			else 
				:
				$3
			fi
			]
		  )
