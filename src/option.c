/*
 * options.c - Option handling for line folding utility.
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

#include <stdio.h>
#include "common.h"
#include "linefold.h"

struct option_table {
  char shortname;
  const char *longname;
  const char *argname;
  int negated;
  linefold_flags flags;
  int *varboolean;
  int *varinteger;
  char **varstring;
  linefold_char **varunicode;
  size_t *varunicodelen;
  const char *description;
};

static int parse_boolean(char *);
static int parse_integer(char *);
static linefold_char *parse_unicode(char *, size_t *);
static int optioncmp(const char *, const char *);

#define DEFAULT_CHARSET "ASCII"
#define DEFAULT_LINE_WIDTH 72

linefold_flags option_flags=LINEFOLD_OPTION_DEFAULT;
char *option_context_code=NULL;
int option_conversion=0;
char *option_conversion_str=NULL;
char *option_from_code=NULL;
int option_help=0;
/* linefold_char *option_line_starter=NULL;
   size_t option_line_starter_len=0; */
linefold_char *option_line_terminator=NULL;
size_t option_line_terminator_len=0;
int option_line_width=0;
char *option_output=NULL;
/* linefold_char *option_paragraph_starter=NULL;
   size_t option_paragraph_starter_len=0; */
linefold_char *option_paragraph_terminator=NULL;
size_t option_paragraph_terminator_len=0;
int option_nostrip_eof = 0;
/* linefold_char *option_text_starter=NULL;
   size_t option_text_starter_len=0; */
linefold_char *option_text_terminator=NULL;
size_t option_text_terminator_len=0;
char *option_to_code=NULL;
int option_notrim_sp = 0;
int option_version=0;

