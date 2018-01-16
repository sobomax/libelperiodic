lib_LTLIBRARIES = libelperiodic.la
libelperiodic_la_SOURCES = periodic.c prdic_math.c \
  elperiodic.h prdic_math.h prdic_timespecops.h
libelperiodic_la_DEPENDENCIES = Symbol.map
libelperiodic_la_CFLAGS = -g3 -O0 -fPIC
libelperiodic_la_LDFLAGS = -version-info 1 -Wl,--version-script=$(srcdir)/Symbol.map

noinst_PROGRAMS = testskew # gpdrive_test
testskew_SOURCES = testskew.c
testskew_LDADD = libelperiodic.la -lm @RT_LIB@

gpdrive_test_SOURCES = gpdrive_test.c
gpdrive_test_LDADD = libelperiodic.la -lm @RT_LIB@ -lgpio
