#! python
#-*- coding: utf-8 -*-

"""
mklbproptab.py - Generate linefoldtab.c & linefoldtab.h

Copyright (C) 2006 by Hatuka*nezumi - IKEDA Soji.  All rights reserved.

This file is part of the Linefold Package.  This program is free
software; you can redistribute it and/or modify it under the terms of
the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any
later version.  This program is distributed in the hope that it will
be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
COPYING file for more details.

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
LBPROPTAB = 'linefoldtab.c'
LBPROPHEADER = 'linefoldtab.h'
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
for eaw, lbc in propmap.values():
    if lbc not in allclasses:
        allclasses.append(lbc)
for m in legacy_maps.values():
    for eaw, lbc in m.values():
        if lbc not in allclasses:
            allclasses.append(lbc)
allclasses.sort()        

idx = 0
class_idx = {}
for c in ORDINAL_CLASSES:
    class_idx[c] = idx
    idx += 1
class_idx['SP'] = idx
idx += 1
for c in EXPLICIT_BREAK_CLASSES:
    class_idx[c] = idx
    idx += 1
for c in [c for c in allclasses
          if not c in ORDINAL_CLASSES+EXPLICIT_BREAK_CLASSES+['SP']]:
    class_idx[c] = idx
    idx += 1


fp = file(LBPROPTAB, 'w')
print >>fp, """\
/*
 * NOTICE: Don't edit this file manually.  To update this file, read
 * %(myname)s.
 */

#include "common.h"
#include "linefold.h"
""" % { 'myname': MYNAME, }

idx = 0
print >>fp, """\
/*
 * Classes appear in linefold_lbpairs[][] table.
 * NOTE: Order is important.
 */
"""
for c in ORDINAL_CLASSES:
    print >>fp, "const linefold_class LINEFOLD_CLASS_%s = %2d;" % (c, idx)
    idx += 1
print >>fp, ""

print >>fp, """\
/* Placeholder to indicate end of ordinal classes. */
const linefold_class LINEFOLD_CLASS_TABLESIZE = %2d;
""" % idx

print >>fp, """\
/*
 * Below from here shouldn't appear in linefold_lbpairs[][] table.
 */

/* SPACE */
const linefold_class LINEFOLD_CLASS_SP = %2d;
""" % idx
idx += 1
print >>fp, ""

print >>fp, """\
/* Explicit break. */
"""
for c in EXPLICIT_BREAK_CLASSES:
    print >>fp, "const linefold_class LINEFOLD_CLASS_%s = %2d;" % (c, idx)
    idx += 1
print >>fp, ""

for c in [c for c in allclasses
          if not c in ORDINAL_CLASSES+EXPLICIT_BREAK_CLASSES+['SP']]:
    idx += 1
print >>fp, """\
/* User defined class. */

const linefold_class LINEFOLD_CLASS_USERDEFINED = %2d;
""" % idx

print >>fp, """\
/*
 * Character properties related on line breaking behavior.
 */
typedef struct
{
	linefold_char   range_s;
	linefold_char   range_e;
	int             width;        /* UAX#11 East Asian Width */
	linefold_class  lbclass;      /* UAX#14 Line Breaking Class */
} lbprop;

/*
 * Unicode Character Property Table
 */
"""

# Write generic property map.
print >>fp, "static lbprop lbprop_tab[] = {"
num4, num2, num1 = printpropmap(fp, propmap, class_idx)
print >>fp, """\
#if SIZEOF_LINEFOLD_CHAR > 2
    {0x20000, 0x2FFFD, 2, %(ID)2d /* ID */},   /* Supplemental Ideographs */
    {0x30000, 0x3FFFD, 2, %(ID)2d /* ID */},   /* Ideographs in the future */
    {0xE0000, 0xE01EF, 0, %(CM)2d /* CM */},   /* Tags */
    {0xF0000, 0xFFFFD, 1, %(AL)2d /* AL */},   /* Plane 15 Private Use */
    {0x100000, 0x10FFFD, 1, %(AL)2d /* AL */}, /* Plane 16 Private Use */
#endif /* SIZEOF_LINEFOLD_CHAR > 2 */
    {0, 0, 0, 0}
};

#if SIZEOF_LINEFOLD_CHAR > 2
static size_t lbprop_len = %(num4)d;
#elif SIZEOF_LINEFOLD_CHAR > 1
static size_t lbprop_len = %(num2)d;
#else
static size_t lbprop_len = %(num1)d;
#endif /* SIZEOF_LINEFOLD_CHAR */
""" % { 'num4': num4+5,
        'num2': num2,
        'num1': num1,
        'AL': class_idx['AL'],
        'CM': class_idx['CM'],
        'ID': class_idx['ID'],
        }

## Write property maps with custom widths.
lmkeys = legacy_maps.keys()
lmkeys.sort()
for r in lmkeys:
    print >>fp, "static lbprop lbprop_%s_tab[] = {" % r
    num4, num2, num1 = printpropmap(fp, legacy_maps[r], class_idx)
    print >>fp, """\
    {0, 0, 0, 0}
};

