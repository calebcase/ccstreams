AM_CFLAGS = -I$(top_srcdir)/include --include=config.h @CHECK_CFLAGS@

TESTS = str mem
check_PROGRAMS = str mem

LDADD = $(top_builddir)/src/libccstreams.la -lpthread @CHECK_LIBS@
