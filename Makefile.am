lib_LTLIBRARIES = libinefold.la
libinefold_la_SOURCES = lib/linefold.c lib/linefoldtab.c include/common.h
libinefold_la_CFLAGS = -Iinclude
libinefold_la_LDFLAGS = -version-info 1:0:0

include_HEADERS = include/linefold.h include/linefoldtab.h

if ENABLE_BIN
bin_PROGRAMS = linefold
linefold_SOURCES = src/main.c src/iconv_wrap.c include/common.h
linefold_CFLAGS = -Iinclude
linefold_LDFLAGS = @ICONV_LIB@ @CHARSET_LIB@
linefold_LDADD = libinefold.la
endif

pkgdata_DATA = mklbproptab.py linebreakrule.html
pkgdatasubdir = $(pkgdatadir)/LineBreak
pkgdatasub_DATA = LineBreak/*.py
EXTRA_DIST = $(pkgdata_DATA) $(pkgdatasub_DATA)

if USE_PYTHON
lib/linefoldtab.c include/linefoldtab.h: $(pkgdata_DATA) $(pkgdatasub_DATA)
	-test -e linefoldtab.c && rm linefoldtab.c
	-test -e linefoldtab.h && rm linefoldtab.h
	@PYTHON@ mklbproptab.py && mv linefoldtab.c lib && mv linefoldtab.h include
endif
