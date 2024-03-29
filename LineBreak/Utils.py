#-*- python -*-
#-*- coding: utf-8 -*-

"""
Utils.py - Utility Functions.

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
"""

from types import DictType, ListType
import cgi

from LineBreak.Rules import *


def readunidata(fp):
    ret = {}
    for line in fp:
        i = line.find('#')
        if i >= 0:
            line = line[:i]
        line = line.strip()
        if not line:
            continue
        code, val = line.split(';', 1)
        rnge = code.split('..')
        if len(rnge) > 1:
            start, end = rnge[0:2]
        else:
            start, end = (rnge[0], rnge[0])
        start = int(start, 16)
        end = int(end, 16)

        for c in range(start, end+1):
            ret[c] = val
    return ret

def printpropmap(fp, propmap, class_idx):
    num4 = num2 = num1 = None
    num = 0

    keys = propmap.keys()
    keys.sort()
    s = None
    e = None
    for k in keys:
        v = propmap[k]
        if s is None:
            s = (k, v)
            e = (k, v)
            continue
        elif e[0] <= 0xFF and k >= 0x100:
            num1 = num + 1
        elif e[0] <= 0xFFFF and k >= 0x10000:
            num2 = num + 1
        elif e[0]+1 == k and e[1] == v:
            e = (k, v)
            continue
        print >>fp, '    {0x%04X, 0x%04X, %d, %2d /* %s */},' % \
              (s[0], e[0], s[1][0], class_idx[s[1][1]], s[1][1])
        if e[0] <= 0xFF and k >= 0x100:
            print >>fp, '#if SIZEOF_LINEFOLD_CHAR > 1'
        elif e[0] <= 0xFFFF and k >= 0x10000:
            print >>fp, '#if SIZEOF_LINEFOLD_CHAR > 2'
        s = (k, v)
        e = (k, v)
        num += 1
    if s is not None:
        print >>fp, '    {0x%04X, 0x%04X, %d, %2d /* %s */},' % \
              (s[0], e[0], s[1][0], class_idx[s[1][1]], s[1][1])
        num += 1
    if num4 is None:
        num4 = num
    if num2 is None:
        num2 = num
    else:
        print >>fp, '#endif /* SIZEOF_LINEFOLD_CHAR > 2 */'
    if num1 is None:
        num1 = num
    else:
        print >>fp, '#endif /* SIZEOF_LINEFOLD_CHAR > 1 */'
    return num4, num2, num1

def generate_lbclass_pair(classes, lbrules):
    table = {}
    rulemap = {}
    for before in classes:
        if not table.has_key(before):
            table[before] = {}
            rulemap[before] = {}

        obefore = before
        if before == 'CM':
            before_is_CM = True
            before = 'AL'
        else:
            before_is_CM = False

        for after in classes:
            oafter = after
            if after == 'CM':
                after_is_CM = True
            else:
                after_is_CM = False

            rulename = None
            rules = []
            direct = None
            indirect = None
            for rule in lbrules:
                if isinstance(rule, DictType):
                    rulename = rule.get('name', '?')
                    continue

                b, r, a = rule
                if before in b and after in a:
                    if r == XCMasX:
                        if direct is None: direct = False
                        rules.append((rulename, r, obefore, oafter))
                        continue
                    elif r == CMasAL:
                        after = 'AL'
                        rules.append((rulename, r, obefore, oafter))
                        continue
                    elif r == prohSP:
                        if direct is None or indirect is None:
                            if direct is None: direct = False
                            if indirect is None: indirect = False
                            rules.append((rulename, r, obefore, oafter))
                    elif r == prohib:
                        if direct is None:
                            direct = False
                            rules.append((rulename, r, obefore, oafter))
                    elif r == allows:
                        if direct is None:
                            direct = True
                            rules.append((rulename, r, obefore, oafter))
                if 'SP' in b and after in a:
                    if r == allows:
                        if indirect is None:
                            indirect = True
                            rules.append((rulename, r, 'SP', oafter))
                    elif r == prohib or r == prohSP:
                        if indirect is None:
                            indirect = False
                            rules.append((rulename, r, 'SP', oafter))
                if direct is not None and indirect is not None:
                    break

            if direct and indirect:
                table[obefore][oafter] = DIRECT
            elif not direct and indirect:
                if after_is_CM:
                    table[obefore][oafter] = COMBINING_INDIRECT
                else:
                    table[obefore][oafter] = INDIRECT
            elif not direct and not indirect:
                if after_is_CM:
                    table[obefore][oafter] = COMBINING_PROHIBITED
                else:
                    table[obefore][oafter] = PROHIBITED
            elif direct and not indirect:
                # Allow direct but not allow indirect.  Lack of rules?
                table[obefore][oafter] = DIRECT
            rulemap[obefore][oafter] = rules
    return table, rulemap

