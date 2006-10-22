#-*- python -*-
#-*- coding: utf-8 -*-

"""
Rules.py - Definitions of UAX#14 Line Breaking Rules.

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

This is based on ``Pair Table Implementation'' described in UAX#14.
"""

# Classes appear in pair table.
ORDINAL_CLASSES = [
    'OP', 'CL', 'QU', 'GL', 'NS', 'EX', 'SY', 'IS', 'PR', 'PO', 'NU', 'AL',
    'ID', 'IN', 'HY', 'BA', 'BB', 'B2', 'ZW', 'CM', 'WJ',
    # Korean syllables and combining jamos.
    'H2', 'H3', 'JL', 'JV', 'JT',
    # Extended classes.
    'IDSP',
    'SPOP', 'CLH', 'CLSP', 'CLHSP',
    ]
# Classes for explicit break. THESE SHOULDN'T BE MODIFIED.
EXPLICIT_BREAK_CLASSES = [
    'BK', 'CR', 'LF', 'NL',
    ]

# set of classes
ALL = ORDINAL_CLASSES+['SP']
ALL_except_SP = [c for c in ALL
                 if not c in ['SP', 'CLSP', 'CLHSP']]
ALL_except_SP_and_ZW = [c for c in ALL
                        if not c in ['SP', 'CLSP', 'CLHSP', 'ZW']]
# rules.
allows = '%(B)s ÷ %(A)s'
prohSP = '%(B)s SP* × %(A)s'
prohib = '%(B)s × %(A)s'
XCMasX = 'X CM* -> X'
CMasAL = 'X CM CM+ -> X AL+'
# rules for pair table.
DIRECT = '_'
INDIRECT = '%'
COMBINING_INDIRECT = '#'
PROHIBITED = '^'
COMBINING_PROHIBITED = '@'


###
### List of rules by UAX#14 rev. 19 (Unicode 5.0.0).
###

