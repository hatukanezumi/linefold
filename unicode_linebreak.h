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
 * Options to tailor line breaking behavior.
 */

/*
 * Resolve character width within generic (Unicode) context, even if
 * character set context is specified.
 */
#define UNICODE_LBOPTION_GENERIC_WIDTH           1

/*
 * By default, in multibyte character set context, several Latin, Greek
 * or Cyrillic letters are treated as Wide.  In some display
 * enviroments they are usually rendered as Narrow, even if in
 * multibyte context.  Set these options to treat such letters always
 * Narrow.
 */
#define UNICODE_LBOPTION_ALWAYS_NARROW_LATIN     2
#define UNICODE_LBOPTION_ALWAYS_NARROW_GREEK     4
#define UNICODE_LBOPTION_ALWAYS_NARROW_CYRILLIC  8

/*
 * Set this to allow break after soft hyphen. By default, breaking
 * directly after soft hyphen is not allowed.
 */
#define UNICODE_LBOPTION_BREAK_SOFT_HYPHEN       16

/*
 * Set this to treat U+0085 NEL as CM, not BK. [UAX#14]
 */
#define UNICODE_LBOPTION_NOBREAK_NL              32

/*
 * Set this to treat U+000B vertical tab as CM, not BK. [UAX#14]
 */
#define UNICODE_LBOPTION_NOBREAK_VT              64

/*
 * Set this to disable "burasage" (hunging punctuation in East Asian
 * text).
 */
#define UNICODE_LBOPTION_NO_HUNGING_PUNCT        128

/*
 * By default, Wide OP/CL characters are treated as if they are a
 * Narrow character preceded/followed by SP.  Set this to disable
 * such feature.
 */
#define UNICODE_LBOPTION_WIDE_PUNCT_WITHOUT_GLUE 256

/*
 * By default, IDEOGRAPHIC SPACE is treated as ID.  Set this to treat
 * it as SP.
 */
#define UNICODE_LBOPTION_IDSP_IS_SP              512

/*
 * By default, small hiragana/katakana and KATAKANA-HIRAGANA PROLONGED
 * SIGN are NS.  Set this to treat these characters as ID. [JIS X 4051]
 */
#define UNICODE_LBOPTION_RELAX_KANA_NS           1024

/*
 * Set this to treat spanish quotations INVERTED EXCLAMATION MARK and
 * INVERTED QUESTION MARK as AL, not OP. [UAX#14]
 */
#define UNICODE_LBOPTION_OPAL_IS_AL              2048

/*
 * Set this option to treat any of ID, hangul syllables (H2, H3) or
 * hangul combining jamos (JT, JV, JL) as AL.
 * WARNING: This may fold non-alphabetic (or mixed) texts *very* ugrily.
 */
#define UNICODE_LBOPTION_FORCE_AL                4096

/*
 * Set this to break SP+CM sequence. [UAX#14]
 */
#define UNICODE_LBOPTION_BREAK_SPCM              8192

/* 
 * Set this option to force break unbreakable text longer than specified
 + width.  By default, longer text won't be broken.
 * This violate line breaking rules then may cause break within words,
 * syllables, characters or on somewhere unpreferred.
 */
#define UNICODE_LBOPTION_FORCE_LINEWIDTH         16384

#define UNICODE_LBOPTION_DEFAULT                 0

/*
 * Hard limit of line length (0 means unlimitd).
 */
#define UNICODE_LINEBREAK_HARD_LIMIT             998

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
  void *voidarg;                    /* pointer to custom data. */
};

/*
 * Public functions.
 */
extern struct unicode_lbinfo
*unicode_lbinfo_alloc(unicode_char *, size_t,
                      unicode_lbprop_funcptr(*)(const char *, int),
                      int (*)(unicode_char, int, enum unicode_lbclass, int),
                      enum unicode_lbclass
		      (*)(unicode_char, int, enum unicode_lbclass, int),
                      const char *, int, void *);

extern void unicode_lbinfo_free(struct unicode_lbinfo *);

extern enum unicode_lbaction
unicode_do_linebreak(struct unicode_lbinfo *, unicode_char *,
                     int (*)(struct unicode_lbinfo *, unicode_char *,
			     off_t, size_t, int),
                     void (*)(struct unicode_lbinfo *, unicode_char *,
			      off_t, size_t, enum unicode_lbaction),
                     int);

#endif /* UNICODE_LINEBREAK_H */
