/*
 * An implementation of line breaking algorithm described in
 * Unicode Standard Annex #14 (UAX#14).
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
 * $Id$
 */

#include <assert.h>
#include "common.h"
#include "linefold.h"

static size_t
find_linebreak(size_t, linefold_class *, linefold_action *, linefold_flags);
static int
charsetcmp(const char *, const char *);

/*
 * Public Functions
 */

/* Allocate line break informations. */
struct linefold_info *
linefold_alloc(const linefold_char *text, size_t textlen,
	       linefold_lbprop_funcptr
	       (*find_lbprop_func)(const char *, linefold_flags),
	       void (*tailor_lbprop)(linefold_char,
				     linefold_width *, linefold_class *,
				     linefold_flags),
	       const char *chset, linefold_flags flags)
{
  struct linefold_info *lbinfo;
  linefold_width *widths;
  linefold_class *lbclasses;
  linefold_action *lbactions;
  char *charset=NULL;
  size_t i;
  linefold_lbprop_funcptr lbprop_func;

  if (text == NULL || textlen == 0)
    return NULL;

  if ((widths = malloc(sizeof(linefold_width) * textlen)) == NULL) {
    return NULL;
  }
  if ((lbclasses = malloc(sizeof(linefold_class) * textlen)) == NULL) {
    free(widths);
    return NULL;
  }
  if ((lbactions = malloc(sizeof(linefold_action) * textlen)) == NULL) {
    free(widths);
    free(lbclasses);
    return NULL;
  }
  if (chset && *chset) {
    if ((charset = malloc(strlen(chset)+1)) == NULL) {
      free(widths);
      free(lbclasses);
      free(lbactions);
      return NULL;
    } else {
      memcpy(charset, chset, strlen(chset)+1);
    }
  }
  if ((lbinfo = malloc(sizeof(struct linefold_info))) == NULL) {
    free(widths);
    free(lbclasses);
    free(lbactions);
    if (charset) free(charset);
    return NULL;
  }

  if (find_lbprop_func == NULL)
    find_lbprop_func = &linefold_find_lbprop_func;
  if (tailor_lbprop == NULL)
    tailor_lbprop = &linefold_tailor_lbprop;

  lbprop_func = (*find_lbprop_func)(charset, flags);
  for (i=0; i < textlen; i++) {
    (*lbprop_func)(text[i], widths+i, lbclasses+i);
    (*tailor_lbprop)(text[i], widths+i, lbclasses+i, flags);
    lbactions[i] = LINEFOLD_ACTION_PROHIBITED;
  }

  if (find_linebreak(textlen, lbclasses, lbactions, flags) == 0) {
    free(widths);
    free(lbclasses);
    free(lbactions);
    if (charset) free(charset);
    return NULL;
  }

  lbinfo->widths = widths;
  lbinfo->lbclasses = lbclasses;
  lbinfo->lbactions = lbactions;
  lbinfo->charset = charset;
  lbinfo->length = textlen;
  lbinfo->flags = flags;
  lbinfo->linp = lbinfo->lint = lbinfo->pint = 0;
  return lbinfo;
}

/* Free storage of line break informations. */
void linefold_free(struct linefold_info *lbinfo)
{
  if (lbinfo == NULL)
    return;

  free((void *)lbinfo->widths);
  free((void *)lbinfo->lbclasses);
  free((void *)lbinfo->lbactions);
  if (lbinfo->charset) free((void *)lbinfo->charset);
  free(lbinfo);
}

