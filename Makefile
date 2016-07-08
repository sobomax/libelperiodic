LIB=            periodic
SHLIB_MAJOR=    0

SRCS=		periodic.c periodic.h prdic_math.c prdic_math.h

WARNS?=		4

VERSION_DEF=	${.CURDIR}/Versions.def
SYMBOL_MAPS=	${.CURDIR}/Symbol.map
CFLAGS+=	-DSYMBOL_VERSIONING

.include <bsd.lib.mk>
