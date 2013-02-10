AM_CFLAGS = -I$(top_srcdir)/include --include=config.h @CHECK_CFLAGS@

TESTS = copy str mem
check_PROGRAMS = copy str mem

LDADD = $(top_builddir)/src/libccstreams.la -lpthread @CHECK_LIBS@
