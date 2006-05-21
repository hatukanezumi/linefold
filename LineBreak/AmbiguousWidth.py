#-*- python -*-
#-*- coding: utf-8 -*-

"""
AmbiguousWidth.py - Ambiguous widths for East Asian multibyte
character sets.

Copyright © 2006 by Hatuka*nezumi - IKEDA Soji <hatuka(at)nezumi.nu>,
redistributed under GNU General Public License version 2 (or later
version you prefer).

$id$
"""

# Several characters assigned A or N width properties by UAX#11 are
# Wide in fonts supporting ``legacy character sets'' --- Exactly say,
# widths depend on font metrics, rather than character sets.  Any of
# domestic character set standards *don't* define rigid character
# width property.  But width assignments are approximately reflect
# typesetting customs on where each domestic standards are establised.
# 
# Note:
#   Following map is based on limited survey on some East Asian fonts.
#   So updates are expected when any character sets or repertoires are
#   added or extended.
# 
# Keys abbreviate domestic standards which fontsets are based on:
#   G: GB - 国家标准, China PR.; GB2312, GB18030, CP936 (GBK).
#   J: JIS - 日本工業規格, Japan; JIS X 0208, JIS X 0212, JIS X 0213, CP932.
#   K: KS - 한국산업규격, Korea R.; KS X 1001, CP949 (UHC).
#   C: CNS - 中華民國國家標準, Taiwan; CNS 11643, Big5.