struct option_table options[] = {
  {
    '-', "break after HYPHEN", "yes|no",
    0, LINEFOLD_OPTION_BREAK_HY,0,0,0,0,0,
    "Allow break between HY (U+002D HYPHEN-MINUS) and AL\n"
    "(alphabetic character)."
  },
  {
    '-', "break after SOFT HYPHEN", "yes|no",
    0, LINEFOLD_OPTION_BREAK_SOFT_HYPHEN,0,0,0,0,0,
    "Allow break after U+00AD SOFT HYPHEN."
  },
  {
    '-', "break at FORM FEED", "yes|no",
    1, LINEFOLD_OPTION_NOBREAK_FF,0,0,0,0,0,
    "Treat U+000C form feed as BK (mandatory break), not CM\n"
    "(combining mark)."
  },
  {
    '-', "break at LINE TABULATION", "yes|no",
    1, LINEFOLD_OPTION_NOBREAK_VT,0,0,0,0,0,
    "Treat U+000B vertical tab as BK (mandatory break), not CM\n"
    "(combining mark) [cf. UAX #14]."
  },
  {
    '-', "break at NEXT LINE", "yes|no",
    1, LINEFOLD_OPTION_NOBREAK_NL,0,0,0,0,0,
    "Treat U+0085 NEXT LINE (NL, NEL) as BK (mandatory break), not\n"
    "CM (combining mark) [cf. UAX #14]."
  },
  {
    '-', "break before ellipsis", "yes|no",
    0, LINEFOLD_OPTION_INB2_IS_B2,0,0,0,0,0,
    "Allow break before U+2025 TWO DOT LEADER, U+2026 HORIZONTAL\n"
    "ELLIPSIS or their vertical forms."
  }, 
  {
    '-', "break between SP and CM", "yes|no",
    0, LINEFOLD_OPTION_BREAK_SPCM,0,0,0,0,0,
    "Allow break between SP (space) and following CM (combining\n"
    "mark) [cf. UAX #14]."
  },
  {
    '-', "combine hangul jamo", "yes|no",
    1, LINEFOLD_OPTION_NOCOMBINE_HANGUL_JAMO,0,0,0,0,0,
    "When line length are determined, sequences consist of JT, JV\n"
    "and JL (hangul combining jamo) are assumed to be combined to\n"
    "hangul syllable (if possible)."
  },
  {
    'c', "context code", "encoding",
    0, 0,0,0,&option_context_code,0,0,
    "Context character set to determine character width properties.\n"
    "Default is the value of ``to code'' option."
  },
  {
    'C', "conversion", "replace|ignore|strict",
    0, 0,0,0,&option_conversion_str,0,0,
    "How to handle unconvertible characters.  If ignore is\n"
    "specified, skip illegal characters and continue conversion.\n"
    "If strict is specified, exit with non-zero status.  If replace\n"
    "is specified, illegal characters in input are replaced to\n"
    "U+FFFD REPLACEMENT CHARACTER and unconvertible characters in\n"
    "output are replaced to substitution characters trying to\n"
    "preserve character widths.  Default is replace."
  },
  {
    '-', "direct break", "yes|no",
    1, LINEFOLD_OPTION_NOBREAK_DIRECT,0,0,0,0,0,
    "Allow breaking at direct break oppotunities.\n"
    "WARNING: Disabling this may fold non-alphabetic (or mixed)\n"
    "texts *very* ugrily."
  },
  {
    '-', "force line width", "yes|no",
    0, LINEFOLD_OPTION_FORCE_LINEWIDTH,0,0,0,0,0,
    "Force breaking unbreakable text longer than specified width.\n"
    "NOTE: This violate line breaking rules so that break within\n"
    "words, syllables, characters or on somewhere unpreferred may\n"
    "be caused."
  },
  {
    'f', "from code", "encoding",
    0, 0,0,0,&option_from_code,0,0,
    "Character set of input.  Default depends on system locale."
  },
  {
    '-', "generic character width", "yes|no",
    0, LINEFOLD_OPTION_GENERIC_WIDTH,0,0,0,0,0,
    "Resolve character width within generic (Unicode) context, even\n"
    "if character set context was specified."
  },
  {
    'h', "help", NULL,
    0, 0,&option_help,0,0,0,0,
    "Print usage and exit."
  },
  {
    '-', "hunging punctuation", "yes|no",
    1, LINEFOLD_OPTION_NOHUNG_PUNCT,0,0,0,0,0,
    "Enable hunging punctuation in East Asian text (a.k.a.\n"
    "``burasage'')."
  },
  {
    '-', "IDEOGRAPHIC SPACE is SP", "yes|no",
    0, LINEFOLD_OPTION_IDSP_IS_SP,0,0,0,0,0,
    "U+3000 IDEOGRAPHIC SPACE is SP (space)."
  },
  {
    '-', "inverted exclamation is AL", "yes|no",
    0, LINEFOLD_OPTION_OPAL_IS_AL,0,0,0,0,0,
    "U+00A1 INVERTED EXCLAMATION MARK and U+00BF INVERTED QUESTION\n"
    "MARK are AL (alphabetic letters), not OP (opening punctuation)\n"
    "[cf. UAX#14]."
  }, 
  /* {
    '-', "line starter", "string",
    0, 0,0,0,0,&option_line_starter,&option_line_starter_len,
    "[Not yet implemented]\n"
    "String prepended to broken lines.  Default is none."
    }, */
  {
    '-', "line terminator", "string",
    0, 0,0,0,0,&option_line_terminator,&option_line_terminator_len,
    "String inserted between broken lines.  Default is ``\\n''."
  },
  {
    'w', "line width", "width",
    0, 0,0,&option_line_width,0,0,0,
    "Limit of line length.  Default is 72."
  },
  {
    '-', "narrow ambiguous alphabetics", "yes|no",
    0, LINEFOLD_OPTION_NARROW_CYRILLIC|
    LINEFOLD_OPTION_NARROW_GREEK|
    LINEFOLD_OPTION_NARROW_LATIN,0,0,0,0,0,
    "Equivalent to narrow Cyrillic[=yes|no], narrow Greek[=yes|no]\n"
    "and narrow Latin[=yes|no]."
  },
  {
    '-', "narrow Cyrillic", "yes|no",
    0, LINEFOLD_OPTION_NARROW_CYRILLIC,0,0,0,0,0,
    "In multibyte character set context, several Cyrillic letters\n"
    "are treated as wide by default.  Enable this to treat such\n"
    "letters always narrow."
  },
  {
    '-', "narrow Greek", "yes|no",
    0, LINEFOLD_OPTION_NARROW_GREEK,0,0,0,0,0,
    "In multibyte character set context, several Greek letters are\n"
    "treated as wide by default.  Enable this to treat such letters\n"
    "always narrow."
  },
  {
    '-', "narrow Latin", "yes|no",
    0, LINEFOLD_OPTION_NARROW_LATIN,0,0,0,0,0,
    "In multibyte character set context, several Latin letters are\n"
    "treated as wide by default.  Enable this to treat such letters\n"
    "always narrow."
  },
  {
    'o', "output", "file",
    0, 0,0,0,&option_output,0,0,
    "Write output to file instead of standard output.  `-' means\n"
    "standard output."
  },
  /* {
    '-', "paragraph starter", "string",
    0, 0,0,0,0,&option_paragraph_starter,&option_paragraph_starter_len,
    "[Not yet implemented]\n"
    "String prepended to paragraph.  Default is none."
    }, */
  {
    '-', "paragraph terminator", "string",
    0, 0,0,0,0,&option_paragraph_terminator,&option_paragraph_terminator_len,
    "Newline sequnece to replace end of paragraph.  Default is no\n"
    "replacement."
  },
  {
    '-', "relax kana non-starter", "yes|no",
    0, LINEFOLD_OPTION_RELAX_KANA_NS,0,0,0,0,0,
    "By default, hiragana/katakana small letters and\n"
    "U+30FC KATAKANA-HIRAGANA PROLONGED SOUND MARK are NS (non-\n"
    "starter).  Set this option to treat these characters as ID\n"
    "(ideograph-like characters) [cf. JIS X 4051]."
  },
  {
    '-', "strip EOF", "yes|no",
    1, 0,&option_nostrip_eof,0,0,0,0,
    "Strip DOS/Windows EOF control character (U+001A) at end of\n"
    "each input files."
  },
  /* {
    '-', "text starter", "string",
    0, 0,0,0,0,&option_text_starter,&option_text_starter_len,
    "[Not yet implemented]\n"
    "String to prepend to the text.  Default is the value of\n"
    "``paragraph starter'' option."
    }, */
  {
    '-', "text terminator", "string",
    0, 0,0,0,0,&option_text_terminator,&option_text_terminator_len,
    "Newline sequence to append or replace end of text.  Default is\n"
    "no modifications."
  },
  {
    't', "to code", "encoding",
    0, 0,0,0,&option_to_code,0,0,
    "Character set for output.  Default depends on system locale."
  },
  {
    '-', "trim SP", "yes|no",
    1, 0,&option_notrim_sp,0,0,0,0,
    "Trim SP (space) at end of lines in output."
  },
  {
    'V', "version", NULL,
    0, 0,&option_version,0,0,0,0,
    "Print version and copyright informations then exit."
  },
  {
    '-', "wide punctuation with glue", "yes|no",
    1, LINEFOLD_OPTION_NOGLUE_PUNCT,0,0,0,0,0,
    "Treat wide punctuations as if they are a narrow character\n"
    "preceded/followed by SP (space)."
  },
  { 0, NULL, NULL, 0, 0,0,0,0,0,0, NULL }
};


