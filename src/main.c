/*
 * main.c - Main module for line folding utility.
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

extern size_t
decode(const char *, char *, size_t *, size_t, linefold_char **, size_t, int);
extern size_t
encode(const char *, const struct linefold_info *, const linefold_char *,
       size_t, size_t, char **, int);
extern void
usage(char **);
extern int
setlongoption(char *, char *);
extern int
setshortoption(char, char *);
extern int
setdefaultoption(void);

extern linefold_flags option_flags;
extern char *option_context_code;
extern int option_conversion;
extern char *option_from_code;
extern int option_help;
/* extern linefold_char *option_line_starter;
   extern size_t option_line_starter_len; */
extern linefold_char *option_line_terminator;
extern size_t option_line_terminator_len;
extern int option_line_width;
extern char *option_output;
FILE *option_output_fp;
/* extern linefold_char *option_paragraph_starter;
   extern size_t option_paragraph_starter_len; */
extern linefold_char *option_paragraph_terminator;
extern size_t option_paragraph_terminator_len;
extern int option_nostrip_eof;
/* extern linefold_char *option_text_starter;
   extern size_t option_text_starter_len; */
extern linefold_char *option_text_terminator;
extern size_t option_text_terminator_len;
extern char *option_to_code;
extern int option_notrim_sp;
extern int option_version;


void error_exit(int, const char *);

/*
 * Customizable Functions for line breaking module.
 */

/* Write out one broken line */
void
writeout_cb(const struct linefold_info *lbinfo,
	    const linefold_char * text,
	    size_t start, size_t linelen, linefold_action action,
	    void *voidarg)
{
  char *str = NULL;
  size_t nlseqstart, nlseqlen, len;
  const linefold_class *lbclasses = lbinfo->lbclasses;
  linefold_class lbc;

  nlseqlen = 0;
  nlseqstart = start + linelen;
  lbc = lbclasses[nlseqstart - 1];
  if (nlseqstart > start) {
    if (lbc == LINEFOLD_CLASS_LF || lbc == LINEFOLD_CLASS_CR) {
      linelen--;
      nlseqstart--;
      nlseqlen++;
      while (nlseqstart > start &&
	     lbclasses[nlseqstart - 1] == LINEFOLD_CLASS_CR) {
	linelen--;
	nlseqstart--;
	nlseqlen++;
      }
    } else if (lbc == LINEFOLD_CLASS_BK || lbc == LINEFOLD_CLASS_NL) {
      linelen--;
      nlseqstart--;
      nlseqlen++;
    }
  }
  if (!option_notrim_sp)
    while (linelen > 0)
      if (lbclasses[start + linelen - 1] == LINEFOLD_CLASS_SP)
	linelen--;
      else
	break;

  if ((len = encode(option_to_code, lbinfo, text, start, linelen,
		    &str, option_conversion)) == -1)
    error_exit(errno,
	       (errno == EINVAL) ?
	       "Unsupported character set for output" : NULL);

  if (str != NULL) {
    if (len > 0)
      fwrite(str, len, 1, option_output_fp);
    free(str);
    str = NULL;
  }

  if (action == LINEFOLD_ACTION_EXPLICIT) {
    if (option_paragraph_terminator) {
      if ((len = encode(option_to_code, NULL,
			option_paragraph_terminator,
			0, option_paragraph_terminator_len,
			&str, option_conversion)) == -1)
	error_exit(errno, NULL);
    } else {
      if ((len = encode(option_to_code, lbinfo,
			text, nlseqstart, nlseqlen,
			&str, option_conversion)) == -1)
	error_exit(errno, NULL);
    }

    if (str != NULL) {
      if (len > 0)
	fwrite(str, len, 1, option_output_fp);
      free(str);
      str = NULL;
    }
  } else if (action == LINEFOLD_ACTION_DIRECT ||
	     action == LINEFOLD_ACTION_INDIRECT) {
    if ((len = encode(option_to_code, NULL,
		      option_line_terminator, 0, option_line_terminator_len,
		      &str, option_conversion)) == -1)
      error_exit(errno, NULL);
    fwrite(str, len, 1, option_output_fp);
    free(str);
  } else if (LINEFOLD_ACTION_EOT) {
    if (option_text_terminator) {
      if ((len = encode(option_to_code, NULL,
			option_text_terminator, 0, option_text_terminator_len,
			&str, option_conversion)) == -1)
	error_exit(errno, NULL);
    } else if (nlseqlen) {
      if ((len = encode(option_to_code, lbinfo,
			text, nlseqstart, nlseqlen,
			&str, option_conversion)) == -1)
	error_exit(errno, NULL);
    }

    if (str != NULL) {
      if (len > 0)
	fwrite(str, len, 1, option_output_fp);
      free(str);
      str = NULL;
    }


    if ((len = encode(option_to_code, NULL,
		      NULL, 0, 0, &str, option_conversion)) == -1)
      error_exit(errno, NULL);
    if (str != NULL) {
      if (len > 0)
	fwrite(str, len, 1, option_output_fp);
      free(str);
    }
  }
}

