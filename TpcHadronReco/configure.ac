AC_INIT([TpcHadronReco], [1.00])
AC_CONFIG_SRCDIR([TpcV0CandidateTree.cc])

AM_INIT_AUTOMAKE
AC_PROG_CXX(CC g++)
LT_INIT([disable-static])

if test $ac_cv_prog_gxx = yes; then
  CXXFLAGS="$CXXFLAGS -Wall -Wextra -Wshadow -Werror"
fi

case $CXX in
  clang++) CXXFLAGS="$CXXFLAGS -fopenmp" ;;
esac

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
