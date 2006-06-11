/*
 * main.c - Main module for line folding utility.
 * 
 * Copyright (C) 2006 by Hatuka*nezumi - IKEDA Soji <hatuka(at)nezumi.nu>,
 * redistributed under GNU General Public License version 2 (or later
 * version you prefer).
 *
 * $id$
 */

#include <errno.h>
#include <stdio.h>
#include "common.h"
#include "linefold.h"

#define DEFAULT_CHARSET "ASCII"

extern size_t
decode(const char *, char *, int *, size_t, unicode_char **, int);
extern size_t
encode(const char *, struct linefold_info *, unicode_char *, int, size_t,
       char **);

void error_exit(int, char *);
void usage(char **);


/*
 * Customizable Functions for line breaking module.
 */

/* Write out one broken line */
void writeout_cb(struct linefold_info *lbinfo, unicode_char *text,
		 size_t start, size_t linelen, linefold_action action,
		 void *voidarg)
{
	char *str=NULL;
	size_t len;
	unicode_char newline_text[1] = { (unicode_char)0x000A };

	if ((len = encode((char *)voidarg, lbinfo, text, start, linelen,
			  &str)) < 0)
		error_exit(errno, strerror(errno));

	if (str != NULL) {
		if (len > 0) fwrite(str, len, 1, stdout);
		free(str);
	}

	switch (action)
	{
	case LINEFOLD_ACTION_EXPLICIT:
		break;
	case LINEFOLD_ACTION_DIRECT:
	case LINEFOLD_ACTION_INDIRECT:
		if ((len = encode((char *)voidarg, NULL,
				  newline_text, 0, 1, &str)) < 0)
			error_exit(errno, strerror(errno));
		fwrite(str, len, 1, stdout);
		free(str);
		break;
	case LINEFOLD_ACTION_EOT:
		if ((len = encode((char *)voidarg, NULL,
				  NULL, 0, 0, &str)) < 0)
			error_exit(errno, strerror(errno));
		if (str != NULL) {
			if (len > 0) fwrite(str, len, 1, stdout);
			free(str);
		}
		break;
	}
}

/*
 * Main Routine.
 */

void usage(char **argv)
{
	printf("USAGE: %s [-c charset] [-f charset] [-o option] [-t charset]\n"
	       "           [-w linewidth] [-h] [file...]\n"
	       "\n"
	       "Options:\n"
	       "    -c charset\n"
	       "        Specify charset context (default is same as -t).\n"
	       "    -f charset\n"
	       "        Specify input charset (default depends on locale).\n"
	       "    -o option\n"
	       "        Set LINEFOLD_OPTION_* flags.\n"
	       "    -t charset\n"
	       "        Specify output charset (default depends on locale).\n"
	       "    -w linewidth\n"
	       "        Specify limit of line length (default is 72).\n"
	       "    -h\n"
	       "        Print this message and exit.\n"
	       "    file\n"
	       "        Input file (default is stdin).\n",
	       argv[0]);
}

void error_exit(int errnum, char *msg)
{
	fprintf(stderr, "linefold: %s\n", msg);
	exit(errnum? errnum: 255);
}

int main(int argc, char** argv)
{
	unicode_char *text=NULL;
	size_t textlen=0, alloced=0;
	FILE *ifp;
	char buf[4096], *nbuf;
	struct linefold_info *lbi;
	int maxlen=0, i;
	linefold_flags flags=(linefold_flags)-1;
	char *locale_chset=NULL;
	char *context_chset=NULL, *from_chset=NULL, *to_chset=NULL;

#if HAVE_LOCALE_H
#  if HAVE_SETLOCALE
	char *lc_ctype = setlocale(LC_CTYPE, NULL);
#    if USE_LIBCHARSET
	setlocale(LC_CTYPE, "");
	locale_chset = locale_charset();
#    elif HAVE_LANGINFO_CODESET
	setlocale(LC_CTYPE, "");
	locale_chset = nl_langinfo(CODESET);
#    endif  /* USE_LIBCHARSET */
	setlocale(LC_CTYPE, lc_ctype);
#  endif /* HAVE_SETLOCALE */
#endif /* HAVE_LOCALE_H */
	if (locale_chset == NULL || *locale_chset == '\0')
		locale_chset = DEFAULT_CHARSET;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-' && argv[i][1] != '\0')
		{
			char o=argv[i][1];
			char *arg=argv[i]+2;

			if (*arg == '\0')
			{
				if (++i < argc)
					arg = argv[i];
				else
				{
					usage(argv);
					exit(1);
				}
			}

			switch (o)
			{	
			case 'c':
				context_chset = arg;
				break;
			case 'f':
				from_chset = arg;
				break;
			case 'o':
				flags = (linefold_flags)atol(arg);
				break;
			case 't':
				to_chset = arg;
				break;
			case 'w':
				maxlen = atoi(arg);
				break;
			case 'h':
			case '?':
				usage(argv);
				exit(0);
			default:
				usage(argv);
				exit(1);
			}
		}
		else
			break;
	}
	if (to_chset == NULL)
		to_chset = locale_chset;
	if (from_chset == NULL)
		from_chset = locale_chset;
	if (context_chset == NULL)
		context_chset = to_chset;
	if (maxlen <= 0)
		maxlen = 72;
	if (flags == (linefold_flags)-1)
		flags = LINEFOLD_OPTION_DEFAULT;

	if (i >= argc)
	{
		argc = 1;
		argv[0] = "-";
		i = 0;
	}
	text = NULL;
	textlen = 0;
	while (i < argc)
	{
		if (argv[i][0] == '-' && argv[i][1] == '\0')
			ifp = stdin;
		else if ((ifp = fopen(argv[i], "rb")) == NULL)
			error_exit(errno, strerror(errno));
		i++;

		nbuf = buf;
		while (fgets(nbuf, sizeof(buf)-(nbuf-buf)-1, ifp) != NULL)
		{
			int bufpos, buflen;
			bufpos = 0;
			buflen = strlen(buf);
			errno = 0;
			if ((textlen = decode(from_chset, buf, &bufpos, buflen,
					      &text, textlen)) == -1) {
				if (errno == ENOMEM)
					error_exit(errno, "memory exausted.");
				else if (errno == EINVAL)
					error_exit(errno,
						   "unsupported character set for input.");
				else if (errno == EILSEQ)
					error_exit(errno,
						   "illegal input sequence.");
				else
					error_exit(errno, strerror(errno));
			}

			if (errno == EINVAL) {
				memmove(buf, buf + bufpos, buflen - bufpos);
				nbuf = buf + buflen - bufpos;
			} else {
				nbuf = buf;
			}
		}
		fclose(ifp);
		/* Trim EOF at end of file. */
		while (textlen > 0 && text[textlen-1] == (unicode_char)0x001A)
			textlen--;
	}

	if ((lbi = linefold_alloc(text, textlen, NULL, NULL,
				  context_chset, flags)) == NULL)
	{
		if (errno == ENOMEM)
			error_exit(errno, "memory exausted.");
		else if (errno)
			error_exit(errno, strerror(errno));
		if (text != NULL) free(text);
		exit(0);
	}
	linefold(lbi, text, NULL, &writeout_cb, maxlen,
		 (void *)to_chset);
	linefold_free(lbi);
	free(text);

	exit(0);
}
