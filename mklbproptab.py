#! python
#-*- coding: utf-8 -*-

"""
mklbproptab.py - Generate unicode_linebreaktab.c & unicode_linebreaktab.h

Copyright © 2006 by Hatuka*nezumi - IKEDA Soji <hatuka(at)nezumi.nu>,
redistributed under GNU General Public License version 2 (or later
version you prefer).

$Id$

This script generate arrays of properties related to line breaking
defined in Unicode Standard Annex #14 (UAX#14) ``Line Breaking
Properties'' and UAX#11 ``East Asian Width'', partially customized
referring JIS X 4051:2004 『日本語文書の組版方法』 (Formatting rules
for Japanese documents).  This also include workaround for complex
line breaking behavior on some South Asian scripts.

This script requires these data files:
    http://www.unicode.org/Public/UNIDATA/LineBreak.txt
    http://www.unicode.org/Public/UNIDATA/EastAsianWidth.txt

To customize line breaking properties or rules, edit following
files:
    LineBreak/AmbiguousWidth.py
        Ambiguous character width for East Asian character sets.
    LineBreak/CharacterProperties.py
        Line breaking class and generic character width.
    LineBreak/Rules.py
        Ruleset controlling line breaking behavior.
then, run this script.
"""

import os
import sys

from LineBreak import CharacterProperties
from LineBreak.Rules import *
from LineBreak import AmbiguousWidth
from LineBreak.Utils import *


# Files to input.
LINEBREAK = 'LineBreak.txt'
EASTASIANWIDTH = 'EastAsianWidth.txt'
# Files to output.
LBPROPTAB = 'unicode_linebreaktab.c'
LBPROPHEADER = 'unicode_linebreaktab.h'
LBRULEHTML = 'linebreakrule.html'

# My name.
MYNAME = os.path.split(sys.argv[0])[-1]

###
### Read Properties.
###

try:
    fp = file(LINEBREAK, 'r')
except:
    print >>sys.stderr, 'Retrieve %s then try again.' % LINEBREAK
    sys.exit(1)
lbclassmap = readunidata(fp)
fp.close()
try:    
    fp = file(EASTASIANWIDTH, 'r')
except:
    print >>sys.stderr, 'Retrieve %s then try again.' % EASTASIANWIDTH
    sys.exit(1)
eawidthmap = readunidata(fp)
fp.close()

# Make generic property map.  Currenly we support only BMP & SMP.
propmap = {}
for u in range(0, 0x1FFFF+1):
    eaw = eawidthmap.get(u, 'X')
    lbc = lbclassmap.get(u, 'XX')
    if eaw == 'X' and lbc == 'XX':
        continue

    # Zero-width space and Combined marks are treated as zero-width (but
    # about SA, see LineBreak.CharacterProperties also).
    # Explicitly 'Wide' characters, W and F, are Wide (2-columns).
    # Anything else are Narrow (1-column).
    if lbc in ('ZW', 'CM'):
        if 0x00 <= u and u <= 0x1F or 0x7F <= u and u <= 0x9F:
            # ...but C0/C1 controls and DEL are 1-column.
            width = 1
        else:
            width = 0
    elif eaw in ('W', 'F'):
        width = 2
    else:
        width = 1

    # Surrogates and unknown characters are treated as AL.
    # AI are treated as AL.
    # CB and SA are ID (but about SA see LineBreak.CharacterProperties).
    if lbc in ('SG', 'XX'):
        lbc = 'AL'
    elif lbc in ('AI'):
        lbc = 'AL'
    elif lbc in ('CB', 'SA'):
        lbc = 'ID'

    propmap[u] = (width, lbc,)

###
### Customizations
###

CharacterProperties.customize(propmap)
legacy_maps = AmbiguousWidth.getmap(propmap)

###
### Output.
### 

allclasses = []

fp = file(LBPROPTAB, 'w')
print >>fp, """\
/*
 * CAUTION: Don't edit this file manually.  To update this file, read
 * %(myname)s.
 */

#include "unicode_linebreak.h"

/*
 * Unicode Character Property Table
 */
""" % { 'myname': MYNAME }