/* Do line breaking */
linefold_action
linefold(struct linefold_info *lbinfo, linefold_char *text,
	 int (*is_line_excess)(const struct linefold_info *,
			       const linefold_char *,
			       size_t, size_t, size_t, void *),
	 void (*writeout_cb)(const struct linefold_info *,
			     const linefold_char *,
			     size_t, size_t, linefold_action, void *),
	 size_t maxlen, void *voidarg)
{
  size_t textlen;
  const linefold_action *lbactions;
  linefold_flags flags;
  linefold_action global_action=LINEFOLD_ACTION_NOMOD,
    action, prevaction;
  size_t i=0, linestart, prevopp;

  if (lbinfo == NULL)
    return LINEFOLD_ACTION_NOMOD;
  textlen = lbinfo->length;
  lbactions = lbinfo->lbactions;
  flags = lbinfo->flags;

  if (is_line_excess == NULL)
    is_line_excess = &linefold_is_line_excess;

  while (i < textlen) {
    prevaction = LINEFOLD_ACTION_PROHIBITED;
    prevopp = linestart = i;
    for ( ; i < textlen; i++) {
      action = lbactions[i];
      if (action == LINEFOLD_ACTION_COMBINING_INDIRECT)
	action = LINEFOLD_ACTION_INDIRECT;

      if (action == LINEFOLD_ACTION_PROHIBITED ||
	  action == LINEFOLD_ACTION_COMBINING_PROHIBITED)
	/* Prohibited break. */
	continue;
      else if ((flags & LINEFOLD_OPTION_NOBREAK_DIRECT) &&
	       action == LINEFOLD_ACTION_DIRECT)
	/* Ommited Direct break */
	continue;
      else if ((*is_line_excess)(lbinfo, text, linestart, i-linestart+1,
				 maxlen, voidarg)) {
	/* Line has exceeded the limit. Search previous line breaking
	   oppotunity. */
	if (prevaction != LINEFOLD_ACTION_PROHIBITED) {
	  /* Previous oppotunity was found. */
	  i = prevopp;
	  action = prevaction;
	} else if (flags & LINEFOLD_OPTION_FORCE_LINEWIDTH &&
		   i > linestart) {
	  /* If FORCE_LINEWIDTH option was set on, force maxlen,
	     avoiding break before combining marks. */
	  while (i >= linestart) {
	    i--;
	    if (lbactions[i] != LINEFOLD_ACTION_COMBINING_PROHIBITED &&
		!(*is_line_excess)(lbinfo, text, linestart, i-linestart+1,
				   maxlen, voidarg))
	      break;
	  }
	  action = LINEFOLD_ACTION_DIRECT;
	} else if (i-linestart+1 > LINEFOLD_HARD_LIMIT) {
	  /* Try forcing hard limit. */
	  i = linestart + LINEFOLD_HARD_LIMIT - 1;
	  action = LINEFOLD_ACTION_DIRECT;
	}

	/* Write out a broken line. */
	if (writeout_cb != NULL)
	  (*writeout_cb)(lbinfo, text, linestart, i-linestart+1, action,
			 voidarg);
	/* Save line breaking action. */
	if (action == LINEFOLD_ACTION_DIRECT ||
	    (action == LINEFOLD_ACTION_INDIRECT &&
	     global_action != LINEFOLD_ACTION_DIRECT))
	  global_action = action;

	i++;
	break;
      } else if (action == LINEFOLD_ACTION_EXPLICIT ||
		 action == LINEFOLD_ACTION_EOT) {
	/* Explicit break or End of Text. */
	if (writeout_cb != NULL)
	  (*writeout_cb)(lbinfo, text, linestart, i-linestart+1, action,
			 voidarg);

        i++;
        break;
      } else {
	/* Save line breaking oppotunity. */
        prevopp = i;
	prevaction = action;
      }
    }
    /* update line indice */
    if (action == LINEFOLD_ACTION_EXPLICIT ||
	action == LINEFOLD_ACTION_EOT) {
      lbinfo->linp = 0;
      lbinfo->lint++;
      lbinfo->pint++;
    } else {
      lbinfo->linp++;
      lbinfo->lint++;
    }
  }

  return global_action;
}

/*
 * Private functions
 */

/* Internal default of function to find function that get properties of
   a character.  */
