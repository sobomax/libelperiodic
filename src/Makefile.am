noinst_LIBRARIES = libelperiodic.a
libelperiodic_a_SOURCES = elperiodic.h periodic.c prdic_math.c prdic_math.h \
 prdic_timespecops.h
libelperiodic_a_CFLAGS = -g3 -O0 -fPIC

noinst_PROGRAMS = testskew
testskew_SOURCES = testskew.c
testskew_LDADD = libelperiodic.a -lm @RT_LIB@

