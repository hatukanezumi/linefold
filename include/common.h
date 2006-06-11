#ifndef COMMON_H
#define COMMON_H

#ifdef HAVE_CONFIG_H
#    include "config.h"
#endif

#include "sys/types.h"

#ifdef STDC_HEADERS
#    include <stdlib.h>
#    include <string.h>
#elif HAVE_STRINGS_H
#    include <strings.h>
#endif /* STDC_HEADERS */

#if HAVE_LOCALE_H
#    if HAVE_SETLOCALE
#        include <locale.h>
#        if USE_LIBCHARSET
#            if HAVE_LOCALCHARSET_H
#                include <localcharset.h>
#            elif HAVE_LIBCHARSET_H
#                include <libcharset.h>
#            endif /* HAVE_LOCALCHARSET_H */
#        elif HAVE_LANGINFO_CODESET
#            include <langinfo.h>
#        endif /* USE_LIBCHARSET */
#    endif /* HAVE_SETLOCALE */
#endif /* HAVE_LOCALE_H */	

#endif /* COMMON_H */