linefold_lbprop_funcptr
linefold_find_lbprop_func(const char *chset, linefold_flags flags)
{
  if (chset == NULL || flags & LINEFOLD_OPTION_GENERIC_WIDTH)
    return &linefold_getprop_generic;
  else if (charsetcmp(chset, "CNS")==0 || /* not a charset name. */
	   charsetcmp(chset, "BIG5")==0 ||
	   charsetcmp(chset, "BIG5-ETEN")==0 ||
	   charsetcmp(chset, "BIG5-HKSCS")==0 ||
	   charsetcmp(chset, "EUC-TW")==0 ||
	   charsetcmp(chset, "CP950")==0)
    return &linefold_getprop_C;
  else if (charsetcmp(chset, "GB")==0 || /* not a charset name */
	   charsetcmp(chset, "EUC-CN")==0 ||
	   charsetcmp(chset, "ISO-2022-CN")==0 ||
	   charsetcmp(chset, "ISO-2022-CN-EXT")==0 ||
	   charsetcmp(chset, "HZ")==0 ||
	   charsetcmp(chset, "HZ-GB-2312")==0 ||
	   charsetcmp(chset, "GB2312")==0 ||
	   charsetcmp(chset, "GBK")==0 ||
	   charsetcmp(chset, "GB18030")==0 ||
	   charsetcmp(chset, "CP936")==0)
    return &linefold_getprop_G;
  else if (charsetcmp(chset, "JIS")==0 || /* not a charset name */
	   charsetcmp(chset, "ISO-2022-JP")==0 ||
	   charsetcmp(chset, "ISO-2022-JP-1")==0 ||
	   charsetcmp(chset, "ISO-2022-JP-2")==0 ||
	   charsetcmp(chset, "ISO-2022-JP-3")==0 ||
	   charsetcmp(chset, "ISO-2022-JP-3-PLANE1")==0 ||
	   charsetcmp(chset, "ISO-2022-JP-2004")==0 ||
	   charsetcmp(chset, "ISO-2022-JP-2004-PLANE1")==0 ||
	   charsetcmp(chset, "EUC-JP")==0 ||
	   charsetcmp(chset, "EUC-JISX0213")==0 ||
	   charsetcmp(chset, "EUC-JISX0213-PLANE1")==0 ||
	   charsetcmp(chset, "EUC-JIS-2004")==0 ||
	   charsetcmp(chset, "EUC-JIS-2004-PLANE1")==0 ||
	   charsetcmp(chset, "SHIFT_JIS")==0 ||
	   charsetcmp(chset, "SHIFT_JISX0213")==0 ||
	   charsetcmp(chset, "SHIFT_JISX0213-PLANE1")==0 ||
	   charsetcmp(chset, "SHIFT_JIS-2004")==0 ||
	   charsetcmp(chset, "SHIFT_JIS-2004-PLANE1")==0 ||
	   charsetcmp(chset, "CP932")==0 ||
	   charsetcmp(chset, "CP942")==0 || /* IBM */
	   charsetcmp(chset, "CP943")==0) /* IBM */
    return &linefold_getprop_J;
  else if (charsetcmp(chset, "KS")==0 || /* not a charset name */
	   charsetcmp(chset, "ISO-2022-KR")==0 ||
	   charsetcmp(chset, "EUC-KR")==0 ||
	   charsetcmp(chset, "CP949")==0 ||
	   charsetcmp(chset, "UHC")==0 ||
	   charsetcmp(chset, "KS_C_5601-1987")==0)
    return &linefold_getprop_K;
  else
    return &linefold_getprop_generic;
}

static int
charsetcmp(const char *s1, const char *s2)
{
  unsigned char c1, c2;

  while ((c1 = (unsigned char)*s1) && (c2 = (unsigned char)*s2)) {
    while (c1 && !('0' <= c1 && c1 <= '9') &&
	   !('A' <= c1 && c1 <= 'Z') && !('a' <= c1 && c1 <= 'z'))
      c1 = (unsigned char)*(++s1);
    while (c2 && !('0' <= c2 && c2 <= '9') &&
	   !('A' <= c2 && c2 <= 'Z') && !('a' <= c2 && c2 <= 'z'))
      c2 = (unsigned char)*(++s2);
    if (!c1 || !c2)
      return c1 - c2;

    if (('a' <= c1 && c1 <= 'z'))
      c1 -= 'a'-'A';
    if ('a' <= c2 && c2 <= 'z')
      c2 -= 'a'-'A';
    if (c1 != c2) return c1 - c2;
    s1++;
    s2++;
  }
  return (unsigned char)*s1 - (unsigned char)*s2;
}


