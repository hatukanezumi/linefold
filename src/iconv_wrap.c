/*
 * iconv_wrap.c - Wrapper functions for libiconv.
 * 
 * Copyright (C) 2006 by Hatuka*nezumi - IKEDA Soji.  All rights reserved.
 *
 * This file is part of the Linefold Package.  This program is free
 * software; you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.  This program is distributed in the hope that
 * it will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the COPYING file for more details.
 *
 * $id$
 */

#include <iconv.h>
#include "common.h"
#include "linefold.h"

#define REPLACEMENT_CHARACTER ((linefold_char)0xFFFD)
static linefold_char SUBST_NARROW = (linefold_char)0x3F; /* QUESTION MARK */
static linefold_char SUBST_WIDE = (linefold_char)0x3013; /* GETA MARK */


#define EXPAND_BUF(chartype, addlen, extralen)				\
  clen = op - ostr;							\
  alloclen += sizeof(chartype)*addlen;					\
  if ((newostr = (chartype *)malloc(alloclen + extralen)) == NULL) {	\
    free(ostr);								\
    return -1;								\
  }									\
  memcpy(newostr, ostr, sizeof(chartype) * clen);			\
  free(ostr);								\
  ostr = newostr;							\
  op = ostr + clen;							\
  oleft += sizeof(chartype)*addlen;


/*
 * Decode string in legacy character set to Unicode string.
 */
size_t decode(const char *chset, char *istr, size_t *istartp, size_t ilen,
              linefold_char **ostrp, size_t ostart, int conversion)
{
  static iconv_t cd = (iconv_t)-1;
  static size_t alloclen = 0;
  char *ip;
  linefold_char *ostr, *op, *newostr;
  size_t ileft, oleft, clen;

  if (istr == NULL || ilen == 0)
    return 0;

  if (ostart == 0) {
    if (cd == (iconv_t)-1 &&
	(cd = iconv_open(INTERNAL_LINEFOLD_CHARSET, chset)) == (iconv_t)-1)
      return -1;
    alloclen = sizeof(linefold_char);
    if ((ostr = (linefold_char *)malloc(alloclen)) == NULL)
      return -1;
  } else
    ostr = *ostrp;

  ip = istr + *istartp;
  op = ostr + ostart;
  ileft = ilen;
  oleft = alloclen - sizeof(linefold_char) * ostart;

  if (istr == NULL) {
    iconv_close(cd);
    cd = (iconv_t)-1;
    *ostrp = ostr;
    return op - ostr;
  } else if (ilen == 0) {
    return 0;
  } else {
    while (iconv(cd, &ip, &ileft, (char **)&op, &oleft) == (size_t)-1) {
      if (errno == E2BIG) {
	EXPAND_BUF(linefold_char, 1, 0);
      } else if (errno == EILSEQ) {
	if (conversion == -1) { /* strict */
	  return -1;
	} else if (conversion == 0) { /* ignore */
	  ip++;
	  ileft--;
	} else { /* replace */
	  if (oleft < sizeof(linefold_char)) {
	    EXPAND_BUF(linefold_char, 1, 0);
	  }
	  ip++;
	  ileft--;
	  *(op++) = REPLACEMENT_CHARACTER;
	  oleft -= sizeof(linefold_char);
	}
      } else if (errno == EINVAL) {
	if (oleft < sizeof(linefold_char)) {
	  EXPAND_BUF(linefold_char, 1, 0);
	}
	break;
      } else { /* NOTREACHED */
	return -1;
      }
    }

    *istartp = ip - istr;
    *ostrp = ostr;
    return op - ostr;
  }
}

/*
 * Encode Unicode string to legacy character set.
 */
size_t encode(const char *chset, const struct linefold_info *lbi,
	      const linefold_char *istr, size_t istart, size_t ilen,
	      char **ostrp, int conversion)
{
  static iconv_t cd = (iconv_t)-1;
  const linefold_char *ip;
  char *ostr, *op, *newostr;
  size_t alloclen, ileft, oleft, clen;

  if (cd == (iconv_t)-1 &&
      (cd = iconv_open(chset, INTERNAL_LINEFOLD_CHARSET)) == (iconv_t)-1)
    return -1;

  alloclen = sizeof(char);
  if ((ostr = (char *)malloc(alloclen+1)) == NULL)
    return -1;

  ip = istr + istart;
  op = ostr;
  ileft = ilen * sizeof(linefold_char);
  oleft = alloclen;

  if (istr == NULL) {
    while ((iconv(cd, NULL, NULL, &op, &oleft)) == (size_t)-1) {
      if (errno == E2BIG) {
	EXPAND_BUF(char, 1, 1);
      } else { /* NOTREACHED */
	return -1;
      }
    }
    iconv_close(cd);
    cd = (iconv_t)-1;
    ostr[op - ostr] = '\0';
    *ostrp = ostr;
    return op - ostr;
  } else if (ilen == 0) {
    return 0;
  } else {
    while (iconv(cd, (char **)&ip, &ileft, &op, &oleft) == (size_t)-1) {
      if (errno == E2BIG) {
	EXPAND_BUF(char, 1, 1);
      } else if (errno == EILSEQ) {
	if (conversion == -1) { /* strict */
	  return -1;
	} else if (conversion == 0) { /* ignore */
	  ip++;
	  ileft -= sizeof(linefold_char);
	} else { /* replace */
	  size_t subst_left = sizeof(linefold_char);
	  linefold_char *subst_str = &SUBST_WIDE;

	  if (oleft < sizeof(char)*7) {
	    EXPAND_BUF(char, 7, 1);
	  }

	  if (lbi == NULL || lbi->widths[ip-istr] <= 1 ||
	      iconv(cd, (char **)&subst_str, &subst_left, &op, &oleft) == -1) {
	    subst_str = &SUBST_NARROW;
	    if (iconv(cd, (char **)&subst_str, &subst_left, &op, &oleft) ==
		(size_t)-1) {
	      *(op++) = (char)SUBST_NARROW;
	      oleft -= sizeof(char);
	    }
	  }
	  ip++;
	  ileft -= sizeof(linefold_char);
	}
      } else { /* NOTREACHED */
	return -1;
      }
    }

    ostr[op - ostr] = '\0';
    *ostrp = ostr;
    return op - ostr;
  }
}
