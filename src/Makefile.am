lib_LTLIBRARIES = libelperiodic.la
libelperiodic_la_SOURCES = periodic.c prdic_math.c \
  elperiodic.h prdic_math.h prdic_timespecops.h prdic_fd.c prdic_fd.h \
  prdic_pfd.c prdic_pfd.h prdic_main_fd.c prdic_main_fd.h \
  prdic_main_pfd.c prdic_main_pfd.h prdic_main.c prdic_main.h \
  prdic_recfilter.c prdic_recfilter.h
libelperiodic_la_DEPENDENCIES = Symbol.map
libelperiodic_la_CFLAGS = -g3 -O0 -fPIC
libelperiodic_la_LDFLAGS = -version-info 1 -Wl,--version-script=$(srcdir)/Symbol.map -lm @RT_LIB@

noinst_PROGRAMS = testskew # gpdrive_test
testskew_SOURCES = testskew.c
testskew_LDADD = libelperiodic.la

gpdrive_test_SOURCES = gpdrive_test.c
gpdrive_test_LDADD = libelperiodic.la -lgpio
