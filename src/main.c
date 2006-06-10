/*
 * main.c - Main module for line folding utility.
 * 
 * Copyright (C) 2006 by Hatuka*nezumi - IKEDA Soji <hatuka(at)nezumi.nu>,
 * redistributed under GNU General Public License version 2 (or later
 * version you prefer).
 *
 * $id$
 */

#include <stdio.h>
#include "common.h"
#if HAVE_LOCALE_H
#  if HAVE_SETLOCALE
#    include <locale.h>
#    if USE_LIBCHARSET
#      if HAVE_LOCALCHARSET_H
#        include <localcharset.h>
#      elif HAVE_LIBCHARSET_H
#        include <libcharset.h>
#      endif /* HAVE_LOCALCHARSET_H */
#    elif HAVE_LANGINFO_CODESET
#      include <langinfo.h>
#    endif /* USE_LIBCHARSET */
#  endif /* HAVE_SETLOCALE */
#endif /* HAVE_LOCALE_H */	
#include "linefold.h"

#define DEFAULT_CHARSET "UTF-8"

extern size_t decode(const char *, char *, size_t, unicode_char **);
extern size_t encode(const char *, unicode_char *, size_t, char **);


/*
 * Customizable Functions for line breaking module.
 */

/* Write out one broken line */
void writeout_cb(struct linefold_info *lbinfo, unicode_char *text,
		 size_t start, size_t linelen, linefold_action action,
		 void *voidarg)
{
	char *str;
	if (encode((char *)voidarg, text+start, linelen, &str) <= 0)
		return;

	switch (action)
	{
	case LINEFOLD_ACTION_EXPLICIT:
		fputs(str, stdout);
		break;
	case LINEFOLD_ACTION_DIRECT:
		fputs(str, stdout);
		putc('\n', stdout);
		break;
	case LINEFOLD_ACTION_INDIRECT:
		fputs(str, stdout);
		putc('\n', stdout);
		break;
	case LINEFOLD_ACTION_EOT:
		fputs(str, stdout);
		break;
	}
	free(str);
}

/*
 * Main Routine.
 */

void usage(char **argv)
{
	printf("USAGE: %s [-c charset] [-o option] [-w linewidth] [-h] [file]\n"
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

int main(int argc, char** argv)
{
	unicode_char *text=NULL;
	size_t textlen=0, alloced=0;
	FILE *ifp;
	char buf[4096];
	struct linefold_info *lbi;
	int maxlen=0, opt=-1, i;
	char *locale_chset=NULL;
	char *context_chset=NULL, *from_chset=NULL, *to_chset=NULL;

#if HAVE_LOCALE_H
#  if HAVE_SETLOCALE
        setlocale(LC_ALL, "");
#    if USE_LIBCHARSET
        setlocale(LC_CTYPE, "");
        locale_chset = locale_charset();
#    elif HAVE_LANGINFO_CODESET
        setlocale(LC_CTYPE, "");
        locale_chset = nl_langinfo(CODESET);
#    endif  /* USE_LIBCHARSET */
        setlocale(LC_CTYPE, "C");
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
				opt = atoi(arg);
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
	if (opt < 0)
		opt = LINEFOLD_OPTION_DEFAULT;

	if (i < argc)
	{
		if ((ifp = fopen(argv[i], "rb")) == NULL)
			return 1;
	}
	else
		ifp = stdin;

	while (fgets(buf, 4095, ifp) != NULL)
	{
		unicode_char *ubuf=NULL;
		alloced = decode(from_chset, buf, strlen(buf), &ubuf);
		if (alloced > 0)
		{
			unicode_char *newtext = malloc(sizeof(unicode_char) * (textlen + alloced));
			memcpy(newtext, text, sizeof(unicode_char) * textlen);
			memcpy(newtext + textlen, ubuf, sizeof(unicode_char) * alloced);
			if (text != NULL)
				free(text);
			text = newtext;
			textlen += alloced;
			free(ubuf);
		}
	}
	fclose(ifp);

	if ((lbi = linefold_alloc(text, textlen, NULL, NULL,
					   context_chset, opt)) == NULL)
	{
		if (text != NULL) free(text);
		exit(0);
	}
	linefold(lbi, text, NULL, &writeout_cb, maxlen,
			  (void *)to_chset);
	linefold_free(lbi);

	free(text);
	exit(0);
}
