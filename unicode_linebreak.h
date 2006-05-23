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

#ifndef UNICODE_LINEBREAK_H
#define UNICODE_LINEBREAK_H

#include "unicode.h"
#include "unicode_linebreaktab.h"

/*
 * Line Breaking Actions.
 * [*] may be feed to callback funtion by unicode_linebreak().
 */
enum unicode_lbaction {
	UNICODE_LBACTION_PROHIBITED=0,         /* Prohibited Break */
	UNICODE_LBACTION_COMBINING_PROHIBITED, /* Prohibited Break for CM */
	UNICODE_LBACTION_COMBINING_INDIRECT,   /* Indirect Break for CM */
	UNICODE_LBACTION_INDIRECT,             /* Indirect Break [*] */
	UNICODE_LBACTION_DIRECT,               /* Direct Break [*] */
	UNICODE_LBACTION_EXPLICIT,             /* Explicit Break [*] */
	UNICODE_LBACTION_EOT                   /* End of Text [*] */
};
#define UNICODE_LBACTION_NOMOD UNICODE_LBACTION_PROHIBITED

extern const enum unicode_lbaction *unicode_lbpairs[];

/*
 * Character properties related on line breaking behavior.
 */
struct unicode_lbprop
{
	unicode_char    range_s;
	unicode_char    range_e;
	int             width;        /* UAX#11 East Asian Width */
	enum unicode_lbclass lbclass; /* UAX#14 Line Breaking Class */
};

typedef struct unicode_lbprop *(*unicode_lbprop_funcptr)(unicode_char);

/*
 * Data required by line breaking algorithm.
 */
struct unicode_lbinfo
{
  int *widths;                      /* character widths */
  enum unicode_lbclass *lbclasses;  /* line breaking classes */
  enum unicode_lbaction *lbactions; /* probable line breaking actions */
  char *charset;                    /* charset context */
  size_t length;                    /* length of text */
  int flags;                        /* options to tailor line breaking
                                       behavior */
};

/*
 * Public functions.
 */
extern struct unicode_lbinfo
*unicode_linebreak_alloc(unicode_char *, size_t,
			 unicode_lbprop_funcptr(*)(const char *, int),
			 void (*)(unicode_char, int *, enum unicode_lbclass *,
				  int),
			 const char *, int);

extern void unicode_linebreak_free(struct unicode_lbinfo *);

extern enum unicode_lbaction
unicode_linebreak(struct unicode_lbinfo *, unicode_char *,
		  int (*)(struct unicode_lbinfo *, unicode_char *,
			  off_t, size_t, int, void *),
		  void (*)(struct unicode_lbinfo *, unicode_char *,
			   off_t, size_t, enum unicode_lbaction, void *),
		  int, void *);

/* Built-in support functions */
extern unicode_lbprop_funcptr
unicode_linebreak_find_lbprop_func(const char *, int);
extern void unicode_linebreak_tailor_lbprop(unicode_char,
					    int *, enum unicode_lbclass *,
					    int);
extern int unicode_linebreak_check_length(struct unicode_lbinfo *,
					  unicode_char *,
					  off_t, size_t, int, void* voidarg);

/*
 * Options to tailor line breaking behavior.
 */

/*
 * Resolve character width within generic (Unicode) context, even if
 * character set context was specified.
 */
#define UNICODE_LBOPTION_GENERIC_WIDTH           1

/*
 * By default, in multibyte character set context, several Latin, Greek
 * or Cyrillic letters are treated as Wide.  In some display
 * enviroments they are usually rendered as Narrow, even if in
 * multibyte context.  Set these options to treat such letters always
 * Narrow.
 */
#define UNICODE_LBOPTION_NARROW_LATIN            (1<<1)
#define UNICODE_LBOPTION_NARROW_GREEK            (1<<2)
#define UNICODE_LBOPTION_NARROW_CYRILLIC         (1<<3)

/*
 * Set this to allow break after HY (U+002D HYPHEN-MINUS), before AL.
 * By default, breaking directly before AL is not allowed.
 */
#define UNICODE_LBOPTION_BREAK_HY                (1<<4)

/*
 * Set this to allow break after U+00AD SOFT HYPHEN.  By default, breaking
 * directly after soft hyphen is not allowed.
 */
#define UNICODE_LBOPTION_BREAK_SOFT_HYPHEN       (1<<5)

/*
 * Set this to treat U+0085 NEL as CM, not BK. [UAX#14]
 */
#define UNICODE_LBOPTION_NOBREAK_NL              (1<<6)

/*
 * Set this to treat U+000B vertical tab as CM, not BK. [UAX#14]
 */
#define UNICODE_LBOPTION_NOBREAK_VT              (1<<7)

/*
 * Set this to disable "burasage" (hunging punctuation in East Asian
 * text).
 */
#define UNICODE_LBOPTION_NOHUNG_PUNCT            (1<<8)

/*
 * By default, Wide OP/CL characters are treated as if they are a
 * Narrow character preceded/followed by SP.  Set this to disable
 * such feature.
 */
#define UNICODE_LBOPTION_NOGLUE_PUNCT            (1<<9)

/*
 * By default, IDEOGRAPHIC SPACE is treated as ID.  Set this to treat
 * it as SP.
 */
#define UNICODE_LBOPTION_IDSP_IS_SP              (1<<10)

/*
 * By default, small hiragana/katakana and KATAKANA-HIRAGANA PROLONGED
 * SIGN are NS.  Set this to treat these characters as ID. [JIS X 4051]
 */
#define UNICODE_LBOPTION_RELAX_KANA_NS           (1<<11)

/*
 * Set this to treat spanish quotations INVERTED EXCLAMATION MARK and
 * INVERTED QUESTION MARK as AL, not OP. [UAX#14]
 */
#define UNICODE_LBOPTION_OPAL_IS_AL              (1<<12)

/*
 * Set this option to ommit Direct break oppotunities.
 * WARNING: This may fold non-alphabetic (or mixed) texts *very* ugrily.
 */
#define UNICODE_LBOPTION_NOBREAK_DIRECT          (1<<13)

/*
 * Set this option not to combine hangul combining jamos (JT, JV, JL)
 * when line lengths are determined.
 */
#define UNICODE_LBOPTION_NOCOMBINE_HANGUL_JAMO   (1<<14)

/*
 * Set this to break SP+CM sequence. [UAX#14]
 */
#define UNICODE_LBOPTION_BREAK_SPCM              (1<<15)

/* 
 * Set this option to force break unbreakable text longer than specified
 + width.  By default, longer text won't be broken.
 * This violate line breaking rules then may cause break within words,
 * syllables, characters or on somewhere unpreferred.
 */
#define UNICODE_LBOPTION_FORCE_LINEWIDTH         (1<<16)

#define UNICODE_LBOPTION_DEFAULT                 0

/*
 * Hard length limit of unboken squence.
 * Note:
 *   Following value is the maximum number of octet sequences consisting
 *   character, in 1000 octets terminated by CRLF.  Because, in `legacy'
 *   encodings single character can be assumed occupying 5 octets or less
 *   (6 octets is very rare).
 */
#define UNICODE_LINEBREAK_HARD_LIMIT             199

#endif /* UNICODE_LINEBREAK_H */
