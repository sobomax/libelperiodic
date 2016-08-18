LIB=            elperiodic
SHLIB_MAJOR=    0

SRCS=		periodic.c prdic_math.c prdic_math.h
INCS=		elperiodic.h

WARNS?=		4

VERSION_DEF=	${.CURDIR}/Versions.def
SYMBOL_MAPS=	${.CURDIR}/Symbol.map
CFLAGS+=	-DSYMBOL_VERSIONING

LOCALBASE?=	/usr/local
LIBDIR?=	${LOCALBASE}/lib
INCLUDEDIR?=	${LOCALBASE}/include

.include <bsd.lib.mk>