void usage(char **argv)
{
  struct option_table *p;
  const char *cp;

  fputs("USAGE: linefold [options...] [file...]\n"
	"\n"
	"Options:\n"
	"\n"
	"\tLong name of options are case-insensitive.  In addition, ``_''\n"
	"\tin the name of options may be replaced by any non-alphanumeric\n"
	"\tcharacters except ``=''.\n"
	"\n"
	"\tOmmited optional boolean value means ``yes''.\n"
	"\n"
	"\tFollowing escape sequences may be used in string value:\n"
	"\t\\0, \\a, \\b, \\t, \\n, \\v, \\f, \\r, \\\\ - same as C language.\n"
	"\t\\e - ESC\n"
	"\t\\N - U+0085 NEXT LINE\n"
	"\t\\L - U+2028 LINE SEPARATOR\n"
	"\t\\P - U+2029 PARAGRAPH SEPARATOR\n"
	"\t\\? - U+FFFD REPLACEMENT CHARACTER\n"
	"\t\\xhh, \\uhhhh, \\Uhhhhhhhh - character of hexadecimal value.\n"
	"\n", stdout);
  for (p = options; p->shortname || p->longname; p++) {
    if (p->shortname && p->shortname != '-') {
      fputs("\t-", stdout);
      fputc(p->shortname, stdout);
      if (p->argname) {
	fputs(" ", stdout);
	fputs(p->argname, stdout);
      }
      fputs("\n", stdout);
    }
    if (p->longname) {
      fputs("\t--", stdout);
      for (cp=p->longname; *cp; cp++)
	if ('A' <= *cp && *cp <= 'Z')
	  fputc(*cp - ('A'-'a'), stdout);
        else if (('a' <= *cp && *cp <= 'z') || ('0' <= *cp && *cp <= '9'))
	  fputc(*cp, stdout);
	else
	  fputc('_', stdout);
      if (p->argname) {
	if (p->flags || p->varboolean)
	  fputs("[", stdout);
	fputs("=", stdout);
	fputs(p->argname, stdout);
	if (p->flags || p->varboolean)
	  fputs("]", stdout);
      }
      fputs("\n", stdout);
    }
    fputs("\t\t", stdout);
    if (p->description) {
      for (cp=p->description; *cp; cp++)
	if (*cp == '\n')
	  fputs("\n\t\t", stdout);
	else
	  fputc(*cp, stdout);
    }
    if ((p->flags || p->varboolean) && p->argname) {
      fputs("  Default is ", stdout);
      fputs(p->negated? "yes": "no", stdout);
      fputs(".", stdout);
    }
    fputs("\n\n", stdout);
  }
  fputs("\t--\n"
	"\t\tIndicate end of options.\n"
	"\n"
	"\tWhen multiple files are given, contents of all files will be\n"
	"\tconcatinated then processed.  If `-' is specified, input will be\n"
	"\tread from standard input.\n"
	"\n", stdout);
}

