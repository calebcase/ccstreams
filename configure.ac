AC_INIT([ccstreams], [0.1], [calebcase@gmail.com])
: ${CFLAGS="-g"}
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
LT_INIT
AC_PROG_CC
AM_PROG_CC_C_O
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
AC_CHECK_FUNC(fopencookie,,AC_MSG_ERROR(fopencookie is required))
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    include/Makefile
    src/Makefile
    test/Makefile
    test/check/Makefile
    test/example/Makefile
])
AC_OUTPUT