# Write generic property map.
print >>fp, "struct unicode_lbprop unicode_lbprop_tab[] = {"
num = printpropmap(fp, propmap, allclasses)
print >>fp, """\
    {0x20000, 0x2FFFD, 2, UNICODE_LBCLASS_ID},   /* Supplemental Ideographs */
    {0x30000, 0x3FFFD, 2, UNICODE_LBCLASS_ID},   /* Ideographs in the future */
    {0xE0000, 0xE01EF, 0, UNICODE_LBCLASS_CM},   /* Tags */
    {0xF0000, 0xFFFFD, 1, UNICODE_LBCLASS_AL},   /* Plane 15 Private Use */
    {0x100000, 0x10FFFD, 1, UNICODE_LBCLASS_AL}, /* Plane 16 Private Use */
    {0, 0, 0, 0}
};

size_t unicode_lbprop_len = %d;
""" % (num+5)

## Write property maps with custom widths.
lmkeys = legacy_maps.keys()
lmkeys.sort()
for r in lmkeys:
    print >>fp, "struct unicode_lbprop unicode_lbprop_%s_tab[] = {" % r
    num = printpropmap(fp, legacy_maps[r], allclasses)
    print >>fp, """\
    {0, 0, 0, 0}
};

size_t unicode_lbprop_%s_len = %d;
""" % (r, num)

## Generate line breaking class pair table and write it.
print >>fp, """\
/*
 * UAX #14 Line Breaking Class pair table.
 */

#define X  UNICODE_LBACTION_PROHIBITED
#define Xc UNICODE_LBACTION_COMBINING_PROHIBITED
#define Ic UNICODE_LBACTION_COMBINING_INDIRECT
#define I  UNICODE_LBACTION_INDIRECT
#define D  UNICODE_LBACTION_DIRECT
"""
table, rulemap = generate_lbclass_pair(ORDINAL_CLASSES, LINE_BREAKING_RULES)
printpairtable(fp, ORDINAL_CLASSES, table, rulemap)

print >>fp, """\
/*
 * Functions to get property
 */

static struct unicode_lbprop unicode_lbprop_unknown[1] =
{
	{0, 0, 1, UNICODE_LBCLASS_AL}
};

static struct unicode_lbprop *find_lbprop(struct unicode_lbprop *t,
					  size_t n, unicode_char c)
{
	struct unicode_lbprop *top = t;
	struct unicode_lbprop *bot = t + n - 1;
	struct unicode_lbprop *cur;
	
	while (top <= bot)
	{
		cur = top + (bot - top) / 2;
		if (c < (unicode_char)(cur->range_s))
			bot = cur-1;
		else if ((unicode_char)(cur->range_e) < c)
			top = cur+1;
		else
			return cur;
	}
	return NULL;
}

/* Generic */
struct unicode_lbprop *unicode_getprop_generic(unicode_char c)
{
	struct unicode_lbprop *ucp;
	if ((ucp = find_lbprop(unicode_lbprop_tab,
			       unicode_lbprop_len, c)) == NULL)
		ucp = unicode_lbprop_unknown;
	return ucp;
}
"""
for r in lmkeys:
    print >>fp, """\
struct unicode_lbprop *unicode_getprop_%(region)s(unicode_char c)
{
	struct unicode_lbprop *ucp;
	if ((ucp=find_lbprop(unicode_lbprop_%(region)s_tab,
			     unicode_lbprop_%(region)s_len, c)) == NULL &&
	    (ucp=find_lbprop(unicode_lbprop_tab,
			     unicode_lbprop_len, c)) == NULL)
		ucp = unicode_lbprop_unknown;
	return ucp;
}
""" % { 'region': r }

fp.close()

## Write header file.
fp = file(LBPROPHEADER, 'w')
allclasses.sort()
print >>fp, """\
/*
 * CAUTION: Don't edit this file manually.  To update this file, read
 * %(myname)s.
 */

#ifndef UNICODE_LINEBREAKTAB_H
#define UNICODE_LINEBREAKTAB_H

/*
 * UAX#14 Line Breaking Classes.
 */

enum unicode_lbclass
{
  /*
   * Classes appear in unicode_lbpairs[][] table.
   * NOTE: Order is important.
   */
""" % { 'myname': MYNAME }