int
setlongoption(char *opt, char *val)
{
  struct option_table *p;

  if (opt == NULL || *opt == '\0')
    return EINVAL;

  for (p = options; p->shortname || p->longname; p++) {
    if (p->longname == NULL)
      continue;

    if (optioncmp(p->longname, opt) == 0) {
      if (p->flags) {
	int v;
	if (val == NULL)
	  val = "yes";
	if ((v = parse_boolean(val)) == -1)
	  return EINVAL;
	if ((p->negated && !v) || (!p->negated && v))
	  option_flags |= p->flags;
	else
	  option_flags &= ~(p->flags);
      } else if (p->varboolean) {
	int v;
	if (val == NULL)
	  val = "yes";
	if ((v = parse_boolean(val)) == -1)
	  return EINVAL;
	*(p->varboolean) = ((p->negated && !v) || (!p->negated && v))? 1: 0;
      } else if (p->varinteger) {
	int v;
	if ((v = parse_integer(val)) == -1)
	  return EINVAL;
	*(p->varinteger) = v;
      } else if (p->varstring) {
	if (val == NULL)
	  return EINVAL;
	*(p->varstring) = val;
      } else if (p->varunicode) {
	linefold_char *v;
	if ((v = parse_unicode(val, p->varunicodelen)) == NULL)
	  return errno;
	*(p->varunicode) = v;
      } else /* NOTREACHED */
	return EINVAL;

      return 0;
    }
  }
  return EINVAL;
}