/* Internal default of functin to tailor character property. */
void
linefold_tailor_lbprop(linefold_char c,
		       linefold_width *widthptr, linefold_class *lbcptr,
		       linefold_flags flags)
{
  linefold_width width = *widthptr;
  linefold_class lbc = *lbcptr;

  if (flags & LINEFOLD_OPTION_NARROW_LATIN &&
      width == 2 &&
      (linefold_char)0x00C0 <= c && c <= (linefold_char)0x01FF &&
      (linefold_char)0x00D7 != c && (linefold_char)0x00F7 != c)
    width = 1;
  else if (flags & LINEFOLD_OPTION_NARROW_GREEK &&
	   width == 2 &&
	   (linefold_char)0x0370 <= c && c <= (linefold_char)0x03FF)
    width = 1;
  else if (flags & LINEFOLD_OPTION_NARROW_CYRILLIC &&
	   width == 2 &&
	   (linefold_char)0x0400 <= c && c <= (linefold_char)0x04FF)
    width = 1;

  if (lbc == LINEFOLD_CLASS_HY && /* HYPHEN-MINUS */
      !(flags & LINEFOLD_OPTION_BREAK_HY))
    lbc = LINEFOLD_CLASS_AL;

  if (c == (linefold_char)0x00AD && /* SOFT HYPHEN */
      !(flags & LINEFOLD_OPTION_BREAK_SOFT_HYPHEN))
    lbc = LINEFOLD_CLASS_GL;

  if (lbc == LINEFOLD_CLASS_NL) {
    if (flags & LINEFOLD_OPTION_NOBREAK_NL)
      lbc = LINEFOLD_CLASS_CM;
    else
      lbc = LINEFOLD_CLASS_BK;
  } else if (lbc == LINEFOLD_CLASS_CLH) {
    if (flags & LINEFOLD_OPTION_NOHUNG_PUNCT)
      lbc = LINEFOLD_CLASS_CL;
  } else if (lbc == LINEFOLD_CLASS_CLSP) {
    if (flags & LINEFOLD_OPTION_NOGLUE_PUNCT)
      lbc = LINEFOLD_CLASS_CL;
  } else if (lbc == LINEFOLD_CLASS_CLHSP) {
    if (flags & LINEFOLD_OPTION_NOGLUE_PUNCT) {
      if (flags & LINEFOLD_OPTION_NOHUNG_PUNCT)
	lbc = LINEFOLD_CLASS_CL;
      else
	lbc = LINEFOLD_CLASS_CLH;
    } else if (flags & LINEFOLD_OPTION_NOHUNG_PUNCT)
      lbc = LINEFOLD_CLASS_CLSP;
  } else if (lbc == LINEFOLD_CLASS_SPOP) {
    if (flags & LINEFOLD_OPTION_NOGLUE_PUNCT)
      lbc = LINEFOLD_CLASS_OP;
#ifdef LINEFOLD_CLASS_BKVT
  } else if (lbc == LINEFOLD_CLASS_BKVT) {
    if (flags & LINEFOLD_OPTION_NOBREAK_VT)
      lbc = LINEFOLD_CLASS_CM;
    else
      lbc = LINEFOLD_CLASS_BK;
#endif
#ifdef LINEFOLD_CLASS_BKFF
  } else if (lbc == LINEFOLD_CLASS_BKFF) {
    if (flags & LINEFOLD_OPTION_NOBREAK_FF)
      lbc = LINEFOLD_CLASS_CM;
    else
      lbc = LINEFOLD_CLASS_BK;
#endif
  } else if (lbc == LINEFOLD_CLASS_IDSP) {
    if (flags & LINEFOLD_OPTION_IDSP_IS_SP)
      lbc = LINEFOLD_CLASS_SP;
#ifdef LINEFOLD_CLASS_NSK
  } else if (lbc == LINEFOLD_CLASS_NSK) {
    if (flags & LINEFOLD_OPTION_RELAX_KANA_NS)
      lbc = LINEFOLD_CLASS_ID;
    else
      lbc = LINEFOLD_CLASS_NS;
#endif
#ifdef LINEFOLD_CLASS_OPAL
  } else if (lbc == LINEFOLD_CLASS_OPAL) {
    if (flags & LINEFOLD_OPTION_OPAL_IS_AL)
      lbc = LINEFOLD_CLASS_AL;
    else
      lbc = LINEFOLD_CLASS_OP;
#endif
#ifdef LINEFOLD_CLASS_INB2
  } else if (lbc == LINEFOLD_CLASS_INB2) {
    if (flags & LINEFOLD_OPTION_INB2_IS_B2)
      lbc = LINEFOLD_CLASS_B2;
    else
      lbc = LINEFOLD_CLASS_IN;
#endif
#ifdef LINEFOLD_CLASS_NSEX
  } else if (lbc == LINEFOLD_CLASS_NSEX) {
    if (flags & LINEFOLD_OPTION_NSEX_IS_EX)
      lbc = LINEFOLD_CLASS_EX;
    else
      lbc = LINEFOLD_CLASS_NS;
#endif
  }

  *widthptr = width;
  *lbcptr = lbc;
  return;
}