legacy_widths = {
    ## Latin-1 Supplement
    0x00A1: {               'K':2,        }, # INVERTED EXCLAMATION MARK
    0x00A4: { 'G':2,        'K':2,        }, # CURRENCY SIGN
    0x00A7: { 'G':2, 'J':2, 'K':2, 'C':2, }, # SECTION SIGN
    0x00A8: { 'G':2, 'J':2, 'K':2,        }, # DIAERESIS
    0x00AA: {               'K':2,        }, # FEMININE ORDINAL INDICATOR
    0x00AD: {               'K':2,        }, # SOFT HYPHEN
    0x00AE: {               'K':2,        }, # REGISTERED SIGN
    0x00B0: { 'G':2, 'J':2, 'K':2, 'C':2, }, # DEGREE SIGN
    0x00B1: { 'G':2, 'J':2, 'K':2, 'C':2, }, # PLUS-MINUS SIGN
    0x00B2: {               'K':2,        }, # SUPERSCRIPT TWO
    0x00B3: {               'K':2,        }, # SUPERSCRIPT THREE
    0x00B4: {        'J':2, 'K':2,        }, # ACUTE ACCENT
    0x00B6: {        'J':2, 'K':2,        }, # PILCROW SIGN
    0x00B7: { 'G':2,        'K':2, 'C':2, }, # MIDDLE DOT
    0x00B8: {               'K':2,        }, # CEDILLA
    0x00B9: {               'K':2,        }, # SUPERSCRIPT ONE
    0x00BA: {               'K':2,        }, # MASCULINE ORDINAL INDICATOR
    0x00BC: {               'K':2,        }, # VULGAR FRACTION ONE QUARTER
    0x00BD: {               'K':2,        }, # VULGAR FRACTION ONE HALF
    0x00BE: {               'K':2,        }, # VULGAR FRACTION THREE QUARTERS
    0x00BF: {               'K':2,        }, # INVERTED QUESTION MARK
    0x00C6: {               'K':2,        }, # LATIN CAPITAL LETTER AE
    0x00D0: {               'K':2,        }, # LATIN CAPITAL LETTER ETH
    0x00D7: { 'G':2, 'J':2, 'K':2, 'C':2, }, # MULTIPLICATION SIGN
    0x00D8: {               'K':2,        }, # LATIN CAPITAL LETTER O WITH STROKE
    0x00DE: {               'K':2,        }, # LATIN CAPITAL LETTER THORN
    0x00DF: {               'K':2,        }, # LATIN SMALL LETTER SHARP S
    0x00E6: {               'K':2,        }, # LATIN SMALL LETTER AE
    0x00F0: {               'K':2,        }, # LATIN SMALL LETTER ETH
    0x00F7: { 'G':2, 'J':2, 'K':2, 'C':2, }, # DIVISION SIGN
    0x00F8: {               'K':2,        }, # LATIN SMALL LETTER O WITH STROKE
    0x00FE: {               'K':2,        }, # LATIN SMALL LETTER THORN

    ## Latin Extended-A
    0x0111: {               'K':2,        }, # LATIN SMALL LETTER D WITH STROKE
    0x0126: {               'K':2,        }, # LATIN CAPITAL LETTER H WITH STROKE
    0x0127: {               'K':2,        }, # LATIN SMALL LETTER H WITH STROKE
    0x0131: {               'K':2,        }, # LATIN SMALL LETTER DOTLESS I
    0x0132: {               'K':2,        }, # LATIN CAPITAL LIGATURE IJ
    0x0133: {               'K':2,        }, # LATIN SMALL LIGATURE IJ
    0x0138: {               'K':2,        }, # LATIN SMALL LETTER KRA
    0x013F: {               'K':2,        }, # LATIN CAPITAL LETTER L WITH MIDDLE DOT
    0x0140: {               'K':2,        }, # LATIN SMALL LETTER L WITH MIDDLE DOT
    0x0141: {               'K':2,        }, # LATIN CAPITAL LETTER L WITH STROKE
    0x0142: {               'K':2,        }, # LATIN SMALL LETTER L WITH STROKE
    0x0149: {               'K':2,        }, # LATIN SMALL LETTER N PRECEDED BY APOSTROPHE
    0x014A: {               'K':2,        }, # LATIN CAPITAL LETTER ENG
    0x014B: {               'K':2,        }, # LATIN SMALL LETTER ENG
    0x0152: {               'K':2,        }, # LATIN CAPITAL LIGATURE OE
    0x0153: {               'K':2,        }, # LATIN SMALL LIGATURE OE
    0x0166: {               'K':2,        }, # LATIN CAPITAL LETTER T WITH STROKE
    0x0167: {               'K':2,        }, # LATIN SMALL LETTER T WITH STROKE

    ## Latin Extended-B
    0x01CD: {        'J':2,               }, # LATIN CAPITAL LETTER A WITH CARON
    0x01CE: {        'J':2,               }, # LATIN SMALL LETTER A WITH CARON
    0x01CF: {        'J':2,               }, # LATIN CAPITAL LETTER I WITH CARON
    0x01D0: {        'J':2,               }, # LATIN SMALL LETTER I WITH CARON
    0x01D1: {        'J':2,               }, # LATIN CAPITAL LETTER O WITH CARON
    0x01D2: {        'J':2,               }, # LATIN SMALL LETTER O WITH CARON
    0x01D3: {        'J':2,               }, # LATIN CAPITAL LETTER U WITH CARON
    0x01D4: {        'J':2,               }, # LATIN SMALL LETTER U WITH CARON
    0x01D5: {        'J':2,               }, # LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON
    0x01D6: {        'J':2,               }, # LATIN SMALL LETTER U WITH DIAERESIS AND MACRON
    0x01D7: {        'J':2,               }, # LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE
    0x01D8: {        'J':2,               }, # LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE
    0x01D9: {        'J':2,               }, # LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON
    0x01DA: {        'J':2,               }, # LATIN SMALL LETTER U WITH DIAERESIS AND CARON
    0x01DB: {        'J':2,               }, # LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE
    0x01DC: {        'J':2,               }, # LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE
    0x01F5: {        'J':2,               }, # LATIN SMALL LETTER G WITH ACUTE
    0x01F9: { 'G':2,                      }, # LATIN SMALL LETTER N WITH GRAVE

    ## Spacing Modifier Letters
    0x02C7: { 'G':2,        'K':2, 'C':2, }, # CARON
    0x02C9: { 'G':2,               'C':2, }, # MODIFIER LETTER MACRON
    0x02CA: { 'G':2,               'C':2, }, # MODIFIER LETTER ACUTE ACCENT
    0x02CB: { 'G':2,               'C':2, }, # MODIFIER LETTER GRAVE ACCENT
    0x02CD: {                      'C':2, }, # MODIFIER LETTER LOW MACRON
    0x02D0: {               'K':2,        }, # MODIFIER LETTER TRIANGULAR COLON
    0x02D8: {               'K':2,        }, # BREVE
    0x02D9: { 'G':2,        'K':2, 'C':2, }, # DOT ABOVE
    0x02DA: {               'K':2,        }, # RING ABOVE
    0x02DB: {               'K':2,        }, # OGONEK
    0x02DD: {               'K':2,        }, # DOUBLE ACUTE ACCENT

    ## Combining Diacritical Marks
    0x0305: {                      'C':2, }, # COMBINING OVERLINE

    ## Greek: See below.

    ## Cyrillic: See below.

    ## Hangul Jamo: See below.

    ## Mongolian
    0x1801: { 'G':2,                      }, # MONGOLIAN ELLIPSIS

    ## General Punctuation
    0x2001: {        'J':2,               }, # EM QUAD
    0x2003: {        'J':2,               }, # EM SPACE
    0x2010: { 'G':2, 'J':2,               }, # HYPHEN
    0x2013: { 'G':2,               'C':2, }, # EN DASH
    0x2014: { 'G':2, 'J':2, 'K':2, 'C':2, }, # EM DASH
    0x2015: { 'G':2, 'J':2, 'K':2,        }, # HORIZONTAL BAR
    0x2016: { 'G':2, 'J':2, 'K':2,        }, # DOUBLE VERTICAL LINE
    0x2018: { 'G':2, 'J':2, 'K':2, 'C':2, }, # LEFT SINGLE QUOTATION MARK
    0x2019: { 'G':2, 'J':2, 'K':2, 'C':2, }, # RIGHT SINGLE QUOTATION MARK
    0x201C: { 'G':2, 'J':2, 'K':2, 'C':2, }, # LEFT DOUBLE QUOTATION MARK
    0x201D: { 'G':2, 'J':2, 'K':2, 'C':2, }, # RIGHT DOUBLE QUOTATION MARK
    0x2020: {        'J':2, 'K':2,        }, # DAGGER
    0x2021: {        'J':2, 'K':2,        }, # DOUBLE DAGGER
    0x2022: {                      'C':2, }, # BULLET
    0x2024: {                      'C':2, }, # ONE DOT LEADER
    0x2025: { 'G':2, 'J':2, 'K':2, 'C':2, }, # TWO DOT LEADER
    0x2026: { 'G':2, 'J':2, 'K':2, 'C':2, }, # HORIZONTAL ELLIPSIS
    0x2027: {                      'C':2, }, # HYPHENATION POINT
    0x2030: { 'G':2, 'J':2, 'K':2,        }, # PER MILLE SIGN
    0x2032: { 'G':2, 'J':2, 'K':2, 'C':2, }, # PRIME
    0x2033: { 'G':2, 'J':2, 'K':2,        }, # DOUBLE PRIME
    0x2035: { 'G':2,               'C':2, }, # REVERSED PRIME
    0x203B: { 'G':2, 'J':2, 'K':2, 'C':2, }, # REFERENCE MARK

    ## Superscripts and Subscripts
    0x2074: {               'K':2,        }, # SUPERSCRIPT FOUR
    0x207F: {               'K':2,        }, # SUPERSCRIPT LATIN SMALL LETTER N
    0x2081: {               'K':2,        }, # SUBSCRIPT ONE
    0x2082: {               'K':2,        }, # SUBSCRIPT TWO
    0x2083: {               'K':2,        }, # SUBSCRIPT THREE
    0x2084: {               'K':2,        }, # SUBSCRIPT FOUR

    ## Currency Symbols
    0x20A0: {                      'C':2, }, # EURO-CURRENCY SIGN
    0x20A1: {                      'C':2, }, # COLON SIGN
    0x20A2: {                      'C':2, }, # CRUZEIRO SIGN
    0x20A3: {                      'C':2, }, # FRENCH FRANC SIGN
    0x20A4: {                      'C':2, }, # LIRA SIGN
    0x20A5: {                      'C':2, }, # MILL SIGN
    0x20A6: {                      'C':2, }, # NAIRA SIGN
    0x20A7: {                      'C':2, }, # PESETA SIGN
    0x20A8: {                      'C':2, }, # RUPEE SIGN
    0x20A9: {                      'C':2, }, # WON SIGN
    0x20AA: {                      'C':2, }, # NEW SHEQEL SIGN
    0x20AB: {                      'C':2, }, # DONG SIGN
    0x20AC: { 'G':2,        'K':2, 'C':2, }, # EURO SIGN

    ## Letterlike Symbols
    0x2103: { 'G':2, 'J':2, 'K':2, 'C':2, }, # DEGREE CELSIUS
    0x2105: { 'G':2,               'C':2, }, # CARE OF
    0x2109: { 'G':2,        'K':2, 'C':2, }, # DEGREE FAHRENHEIT
    0x2113: {               'K':2,        }, # SCRIPT SMALL L
    0x2116: { 'G':2, 'J':2, 'K':2,        }, # NUMERO SIGN
    0x2121: { 'G':2, 'J':2, 'K':2,        }, # TELEPHONE SIGN
    0x2122: {               'K':2,        }, # TRADE MARK SIGN
    0x2126: {               'K':2,        }, # OHM SIGN
    0x212B: {        'J':2, 'K':2,        }, # ANGSTROM SIGN

    ## Number Forms
    0x2153: {               'K':2,        }, # VULGAR FRACTION ONE THIRD
    0x2154: {               'K':2,        }, # VULGAR FRACTION TWO THIRDS
    0x215B: {               'K':2,        }, # VULGAR FRACTION ONE EIGHTH
    0x215C: {               'K':2,        }, # VULGAR FRACTION THREE EIGHTHS
    0x215D: {               'K':2,        }, # VULGAR FRACTION FIVE EIGHTHS
    0x215E: {               'K':2,        }, # VULGAR FRACTION SEVEN EIGHTHS
    # Roman numerals: see below.

    ## Arrows
    0x2190: { 'G':2, 'J':2, 'K':2, 'C':2, }, # LEFTWARDS ARROW
    0x2191: { 'G':2, 'J':2, 'K':2, 'C':2, }, # UPWARDS ARROW
    0x2192: { 'G':2, 'J':2, 'K':2, 'C':2, }, # RIGHTWARDS ARROW
    0x2193: { 'G':2, 'J':2, 'K':2, 'C':2, }, # DOWNWARDS ARROW
    0x2194: {               'K':2,        }, # LEFT RIGHT ARROW
    0x2195: {               'K':2,        }, # UP DOWN ARROW
    0x2196: { 'G':2,        'K':2, 'C':2, }, # NORTH WEST ARROW
    0x2197: { 'G':2,        'K':2, 'C':2, }, # NORTH EAST ARROW
    0x2198: { 'G':2,        'K':2, 'C':2, }, # SOUTH EAST ARROW
    0x2199: { 'G':2,        'K':2, 'C':2, }, # SOUTH WEST ARROW
    0x21D2: {        'J':2, 'K':2,        }, # RIGHTWARDS DOUBLE ARROW
    0x21D4: {        'J':2, 'K':2,        }, # LEFT RIGHT DOUBLE ARROW

    ## Mathematical Operators
    0x2200: {        'J':2, 'K':2,        }, # FOR ALL
    0x2202: {        'J':2, 'K':2,        }, # PARTIAL DIFFERENTIAL
    0x2203: {        'J':2, 'K':2,        }, # THERE EXISTS
    0x2207: {        'J':2, 'K':2,        }, # NABLA
    0x2208: { 'G':2, 'J':2, 'K':2,        }, # ELEMENT OF
    0x220B: {        'J':2, 'K':2,        }, # CONTAINS AS MEMBER
    0x220F: { 'G':2,        'K':2,        }, # N-ARY PRODUCT
    0x2211: { 'G':2, 'J':2, 'K':2,        }, # N-ARY SUMMATION
    0x2215: { 'G':2,               'C':2, }, # DIVISION SLASH
    0x2218: {                      'C':2, }, # RING OPERATOR
    0x2219: {                      'C':2, }, # BULLET OPERATOR
    0x221A: { 'G':2, 'J':2, 'K':2, 'C':2, }, # SQUARE ROOT
    0x221D: { 'G':2, 'J':2, 'K':2,        }, # PROPORTIONAL TO
    0x221E: { 'G':2, 'J':2, 'K':2, 'C':2, }, # INFINITY
    0x221F: { 'G':2, 'J':2,        'C':2, }, # RIGHT ANGLE
    0x2220: { 'G':2, 'J':2, 'K':2, 'C':2, }, # ANGLE
    0x2223: { 'G':2,               'C':2, }, # DIVIDES
    0x2225: { 'G':2, 'J':2, 'K':2, 'C':2, }, # PARALLEL TO
    0x2227: { 'G':2, 'J':2, 'K':2,        }, # LOGICAL AND
    0x2228: { 'G':2, 'J':2, 'K':2,        }, # LOGICAL OR
    0x2229: { 'G':2, 'J':2, 'K':2, 'C':2, }, # INTERSECTION
    0x222A: { 'G':2, 'J':2, 'K':2, 'C':2, }, # UNION
    0x222B: { 'G':2, 'J':2, 'K':2, 'C':2, }, # INTEGRAL
    0x222C: {        'J':2, 'K':2,        }, # DOUBLE INTEGRAL
    0x222E: { 'G':2, 'J':2, 'K':2, 'C':2, }, # CONTOUR INTEGRAL
    0x2234: { 'G':2, 'J':2, 'K':2, 'C':2, }, # THEREFORE
    0x2235: { 'G':2, 'J':2, 'K':2, 'C':2, }, # BECAUSE
    0x2236: { 'G':2,                      }, # RATIO
    0x2237: { 'G':2,                      }, # PROPORTION
    0x223C: {               'K':2,        }, # TILDE OPERATOR
    0x223D: { 'G':2, 'J':2, 'K':2,        }, # REVERSED TILDE
    0x2248: { 'G':2,                      }, # ALMOST EQUAL TO
    0x224C: { 'G':2,                      }, # ALL EQUAL TO
    0x2252: { 'G':2, 'J':2, 'K':2, 'C':2, }, # APPROXIMATELY EQUAL TO OR THE IMAGE OF
    0x2260: { 'G':2, 'J':2, 'K':2, 'C':2, }, # NOT EQUAL TO
    0x2261: { 'G':2, 'J':2, 'K':2, 'C':2, }, # IDENTICAL TO
    0x2263: {                      'C':2, }, # STRICTLY EQUIVALENT TO
    0x2264: { 'G':2,        'K':2,        }, # LESS-THAN OR EQUAL TO
    0x2265: { 'G':2,        'K':2,        }, # GREATER-THAN OR EQUAL TO
    0x2266: { 'G':2, 'J':2,        'C':2, }, # LESS-THAN OVER EQUAL TO
    0x2267: { 'G':2, 'J':2,        'C':2, }, # GREATER-THAN OVER EQUAL TO
    0x226A: {        'J':2, 'K':2,        }, # MUCH LESS-THAN
    0x226B: {        'J':2, 'K':2,        }, # MUCH GREATER-THAN
    0x226E: { 'G':2,                      }, # NOT LESS-THAN
    0x226F: { 'G':2,                      }, # NOT GREATER-THAN
    0x2282: {        'J':2, 'K':2,        }, # SUBSET OF
    0x2283: {        'J':2, 'K':2,        }, # SUPERSET OF
    0x2286: {        'J':2, 'K':2,        }, # SUBSET OF OR EQUAL TO
    0x2287: {        'J':2, 'K':2,        }, # SUPERSET OF OR EQUAL TO
    0x2295: { 'G':2,               'C':2, }, # CIRCLED PLUS
    0x2299: { 'G':2,        'K':2, 'C':2, }, # CIRCLED DOT OPERATOR
    0x22A5: { 'G':2, 'J':2, 'K':2, 'C':2, }, # UP TACK
    0x22BF: { 'G':2, 'J':2,        'C':2, }, # RIGHT TRIANGLE

    ## Miscellaneous Technical
    0x2312: { 'G':2, 'J':2, 'K':2,        }, # ARC

    ## Enclosed Alphanumerics: See below.

    ## Box Drawing: See also below.
    0x2500: {        'J':2, 'K':2, 'C':2, }, # BOX DRAWINGS LIGHT HORIZONTAL
    0x2501: {        'J':2, 'K':2, 'C':2, }, # BOX DRAWINGS HEAVY HORIZONTAL
    0x2502: {        'J':2, 'K':2, 'C':2, }, # BOX DRAWINGS LIGHT VERTICAL
    0x2503: {        'J':2, 'K':2,        }, # BOX DRAWINGS HEAVY VERTICAL
    0x250C: {        'J':2,        'C':2, }, # BOX DRAWINGS LIGHT DOWN AND RIGHT
    0x250F: {        'J':2,               }, # BOX DRAWINGS HEAVY DOWN AND RIGHT
    0x2510: {        'J':2,        'C':2, }, # BOX DRAWINGS LIGHT DOWN AND LEFT
    0x2513: {        'J':2,               }, # BOX DRAWINGS HEAVY DOWN AND LEFT
    0x2514: {        'J':2,        'C':2, }, # BOX DRAWINGS LIGHT UP AND RIGHT
    0x2517: {        'J':2,               }, # BOX DRAWINGS HEAVY UP AND RIGHT
    0x2518: {        'J':2,        'C':2, }, # BOX DRAWINGS LIGHT UP AND LEFT
    0x251B: {        'J':2,               }, # BOX DRAWINGS HEAVY UP AND LEFT
    0x251C: {        'J':2,        'C':2, }, # BOX DRAWINGS LIGHT VERTICAL AND RIGHT
    0x251D: {        'J':2,        'C':2, }, # BOX DRAWINGS VERTICAL LIGHT AND RIGHT HEAVY
    0x2520: {        'J':2,               }, # BOX DRAWINGS VERTICAL HEAVY AND RIGHT LIGHT
    0x2523: {        'J':2,               }, # BOX DRAWINGS HEAVY VERTICAL AND RIGHT
    0x2524: {        'J':2,        'C':2, }, # BOX DRAWINGS LIGHT VERTICAL AND LEFT
    0x2525: {        'J':2,        'C':2, }, # BOX DRAWINGS VERTICAL LIGHT AND LEFT HEAVY
    0x2528: {        'J':2,               }, # BOX DRAWINGS VERTICAL HEAVY AND LEFT LIGHT
    0x252B: {        'J':2,               }, # BOX DRAWINGS HEAVY VERTICAL AND LEFT
    0x252C: {        'J':2,        'C':2, }, # BOX DRAWINGS LIGHT DOWN AND HORIZONTAL
    0x252F: {        'J':2,               }, # BOX DRAWINGS DOWN LIGHT AND HORIZONTAL HEAVY
    0x2530: {        'J':2,               }, # BOX DRAWINGS DOWN HEAVY AND HORIZONTAL LIGHT
    0x2533: {        'J':2,               }, # BOX DRAWINGS HEAVY DOWN AND HORIZONTAL
    0x2534: {        'J':2,        'C':2, }, # BOX DRAWINGS LIGHT UP AND HORIZONTAL
    0x2537: {        'J':2,               }, # BOX DRAWINGS UP LIGHT AND HORIZONTAL HEAVY
    0x2538: {        'J':2,               }, # BOX DRAWINGS UP HEAVY AND HORIZONTAL LIGHT
    0x253B: {        'J':2,               }, # BOX DRAWINGS HEAVY UP AND HORIZONTAL
    0x253C: {        'J':2,        'C':2, }, # BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL
    0x253F: {        'J':2,        'C':2, }, # BOX DRAWINGS VERTICAL LIGHT AND HORIZONTAL HEAVY
    0x2542: {        'J':2,               }, # BOX DRAWINGS VERTICAL HEAVY AND HORIZONTAL LIGHT
    0x254B: {        'J':2,               }, # BOX DRAWINGS HEAVY VERTICAL AND HORIZONTAL

    ## Block Elements
    0x2581: { 'G':2,               'C':2, }, # LOWER ONE EIGHTH BLOCK
    0x2582: { 'G':2,               'C':2, }, # LOWER ONE QUARTER BLOCK
    0x2583: { 'G':2,               'C':2, }, # LOWER THREE EIGHTHS BLOCK
    0x2584: { 'G':2,               'C':2, }, # LOWER HALF BLOCK
    0x2585: { 'G':2,               'C':2, }, # LOWER FIVE EIGHTHS BLOCK
    0x2586: { 'G':2,               'C':2, }, # LOWER THREE QUARTERS BLOCK
    0x2587: { 'G':2,               'C':2, }, # LOWER SEVEN EIGHTHS BLOCK
    0x2588: { 'G':2,               'C':2, }, # FULL BLOCK
    0x2589: { 'G':2,               'C':2, }, # LEFT SEVEN EIGHTHS BLOCK
    0x258A: { 'G':2,               'C':2, }, # LEFT THREE QUARTERS BLOCK
    0x258B: { 'G':2,               'C':2, }, # LEFT FIVE EIGHTHS BLOCK
    0x258C: { 'G':2,               'C':2, }, # LEFT HALF BLOCK
    0x258D: { 'G':2,               'C':2, }, # LEFT THREE EIGHTHS BLOCK
    0x258E: { 'G':2,               'C':2, }, # LEFT ONE QUARTER BLOCK
    0x258F: { 'G':2,               'C':2, }, # LEFT ONE EIGHTH BLOCK
    0x2592: {               'K':2,        }, # MEDIUM SHADE
    0x2593: { 'G':2,               'C':2, }, # DARK SHADE
    0x2594: { 'G':2,               'C':2, }, # UPPER ONE EIGHTH BLOCK
    0x2595: { 'G':2,               'C':2, }, # RIGHT ONE EIGHTH BLOCK

    ## Geometric Shapes
    0x25A0: { 'G':2, 'J':2, 'K':2, 'C':2, }, # BLACK SQUARE
    0x25A1: { 'G':2, 'J':2, 'K':2, 'C':2, }, # WHITE SQUARE
    0x25A3: {               'K':2,        }, # WHITE SQUARE CONTAINING BLACK SMALL SQUARE
    0x25A4: {               'K':2,        }, # SQUARE WITH HORIZONTAL FILL
    0x25A5: {               'K':2,        }, # SQUARE WITH VERTICAL FILL
    0x25A6: {               'K':2,        }, # SQUARE WITH ORTHOGONAL CROSSHATCH FILL
    0x25A7: {               'K':2,        }, # SQUARE WITH UPPER LEFT TO LOWER RIGHT FILL
    0x25A8: {               'K':2,        }, # SQUARE WITH UPPER RIGHT TO LOWER LEFT FILL
    0x25A9: {               'K':2,        }, # SQUARE WITH DIAGONAL CROSSHATCH FILL
    0x25B2: { 'G':2, 'J':2, 'K':2, 'C':2, }, # BLACK UP-POINTING TRIANGLE
    0x25B3: { 'G':2, 'J':2, 'K':2, 'C':2, }, # WHITE UP-POINTING TRIANGLE
    0x25B6: {               'K':2,        }, # BLACK RIGHT-POINTING TRIANGLE
    0x25B7: {               'K':2,        }, # WHITE RIGHT-POINTING TRIANGLE
    0x25BC: { 'G':2, 'J':2, 'K':2, 'C':2, }, # BLACK DOWN-POINTING TRIANGLE
    0x25BD: { 'G':2, 'J':2, 'K':2, 'C':2, }, # WHITE DOWN-POINTING TRIANGLE
    0x25C0: {               'K':2,        }, # BLACK LEFT-POINTING TRIANGLE
    0x25C1: {               'K':2,        }, # WHITE LEFT-POINTING TRIANGLE
    0x25C6: { 'G':2, 'J':2, 'K':2, 'C':2, }, # BLACK DIAMOND
    0x25C7: { 'G':2, 'J':2, 'K':2, 'C':2, }, # WHITE DIAMOND
    0x25C8: {               'K':2,        }, # WHITE DIAMOND CONTAINING BLACK SMALL DIAMOND
    0x25CB: { 'G':2, 'J':2, 'K':2, 'C':2, }, # WHITE CIRCLE
    0x25CE: { 'G':2, 'J':2, 'K':2, 'C':2, }, # BULLSEYE
    0x25CF: { 'G':2, 'J':2, 'K':2, 'C':2, }, # BLACK CIRCLE
    0x25D0: {               'K':2,        }, # CIRCLE WITH LEFT HALF BLACK
    0x25D1: {               'K':2,        }, # CIRCLE WITH RIGHT HALF BLACK
    0x25E2: { 'G':2,               'C':2, }, # BLACK LOWER RIGHT TRIANGLE
    0x25E3: { 'G':2,               'C':2, }, # BLACK LOWER LEFT TRIANGLE
    0x25E4: { 'G':2,               'C':2, }, # BLACK UPPER LEFT TRIANGLE
    0x25E5: { 'G':2,               'C':2, }, # BLACK UPPER RIGHT TRIANGLE
    0x25EF: {        'J':2,               }, # LARGE CIRCLE

    ## Miscellaneous Symbols: See also below.
    # Playing card symbols, etc.
    0x2660: {               'K':2,        }, # BLACK SPADE SUIT
    0x2661: {               'K':2,        }, # WHITE HEART SUIT
    0x2663: {               'K':2,        }, # BLACK CLUB SUIT
    0x2664: {               'K':2,        }, # WHITE SPADE SUIT
    0x2665: {               'K':2,        }, # BLACK HEART SUIT
    0x2667: {               'K':2,        }, # WHITE CLUB SUIT
    0x2668: {        'J':2, 'K':2,        }, # HOT SPRINGS
    0x2669: {               'K':2,        }, # QUARTER NOTE
    0x266A: {        'J':2, 'K':2,        }, # EIGHTH NOTE
    0x266C: {               'K':2,        }, # BEAMED SIXTEENTH NOTES
    0x266D: {        'J':2, 'K':2,        }, # MUSIC FLAT SIGN
    0x266F: {        'J':2,               }, # MUSIC SHARP SIGN

    ## Dingbats: See below.

    ## Specials
    0xFFFC: {               'K':2,        }, # OBJECT REPLACEMENT CHARACTER
    0xFFFD: {        'J':2,               }, # REPLACEMENT CHARACTER
}
# Greek - Some basic letters are Wide.
for u in range(0x0391,0x03A9+1)+range(0x03B1,0x03C9+1):
    if u in (0x03A2, 0x03C2): continue       # except FINAL SIGMA
    if not legacy_widths.has_key(u): legacy_widths[u] = {}
    for r in ('G', 'J', 'K', 'C',):
        legacy_widths[u][r] = 2
