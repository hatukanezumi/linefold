/*
 * Sample of customizable functions for line breaking algorithm.
 * 
 * Copyright (C) 2006 by Hatuka*nezumi - IKEDA Soji <hatuka(at)nezumi.nu>,
 * redistributed under GNU General Public License version 2 (or later
 * version you prefer).
 *
 * $id$
 */

#include <stdio.h>
#include <string.h>
#include "unicode_linebreak.h"

void printutf8(const unicode_char *, size_t);
size_t catutf8(unicode_char **, size_t *, size_t, const char *);


/*
 * Customizable Functions for line breaking module.
 */

/* Write out one broken line */
void writeout_cb(struct unicode_lbinfo *lbinfo, unicode_char *text,
		 off_t start, size_t linelen, enum unicode_lbaction action,
		 void *voidarg)
{
	printutf8(text+start, linelen);

	switch (action)
	{
	case UNICODE_LBACTION_EXPLICIT:
		printf("/E");
		break;
	case UNICODE_LBACTION_DIRECT:
		printf("/D");
		break;
	case UNICODE_LBACTION_INDIRECT:
		printf("/I");
		break;
	case UNICODE_LBACTION_EOT:
		printf("/EOT");
		break;
	default:
		printf("/?");
		break;
	}
	putc('\n', stdout);
}

/*
 * Main Routine.
 */