initzero = ' = 0'
for c in ORDINAL_CLASSES:
    fp.write("  UNICODE_LBCLASS_%s%s" % (c, initzero))
    if initzero: initzero = ''
    fp.write(',\n')
print >>fp, """\
  /* Placeholder to indicate end of ordinal classes. */
  UNICODE_LBCLASS_TABLESIZE,

  /*
   * Below from here shouldn't appear in unicode_lbpairs[][] table.
   */

  /* SPACE */
  UNICODE_LBCLASS_SP,

  /* Explicit break. */
  %s,

  /*
   * Custom classes.  These should be solved to any of classes above
   * by tailor_lbclass().
   */

  %s
};
""" % (
    ',\n  '.join(['UNICODE_LBCLASS_'+c for c in EXPLICIT_BREAK_CLASSES]),
    ',\n  '.join(['UNICODE_LBCLASS_'+c for c in allclasses
                  if not c in ORDINAL_CLASSES+EXPLICIT_BREAK_CLASSES+['SP']]),
)
for c in ORDINAL_CLASSES:
    print >>fp, "#define UNICODE_LBCLASS_%s_DEFINED" % c
for c in allclasses:
    if not c in ORDINAL_CLASSES+EXPLICIT_BREAK_CLASSES+['SP']:
        print >>fp, "#define UNICODE_LBCLASS_%s_DEFINED" % c
print >>fp, ""

print >>fp, """\
extern struct unicode_lbprop *unicode_getprop_generic(unicode_char);"""
for r in lmkeys:
    print >>fp, """\
extern struct unicode_lbprop *unicode_getprop_%(region)s(unicode_char);""" % \
{ 'region': r }
print >>fp, ''
print >>fp, '#endif /* UNICODE_LINEBREAKTAB_H */'
fp.close()

## Write HTML pair table.
fp = file(LBRULEHTML, 'w')
print >>fp, """\
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN"
    "http://www.w3.org/TR/REC-html40/loose.dtd">
<html>
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <meta http-equiv="Content-Language" content="en-us">
    <meta name="GENERATOR" content="%(myname)s">
    <link rel="stylesheet" href="http://www.unicode.org/reports/reports.css">
    <title>
      UAX#14: Line Breaking Properties - Line Breaking Rules
    </title>
  </head>
  <body>
  <div class="body">

  <p>This file was generated automatically
  for convenience of verification.
  To customize line breaking rules, read %(myname)s.</p>

  <h2>Line Breaking Rules</h2>

  <blockquote>
    <p>! Mandatory break at the indicated position</p>
    <p>× No break allowed at the indicated position</p>
    <p>÷ Break allowed at the indicated position</p>
  </blockquote>
""" % { 'myname': MYNAME }

printlbrulehtml(fp, LINE_BREAKING_RULES)

print >>fp, """\
  <h2>Line Breaking Class Pair Table</h2>

  <p>Following table has been generated by rules above.</p>

  <div align="center">
"""

printpairhtml(fp, ORDINAL_CLASSES, table, rulemap)
print >>fp, """\
  </div>
  <blockquote>
    <p>This table uses the followng notation:</p>
    <p>^ denotes a <i>prohibited break</i>: B ^ A is equivalent to
      <b>B</b> SP* × <b>A</b>.</p>
    <p>% denotes an <i>indirect break opportunity</i>.
      B % A is equivalent to <b>B</b> × <b>A</b> <i>and</i>
      <b>B</b> SP+ ÷ <b>A</b>;
      in other words, do not break before A, unless one or more spaces
      follow B.</p>
    <p>@ denotes a <i>prohibited break for combining marks</i>:
      B @ A is equivalent to <b>B</b> SP* × <b>A</b>, where <b>A</b> is of
      class CM.</p>
    <p># denotes an <i>indirect break opportunity for combining marks
      following a space</i>. B # A is equivalent to
      (<b>B</b> × <b>A</b> <i>and</i> <b>B</b> SP+ ÷ <b>A</b>)
      where <b>A</b> is of cla5ss CM.</p>
    <p>_ denotes a <i>direct break opportunity</i>.</p>
  </blockquote>
  </div>
  </body>
</html>
"""
fp.close()