static size_t
find_linebreak(size_t textlen,
	       linefold_class *lbclasses, linefold_action *lbactions,
	       linefold_flags flags)
{
  linefold_class before; /* class of 'before' character */
  linefold_class after;
  linefold_action action;
  size_t idx = 0;
  size_t linestart;

  if (textlen == 0)
    return 0;

  while (idx < textlen) {
    /* Loop on one line ending with Explicit Break or end of text. */
    linestart = idx;
    before = lbclasses[idx];
    idx++;
    for (; idx < textlen &&
	   before != LINEFOLD_CLASS_BK &&
	   before != LINEFOLD_CLASS_LF &&
	   before != LINEFOLD_CLASS_NL &&
	   (before != LINEFOLD_CLASS_CR ||
	    lbclasses[idx] == LINEFOLD_CLASS_LF);
	 idx++) {
      after = lbclasses[idx];

      /* Explicit Breaks */
      if (after == LINEFOLD_CLASS_BK ||
	  after == LINEFOLD_CLASS_LF ||
	  after == LINEFOLD_CLASS_NL) {
	/* BK, LF, NL */
	lbactions[idx-1] = LINEFOLD_ACTION_PROHIBITED;
	before = after;
	continue;
      } else if (after == LINEFOLD_CLASS_CR) {
	/* CR */
	lbactions[idx-1] = LINEFOLD_ACTION_PROHIBITED;
	before = after;
	continue;
      }
      /* Space */
      else if (after == LINEFOLD_CLASS_SP) {
	/* SP */
	lbactions[idx-1] = LINEFOLD_ACTION_PROHIBITED;
	/* do not update class of 'before' character */
	continue;
      }
      /* In case line starts with Space(s) */
      if (before == LINEFOLD_CLASS_SP) {
	lbactions[idx-1] = LINEFOLD_ACTION_PROHIBITED;
	before = after;
	continue;
      }

      /* Lookup pair table */
      assert(before < LINEFOLD_CLASS_TABLESIZE);
      assert(after < LINEFOLD_CLASS_TABLESIZE);
      action = linefold_lbpairs[before][after];
      /* Save line breaking action */
      lbactions[idx-1] = action;

      /* Direct Break */
      if (action == LINEFOLD_ACTION_DIRECT) {
	if (lbclasses[idx-1] == LINEFOLD_CLASS_SP)
	  lbactions[idx-1] = LINEFOLD_ACTION_INDIRECT;
      } 
      /* Indirect Break */
      else if (action == LINEFOLD_ACTION_INDIRECT) {
	if (lbclasses[idx-1] == LINEFOLD_CLASS_SP)
	  lbactions[idx-1] = LINEFOLD_ACTION_INDIRECT;
	else
	  lbactions[idx-1] = LINEFOLD_ACTION_PROHIBITED;
      } 
      /* Indirect Break after combining mark(s) */
      else if (action == LINEFOLD_ACTION_COMBINING_INDIRECT) {
	lbactions[idx-1] = LINEFOLD_ACTION_PROHIBITED;
	if (lbclasses[idx-1] == LINEFOLD_CLASS_SP) {
	  if (flags & LINEFOLD_OPTION_BREAK_SPCM)
	    lbactions[idx-1] = LINEFOLD_ACTION_COMBINING_INDIRECT;
	  else {
	    lbactions[idx-1] = LINEFOLD_ACTION_PROHIBITED;
	    if (idx - linestart > 1) {
	      if (lbclasses[idx-2] == LINEFOLD_CLASS_SP)
		lbactions[idx-2] = LINEFOLD_ACTION_INDIRECT;
	      else
		lbactions[idx-2] = LINEFOLD_ACTION_DIRECT;
	    }
	  }
	}
	else
	  continue;
      }
      /* Prohibited Break with combining mark(s) */
      else if (action == LINEFOLD_ACTION_COMBINING_PROHIBITED) {
	lbactions[idx-1] = action;
	if (lbclasses[idx-1] != LINEFOLD_CLASS_SP)
	  continue;
      } 

      /* Save class of 'before' character */
      before = lbclasses[idx];
    } /* for () */
      /* Always break at end of line. */
    lbactions[idx-1] = LINEFOLD_ACTION_EXPLICIT;
  }
  /* End of text */
  lbactions[textlen-1] = LINEFOLD_ACTION_EOT;
    
  return idx;
}