/*
 * Main Routine.
 */

void
version(void)
{
  static char notice[] =
    "Copyright (C) 2006 by Hatuka*nezumi - IKEDA Soji.  All rights reserved.\n"
    "\n"
    "This program is free software; you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation; either version 2 of the License, or (at\n"
    "your option) any later version.  This program is distributed in the\n"
    "hope that it will be useful, but WITHOUT ANY WARRANTY; without even\n"
    "the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR\n"
    "PURPOSE.  See the COPYING file for more details.\n";

  fputs(PACKAGE_NAME " " PACKAGE_VERSION "\n\n", stdout);
  fputs(notice, stdout);
  fputs("\nPlease send comments or bug reports to <"
	PACKAGE_BUGREPORT ">.\n\n", stdout);
}

void
error_exit(int errnum, const char *msg)
{
  errno = 0;
  if (msg == NULL) {
    msg = strerror(errnum);
    if (errno != 0)
      msg = "Unknown error";
  }
  fputs("linefold: ", stderr);
  fputs(msg, stderr);
  fputc('\n', stderr);
  exit(errnum ? errnum : 255);
}

int
main(int argc, char **argv)
{
  linefold_char *text = NULL;
  size_t textlen = 0;
  FILE *ifp;
  char buf[4096], *nbuf;
  struct linefold_info *lbi;
  int i;

#if HAVE_LOCALE_H
#if HAVE_SETLOCALE
  setlocale(LC_ALL, "");
  setlocale(LC_MESSAGES, "");
#endif /* HAVE_SETLOCALE */
#endif /* HAVE_LOCALE_H */

  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-' && argv[i][1] == '-') {
      char *opt = argv[i] + 2;
      char *arg;

      if (*opt == '\0') {
	i++;
	break;
      }

      if ((arg = index(opt, '=')) == NULL)
	arg = NULL;
      else
	*arg++ = '\0';
      if ((errno = setlongoption(opt, arg)) != 0)
	error_exit(errno, NULL);
    } else if (argv[i][0] == '-' && argv[i][1] != '\0') {
      char opt = argv[i][1];
      char *arg = argv[i] + 2;

      if (opt == 'h' || opt == 'V') {
	if (*arg != '\0')
	  error_exit(EINVAL, NULL);
	else
	  arg = "yes";
      } else if (*arg == '\0') {
	if (++i < argc)
	  arg = argv[i];
	else
	  error_exit(EINVAL, NULL);
      }

      if ((errno = setshortoption(opt, arg)) != 0)
	error_exit(errno, NULL);
    } else
      break;
  }
  if ((errno = setdefaultoption()) != 0)
    error_exit(errno, NULL);

  if (option_version || option_help) {
    if (option_version)
      version();
    if (option_help)
      usage(argv);
    exit(0);
  }

  if (option_output == NULL ||
      (option_output[0] == '-' && option_output[1] == '\0'))
    option_output_fp = stdout;
  else if ((option_output_fp = fopen(option_output, "wb")) == NULL)
    error_exit(errno, NULL);

  if (i >= argc) {
    argc = 1;
    argv[0] = "-";
    i = 0;
  }
  text = NULL;
  textlen = 0;
  while (i < argc) {
    if (argv[i][0] == '-' && argv[i][1] == '\0')
      ifp = stdin;
    else if ((ifp = fopen(argv[i], "rb")) == NULL)
      error_exit(errno, NULL);
    i++;

    nbuf = buf;
    while (fgets(nbuf, sizeof(buf) - (nbuf - buf) - 1, ifp) != NULL) {
      size_t bufpos, buflen;
      bufpos = 0;
      buflen = strlen(buf);
      errno = 0;
      if ((textlen = decode(option_from_code,
			    buf, &bufpos, buflen,
			    &text, textlen, option_conversion)) == -1)
	error_exit(errno,
		   (errno == EINVAL) ?
		   "Unsupported character set for input" : NULL);

      nbuf = buf;
      if (errno == EINVAL)
	while (bufpos < buflen)
	  *(nbuf++) = buf[bufpos++];
    }
    fclose(ifp);
    /* Trim EOF at end of file. */
    if (!option_nostrip_eof)
      while (textlen > 0 && text[textlen - 1] == (linefold_char) 0x001A)
	textlen--;
  }

  if ((lbi = linefold_alloc(text, textlen, NULL, NULL,
			    option_context_code, option_flags)) == NULL) {
    if (errno)
      error_exit(errno, NULL);
    if (text != NULL)
      free(text);
    exit(0);
  }
  linefold(lbi, text, NULL, &writeout_cb, option_line_width, NULL);
  linefold_free(lbi);
  free(text);
  fclose(option_output_fp);

  exit(0);
}
