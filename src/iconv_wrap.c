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
static unicode_char SUBST_NARROW = (unicode_char)0x3F; /* QUESTION MARK */
static unicode_char SUBST_WIDE = (unicode_char)0x3013; /* GETA MARK */


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
size_t decode(const char *chset, char *istr, int *istartp, size_t ilen,
              unicode_char **ostrp, int ostart)
{
  static iconv_t cd = (iconv_t)-1;
  static size_t alloclen = 0;
  char *ip;
  unicode_char *ostr, *op, *newostr;
  size_t ileft, oleft, clen;

  if (istr == NULL || ilen == 0)
    return 0;

  if (ostart == 0) {
    if (cd == (iconv_t)-1 &&
	(cd = iconv_open(INTERNAL_UNICODE_CHARSET, chset)) == (iconv_t)-1)
      return -1;
    alloclen = sizeof(unicode_char);
    if ((ostr = (unicode_char *)malloc(alloclen)) == NULL)
      return -1;
  } else
    ostr = *ostrp;

  ip = istr + *istartp;
  op = ostr + ostart;
  ileft = ilen;
  oleft = alloclen - sizeof(unicode_char) * ostart;
  while (iconv(cd, &ip, &ileft, (char **)&op, &oleft) == (size_t)-1) {
    if (errno == E2BIG) {
      EXPAND_BUF(unicode_char, 1, 0)
	} else if (errno == EILSEQ) {
      if (oleft < sizeof(unicode_char)) {
	EXPAND_BUF(unicode_char, 1, 0)
	  }
      ip++;
      ileft--;
      *(op++) = REPLACEMENT_CHARACTER;
      oleft -= sizeof(unicode_char);
    } else if (errno == EINVAL) {
      if (oleft < sizeof(unicode_char)) {
	EXPAND_BUF(unicode_char, 1, 0)
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

/*
 * Encode Unicode string to legacy character set.
 */
size_t encode(const char *chset, struct linefold_info *lbi,
	      unicode_char *istr, int istart, size_t ilen, char **ostrp)
{
  static iconv_t cd = (iconv_t)-1;
  unicode_char *ip;
  char *ostr, *op, *newostr;
  size_t alloclen, ileft, oleft, clen;

  if (cd == (iconv_t)-1 &&
      (cd = iconv_open(chset, INTERNAL_UNICODE_CHARSET)) == (iconv_t)-1)
    return -1;

  alloclen = sizeof(char);
  if ((ostr = (char *)malloc(alloclen+1)) == NULL)
    return -1;

  ip = istr + istart;
  op = ostr;
  ileft = ilen * sizeof(unicode_char);
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
    ostr[op - ostr] = '\0';
    *ostrp = ostr;
    return op - ostr;
  } else if (ilen == 0) {
    return 0;
  } else {
    while (iconv(cd, (char **)&ip, &ileft, &op, &oleft) == (size_t)-1) {
      if (errno == E2BIG) {
	EXPAND_BUF(char, 1, 1)
	  } else if (errno == EILSEQ) {
	unicode_char *subst_str =
	  (lbi && (lbi->widths)[ip-istr] > 1)? &SUBST_WIDE: &SUBST_NARROW;
	size_t subst_left = sizeof(unicode_char);

	if (oleft < sizeof(char)*7) {
	  EXPAND_BUF(char, 7, 1)
	    }
	if (iconv(cd, (char **)&subst_str, &subst_left, &op, &oleft) ==
	    (size_t)-1) {
	  *(op++) = (char)SUBST_NARROW;
	  oleft -= sizeof(char);
	}
	ip++;
	ileft -= sizeof(unicode_char);
      } else if (errno == EINVAL) { /* NOTREACHED */
	return -1;
      } else { /* NOTREACHED */
	return -1;
      }
    }

    ostr[op - ostr] = '\0';
    *ostrp = ostr;
    return op - ostr;
  }
}
