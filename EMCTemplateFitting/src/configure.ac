AC_INIT(prototype4,[1.00])
AC_CONFIG_SRCDIR([configure.ac])
AM_INIT_AUTOMAKE

AC_PROG_CXX(g++)
LT_INIT([disable-static])

case $CXX in
  clang++)
# boost statistics
  CXXFLAGS="$CXXFLAGS -Wall -Werror -Wno-deprecated-declarations -Wno-unused-parameter"
 ;;
 *g++)
  if test `g++ -dumpversion | gawk '{print $1>=8.0?"1":"0"}'` = 1; then
   CXXFLAGS="$CXXFLAGS -Wall -Wno-deprecated-declarations -Werror -Wno-unused-parameter"
  else
   CXXFLAGS="$CXXFLAGS -Wall -Werror"
  fi
  ;;
esac

CINTDEFS=" -noIncludePaths  -inlineInputHeader "
AC_SUBST(CINTDEFS)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
