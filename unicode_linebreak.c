/*
 * An implementation of line breaking algorithm described in
 * Unicode Standard Annex #14 (UAX#14).
 *
 * Copyright (C) 2006 by Hatuka*nezumi - IKEDA Soji <hatuka(at)nezumi.nu>,
 * redistributed under GNU General Public License version 2 (or later
 * version you prefer).
 * 
 * $Id$
 */

#include "unicode_linebreak.h"
#include <assert.h>
#include <string.h>

static size_t find_linebreak(size_t, enum unicode_lbclass *,
			     enum unicode_lbaction *, int);

/*
 * Public Functions
 */

/* Allocate line break informations. */
struct unicode_lbinfo
*unicode_linebreak_alloc(unicode_char *text, size_t textlen,
			 unicode_lbprop_funcptr
			 (*find_lbprop_func)(const char *, int),
			 void (*tailor_lbprop)(unicode_char,
					       int *, enum unicode_lbclass *,
					       int),
			 const char *chset, int flags)
{
  struct unicode_lbinfo *lbinfo;
  int *widths;
  enum unicode_lbclass *lbclasses;
  enum unicode_lbaction *lbactions;
  char *charset=NULL;
  off_t i;
  unicode_lbprop_funcptr lbprop_func;

  if (text == NULL || textlen == 0)
    return NULL;

  if ((widths = malloc(sizeof(int) * textlen)) == NULL) {
    return NULL;
  }
  if ((lbclasses = malloc(sizeof(enum unicode_lbclass) * textlen)) == NULL) {
    free(widths);
    return NULL;
  }
  if ((lbactions = malloc(sizeof(enum unicode_lbaction) * textlen)) == NULL) {
    free(widths);
    free(lbclasses);
    return NULL;
  }
  if (chset && (charset = strdup(chset)) == NULL) {
    free(widths);
    free(lbclasses);
    free(lbactions);
    return NULL;
  }
  if ((lbinfo = malloc(sizeof(struct unicode_lbinfo))) == NULL) {
    free(widths);
    free(lbclasses);
    free(lbactions);
    if (charset) free(charset);
    return NULL;
  }

  if (find_lbprop_func == NULL)
    find_lbprop_func = &unicode_linebreak_find_lbprop_func;
  if (tailor_lbprop == NULL)
    tailor_lbprop = &unicode_linebreak_tailor_lbprop;

  lbprop_func = (*find_lbprop_func)(charset, flags);
  for (i=0; i < textlen; i++) {
    struct unicode_lbprop *lbprop = (*lbprop_func)(text[i]);
    widths[i] = lbprop->width;
    lbclasses[i] = lbprop->lbclass;
    (*tailor_lbprop)(text[i], widths+i, lbclasses+i, flags);
    lbactions[i] = UNICODE_LBACTION_PROHIBITED;
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
  return lbinfo;
}

/* Free storage of line break informations. */
void unicode_linebreak_free(struct unicode_lbinfo *lbinfo)
{
  if (lbinfo == NULL)
    return;

  free(lbinfo->widths);
  free(lbinfo->lbclasses);
  free(lbinfo->lbactions);
  if (lbinfo->charset) free(lbinfo->charset);
  free(lbinfo);
}

/* Do line breaking */
enum unicode_lbaction
unicode_linebreak(struct unicode_lbinfo *lbinfo, unicode_char *text,
		  int (*is_line_exceeded)(struct unicode_lbinfo *,
					  unicode_char *,
					  off_t, size_t, int, void *),
		  void (*writeout_cb)(struct unicode_lbinfo *,
				      unicode_char *,
				      off_t, size_t, enum unicode_lbaction,
				      void *),
		  int maxlen, void *voidarg)
{
  size_t textlen;
  enum unicode_lbaction *lbactions;
  int flags;
  enum unicode_lbaction global_action=UNICODE_LBACTION_NOMOD,
    action, prevaction;
  off_t i=0, linestart, prevopp;

  if (lbinfo == NULL)
    return UNICODE_LBACTION_NOMOD;
  textlen = lbinfo->length;
  lbactions = lbinfo->lbactions;
  flags = lbinfo->flags;

  if (is_line_exceeded == NULL)
    is_line_exceeded = &unicode_linebreak_check_length;

  while (i < textlen) {
    prevaction = UNICODE_LBACTION_PROHIBITED;
    prevopp = linestart = i;
    for ( ; i < textlen; i++) {
      action = lbactions[i];
      if (action == UNICODE_LBACTION_COMBINING_INDIRECT)
	action = UNICODE_LBACTION_INDIRECT;

      if (action == UNICODE_LBACTION_PROHIBITED ||
	  action == UNICODE_LBACTION_COMBINING_PROHIBITED)
	/* Prohibited break. */
	continue;
      else if ((flags & UNICODE_LBOPTION_NOBREAK_DIRECT) &&
	       action == UNICODE_LBACTION_DIRECT)
	/* Ommited Direct break */
	continue;
      else if ((*is_line_exceeded)(lbinfo, text, linestart, i-linestart+1,
				   maxlen, voidarg)) {
	/* Line has exceeded the limit. Search previous line breaking
	   oppotunity. */
	if (prevaction != UNICODE_LBACTION_PROHIBITED) {
	  /* Previous oppotunity was found. */
	  i = prevopp;
	  action = prevaction;
	} else if (flags & UNICODE_LBOPTION_FORCE_LINEWIDTH &&
		   i > linestart) {
	  /* If FORCE_LINEWIDTH option was set on, force maxlen,
	     avoiding break before combining marks. */
	  while (i >= linestart) {
	    i--;
	    if (lbactions[i] != UNICODE_LBACTION_COMBINING_PROHIBITED &&
		!(*is_line_exceeded)(lbinfo, text, linestart, i-linestart+1,
				     maxlen, voidarg))
	      break;
	  }
	  action = UNICODE_LBACTION_DIRECT;
	} else if (i-linestart+1 > UNICODE_LINEBREAK_HARD_LIMIT) {
	  /* Try forcing hard limit. */
	  i = linestart + UNICODE_LINEBREAK_HARD_LIMIT - 1;
	  action = UNICODE_LBACTION_DIRECT;
	}

	/* Write out a broken line. */
	if (writeout_cb != NULL)
	  (*writeout_cb)(lbinfo, text, linestart, i-linestart+1, action,
			 voidarg);
	/* Save line breaking action. */
	if (action == UNICODE_LBACTION_DIRECT ||
	    (action == UNICODE_LBACTION_INDIRECT &&
	     global_action != UNICODE_LBACTION_DIRECT))
	  global_action = action;

	i++;
	break;
      } else if (action == UNICODE_LBACTION_EXPLICIT ||
		 action == UNICODE_LBACTION_EOT) {
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
  }

  return global_action;
}

/*
 * Private functions
 */

/* Internal default of function to find function that get properties of
   a character.  */
unicode_lbprop_funcptr unicode_linebreak_find_lbprop_func(const char *chset,
							  int flags)
{
  if (chset == NULL || flags & UNICODE_LBOPTION_GENERIC_WIDTH)
    return &unicode_getprop_generic;
  else if (strcasecmp(chset, "BIG5")==0 ||
	   strcasecmp(chset, "BIG5-HKSCS")==0 ||
	   strcasecmp(chset, "EUC-TW")==0)
    return &unicode_getprop_C;
  else if (strcasecmp(chset, "EUC-CN")==0 ||
	   strcasecmp(chset, "ISO-2022-CN")==0 ||
	   strcasecmp(chset, "ISO-2022-CN-EXT")==0 ||
	   strcasecmp(chset, "GB2312")==0 ||
	   strcasecmp(chset, "GB18030")==0 ||
	   strcasecmp(chset, "CP936")==0)
    return &unicode_getprop_G;
  else if (strcasecmp(chset, "ISO-2022-JP")==0 ||
	   strcasecmp(chset, "ISO-2022-JP-1")==0 ||
	   strcasecmp(chset, "ISO-2022-JP-2")==0 ||
	   strcasecmp(chset, "ISO-2022-JP-3")==0 ||
	   strcasecmp(chset, "ISO-2022-JP-3-PLANE1")==0 ||
	   strcasecmp(chset, "ISO-2022-JP-2004")==0 ||
	   strcasecmp(chset, "ISO-2022-JP-2004-PLANE1")==0 ||
	   strcasecmp(chset, "EUC-JP")==0 ||
	   strcasecmp(chset, "EUC-JISX0213")==0 ||
	   strcasecmp(chset, "EUC-JISX0213-PLANE1")==0 ||
	   strcasecmp(chset, "EUC-JIS-2004")==0 ||
	   strcasecmp(chset, "EUC-JIS-2004-PLANE1")==0 ||
	   strcasecmp(chset, "SHIFT_JIS")==0 ||
	   strcasecmp(chset, "SHIFT_JISX0213")==0 ||
	   strcasecmp(chset, "SHIFT_JISX0213-PLANE1")==0 ||
	   strcasecmp(chset, "SHIFT_JIS-2004")==0 ||
	   strcasecmp(chset, "SHIFT_JIS-2004-PLANE1")==0 ||
	   strcasecmp(chset, "CP932")==0)
    return &unicode_getprop_J;
  else if (strcasecmp(chset, "ISO-2022-KR")==0 ||
	   strcasecmp(chset, "EUC-KR")==0 ||
	   strcasecmp(chset, "CP949")==0 ||
	   strcasecmp(chset, "KS_C_5601-1987")==0)
    return &unicode_getprop_K;
  else
    return &unicode_getprop_generic;
}

/* Internal default of functin to tailor character property. */
void unicode_linebreak_tailor_lbprop(unicode_char c,
				    int *widthptr,
				    enum unicode_lbclass *lbcptr,
				    int flags)
{
  int width = *widthptr;
  enum unicode_lbclass lbc = *lbcptr;

  if (flags & UNICODE_LBOPTION_NARROW_LATIN &&
      width == 2 &&
      (unicode_char)0x00C0 <= c && c <= (unicode_char)0x01FF &&
      (unicode_char)0x00D7 != c && (unicode_char)0x00F7 != c)
    width = 1;
  else if (flags & UNICODE_LBOPTION_NARROW_GREEK &&
      width == 2 &&
      (unicode_char)0x0370 <= c && c <= (unicode_char)0x03FF)
    width = 1;
  else if (flags & UNICODE_LBOPTION_NARROW_CYRILLIC &&
      width == 2 &&
      (unicode_char)0x0400 <= c && c <= (unicode_char)0x04FF)
    width = 1;

#ifdef UNICODE_LBCLASS_HY_DEFINED
  if (lbc == UNICODE_LBCLASS_HY && /* HYPHEN-MINUS */
      !(flags & UNICODE_LBOPTION_BREAK_HY))
    lbc = UNICODE_LBCLASS_AL;
#endif

#ifdef UNICODE_LBCLASS_GL_DEFINED
  if (c == (unicode_char)0x00AD && /* SOFT HYPHEN */
      !(flags & UNICODE_LBOPTION_BREAK_SOFT_HYPHEN))
    lbc = UNICODE_LBCLASS_GL;
#endif

  switch ((int)lbc) {
  case UNICODE_LBCLASS_NL:
    if (flags & UNICODE_LBOPTION_NOBREAK_NL)
      lbc = UNICODE_LBCLASS_CM;
    else
      lbc = UNICODE_LBCLASS_BK;
    break;

#ifdef UNICODE_LBCLASS_CLH_DEFINED
#ifdef UNICODE_LBCLASS_CL_DEFINED
  case UNICODE_LBCLASS_CLH:
    if (flags & UNICODE_LBOPTION_NOHUNG_PUNCT)
      lbc = UNICODE_LBCLASS_CL;
    break;
#endif
#endif

#ifdef UNICODE_LBCLASS_CLSP_DEFINED
#ifdef UNICODE_LBCLASS_CL_DEFINED
  case UNICODE_LBCLASS_CLSP:
    if (flags & UNICODE_LBOPTION_NOGLUE_PUNCT)
      lbc = UNICODE_LBCLASS_CL;
    break;
#endif
#endif

#ifdef UNICODE_LBCLASS_CLHSP_DEFINED
#ifdef UNICODE_LBCLASS_CL_DEFINED
  case UNICODE_LBCLASS_CLHSP:
    if (flags & UNICODE_LBOPTION_NOGLUE_PUNCT) {
#  ifdef UNICODE_LBCLASS_CLH_DEFINED
      if (flags & UNICODE_LBOPTION_NOHUNG_PUNCT)
	lbc = UNICODE_LBCLASS_CL;
      else
	lbc = UNICODE_LBCLASS_CLH;
#  else
      lbc = UNICODE_LBCLASS_CL;
#  endif
    } else if (flags & UNICODE_LBOPTION_NOHUNG_PUNCT)
#  ifdef UNICODE_LBCLASS_CLSP_DEFINED
      lbc = UNICODE_LBCLASS_CLSP;
#  else
      lbc = UNICODE_LBCLASS_CL;
#  endif
    break;
#endif
#endif

#ifdef UNICODE_LBCLASS_SPOP_DEFINED
#ifdef UNICODE_LBCLASS_OP_DEFINED
  case UNICODE_LBCLASS_SPOP:
    if (flags & UNICODE_LBOPTION_NOGLUE_PUNCT)
      lbc = UNICODE_LBCLASS_OP;
    break;
#endif
#endif

#ifdef UNICODE_LBCLASS_BKO_DEFINED
#ifdef UNICODE_LBCLASS_CM_DEFINED
  case UNICODE_LBCLASS_BKO:
    if (flags & UNICODE_LBOPTION_NOBREAK_VT)
      lbc = UNICODE_LBCLASS_CM;
    else
      lbc = UNICODE_LBCLASS_BK;
    break;
#endif
#endif

#ifdef UNICODE_LBCLASS_IDSP_DEFINED
#ifdef UNICODE_LBCLASS_ID_DEFINED
  case UNICODE_LBCLASS_IDSP:
    if (flags & UNICODE_LBOPTION_IDSP_IS_SP)
      lbc = UNICODE_LBCLASS_SP;
    else
      lbc = UNICODE_LBCLASS_ID;
    break;
#endif
#endif

#ifdef UNICODE_LBCLASS_NSK_DEFINED
#ifdef UNICODE_LBCLASS_ID_DEFINED
#ifdef UNICODE_LBCLASS_NS_DEFINED
  case UNICODE_LBCLASS_NSK:
    if (flags & UNICODE_LBOPTION_RELAX_KANA_NS)
      lbc = UNICODE_LBCLASS_ID;
    else
      lbc = UNICODE_LBCLASS_NS;
    break;
#endif
#endif
#endif

#ifdef UNICODE_LBCLASS_OPAL_DEFINED
  case UNICODE_LBCLASS_OPAL:
    if (flags & UNICODE_LBOPTION_OPAL_IS_AL)
      lbc = UNICODE_LBCLASS_AL;
    else
      lbc = UNICODE_LBCLASS_OP;
    break;
#endif
  }

  *widthptr = width;
  *lbcptr = lbc;
  return;
}

static size_t find_linebreak(size_t textlen,
			     enum unicode_lbclass *lbclasses,
			     enum unicode_lbaction *lbactions,
			     int flags)
{
  enum unicode_lbclass before; /* class of 'before' character */
  enum unicode_lbclass after;
  enum unicode_lbaction action;
  off_t idx = 0;
  off_t linestart;

  if (textlen == 0)
    return 0;

  while (idx < textlen) {
    /* Loop on one line ending with Explicit Break or end of text. */
    linestart = idx;
    before = lbclasses[idx];
    idx++;
    for (; idx < textlen &&
	   before != UNICODE_LBCLASS_BK &&
	   before != UNICODE_LBCLASS_LF &&
	   before != UNICODE_LBCLASS_NL &&
	   (before != UNICODE_LBCLASS_CR ||
	    lbclasses[idx] == UNICODE_LBCLASS_LF);
	 idx++) {
      after = lbclasses[idx];

      /* Explicit Breaks */
      if (after == UNICODE_LBCLASS_BK ||
	  after == UNICODE_LBCLASS_LF ||
	  after == UNICODE_LBCLASS_NL) {
	/* BK, LF, NL */
	lbactions[idx-1] = UNICODE_LBACTION_PROHIBITED;
	before = after;
	continue;
      } else if (after == UNICODE_LBCLASS_CR) {
	/* CR */
	lbactions[idx-1] = UNICODE_LBACTION_PROHIBITED;
	before = after;
	continue;
      }
      /* Space */
      else if (after == UNICODE_LBCLASS_SP) {
	/* SP */
	lbactions[idx-1] = UNICODE_LBACTION_PROHIBITED;
	/* do not update class of 'before' character */
	continue;
      }
      /* In case line starts with Space(s) */
      if (before == UNICODE_LBCLASS_SP) {
	lbactions[idx-1] = UNICODE_LBACTION_PROHIBITED;
	before = after;
	continue;
      }

      /* Lookup pair table */
      assert(before < UNICODE_LBCLASS_TABLESIZE);
      assert(after < UNICODE_LBCLASS_TABLESIZE);
      action = unicode_lbpairs[before][after];
      /* Save line breaking action */
      lbactions[idx-1] = action;

      /* Direct Break */
      if (action == UNICODE_LBACTION_DIRECT) {
	if (lbclasses[idx-1] == UNICODE_LBCLASS_SP)
	  lbactions[idx-1] = UNICODE_LBACTION_INDIRECT;
      } 
      /* Indirect Break */
      else if (action == UNICODE_LBACTION_INDIRECT) {
	if (lbclasses[idx-1] == UNICODE_LBCLASS_SP)
	  lbactions[idx-1] = UNICODE_LBACTION_INDIRECT;
	else
	  lbactions[idx-1] = UNICODE_LBACTION_PROHIBITED;
      } 
      /* Indirect Break after combining mark(s) */
      else if (action == UNICODE_LBACTION_COMBINING_INDIRECT) {
	lbactions[idx-1] = UNICODE_LBACTION_PROHIBITED;
	if (lbclasses[idx-1] == UNICODE_LBCLASS_SP) {
	  if (flags & UNICODE_LBOPTION_BREAK_SPCM)
	    lbactions[idx-1] = UNICODE_LBACTION_COMBINING_INDIRECT;
	  else {
	    lbactions[idx-1] = UNICODE_LBACTION_PROHIBITED;
	    if (idx - linestart > 1) {
	      if (lbclasses[idx-2] == UNICODE_LBCLASS_SP)
		lbactions[idx-2] = UNICODE_LBACTION_INDIRECT;
	      else
		lbactions[idx-2] = UNICODE_LBACTION_DIRECT;
	    }
	  }
	}
	else
	  continue;
      }
      /* Prohibited Break with combining mark(s) */
      else if (action == UNICODE_LBACTION_COMBINING_PROHIBITED) {
	lbactions[idx-1] = action;
	if (lbclasses[idx-1] != UNICODE_LBCLASS_SP)
	  continue;
      } 

      /* Save class of 'before' character */
      before = lbclasses[idx];
    } /* for () */
      /* Always break at end of line. */
    lbactions[idx-1] = UNICODE_LBACTION_EXPLICIT;
  }
  /* End of text */
  lbactions[textlen-1] = UNICODE_LBACTION_EOT;
    
  return idx;
}

/* Internal default of function to check if length of a line exceeds
   limit. */
int unicode_linebreak_check_length(struct unicode_lbinfo *lbinfo,
				   unicode_char *text,
				   off_t start, size_t len, int maxlen,
				   void *voidarg)
{
  off_t end = start + len;
  int *widths = lbinfo->widths;
  enum unicode_lbclass *lbclasses = lbinfo->lbclasses;
  int flags = lbinfo->flags;

  off_t i;
  int length = 0;
  int real_length = 0;

  for (i=start; i < end; i++)
    {
      switch (lbclasses[i])
	{
	case UNICODE_LBCLASS_SP:
	case UNICODE_LBCLASS_BK:
	case UNICODE_LBCLASS_CR:
	case UNICODE_LBCLASS_LF:
	case UNICODE_LBCLASS_NL:
	  break;

#ifdef UNICODE_LBCLASS_JL_DEFINED
#ifdef UNICODE_LBCLASS_JV_DEFINED
#ifdef UNICODE_LBCLASS_JT_DEFINED
	/* Convention for Hangul combining jamo.
	 * choseong+jungseong or choseong+jungseong+jongseong
	 * is single Wide character.
	 */
	case UNICODE_LBCLASS_JV:
	  if (!(flags & UNICODE_LBOPTION_NOCOMBINE_HANGUL_JAMO) &&
	      i >= start+1 && lbclasses[i-1] == UNICODE_LBCLASS_JL)
	    real_length -= widths[i];
	  else
	    length += widths[i];
	  break;

	case UNICODE_LBCLASS_JT:
	  if (!(flags & UNICODE_LBOPTION_NOCOMBINE_HANGUL_JAMO) &&
	      i >= start+2 && lbclasses[i-2] == UNICODE_LBCLASS_JL &&
	      lbclasses[i-1] == UNICODE_LBCLASS_JV)
	    real_length -= widths[i];
	  else
	    length += widths[i];
	  break;
#endif
#endif
#endif

#if defined(UNICODE_LBCLASS_CLH_DEFINED) || defined(UNICODE_LBCLASS_CLHSP_DEFINED)
#  ifdef UNICODE_LBCLASS_CLH_DEFINED
	case UNICODE_LBCLASS_CLH:
#  endif
#  ifdef UNICODE_LBCLASS_CLHSP_DEFINED
	case UNICODE_LBCLASS_CLHSP:
#  endif
	  if (real_length > maxlen)
	    length = real_length + widths[i];
	  break;
#endif

#ifdef UNICODE_LBCLASS_CLSP_DEFINED
	case UNICODE_LBCLASS_CLSP:
	  if (real_length > maxlen - (widths[i] - 1))
	    length = real_length + widths[i];
	  else
	    length += widths[i]-1;
	  break;
#endif

	default:
	  length = real_length + widths[i];
	}
      real_length += widths[i];

      if (length > maxlen ||
	  (UNICODE_LINEBREAK_HARD_LIMIT > 0 &&
	   real_length >= UNICODE_LINEBREAK_HARD_LIMIT))
	return 1;
    }
  return 0;
}