/* Internal default of function to check if length of a line exceeds
   limit. */
int
linefold_is_line_excess(const struct linefold_info *lbinfo,
			const linefold_char *text,
			size_t start, size_t len, size_t maxlen,
			void *voidarg)
{
  size_t end = start + len;
  const linefold_width *widths = lbinfo->widths;
  const linefold_class *lbclasses = lbinfo->lbclasses;
  linefold_flags flags = lbinfo->flags;
  size_t i, length = 0, real_length = 0;

  for (i=start; i < end; i++) {
    linefold_class lbc = lbclasses[i];
    if (lbc == LINEFOLD_CLASS_SP ||
	lbc == LINEFOLD_CLASS_BK ||
	lbc == LINEFOLD_CLASS_CR ||
	lbc == LINEFOLD_CLASS_LF ||
	lbc == LINEFOLD_CLASS_NL) {
      /* skip */;

      /* Convention for Hangul combining jamo.
       * choseong+jungseong or choseong+jungseong+jongseong
       * is single Wide character.
       */
    } else if (lbc == LINEFOLD_CLASS_JV) {
      if (!(flags & LINEFOLD_OPTION_NOCOMBINE_HANGUL_JAMO) &&
	  i >= start+1 && lbclasses[i-1] == LINEFOLD_CLASS_JL)
	real_length -= widths[i];
      else
	length += widths[i];

    } else if (lbc == LINEFOLD_CLASS_JT) {
      if (!(flags & LINEFOLD_OPTION_NOCOMBINE_HANGUL_JAMO) &&
	  i >= start+2 && lbclasses[i-2] == LINEFOLD_CLASS_JL &&
	  lbclasses[i-1] == LINEFOLD_CLASS_JV)
	real_length -= widths[i];
      else
	length += widths[i];

    } else if (lbc == LINEFOLD_CLASS_CLH ||
	       lbc == LINEFOLD_CLASS_CLHSP ||
	       (lbc == LINEFOLD_CLASS_IDSP &&
		!(flags & LINEFOLD_OPTION_NOHUNG_IDSP))) {
      if (real_length > maxlen)
	length = real_length + widths[i];

    } else if (lbc == LINEFOLD_CLASS_CLSP) {
      if (real_length > maxlen - (widths[i] - 1))
	length = real_length + widths[i];
      else
	length += widths[i]-1;

    } else {
      length = real_length + widths[i];
    }
    real_length += widths[i];

    if (length > maxlen ||
	(LINEFOLD_HARD_LIMIT > 0 &&
	 real_length >= LINEFOLD_HARD_LIMIT))
      return 1;
  }
  return 0;
}