void usage(char **argv)
{
	printf("USAGE: %s [-c charset] [-o option] [-w linewidth] [-h] [file]\n"
	       "\n"
	       "Options:\n"
	       "    -c chset\n"
	       "        Specify charset context (default is generic).\n"
	       "    -o option\n"
               "        Set UNICODE_LBOPTION_* flags.\n"
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
	struct unicode_lbinfo *lbi;
	int maxlen=0, opt=-1, i;
	char *chset=NULL;

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
				chset = arg;
				break;
			case 'o':
                                opt = atoi(arg);
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
	if (maxlen <= 0)
		maxlen = 72;
	if (opt < 0)
		opt = UNICODE_LBOPTION_DEFAULT;

	if (i < argc)
	{
		if ((ifp = fopen(argv[i], "rb")) == NULL)
			return 1;
	}
	else
		ifp = stdin;

	for (i=1; i <= 78; i++)
		if (i == maxlen+1)
			putc('|', stdout);
		else if (i < 10)
			putc(' ', stdout);
		else
			putc(i / 10 + (int)'0', stdout);
	putc('\n', stdout);
	for (i=1; i <= 78; i++)
		if (i == maxlen+1)
			putc('|', stdout);
		else
			putc(i % 10 + (int)'0', stdout);
	putc('\n', stdout);

	while (fgets(buf, 4095, ifp) != NULL)
		alloced = catutf8(&text, &textlen, alloced, buf);
	fclose(ifp);

	if ((lbi = unicode_linebreak_alloc(text, textlen, NULL, NULL,
					   chset, opt)) == NULL)
	{
		if (text != NULL) free(text);
		exit(0);
	}
	switch ((int)unicode_linebreak(lbi, text, NULL, &writeout_cb,
				       maxlen, NULL))
	{
	case UNICODE_LBACTION_NOMOD:
		printf("No breaks or only explicit break(s) occur.\n");
		break;
	case UNICODE_LBACTION_INDIRECT:
		printf("Indirect break(s) occur, but no direct breaks.\n");
		break;
	case UNICODE_LBACTION_DIRECT:
		printf("Direct break(s) occur.\n");
		break;
	}
	unicode_linebreak_free(lbi);

	free(text);
	exit(0);
}

/*
 * Utility Functions
 */

size_t catutf8(unicode_char **ustrp, size_t *lenp, size_t alloced,
	       const char *cp)
{
	size_t i=0, len=*lenp;
	unicode_char *ustr, uc;

        if (*ustrp == NULL)
	{
		alloced = 128;
		if ((ustr = malloc(128 * sizeof(unicode_char))) == NULL)
			return 0;
        }
        else
                ustr = *ustrp;

        while (cp[i])
        {
                if ((cp[i] & 0x80) == 0)
                {
                        uc=cp[i++];
                }
                else if ((cp[i] & 0xE0) == 0xC0 &&
                         (cp[i+1] & 0xC0) == 0x80)
                {
			uc=cp[i++] & 0x1F;
			uc <<= 6; uc |= cp[i++] & 0x3F;
                }
                else if ((cp[i] & 0xF0) == 0xE0 &&
                         (cp[i+1] & 0xC0) == 0x80 &&
			 (cp[i+2] & 0xC0) == 0x80)
		{
			uc=cp[i++] & 0x0F;
			uc <<= 6; uc |= cp[i++] & 0x3F;
			uc <<= 6; uc |= cp[i++] & 0x3F;
                }
                else if ((cp[i] & 0xF8) == 0xF0 &&
                         (cp[i+1] & 0xC0) == 0x80 &&
			 (cp[i+2] & 0xC0) == 0x80 &&
			 (cp[i+3] & 0xC0) == 0x80)
		{
			uc=cp[i++] & 0x07;
			uc <<= 6; uc |= cp[i++] & 0x3F;
			uc <<= 6; uc |= cp[i++] & 0x3F;
			uc <<= 6; uc |= cp[i++] & 0x3F;
                }
                else
                {
			uc=(unicode_char)0xFFFD;
			i++;
                }

                if (alloced <= len+2)
                {
                        unicode_char *newustr;
                        alloced += 128;
                        if ((newustr = malloc(alloced *
					      sizeof(unicode_char))) == NULL)
                        {
				free(ustr);
				*ustrp = NULL;
				*lenp = 0;
				return 0;
                        }
                        memcpy(newustr, ustr, len * sizeof(unicode_char));
                        free(ustr);
                        ustr = newustr;
                }                
                ustr[len++]=uc;
        }
        ustr[len] = 0;
        *ustrp = ustr;
        *lenp = len;
        return alloced;
}

void printutf8(const unicode_char *text, size_t textlen)
{
	off_t i;
        unicode_char uc;
	char buf[4];
        for (i = 0; i < textlen; i++)
        {
                uc = text[i];

		if (uc > (unicode_char)0x10FFFF)
			uc = (unicode_char)0xFFFD;     
		if (uc == (unicode_char)0x007F || uc <= (unicode_char)0x001F)
		{
			printf("[%02X]", (int)uc);
		}
		else if ((unicode_char)uc == (unicode_char)(uc & 0x007F))
	  	{
			putc((char)uc, stdout);
		}
		else if (uc <= (unicode_char)0x009F)
		{
			printf("[%02X]", (int)uc);
		}
		else if ((unicode_char)uc == (unicode_char)(uc & 0x07FF))
		{
			buf[1] = (char)(uc & 0x3F) | 0x80;
			uc >>= 6;
			buf[0] = (char)(uc & 0x1F) | 0xC0;
			fwrite(buf, 1, 2, stdout);
		}
		else if ((unicode_char)uc == (unicode_char)(uc & 0x00FFFF))
		{
			buf[2] = (char)(uc & 0x3F) | 0x80;
			uc >>= 6;
			buf[1] = (char)(uc & 0x3F) | 0x80;
			uc >>= 6;
			buf[0] = (char)(uc & 0x0F) | 0xE0;
			fwrite(buf, 1, 3, stdout);
		}
		else
		{
			buf[3] = (char)(uc & 0x3F) | 0x80;
			uc >>= 6;
			buf[2] = (char)(uc & 0x3F) | 0x80;
			uc >>= 6;
			buf[1] = (char)(uc & 0x3F) | 0x80;
			uc >>= 6;
			buf[0] = (char)(uc & 0x07) | 0xF0;
			fwrite(buf, 1, 4, stdout);
		}
	}
}
