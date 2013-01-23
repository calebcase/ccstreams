AM_CFLAGS = -I$(top_srcdir)/include --include=config.h

lib_LTLIBRARIES = libccstreams.la libecx_ccstreams.la

libccstreams_la_SOURCES = str.c mem.c

libecx_ccstreams_la_SOURCES = ecx_str.c ecx_mem.c
libecx_ccstreams_la_LIBADD = -lec -lccstreams
