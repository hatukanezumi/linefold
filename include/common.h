#ifndef COMMON_H
#define COMMON_H

#if HAVE_CONFIG_H
#    include "config.h"
#endif

#if HAVE_SYS_TYPES_H
#    include "sys/types.h"
#endif

#if STDC_HEADERS || HAVE_STDLIB_H
#    include <stdlib.h>
#endif /* STDC_HEADERS || HAVE_STDLIB_H */
#if STDC_HEADERS || HAVE_STRING_H
#    include <string.h>
#elif HAVE_STRINGS_H
#    include <strings.h>
#endif /* STDC_HEADERS || HAVE_STRING_H */

#if HAVE_MEMORY_H
#    include <memory.h>
#endif

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

#if HAVE_ERRNO_H
#    include <errno.h>
#endif /* HAVE_ERRNO_H */
#ifndef errno
extern int errno;
#endif /* errno */

#if !HAVE_STRERROR
extern char *strerror(int);
#endif

#endif /* COMMON_H */