# Cyrillic - Some basic letters are Wide.
for u in [0x0401]+range(0x0410,0x044F+1)+[0x0451]:
    if not legacy_widths.has_key(u): legacy_widths[u] = {}
    for r in ('G', 'J', 'K',    ):
        legacy_widths[u][r] = 2
# Hangul Combining Jamo - Wide.
for u in range(0x115A,0x11F9+1):
    if not legacy_widths.has_key(u): legacy_widths[u] = {}
    for r in ('G', 'J', 'K', 'C',):
        legacy_widths[u][r] = 2
# Roman Numerals - Wide.
for u in range(0x2160,0x2182+1):
    if not legacy_widths.has_key(u): legacy_widths[u] = {}
    for r in ('G', 'J', 'K', 'C',):
        legacy_widths[u][r] = 2
# Circled/Parenthesized/Full-stopped Alphanumerics - Wide.
for u in range(0x2460,0x24FF+1):
    if not legacy_widths.has_key(u): legacy_widths[u] = {}
    for r in ('G', 'J', 'K', 'C',):
        legacy_widths[u][r] = 2
# Box Drawings
for u in range(0x2500,0x2573+1):
    if not legacy_widths.has_key(u): legacy_widths[u] = {}
    legacy_widths[u]['G'] = 2
for u in range(0x250C,0x254B+1):
    if not legacy_widths.has_key(u): legacy_widths[u] = {}
    legacy_widths[u]['K'] = 2