int
setshortoption(char opt, char *val)
{
  struct option_table *p;

  if (opt == '\0')
    return EINVAL;

  for (p = options; p->shortname || p->longname; p++) {
    if (p->shortname == 0 || p->shortname == '-')
      continue;

    if (p->shortname == opt) {
      if (p->flags) {
	int v;
	if ((v = parse_boolean(val)) == -1)
	  return EINVAL;
	if ((p->negated && !v) || (!p->negated && v))
	  option_flags |= p->flags;
	else
	  option_flags &= ~(p->flags);
      } else if (p->varboolean) {
	int v;
	if ((v = parse_boolean(val)) == -1)
	  return EINVAL;
	*(p->varboolean) = ((p->negated && !v) || (!p->negated && v))? 1: 0;
      } else if (p->varinteger) {
	int v;
	if ((v = parse_integer(val)) == -1)
	  return EINVAL;
	*(p->varinteger) = v;
      } else if (p->varstring) {
	if (val == NULL)
	  return EINVAL;
	*(p->varstring) = val;
      } else if (p->varunicode) {
	linefold_char *v;
	if ((v = parse_unicode(val, p->varunicodelen)) == NULL)
	  return errno;
	*(p->varunicode) = v;
      } else /* NOTREACHED */
	return EINVAL;

      return 0;
    }
  }
  return EINVAL;
}

int
setdefaultoption (void)
{
  char *locale_code;

  if (option_conversion_str == NULL ||
      optioncmp(option_conversion_str, "replace") == 0)
    option_conversion = 1;
  else if (optioncmp(option_conversion_str, "ignore") == 0)
    option_conversion = 0;
  else if (optioncmp(option_conversion_str, "strict") == 0)
    option_conversion = -1;

#if HAVE_LOCALE_H
#  if HAVE_SETLOCALE
  setlocale(LC_CTYPE, "");
#    if USE_LIBCHARSET
  locale_code = locale_charset();
#    elif HAVE_LANGINFO_CODESET
  locale_code = nl_langinfo(CODESET);
#    endif  /* USE_LIBCHARSET */
#  endif /* HAVE_SETLOCALE */
#endif /* HAVE_LOCALE_H */
  if (locale_code == NULL || *locale_code == '\0')
    locale_code = DEFAULT_CHARSET;

  if (option_to_code == NULL)
    option_to_code = locale_code;
  if (option_from_code == NULL)
    option_from_code = locale_code;
  if (option_context_code == NULL)
    option_context_code = option_to_code;

  if (option_line_width <= 0)
    option_line_width = DEFAULT_LINE_WIDTH;

  /* if (option_paragraph_starter == NULL && option_line_starter != NULL) {
    option_paragraph_starter = option_line_starter;
    option_paragraph_starter_len = option_line_starter_len;
  }
  if (option_text_starter == NULL && option_paragraph_starter != NULL) {
    option_text_starter = option_paragraph_starter;
    option_text_starter_len = option_paragraph_starter_len;
    } */

  if (option_line_terminator == NULL) {
    if ((option_line_terminator =
	 parse_unicode("\\n", &option_line_terminator_len)) == NULL)
      return errno;
  }

  return 0;
}

static int
parse_boolean(char *val)
{
  if (val == NULL || *val == '\0')
    return -1;
  else if (optioncmp(val, "yes") == 0 ||
	   optioncmp(val, "Y") == 0 ||
	   optioncmp(val, "on") == 0 ||
	   optioncmp(val, "enable") == 0 ||
	   optioncmp(val, "true") == 0 ||
	   optioncmp(val, "T") == 0 ||
	   optioncmp(val, "1") == 0)
    return 1;
  else if (optioncmp(val, "no") == 0 ||
	   optioncmp(val, "N") == 0 ||
	   optioncmp(val, "off") == 0 ||
	   optioncmp(val, "disable") == 0 ||
	   optioncmp(val, "false") == 0 ||
	   optioncmp(val, "F") == 0 ||
	   optioncmp(val, "0") == 0)
    return 0;
  else
    return -1;
}

static int
parse_integer(char *val)
{
  if (val == NULL || *val == '\0')
    return -1;
  else if ('0' <= *val && *val <= '9')
    return atoi(val);
  else
    return -1;
}