LINE_BREAKING_RULES = [
    { 'name': "7",
    'desc': "Do not break before spaces or zero-width space.", },
    (ALL, prohib, ['SP', 'ZW']),

    { 'name': "8",
      'desc': "Break after zero-width space.", },
    (['ZW'], allows, ALL),

    { 'name': "9",
      'desc': """Do not break a combining character sequence; treat it
      as if it has the LB class of the base character in all of the
      following rules.""", },
    (ALL_except_SP_and_ZW, XCMasX, ['CM']),

    { 'name': "10",
      'desc': "Treat any remaining combining mark as AL.", },
    (['AL', 'CM'], CMasAL, ['CM']),

    { 'name': "11",
      'desc': """Do not break before or after WORD JOINER and related
      characters.""", },
    (ALL, prohib, ['WJ']),
    (['WJ'], prohib, ALL),

    { 'name': "12",
      'desc': "Do not break before or after NBSP and related characters.", },
    (ALL_except_SP, prohib, ['GL']),
    (['GL'], prohib, ALL),

    { 'name': "13",
      'desc': """Do not break before ‘]’ or ‘!’ or ‘;’ or ‘/’, even
      after spaces.""", },
    (ALL, prohib, ['CL', 'EX', 'IS', 'SY']),
    { 'name': "13ext", 'desc': 'ditto.', },
    (ALL, prohib, ['CLH', 'CLSP', 'CLHSP']),        # extended

    { 'name': "14",
      'desc': "Do not break after ‘[’, even after spaces.", },
    (['OP'], prohSP, ALL),
    { 'name': "14ext", 'desc': 'ditto.', },
    (['SPOP'], prohSP, ALL),                        # extended

    { 'name': "15",
      'desc': "Do not break within ‘”[’, even with intervening spaces.", },
    (['QU'], prohSP, ['OP']),
    { 'name': "15ext", 'desc': 'ditto.', },
    (['QU'], prohSP, ['SPOP']),                     # extended

    { 'name': "16",
      'desc': "Do not break within ‘]h’, even with intervening spaces.", },
    (['CL'], prohSP, ['NS']),
    { 'name': "16ext", 'desc': 'ditto.', },
    (['CLH', 'CLSP', 'CLHSP'], prohSP, ['NS']),     # extended

    { 'name': "17",
      'desc': "Do not break within ‘——’, even with intervening spaces.", },
    (['B2'], prohSP, ['B2']),

    { 'name': "18excIDSP",
      'desc': "Do not break within sequence of ideographic spaces.", },
    (['IDSP'], prohib, ['IDSP']),                   # extended

    { 'name': "18",
      'desc': "Break after spaces.", },
    (['SP'], allows, ALL),

    { 'name': "18extCLSP",
      'desc': """...and also break after closing punctuations with
      virtual glue.""", },
    (['CLSP', 'CLHSP'], allows, ALL),               # extended

    { 'name': "19",
      'desc': "Do not break before or after ‘”’.", },
    (ALL, prohib, ['QU']),
    (['QU'], prohib, ALL),

    { 'name': "20",
      'desc': "Break before and after unresolved CB.", },
    (ALL, allows, ['CB']),
    (['CB'], allows, ALL),

    { 'name': "21",
      'desc': """Do not break before hyphen-minus, other hyphens,
      fixed-width spaces, small kana and other non-starters, or after
      acute accents.""", },
    (ALL, prohib, ['BA', 'HY', 'NS']),
    (['BB'], prohib, ALL),

    { 'name': "22",
      'desc': """Do not break between two ellipses, or between letters
      or numbers and ellipsis.""", },
    (['AL', 'ID', 'IN', 'NU'], prohib, ['IN']),
    { 'name': "22ext", 'desc': "ditto.", },
    (['IDSP'], prohib, ['IN']),                     # extended

    { 'name': "23",
      'desc': "Do not break within ‘a9’, ‘3a’, or ‘H%’.", },
    (['ID'], prohib, ['PO']),
    (['AL'], prohib, ['NU']),
    (['NU'], prohib, ['AL']),
    { 'name': "23ext", 'desc': "ditto.", },
    (['IDSP'], prohib, ['PO']),                     # extended

    { 'name': "24",
      'desc': "Do not break between prefix and letters or ideographs.", },
    (['PR'], prohib, ['ID']),
    (['PR'], prohib, ['AL']),
    (['PO'], prohib, ['AL']),
    { 'name': "24ext", 'desc': "ditto.", },
    (['PR'], prohib, ['IDSP']),                     # extended

    { 'name': "25",
      'desc': """Do not break between the following pairs of classes
      relevant to numbers:""", },
    (['CL', 'NU'], prohib, ['PO', 'PR']),
    (['PO', 'PR'], prohib, ['OP', 'NU']),
    (['HY','IS', 'NU', 'SY'], prohib, ['NU']),
    { 'name': "25ext", 'desc': 'ditto.', },
    (['CLH'], prohib, ['PO', 'PR']),                # extended

    { 'name': "25excSPOP",
      'desc': """...but allow break before opening punctuations with
      virtual glue.""", },
    (['PO', 'PR'], allows, ['SPOP']),               # extended

    { 'name': "26",
      'desc': "Do not break a Korean syllable.", },
    (['JL'], prohib, ['JL', 'JV', 'H2', 'H3']),
    (['JV', 'H2'], prohib, ['JV', 'JT']),
    (['JT', 'H3'], prohib, ['JT']),

    { 'name': "27",
      'desc': "Treat a Korean Syllable Block the same as ID.", },
    (['JL', 'JV', 'JT', 'H2', 'H3'], prohib, ['IN']),
    (['JL', 'JV', 'JT', 'H2', 'H3'], prohib, ['PO']),
    (['PR'], prohib, ['JL', 'JV', 'JT', 'H2', 'H3']),

    { 'name': "28",
      'desc': "Do not break between alphabetics (“at”).", },
    (['AL'], prohib, ['AL']),

    { 'name': "29",
      'desc': """Do not break between numeric punctuation and
      alphabetics ("e.g.").""", },
    (['IS'], prohib, ['AL']),

    { 'name': "30",
      'desc': """Do not break between letters, numbers or ordinary
      symbols and opening or closing punctuation""", },
    (['AL', 'NU'], prohib, ['OP']),
    (['CL'], prohib, ['AL', 'NU']),
    { 'name': "30ext", 'desc': 'ditto.', },
    (['CLH'], prohib, ['AL', 'NU']),                # extended

    { 'name': "31excIDSP",
      'desc': """Do not break between exclamation/question marks and
      ideographic space.""", },
    (['EX'], prohib, ['IDSP']),                     # extended

    { 'name': "31",
      'desc': "Break everywhere else.", },
    (ALL, allows, ALL),
]