for u in range(0x2550,0x2574+1):
    if not legacy_widths.has_key(u): legacy_widths[u] = {}
    legacy_widths[u]['C'] = 2
# Miscelaneous symbols - Wide.
for u in range(0x2600,0x265F+1):
    if not legacy_widths.has_key(u): legacy_widths[u] = {}
    for r in ('G', 'J', 'K', 'C',):
        legacy_widths[u][r] = 2
# DingBats - Wide.
for u in range(0x2700,0x27BF+1):
    if not legacy_widths.has_key(u): legacy_widths[u] = {}
    for r in ('G', 'J', 'K', 'C',):
        legacy_widths[u][r] = 2


def getmap(propmap):
    """
    Traverse legacy width map then generate map of legacy propmaps.
    """
    legacy_maps = {}
    for u, v in legacy_widths.items():
        for r in v.keys():
            if not legacy_maps.has_key(r):
                legacy_maps[r] = {}
            width = legacy_widths[u][r]
            lbc = propmap.get(u, (1,'XX'))[1]
            # Wide AL (or AI) characters are ID, except alphabetic letters.
            if width == 2:
                if lbc in ('AL', 'AI', 'XX'):
                    if u > 0x2000:
                        lbc = 'ID'
                    else:
                        lbc = 'AL'
            legacy_maps[r][u] = (width, lbc)

    # In East Asian box-filled folding context, especially on narrow or
    # fixed-width faces, ellipsises occationally allow Break Before but
    # are inseparable between themselves.  As a workaround, assign them to
    # BB.  Vertical double-width kana repeat marks will be handled by same
    # manner.
    # Note:
    # - If it is preferred that breaks within multi-width ellipsises are
    #   allowed, change classes to 'ID'.
    # - If alphabetic behavior is preferred, change classes to 'IN'.
    for r in legacy_maps.keys():
        legacy_maps[r].update( {
            0x2026: (2, 'BB'), # TWO DOT LEADER
            0x2027: (2, 'BB'), # HORIZONTAL ELLIPSIS
            0x3033: (2, 'BB'), # VERTICAL KANA REPEAT MARK UPPER HALF
            0x3034: (2, 'BB'), # VERTICAL KANA REPEAT WITH VOICED SOUND MARK UPPER HALF
            0x3035: (2, 'BB'), # VERTICAL KANA REPEAT MARK LOWER HALF
            0xFE30: (2, 'BB'), # PRESENTATION FORM FOR VERTICAL TWO DOT LEADER
            0xFE19: (2, 'BB'), # PRESENTATION FORM FOR VERTICAL HORIZONTAL ELLIPSIS
            } )

    return legacy_maps
