/*
 * iconv_wrap.c - Wrapper functions for libiconv.
 * 
 * Copyright (C) 2006 by Hatuka*nezumi - IKEDA Soji <hatuka(at)nezumi.nu>,
 * redistributed under GNU General Public License version 2 (or later
 * version you prefer).
 *
 * $id$
 */

#include <errno.h>
#include <iconv.h>
#include "common.h"
#include "linefold.h"

#define REPLACEMENT_CHARACTER ((unicode_char)0xFFFD)
#define SUBSTITUTE_CHARACTER '?'

#define EXPAND_BUF(chartype, extralen) \
  clen = op - ostr; \
  alloclen += sizeof(chartype); \
  if ((newostr = (chartype *)malloc(alloclen + extralen)) == NULL) { \
    free(ostr); \
    errno = ENOMEM; \
    return -1; \
  } \
  memcpy(newostr, ostr, sizeof(chartype) * clen); \
  free(ostr); \
  ostr = newostr; \
  op = ostr + clen; \
  oleft += sizeof(chartype);


size_t decode(const char *chset, char *istr, size_t ilen, unicode_char **ostrp)
{
  static iconv_t cd = (iconv_t)-1;
  char *ip;
  unicode_char *ostr, *op, *newostr;
  size_t alloclen, ileft, oleft, clen;

  if (istr == NULL || ilen == 0)
    return 0;
  if (cd == (iconv_t)-1 &&
      (cd = iconv_open(INTERNAL_UNICODE_CHARSET, chset)) == (iconv_t)-1)
    return -1;
  alloclen = sizeof(unicode_char);
  ostr = (unicode_char *)malloc(alloclen);
  if (ostr == NULL) {
    errno = ENOMEM;
    return -1;
  }

  ip = istr;
  op = ostr;
  ileft = ilen;
  oleft = alloclen;
  while (iconv(cd, &ip, &ileft, (char **)&op, &oleft) == (size_t)-1) {
    if (errno == E2BIG) {
      EXPAND_BUF(unicode_char, 0)
    } else if (errno == EILSEQ) {
      if (oleft < sizeof(unicode_char)) {
	EXPAND_BUF(unicode_char, 0)
      }
      ip++;
      ileft--;
      *(op++) = REPLACEMENT_CHARACTER;
      oleft -= sizeof(unicode_char);
    } else if (errno == EINVAL) {
      if (oleft < sizeof(unicode_char)) {
	EXPAND_BUF(unicode_char, 0)
      }
      *(op++) = REPLACEMENT_CHARACTER;
      oleft -= sizeof(unicode_char);
      break;
    } else { /* NOTREACHED */
      return -1;
    }
  }

  *ostrp = ostr;
  return op - ostr;
}

size_t encode(const char *chset, unicode_char *istr, size_t ilen, char **ostrp)
{
  static iconv_t cd = (iconv_t)-1;
  unicode_char *ip;
  char *ostr, *op, *newostr;
  size_t alloclen, ileft, oleft, clen;

  if (istr == NULL || ilen == 0)
    return 0;
  if (cd == (iconv_t)-1 &&
      (cd = iconv_open(chset, INTERNAL_UNICODE_CHARSET)) == (iconv_t)-1)
    return -1;
  alloclen = sizeof(char);
  ostr = (char *)malloc(alloclen+1);
  if (ostr == NULL) {
    errno = ENOMEM;
    return -1;
  }

  ip = istr;
  op = ostr;
  ileft = ilen * sizeof(unicode_char);
  oleft = alloclen;
  while (iconv(cd, (char **)&ip, &ileft, &op, &oleft) == (size_t)-1) {
    if (errno == E2BIG) {
      EXPAND_BUF(char, 1)
    } else if (errno == EILSEQ) {
      if (oleft < sizeof(char)) {
	EXPAND_BUF(char, 1)
      }
      ip++;
      ileft -= sizeof(unicode_char);
      *(op++) = SUBSTITUTE_CHARACTER;
      oleft -= sizeof(char);
    } else if (errno == EINVAL) { /* NOTREACHED */
      return -1;
    } else { /* NOTREACHED */
      return -1;
    }
  }

  while ((iconv(cd, NULL, NULL, &op, &oleft)) == (size_t)-1) {
    if (E2BIG == errno) {
      EXPAND_BUF(char, 1)
    } else { /* NOTREACHED */
      return -1;
    }
  }

  ostr[op - ostr] = '\0';
  *ostrp = ostr;
  return op - ostr;
}