def printpairtable(fp, classes, table, rulemap):
    for before in classes:
        fp.write('static const linefold_action row_%s[] =\n  {' % before)
        for after in classes:
            r = table[before][after]
            if r == DIRECT:
                fp.write('D')
            elif r == INDIRECT:
                fp.write('I')
            elif r == COMBINING_INDIRECT:
                fp.write('Ic')
            elif r == COMBINING_PROHIBITED:
                fp.write('Xc')
            elif r == PROHIBITED:
                fp.write('X')
            else:
                fp.write('?')
            if after <> classes[-1]:
                fp.write(',')
        fp.write('};\n')
    print >>fp, '''
const linefold_action *linefold_lbpairs[] = 
{
  %s
};
''' % ',\n  '.join(['row_'+c for c in classes])


def fmtrule(ruletuple):
    rulename, r, before, after = ruletuple
    if rulename:
        return '%s: %s' % (rulename, r % {'B': before, 'A': after})
    else:
        return '%s' % (r % {'B': before, 'A': after})

def printpairhtml(fp, classes, table, rulemap):
    print >>fp, """\
<table border="1" cellspacing="0" width="88%%" class="pair">
  <caption>Line Breaking Class Pair Table</caption>
  <thead>
  <tr>
    <td>&nbsp;</td>
    <th style="text-align: center; font-weight: bold;">%s</th>
  </tr>
  </thead>
  <tbody>
""" % '</th>\n    <th style="text-align: center; font-weight: bold;">'.join(\
        classes)

    for before in classes:
        print >>fp, """\
  <tr>
    <th style="text-align: center; font-weight: bold;">%s</th>
""" % before
        for after in classes:
            rulestr = \
            ' ; '.join([fmtrule(r) for r in rulemap[before].get(after, [])])
            print >>fp, """\
    <th style="text-align: center;" title="%s">%s</th>
""" % (cgi.escape(rulestr), table[before].get(after, '?'))
    print >>fp, """\
  </tr>
"""
    print >>fp, """\
  </tbody>
</table>
"""

def printlbrulehtml(fp, rules):
    def fmtclassset(c):
        if c == ALL:
            return 'ALL'
        elif c == ALL_except_SP:
            return '(!SP)'
        elif c == ALL_except_SP_and_ZW:
            return '(!(SP|BK|CR|LF|NL|ZW))'
        elif len(c) <> 1:
            return '( ' + (' | '.join(c)) + ' )'
        else:
            return ' | '.join(c)

    rulename = None
    ruledesc = None
    for rule in rules:
        if isinstance(rule, DictType):
            rulename = rule.get('name', '?')
            ruledesc = rule.get('desc', '')
            print >>fp, \
                  '    <p class="rule">LB %s: %s</p>' % \
                  (rulename, ruledesc)
            continue
        b, r, a = rule
        before = fmtclassset(b)
        after = fmtclassset(a)
        if before <> after:
            if before == 'ALL': before = ''
            if after == 'ALL': after = ''
        print >>fp, '    <p align="center">%s</p>' % \
              cgi.escape(fmtrule((None, r, before, after)))
