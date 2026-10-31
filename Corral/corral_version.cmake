# Writes ${OUT} = corral_version.h with the git commit of the source tree and the build time (run at every build by the
# corral_version target in CMakeLists.txt). corral records them in info/code of its root files (README.md, "info/").
execute_process(COMMAND git -C ${SRC} describe --always --dirty --abbrev=8 OUTPUT_VARIABLE GITV OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
execute_process(COMMAND git -C ${SRC} log -1 --format=%cd --date=format:%Y-%m-%d OUTPUT_VARIABLE GITD OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
if(NOT GITV)
	set(GITV "unknown")
endif()
string(TIMESTAMP NOW "%Y-%m-%d %H:%M")
file(WRITE ${OUT}.tmp "#define CORRAL_GIT \"${GITV}\"\n#define CORRAL_GIT_DATE \"${GITD}\"\n#define CORRAL_BUILD_TIME \"${NOW}\"\n")
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OUT}.tmp ${OUT})