static linefold_char
hextou(char *str, int len)
{
  int i;
  unsigned char c;
  linefold_char u=0;

  for (i = 0; i < len; i++) {
    c = (unsigned char)str[i];
    if ('0' <= c && c <= '9')
      u = u * 16 + c - '0';
    else if ('a' <= c && c <= 'f')
      u = u * 16 + c - 'a' + 10;
    else if ('A' <= c && c <= 'F')
      u = u * 16 + c - 'A' + 10;
    else
      return (linefold_char)-1;
  }
  return u;
}

static linefold_char *
parse_unicode(char *val, size_t *ulenp)
{
  char *vp=val;
  linefold_char *ubuf, *up;

  if (val == NULL) {
    errno = EINVAL;
    return NULL;
  }
  if ((ubuf = malloc(sizeof(linefold_char) * (strlen(val)? strlen(val): 1))) ==
      NULL)
    return NULL;

  up = ubuf;
  while (*vp) {
    if (*vp == '\\') {
      unsigned char c = (unsigned char)*(++vp);
      linefold_char uc;

      if (c == '\0') {
	errno = EINVAL;
	return NULL;
      }
      else if (c == '0')
	uc = (linefold_char)0; /* null */
      else if (c == 'a')
	uc = (linefold_char)0x07; /* bell */
      else if (c == 'b')
	uc = (linefold_char)0x08; /* back space */
      else if (c == 't')
	uc = (linefold_char)0x09; /* horizontal tab */
      else if (c == 'n')
	uc = (linefold_char)0x0A; /* line feed */
      else if (c == 'v')
	uc = (linefold_char)0x0B; /* vertical tab */
      else if (c == 'f')
	uc = (linefold_char)0x0C; /* form feed */
      else if (c == 'r')
	uc = (linefold_char)0x0D; /* carriage return */
      else if (c == 'e')
	uc = (linefold_char)0x1B; /* escape */
      else if (c == 'N')
	uc = (linefold_char)0x85; /* next line */
      else if (c == 'L')
	uc = (linefold_char)0x2028; /* LINE SEPARATOR */
      else if (c == 'P')
	uc = (linefold_char)0x2029; /* PARAGRAPH SEPARATOR */
      else if (c == '?')
	uc = (linefold_char)0xFFFD; /* REPLACEMENT CHARACTER */
      else if (c == 'x') {
	if ((uc = hextou(++vp, 2)) == (linefold_char)-1) {
	  errno = EINVAL;
	  return NULL;
	}
	vp += 2-1;
      } else if (c == 'u') {
	if ((uc = hextou(++vp, 4)) == (linefold_char)-1) {
	  errno = EINVAL;
	  return NULL;
	}
	vp += 4-1;
      } else if (c == 'U') {
	if ((uc = hextou(++vp, 8)) == (linefold_char)-1) {
	  errno = EINVAL;
	  return NULL;
	}
	vp += 8-1;
      } else if (c == '\\')
	uc = (linefold_char)'\\';
      else {
	errno = EINVAL;
	return NULL;
      }
      *(up++) = uc;
    } else
      *(up++) = (linefold_char)(unsigned char)*vp;
    vp++;
  }

  if (ulenp)
    *ulenp = up - ubuf;
  return ubuf;
}


static int
optioncmp(const char *s1, const char *s2)
{
  unsigned char c1, c2;

  while ((c1 = (unsigned char)*s1) && (c2 = (unsigned char)*s2)) {
    if (('a' <= c1 && c1 <= 'z'))
      c1 -= 'a'-'A';
    else if (!('0' <= c1 && c1 <= '9') && !('A' <= c1 && c1 <= 'Z'))
      c1 = '.';
    if ('a' <= c2 && c2 <= 'z')
      c2 -= 'a'-'A';
    else if (!('0' <= c2 && c2 <= '9') && !('A' <= c2 && c2 <= 'Z'))
      c2 = '.';
    if (c1 != c2) return c1 - c2;
    s1++;
    s2++;
  }
  return (unsigned char)*s1 - (unsigned char)*s2;
}