#if SIZEOF_LINEFOLD_CHAR > 2
static size_t lbprop_%(r)s_len = %(num4)d;
#elif SIZEOF_LINEFOLD_CHAR > 1
static size_t lbprop_%(r)s_len = %(num2)d;
#else
static size_t lbprop_%(r)s_len = %(num1)d;
#endif /* SIZEOF_LINEFOLD_CHAR */
""" % {'r': r, 'num4': num4, 'num2': num2, 'num1': num1,}

## Generate line breaking class pair table and write it.
print >>fp, """\
/*
 * UAX #14 Line Breaking Class pair table.
 */

#define X  LINEFOLD_ACTION_PROHIBITED
#define Xc LINEFOLD_ACTION_COMBINING_PROHIBITED
#define Ic LINEFOLD_ACTION_COMBINING_INDIRECT
#define I  LINEFOLD_ACTION_INDIRECT
#define D  LINEFOLD_ACTION_DIRECT
"""
table, rulemap = generate_lbclass_pair(ORDINAL_CLASSES, LINE_BREAKING_RULES)
printpairtable(fp, ORDINAL_CLASSES, table, rulemap)

print >>fp, """\
/*
 * Functions to get property
 */

static lbprop lbprop_unknown[1] =
{
	{0, 0, 1, %(AL)2d /* AL */}
};

static lbprop *find_lbprop(lbprop *t, size_t n, linefold_char c)
{
	lbprop *top = t;
	lbprop *bot = t + n - 1;
	lbprop *cur;
	
	while (top <= bot)
	{
		cur = top + (bot - top) / 2;
		if (c < (linefold_char)(cur->range_s))
			bot = cur-1;
		else if ((linefold_char)(cur->range_e) < c)
			top = cur+1;
		else
			return cur;
	}
	return NULL;
}

/* Generic */
void linefold_getprop_generic(linefold_char c,
                              linefold_width *widthptr, linefold_class *lbcptr)
{
	lbprop *ucp;
	if ((ucp = find_lbprop(lbprop_tab, lbprop_len, c)) == NULL)
		ucp = lbprop_unknown;
	if (widthptr)
		*widthptr = ucp->width;
	if (lbcptr)
		*lbcptr = ucp->lbclass;
}
""" % { 'AL': class_idx['AL'], }
for r in lmkeys:
    print >>fp, """\
void linefold_getprop_%(region)s(linefold_char c,
                        linefold_width *widthptr, linefold_class *lbcptr)
{
	lbprop *ucp;
	if ((ucp=find_lbprop(lbprop_%(region)s_tab, lbprop_%(region)s_len, c)) == NULL &&
	    (ucp=find_lbprop(lbprop_tab, lbprop_len, c)) == NULL)
		ucp = lbprop_unknown;
	if (widthptr)
		*widthptr = ucp->width;
	if (lbcptr)
		*lbcptr = ucp->lbclass;
}
""" % { 'region': r }

fp.close()

## Write header file.
fp = file(LBPROPHEADER, 'w')
print >>fp, """\
/*
 * NOTICE: Don't edit this file manually.  To update this file, read
 * %(myname)s.
 */

#ifndef LINEFOLDTAB_H
#define LINEFOLDTAB_H

/*
 * UAX#14 Line Breaking Classes.
 */

""" % { 'myname': MYNAME }

idx = 0
print >>fp, """\
/*
 * Classes appear in linefold_lbpairs[][] table.
 * NOTE: Order is important.
 */
"""
for c in ORDINAL_CLASSES:
    print >>fp, "extern const linefold_class LINEFOLD_CLASS_%s;" % c
    idx += 1
print >>fp, ""

print >>fp, """\
/* Placeholder to indicate end of ordinal classes. */
extern const linefold_class LINEFOLD_CLASS_TABLESIZE;
"""

print >>fp, """\
/*
 * Below from here shouldn't appear in linefold_lbpairs[][] table.
 */

/* SPACE */
extern const linefold_class LINEFOLD_CLASS_SP;
"""
idx += 1

print >>fp, """\
/* Explicit break. */
"""
for c in EXPLICIT_BREAK_CLASSES:
    print >>fp, "extern const linefold_class LINEFOLD_CLASS_%s;" % c
    idx += 1
print >>fp, ""

print >>fp, """\
/*
 * Custom classes.  These should be solved to any of classes above
 * by tailor_lbclass().
 */
"""
for c in [c for c in allclasses
          if not c in ORDINAL_CLASSES+EXPLICIT_BREAK_CLASSES+['SP']]:
    print >>fp, "#define LINEFOLD_CLASS_%s %d" % (c, idx)
    idx += 1
print >>fp, ""

print >>fp, """\
/* User defined class. */

extern const linefold_class LINEFOLD_CLASS_USERDEFINED;
"""

print >>fp, """\
extern void linefold_getprop_generic(linefold_char,
                                     linefold_width *, linefold_class *);"""
for r in lmkeys:
    print >>fp, """\
extern void linefold_getprop_%(region)s(linefold_char,
                               linefold_width *, linefold_class *);\
""" % { 'region': r }
print >>fp, ''
print >>fp, '#endif /* LINEFOLDTAB_H */'
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
