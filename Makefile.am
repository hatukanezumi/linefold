lib_LTLIBRARIES = libinefold.la
libinefold_la_SOURCES = lib/linefold.c lib/linefoldtab.c include/common.h
libinefold_la_CFLAGS = -Iinclude
libinefold_la_LDFLAGS = -version-info 1:0:0

include_HEADERS = include/linefoldtab.h
nodist_include_HEADERS = include/linefold.h

if LINEFOLD_ENABLE_BIN
bin_PROGRAMS = linefold
linefold_SOURCES = src/main.c src/iconv_wrap.c src/option.c include/common.h
if !HAVE_STRERROR
linefold_SOURCES += src/strerror.c
endif
linefold_CFLAGS = -Iinclude @ICONV_INC@ @CHARSET_INC@
linefold_LDFLAGS = @ICONV_LIB@ @CHARSET_LIB@
linefold_LDADD = libinefold.la
endif

pkgdata_DATA = mklbproptab.py linebreakrule.html
pkgdatasubdir = $(pkgdatadir)/LineBreak
pkgdatasub_DATA = LineBreak/*.py
EXTRA_DIST = $(pkgdata_DATA) $(pkgdatasub_DATA)

if LINEFOLD_USE_PYTHON
linebreakrule.html lib/linefoldtab.c include/linefoldtab.h: $(pkgdatasub_DATA) mklbproptab.py
	if test -e linefoldtab.c; then rm linefoldtab.c; fi
	if test -e linefoldtab.h; then rm linefoldtab.h; fi
	@PYTHON@ mklbproptab.py && mv linefoldtab.c lib && mv linefoldtab.h include
endif
