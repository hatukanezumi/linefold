#-*- python -*-
#-*- coding: utf-8 -*-

"""
CharacterProperties.py - Customizations of UAX#14 Line Breaking
Properties and UAX#11 East Asian Width.

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

def customize(propmap):
    ## Revisions between rev. 17 (data version 4.1.0) and 19 (data version
    ## 5.0.0) of UAX#14.  Note: Only modifications of classification
    ## are followed.  Added characters are not followed.

    propmap.update( {
        0x000B: (1, 'BKVT'), # <control> vertical tab (tailorable) [CM]
        0x035C: (1, 'GL'), # COMBINING DOUBLE BREVE BELOW [CM]
        0x05BE: (1, 'BA'), # HEBREW PUNCTUATION MAQAF [AL]
        0x1735: (1, 'BA'), # PHILIPPINE SINGLE PUNCTUATION [AL]
        0x1736: (1, 'BA'), # PHILIPPINE DOUBLE PUNCTUATION [AL]
        0x17D9: (1, 'AL'), # KHMER SIGN PHNAEK MUAN [NS]
        0x203D: (1, 'NS'), # INTERROBANG [AL]
        0x2047: (1, 'NS'), # DOUBLE QUESTION MARK [AL]
        0x2048: (1, 'NS'), # QUESTION EXCLAMATION MARK [AL]
        0x2049: (1, 'NS'), # EXCLAMATION QUESTION MARK [AL]
        0x23B4: (1, 'AL'), # TOP SQUARE BRACKET [OP]
        0x23B5: (1, 'AL'), # BOTTOM SQUARE BRACKET [CL]
        0x23B6: (1, 'AL'), # BOTTOM SQUARE BRACKET OVER TOP SQUARE BRACKET [QU]
        0x103D0: (1, 'BA'), # OLD PERSIAN WORD DIVIDER [AL]
        } )

    ## Spanish inverted punctuations (tailorable).

    propmap.update( {
        0x00A1: (1, 'OPAL'), # INVERTED EXCLAMATION MARK
        0x00BF: (1, 'OPAL'), # INVERTED QUESTION MARK
        } )

    ## East Asian Scripts (Han, Hiragana, Katakana, Hangul).

    # Hangul combining jamo are Wide when they aren't combined.
    for k, v in propmap.items():
        if v[1] in ('JV', 'JT'):
            propmap[k] = (2, v[1])
        
    # Compatibilities between UAX#14 and JIS X 4051.
    # - EM DASH is classified as B2 by UAX and class (8) 分離禁止文字
    #   (inseparable characters; characters of this class by JIS allow
    #   break after AL, ID etc.) by JIS.  So vertical form also may be
    #   B2.
    # - TWO DOT LEADER and HORIZONTAL ELLIPSIS are classified as IN by
    #   UAX but class (8) 分離禁止文字 by JIS.  So they and their
    #   vertical forms might be B2 (tailorable).
    # - EN DASH is BA by UAX and class (4) ハイフン類 (hyphen class)
    #   by JIS.  So vertical form of it is also BA.
    # - Compound exclamations/questions are classified as class (5) 区
    #   切り約物 (separating symbols) by JIS, acompany with single
    #   exclamation / question marks.  So they might be EX
    #   (tailorable).
    #   N.B.: JIS doesn't assign INTERROBANG to this class because it
    #   isn't assigned to JIS X 0213.
    # - NUMBER SIGN is classified as class (9) 前置省略記号 (prefixing
    #   abbreviations) by JIS.  So only narrow form might be PR.  Its
    #   fullwidth counterpart is remained to be ID.
    # - Some characters are classified as class (19) 単位記号中の文字 
    #   (characters in physical units) by JIS.  So these characters
    #   may be classified as PO.
    #   N.B.: JIS assigns GREEK CAPITAL LETTER OMEGA and GREEK SMALL
    #   LETTER MU to this class, instead of OHM SIGN and MICRO SIGN.
    #   Because latters aren't assigned to JIS X 0213.
    # About these customizations viewed from JIS side, see
    # http://hatuka.nezumi.nu/log/2006/05/uax14_vs_jisx4051.html
    propmap.update( {
        0x0023: (1, 'PR'), # NUMBER SIGN
        0x00B5: (1, 'PO'), # MICRO SIGN (not in JIS: see note above)
        0x2025: (1, 'INB2'), # TWO DOT LEADER
        0x2026: (1, 'INB2'), # HORIZONTAL ELLIPSIS
        0x203C: (1, 'NSEX'), # DOUBLE EXCLAMATION MARK
        0x203D: (1, 'NSEX'), # INTERROBANG (not in JIS: see note above)
        0x2047: (1, 'NSEX'), # DOUBLE QUESTION MARK
        0x2048: (1, 'NSEX'), # QUESTION EXCLAMATION MARK
        0x2049: (1, 'NSEX'), # EXCLAMATION QUESTION MARK
        0x2113: (1, 'PO'), # SCRIPT SMALL L [liter]
        0x2126: (1, 'PO'), # OHM SIGN (not in JIS: see note above)
        0x2127: (1, 'PO'), # INVERTED OHM SIGN [mho]
        0x212B: (1, 'PO'), # ANGSTROM SIGN
        0x33CB: (2, 'PO'), # SQUARE HP [horse power]
        0xFE19: (2, 'INB2'), # PRESENTATION FORM FOR VERTICAL HORIZONTAL ELLIPSIS
        0xFE30: (2, 'INB2'), # PRESENTATION FORM FOR VERTICAL TWO DOT LEADER
        0xFE31: (2, 'B2'), # PRESENTATION FORM FOR VERTICAL EM DASH
        0xFE32: (2, 'BA'), # PRESENTATION FORM FOR VERTICAL EN DASH
        } )

    # Hiragana/katakana small letters and prolonged sign are treated
    # as NS.  They may be treated as ID (this behavior may be
    # tailored.  cf. JIS X 4051:2004, section 6.1.1).
    # Halfwidth form of katakana small letters and prolonged sign are
    # treated as AL, since other halfwidth katakana are AL.
    propmap.update( {
        0x3041: (2, 'NSK'), # HIRAGANA LETTER SMALL A
        0x3043: (2, 'NSK'), # HIRAGANA LETTER SMALL I
        0x3045: (2, 'NSK'), # HIRAGANA LETTER SMALL U
        0x3047: (2, 'NSK'), # HIRAGANA LETTER SMALL E
        0x3049: (2, 'NSK'), # HIRAGANA LETTER SMALL O
        0x3063: (2, 'NSK'), # HIRAGANA LETTER SMALL TU
        0x3083: (2, 'NSK'), # HIRAGANA LETTER SMALL YA
        0x3085: (2, 'NSK'), # HIRAGANA LETTER SMALL YU
        0x3087: (2, 'NSK'), # HIRAGANA LETTER SMALL YO
        0x308E: (2, 'NSK'), # HIRAGANA LETTER SMALL WA
        0x3095: (2, 'NSK'), # HIRAGANA LETTER SMALL KA
        0x3096: (2, 'NSK'), # HIRAGANA LETTER SMALL KE
        #0x309B: (2, 'NS'), # KATAKANA-HIRAGANA VOICED SOUND MARK
        #0x309C: (2, 'NS'), # KATAKANA-HIRAGANA SEMI-VOICED SOUND MARK
        0x30A1: (2, 'NSK'), # KATAKANA LETTER SMALL A
        0x30A3: (2, 'NSK'), # KATAKANA LETTER SMALL I
        0x30A5: (2, 'NSK'), # KATAKANA LETTER SMALL U
        0x30A7: (2, 'NSK'), # KATAKANA LETTER SMALL E
        0x30A9: (2, 'NSK'), # KATAKANA LETTER SMALL O
        0x30C3: (2, 'NSK'), # KATAKANA LETTER SMALL TU
        0x30E3: (2, 'NSK'), # KATAKANA LETTER SMALL YA
        0x30E5: (2, 'NSK'), # KATAKANA LETTER SMALL YU
        0x30E7: (2, 'NSK'), # KATAKANA LETTER SMALL YO
        0x30EE: (2, 'NSK'), # KATAKANA LETTER SMALL WA
        0x30F5: (2, 'NSK'), # KATAKANA LETTER SMALL KA
        0x30F6: (2, 'NSK'), # KATAKANA LETTER SMALL KE
        0x30FC: (2, 'NSK'), # KATAKANA-HIRAGANA PROLONGED SOUND MARK
        0x31F0: (2, 'NSK'), # KATAKANA LETTER SMALL KU
        0x31F1: (2, 'NSK'), # KATAKANA LETTER SMALL SI
        0x31F2: (2, 'NSK'), # KATAKANA LETTER SMALL SU
        0x31F3: (2, 'NSK'), # KATAKANA LETTER SMALL TO
        0x31F4: (2, 'NSK'), # KATAKANA LETTER SMALL NU
        0x31F5: (2, 'NSK'), # KATAKANA LETTER SMALL HA
        0x31F6: (2, 'NSK'), # KATAKANA LETTER SMALL HI
        0x31F7: (2, 'NSK'), # KATAKANA LETTER SMALL HU
        0x31F8: (2, 'NSK'), # KATAKANA LETTER SMALL HE
        0x31F9: (2, 'NSK'), # KATAKANA LETTER SMALL HO
        0x31FA: (2, 'NSK'), # KATAKANA LETTER SMALL MU
        0x31FB: (2, 'NSK'), # KATAKANA LETTER SMALL RA
        0x31FC: (2, 'NSK'), # KATAKANA LETTER SMALL RI
        0x31FD: (2, 'NSK'), # KATAKANA LETTER SMALL RU
        0x31FE: (2, 'NSK'), # KATAKANA LETTER SMALL RE
        0x31FF: (2, 'NSK'), # KATAKANA LETTER SMALL RO
        0xFF67: (1, 'AL'), # HALFWIDTH KATAKANA LETTER SMALL A
        0xFF68: (1, 'AL'), # HALFWIDTH KATAKANA LETTER SMALL I
        0xFF69: (1, 'AL'), # HALFWIDTH KATAKANA LETTER SMALL U
        0xFF6A: (1, 'AL'), # HALFWIDTH KATAKANA LETTER SMALL E
        0xFF6B: (1, 'AL'), # HALFWIDTH KATAKANA LETTER SMALL O
        0xFF6C: (1, 'AL'), # HALFWIDTH KATAKANA LETTER SMALL YA
        0xFF6D: (1, 'AL'), # HALFWIDTH KATAKANA LETTER SMALL YU
        0xFF6E: (1, 'AL'), # HALFWIDTH KATAKANA LETTER SMALL YO
        0xFF6F: (1, 'AL'), # HALFWIDTH KATAKANA LETTER SMALL TU
        0xFF70: (1, 'AL'), # HALFWIDTH KATAKANA-HIRAGANA PROLONGED SOUND MARK
        0xFF9E: (1, 'AL'), # HALFWIDTH KATAKANA VOICED SOUND MARK
        0xFF9F: (1, 'AL'), # HALFWIDTH KATAKANA SEMI-VOICED SOUND MARK
        } )

    # In traditional East Asian line folding, comma and full stop can
    # be protruded into the right -- bottom, in vertical line -- margin
    # (``ぶら下げ''; hunging punctuation) so that ``widow characters''
    # are reduced or simply aesthetism is satisfied.  We apply this
    # custom to Ideographic and Fullwidth Latin ones (this behavior
    # may be tailored).  To support these features, extended line
    # breaking classes are introduced (about CLHSP see also below).
    propmap.update( {
        0x3001: (2, 'CLHSP'), # IDEOGRAPHIC COMMA
        0x3002: (2, 'CLHSP'), # IDEOGRAPHIC FULL STOP
        0xFE11: (2, 'CLHSP'), # PRESENTATION FORM FOR VERTICAL IDEOGRAPHIC COMMA
        0xFE12: (2, 'CLHSP'), # PRESENTATION FORM FOR VERTICAL IDEOGRAPHIC FULL STOP
        0xFF0C: (2, 'CLHSP'), # FULLWIDTH COMMA
        0xFF0E: (2, 'CLHSP'), # FULLWIDTH FULL STOP
        0xFF61: (1, 'CLH'), # HALFWIDTH IDEOGRAPHIC FULL STOP
        0xFF64: (1, 'CLH'), # HALFWIDTH IDEOGRAPHIC COMMA
        } )

    # In East Asian text handling, Wide punctuations are often treated
    # as a Narrow character preceeded/followed by one SP (glue).  So,
    # they allow Direct Break where either Direct or Indirect break is
    # possible.  And so, Wide CL characters allow ``half''-hunging at
    # end of line.
    for k, v in propmap.items():
        if v[0] == 2:
            if v[1] == 'OP':
                propmap[k] = (v[0], 'SPOP')
            elif v[1] == 'CL':
                propmap[k] = (v[0], 'CLSP')

    # Usually IDEOGRAPHIC SPACE behaves as ID.  But in japanese
    # typsetting custom, exclamation/question marks at end of sentence
    # are followed by a space with width of one character (i.e. Wide
    # space); IDEOGRAPHIC SPACE is often used.  So don't break between
    # EX and IDEOGRAPHIC SPACE.  IDEOGRAPHIC SPACE is also often used
    # to justify text alignment.  So sequence of multiple IDEOGRAPHIC
    # SPACEs won't be broken.  Also, IDEOGRAPHIC SPACE can be protruded
    # into right (bottom) margin.
    propmap.update( {
        0x3000: (2, 'IDSP'), # IDEOGRAPHIC SPACE
        } )

    # Paired vertical kana repeat marks shouldn't be separable.  As a
    # workaround, assign them to B2.
    propmap.update( {
        0x3033: (2, 'B2'), # VERTICAL KANA REPEAT MARK UPPER HALF
        0x3034: (2, 'B2'), # VERTICAL KANA REPEAT WITH VOICED SOUND MARK UPPER HALF
        0x3035: (2, 'B2'), # VERTICAL KANA REPEAT MARK LOWER HALF
        } )


    ## Complex Line Breaking behavior on some South East Asian scripts
    ## (Thai, Lao, Myanmar, Khmer, Tai Le, New Tai Lue).

    # These characters are classified as SA by UAX#14.  Word separator
    # won't be used.  For workaround, assign classes as below:
    # o Vowels preceding consonant (in Thai and Lao):   BB.
    # o Semi-vowel consonant (in Thai and Lao):         CM with extra width.
    # o Subscript/superscript (semi-)vowels/consonants: CM.
    # o Independent volwels following consonant:        CM with extra width.
    # o Intermediate signs (mai han, virama, coeng):    GL with zero-width.
    # o Other subscripting/superscripting signs:        CM.
    # o Other letters:                                  ID.
    # This will avoid breaking within V-C(-T) and C-V(-T)(-subC) sequences.
    #
    # Note: This workaround is exactly insufficient.  Line breaks may
    #   occur within compound consonants, before closing consonants,
    #   etc.  To acomplish sufficient line breaking feature,
    #   morphologic analysises are required (about Thai implementation
    #   of such analysis see for example ``การตัดคำ'' (Word Cut Task) by
    #   สัตยมาศ, available at http://thaiwordseg.sourceforge.net/).

    propmap.update( {
        ## Thai
        # Consonants - semi-vowels and independent vowels for Sansakrit are CM.
        0x0E22: (1, 'CM'), # THAI CHARACTER YO YAK
        0x0E24: (1, 'CM'), # THAI CHARACTER RU
        0x0E26: (1, 'CM'), # THAI CHARACTER LU
        0x0E27: (1, 'CM'), # THAI CHARACTER WO WAEN
        0x0E2D: (1, 'CM'), # THAI CHARACTER O ANG
        # Ellipsis or sign for abbreviation.
        0x0E2F: (1, 'IN'), # THAI CHARACTER PAIYANNOI
        # Vowels superscripting/subscripting/following consonants are CM.
        # Mai han sign is GL.
        0x0E30: (1, 'CM'), # THAI CHARACTER SARA A
        0x0E31: (0, 'GL'), # THAI CHARACTER MAI HAN-AKAT
        0x0E32: (1, 'CM'), # THAI CHARACTER SARA AA
        0x0E33: (1, 'CM'), # THAI CHARACTER SARA AM
        0x0E34: (0, 'CM'), # THAI CHARACTER SARA I
        0x0E35: (0, 'CM'), # THAI CHARACTER SARA II
        0x0E36: (0, 'CM'), # THAI CHARACTER SARA UE
        0x0E37: (0, 'CM'), # THAI CHARACTER SARA UEE
        0x0E38: (0, 'CM'), # THAI CHARACTER SARA U
        0x0E39: (0, 'CM'), # THAI CHARACTER SARA UU
        0x0E3A: (0, 'CM'), # THAI CHARACTER PHINTHU
        # Vowels preceding consonants are BB.
        0x0E40: (1, 'BB'), # THAI CHARACTER SARA E
        0x0E41: (1, 'BB'), # THAI CHARACTER SARA AE
        0x0E42: (1, 'BB'), # THAI CHARACTER SARA O
        0x0E43: (1, 'BB'), # THAI CHARACTER SARA AI MAIMUAN
        0x0E44: (1, 'BB'), # THAI CHARACTER SARA AI MAIMALAI
        0x0E45: (1, 'BB'), # THAI CHARACTER LAKKHANGYAO
        # Repetition sign.
        0x0E46: (1, 'IN'), # THAI CHARACTER MAIYAMOK
        # Superscripting vowels and signs are CM.
        0x0E47: (0, 'CM'), # THAI CHARACTER MAITAIKHU
        0x0E48: (0, 'CM'), # THAI CHARACTER MAI EK
        0x0E49: (0, 'CM'), # THAI CHARACTER MAI THO
        0x0E4A: (0, 'CM'), # THAI CHARACTER MAI TRI
        0x0E4B: (0, 'CM'), # THAI CHARACTER MAI CHATTAWA
        0x0E4C: (0, 'CM'), # THAI CHARACTER THANTHAKHAT
        0x0E4D: (0, 'CM'), # THAI CHARACTER NIKHAHIT
        0x0E4E: (0, 'CM'), # THAI CHARACTER YAMAKKAN

        ## Lao
        # Same as Thai.  In addtion, U+0EBB-U+0EBD vowel/semi-vowel signs
        # are CM.
        0x0EA2: (1, 'CM'), # LAO LETTER YO
        0x0EA7: (1, 'CM'), # LAO LETTER WO
        0x0EAD: (1, 'CM'), # LAO LETTER O
        0x0EAF: (1, 'IN'), # LAO ELLIPSIS
        0x0EB0: (1, 'CM'), # LAO VOWEL SIGN A
        0x0EB1: (0, 'GL'), # LAO VOWEL SIGN MAI KAN
        0x0EB2: (1, 'CM'), # LAO VOWEL SIGN AA
        0x0EB3: (1, 'CM'), # LAO VOWEL SIGN AM
        0x0EB4: (0, 'CM'), # LAO VOWEL SIGN I
        0x0EB5: (0, 'CM'), # LAO VOWEL SIGN II
        0x0EB6: (0, 'CM'), # LAO VOWEL SIGN Y
        0x0EB7: (0, 'CM'), # LAO VOWEL SIGN YY
        0x0EB8: (0, 'CM'), # LAO VOWEL SIGN U
        0x0EB9: (0, 'CM'), # LAO VOWEL SIGN UU
        0x0EBB: (0, 'CM'), # LAO VOWEL SIGN MAI KON
        0x0EBC: (0, 'CM'), # LAO SEMIVOWEL SIGN LO
        0x0EBD: (1, 'CM'), # LAO SEMIVOWEL SIGN NYO
        0x0EC0: (1, 'BB'), # LAO VOWEL SIGN E
        0x0EC1: (1, 'BB'), # LAO VOWEL SIGN EI
        0x0EC2: (1, 'BB'), # LAO VOWEL SIGN O
        0x0EC3: (1, 'BB'), # LAO VOWEL SIGN AY
        0x0EC4: (1, 'BB'), # LAO VOWEL SIGN AI
        0x0EC6: (1, 'IN'), # LAO KO LA
        0x0EC8: (0, 'CM'), # LAO TONE MAI EK
        0x0EC9: (0, 'CM'), # LAO TONE MAI THO
        0x0ECA: (0, 'CM'), # LAO TONE MAI TI
        0x0ECB: (0, 'CM'), # LAO TONE MAI CATAWA
        0x0ECC: (0, 'CM'), # LAO CANCELLATION MARK
        0x0ECD: (0, 'CM'), # LAO NIGGAHITA
        0x0EDC: (1, 'ID'), # LAO HO NO
        0x0EDD: (1, 'ID'), # LAO HO MO

        ## Myanmar
        # Independent Vowels - CM, except MYANMAR LETTER A (glottal stop).
        0x1021: (1, 'ID'), # MYANMAR LETTER A
        0x1023: (1, 'CM'), # MYANMAR LETTER I
        0x1024: (1, 'CM'), # MYANMAR LETTER II
        0x1025: (1, 'CM'), # MYANMAR LETTER U
        0x1026: (1, 'CM'), # MYANMAR LETTER UU
        0x1027: (1, 'CM'), # MYANMAR LETTER E
        0x1029: (1, 'CM'), # MYANMAR LETTER O
        0x102A: (1, 'CM'), # MYANMAR LETTER AU
        # Dependent Vowel Signs - CM.  Virama is GL.
        0x102C: (1, 'CM'), # MYANMAR VOWEL SIGN AA
        0x102D: (0, 'CM'), # MYANMAR VOWEL SIGN I
        0x102E: (0, 'CM'), # MYANMAR VOWEL SIGN II
        0x102F: (0, 'CM'), # MYANMAR VOWEL SIGN U
        0x1030: (0, 'CM'), # MYANMAR VOWEL SIGN UU
        0x1031: (1, 'CM'), # MYANMAR VOWEL SIGN E
        0x1032: (0, 'CM'), # MYANMAR VOWEL SIGN AI
        0x1036: (0, 'CM'), # MYANMAR SIGN ANUSVARA
        0x1037: (0, 'CM'), # MYANMAR SIGN DOT BELOW
        0x1038: (1, 'CM'), # MYANMAR SIGN VISARGA
        0x1039: (0, 'GL'), # MYANMAR SIGN VIRAMA
        # Pali and Sanskrit extentions
        0x1050: (1, 'CM'), # MYANMAR LETTER SHA
        0x1051: (1, 'CM'), # MYANMAR LETTER SSA
        0x1052: (1, 'CM'), # MYANMAR LETTER VOCALIC R
        0x1053: (1, 'CM'), # MYANMAR LETTER VOCALIC RR
        0x1054: (1, 'CM'), # MYANMAR LETTER VOCALIC L
        0x1055: (1, 'CM'), # MYANMAR LETTER VOCALIC LL
        0x1056: (1, 'CM'), # MYANMAR VOWEL SIGN VOCALIC R
        0x1057: (1, 'CM'), # MYANMAR VOWEL SIGN VOCALIC RR
        0x1058: (0, 'CM'), # MYANMAR VOWEL SIGN VOCALIC L
        0x1059: (0, 'CM'), # MYANMAR VOWEL SIGN VOCALIC LL

        ## Khmer
        # Independent Vowels - CM.
        0x17A3: (1, 'CM'), # KHMER INDEPENDENT VOWEL QAQ
        0x17A4: (1, 'CM'), # KHMER INDEPENDENT VOWEL QAA
        0x17A5: (1, 'CM'), # KHMER INDEPENDENT VOWEL QI
        0x17A6: (1, 'CM'), # KHMER INDEPENDENT VOWEL QII
        0x17A7: (1, 'CM'), # KHMER INDEPENDENT VOWEL QU
        0x17A8: (1, 'CM'), # KHMER INDEPENDENT VOWEL QUK
        0x17A9: (1, 'CM'), # KHMER INDEPENDENT VOWEL QUU
        0x17AA: (1, 'CM'), # KHMER INDEPENDENT VOWEL QUUV
        0x17AB: (1, 'CM'), # KHMER INDEPENDENT VOWEL RY
        0x17AC: (1, 'CM'), # KHMER INDEPENDENT VOWEL RYY
        0x17AD: (1, 'CM'), # KHMER INDEPENDENT VOWEL LY
        0x17AE: (1, 'CM'), # KHMER INDEPENDENT VOWEL LYY
        0x17AF: (1, 'CM'), # KHMER INDEPENDENT VOWEL QE
        0x17B0: (1, 'CM'), # KHMER INDEPENDENT VOWEL QAI
        0x17B1: (1, 'CM'), # KHMER INDEPENDENT VOWEL QOO TYPE ONE
        0x17B2: (1, 'CM'), # KHMER INDEPENDENT VOWEL QOO TYPE TWO
        0x17B3: (1, 'CM'), # KHMER INDEPENDENT VOWEL QAU
        # Inherent Vowels - CM.
        0x17B4: (0, 'CM'), # KHMER VOWEL INHERENT AQ
        0x17B5: (0, 'CM'), # KHMER VOWEL INHERENT AA
        # Dependent Vowel Signs - CM.
        0x17B6: (1, 'CM'), # KHMER VOWEL SIGN AA
        0x17B7: (0, 'CM'), # KHMER VOWEL SIGN I
        0x17B8: (0, 'CM'), # KHMER VOWEL SIGN II
        0x17B9: (0, 'CM'), # KHMER VOWEL SIGN Y
        0x17BA: (0, 'CM'), # KHMER VOWEL SIGN YY
        0x17BB: (0, 'CM'), # KHMER VOWEL SIGN U
        0x17BC: (0, 'CM'), # KHMER VOWEL SIGN UU
        0x17BD: (0, 'CM'), # KHMER VOWEL SIGN UA
        # Two-part Dependent Vowel Signs - CM occupying 1 or 2 columns.
        0x17BE: (1, 'CM'), # KHMER VOWEL SIGN OE
        0x17BF: (2, 'CM'), # KHMER VOWEL SIGN YA
        0x17C0: (2, 'CM'), # KHMER VOWEL SIGN IE
        # Dependent Vowel Signs - CM.
        0x17C1: (1, 'CM'), # KHMER VOWEL SIGN E
        0x17C2: (1, 'CM'), # KHMER VOWEL SIGN AE
        0x17C3: (1, 'CM'), # KHMER VOWEL SIGN AI
        # Two-part Dependent Vowel Signs - CM occupying 2 columns.
        0x17C4: (2, 'CM'), # KHMER VOWEL SIGN OO
        0x17C5: (2, 'CM'), # KHMER VOWEL SIGN AU
        # other signs.
        0x17C6: (0, 'CM'), # KHMER SIGN NIKAHIT
        0x17C7: (1, 'CM'), # KHMER SIGN REAHMUK
        0x17C8: (1, 'CM'), # KHMER SIGN YUUKALEAPINTU
        # Consonant Shifters - CM.
        0x17C9: (0, 'CM'), # KHMER SIGN MUUSIKATOAN
        0x17CA: (0, 'CM'), # KHMER SIGN TRIISAP
        # Various Signs - CM.  coeng and viriam are GL.
        0x17CB: (0, 'CM'), # KHMER SIGN BANTOC
        0x17CC: (0, 'CM'), # KHMER SIGN ROBAT
        0x17CD: (0, 'CM'), # KHMER SIGN TOANDAKHIAT
        0x17CE: (0, 'CM'), # KHMER SIGN KAKABAT
        0x17CF: (0, 'CM'), # KHMER SIGN AHSDA
        0x17D0: (0, 'CM'), # KHMER SIGN SAMYOK SANNYA
        0x17D1: (0, 'GL'), # KHMER SIGN VIRIAM
        0x17D2: (0, 'GL'), # KHMER SIGN COENG
        0x17D3: (0, 'CM'), # KHMER SIGN BATHAMASAT
        # Repetition sign.
        0x17D7: (1, 'IN'), # KHMER SIGN LEK TOO
        # Avakraha sannya and atthacan is CM.
        0x17DC: (1, 'CM'), # KHMER SIGN AVAKRAHASANYA
        0x17DD: (0, 'CM'), # KHMER SIGN ATTHACAN

        ## Tai Le
        # N.B: This block is assigned to AL in data file version 4.1.0.
        # Consonants - ID.
        0x1950: (1, 'ID'), # TAI LE LETTER KA
        0x1951: (1, 'ID'), # TAI LE LETTER XA
        0x1952: (1, 'ID'), # TAI LE LETTER NGA
        0x1953: (1, 'ID'), # TAI LE LETTER TSA
        0x1954: (1, 'ID'), # TAI LE LETTER SA
        0x1955: (1, 'ID'), # TAI LE LETTER YA
        0x1956: (1, 'ID'), # TAI LE LETTER TA
        0x1957: (1, 'ID'), # TAI LE LETTER THA
        0x1958: (1, 'ID'), # TAI LE LETTER LA
        0x1959: (1, 'ID'), # TAI LE LETTER PA
        0x195A: (1, 'ID'), # TAI LE LETTER PHA
        0x195B: (1, 'ID'), # TAI LE LETTER MA
        0x195C: (1, 'ID'), # TAI LE LETTER FA
        0x195D: (1, 'ID'), # TAI LE LETTER VA
        0x195E: (1, 'ID'), # TAI LE LETTER HA
        0x195F: (1, 'ID'), # TAI LE LETTER QA
        0x1960: (1, 'ID'), # TAI LE LETTER KHA
        0x1961: (1, 'ID'), # TAI LE LETTER TSHA
        0x1962: (1, 'ID'), # TAI LE LETTER NA
        # Vowels & Tone letters - CM.
        0x1963: (1, 'CM'), # TAI LE LETTER A
        0x1964: (1, 'CM'), # TAI LE LETTER I
        0x1965: (1, 'CM'), # TAI LE LETTER EE
        0x1966: (1, 'CM'), # TAI LE LETTER EH
        0x1967: (1, 'CM'), # TAI LE LETTER U
        0x1968: (1, 'CM'), # TAI LE LETTER OO
        0x1969: (1, 'CM'), # TAI LE LETTER O
        0x196A: (1, 'CM'), # TAI LE LETTER UE
        0x196B: (1, 'CM'), # TAI LE LETTER E
        0x196C: (1, 'CM'), # TAI LE LETTER AUE
        0x196D: (1, 'CM'), # TAI LE LETTER AI
        0x1970: (1, 'CM'), # TAI LE LETTER TONE-2
        0x1971: (1, 'CM'), # TAI LE LETTER TONE-3
        0x1972: (1, 'CM'), # TAI LE LETTER TONE-4
        0x1973: (1, 'CM'), # TAI LE LETTER TONE-5
        0x1974: (1, 'CM'), # TAI LE LETTER TONE-6

        ## New Tai Lue
        # N.B: This block is assigned to AL in data file version 4.1.0.
        # Consonants - ID.
        0x1980: (1, 'ID'), # NEW TAI LUE LETTER HIGH QA
        0x1981: (1, 'ID'), # NEW TAI LUE LETTER LOW QA
        0x1982: (1, 'ID'), # NEW TAI LUE LETTER HIGH KA
        0x1983: (1, 'ID'), # NEW TAI LUE LETTER HIGH XA
        0x1984: (1, 'ID'), # NEW TAI LUE LETTER HIGH NGA
        0x1985: (1, 'ID'), # NEW TAI LUE LETTER LOW KA
        0x1986: (1, 'ID'), # NEW TAI LUE LETTER LOW XA
        0x1987: (1, 'ID'), # NEW TAI LUE LETTER LOW NGA
        0x1988: (1, 'ID'), # NEW TAI LUE LETTER HIGH TSA
        0x1989: (1, 'ID'), # NEW TAI LUE LETTER HIGH SA
        0x198A: (1, 'ID'), # NEW TAI LUE LETTER HIGH YA
        0x198B: (1, 'ID'), # NEW TAI LUE LETTER LOW TSA
        0x198C: (1, 'ID'), # NEW TAI LUE LETTER LOW SA
        0x198D: (1, 'ID'), # NEW TAI LUE LETTER LOW YA
        0x198E: (1, 'ID'), # NEW TAI LUE LETTER HIGH TA
        0x198F: (1, 'ID'), # NEW TAI LUE LETTER HIGH THA
        0x1990: (1, 'ID'), # NEW TAI LUE LETTER HIGH NA
        0x1991: (1, 'ID'), # NEW TAI LUE LETTER LOW TA
        0x1992: (1, 'ID'), # NEW TAI LUE LETTER LOW THA
        0x1993: (1, 'ID'), # NEW TAI LUE LETTER LOW NA
        0x1994: (1, 'ID'), # NEW TAI LUE LETTER HIGH PA
        0x1995: (1, 'ID'), # NEW TAI LUE LETTER HIGH PHA
        0x1996: (1, 'ID'), # NEW TAI LUE LETTER HIGH MA
        0x1997: (1, 'ID'), # NEW TAI LUE LETTER LOW PA
        0x1998: (1, 'ID'), # NEW TAI LUE LETTER LOW PHA
        0x1999: (1, 'ID'), # NEW TAI LUE LETTER LOW MA
        0x199A: (1, 'ID'), # NEW TAI LUE LETTER HIGH FA
        0x199B: (1, 'ID'), # NEW TAI LUE LETTER HIGH VA
        0x199C: (1, 'ID'), # NEW TAI LUE LETTER HIGH LA
        0x199D: (1, 'ID'), # NEW TAI LUE LETTER LOW FA
        0x199E: (1, 'ID'), # NEW TAI LUE LETTER LOW VA
        0x199F: (1, 'ID'), # NEW TAI LUE LETTER LOW LA
        0x19A0: (1, 'ID'), # NEW TAI LUE LETTER HIGH HA
        0x19A1: (1, 'ID'), # NEW TAI LUE LETTER HIGH DA
        0x19A2: (1, 'ID'), # NEW TAI LUE LETTER HIGH BA
        0x19A3: (1, 'ID'), # NEW TAI LUE LETTER LOW HA
        0x19A4: (1, 'ID'), # NEW TAI LUE LETTER LOW DA
        0x19A5: (1, 'ID'), # NEW TAI LUE LETTER LOW BA
        0x19A6: (1, 'ID'), # NEW TAI LUE LETTER HIGH KVA
        0x19A7: (1, 'ID'), # NEW TAI LUE LETTER HIGH XVA
        0x19A8: (1, 'ID'), # NEW TAI LUE LETTER LOW KVA
        0x19A9: (1, 'ID'), # NEW TAI LUE LETTER LOW XVA
        # Vowel Signs - CM.
        0x19B0: (1, 'CM'), # NEW TAI LUE VOWEL SIGN VOWEL SHORTENER
        0x19B1: (1, 'CM'), # NEW TAI LUE VOWEL SIGN AA
        0x19B2: (1, 'CM'), # NEW TAI LUE VOWEL SIGN II
        0x19B3: (1, 'CM'), # NEW TAI LUE VOWEL SIGN U
        0x19B4: (1, 'CM'), # NEW TAI LUE VOWEL SIGN UU
        0x19B5: (1, 'CM'), # NEW TAI LUE VOWEL SIGN E
        0x19B6: (2, 'CM'), # NEW TAI LUE VOWEL SIGN AE
        0x19B7: (1, 'CM'), # NEW TAI LUE VOWEL SIGN O
        0x19B8: (1, 'CM'), # NEW TAI LUE VOWEL SIGN OA
        0x19B9: (1, 'CM'), # NEW TAI LUE VOWEL SIGN UE
        0x19BA: (1, 'CM'), # NEW TAI LUE VOWEL SIGN AY
        0x19BB: (1, 'CM'), # NEW TAI LUE VOWEL SIGN AAY
        0x19BC: (1, 'CM'), # NEW TAI LUE VOWEL SIGN UY
        0x19BD: (1, 'CM'), # NEW TAI LUE VOWEL SIGN OY
        0x19BE: (1, 'CM'), # NEW TAI LUE VOWEL SIGN OAY
        0x19BF: (1, 'CM'), # NEW TAI LUE VOWEL SIGN UEY
        0x19C0: (1, 'CM'), # NEW TAI LUE VOWEL SIGN IY
        # Final Consonants - CM.
        0x19C1: (1, 'CM'), # NEW TAI LUE LETTER FINAL V
        0x19C2: (1, 'CM'), # NEW TAI LUE LETTER FINAL NG
        0x19C3: (1, 'CM'), # NEW TAI LUE LETTER FINAL N
        0x19C4: (1, 'CM'), # NEW TAI LUE LETTER FINAL M
        0x19C5: (1, 'CM'), # NEW TAI LUE LETTER FINAL K
        0x19C6: (1, 'CM'), # NEW TAI LUE LETTER FINAL D
        0x19C7: (1, 'CM'), # NEW TAI LUE LETTER FINAL B
        # Tone Marks - CM.
        0x19C8: (1, 'CM'), # NEW TAI LUE TONE MARK-1
        0x19C9: (1, 'CM'), # NEW TAI LUE TONE MARK-2
        # Various Signs - ID.
        0x19DE: (1, 'ID'), # NEW TAI LUE SIGN LAE
        0x19DF: (1, 'ID'), # NEW TAI LUE SIGN LAEV
        } )


    ## Other customizations.

    # FORM FEED may be either breakable or unbreakable (tailorable).
    propmap.update( {
        0x000C: (1, 'BKFF'), # <control> form feed
        } )

    # Characters in BMP Private Use Area are assumed as wide.  Since they
    # are often used, by some standardize organizations and font
    # manufacturers, for Ideographs not yet registered to Unicode
    # Standard.
    for u in range(0xE000, 0xF8FF+1):
        propmap[u] = (2, 'AL')
