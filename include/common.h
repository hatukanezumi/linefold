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

#ifdef HAVE_WCHAR_H
#    include <wchar.h>
#endif /* HAVE_WCHAR_H */

#endif /* COMMON_H */
