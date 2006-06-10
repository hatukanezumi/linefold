/*
 * NOTICE: Don't edit this file manually.  To update this file, read
 * mklbproptab.py.
 */

#ifndef LINEFOLDTAB_H
#define LINEFOLDTAB_H

/*
 * UAX#14 Line Breaking Classes.
 */


/*
 * Classes appear in linefold_lbpairs[][] table.
 * NOTE: Order is important.
 */

#define LINEFOLD_CLASS_OP 0
#define LINEFOLD_CLASS_CL 1
#define LINEFOLD_CLASS_QU 2
#define LINEFOLD_CLASS_GL 3
#define LINEFOLD_CLASS_NS 4
#define LINEFOLD_CLASS_EX 5
#define LINEFOLD_CLASS_SY 6
#define LINEFOLD_CLASS_IS 7
#define LINEFOLD_CLASS_PR 8
#define LINEFOLD_CLASS_PO 9
#define LINEFOLD_CLASS_NU 10
#define LINEFOLD_CLASS_AL 11
#define LINEFOLD_CLASS_ID 12
#define LINEFOLD_CLASS_IN 13
#define LINEFOLD_CLASS_HY 14
#define LINEFOLD_CLASS_BA 15
#define LINEFOLD_CLASS_BB 16
#define LINEFOLD_CLASS_B2 17
#define LINEFOLD_CLASS_ZW 18
#define LINEFOLD_CLASS_CM 19
#define LINEFOLD_CLASS_WJ 20
#define LINEFOLD_CLASS_H2 21
#define LINEFOLD_CLASS_H3 22
#define LINEFOLD_CLASS_JL 23
#define LINEFOLD_CLASS_JV 24
#define LINEFOLD_CLASS_JT 25
#define LINEFOLD_CLASS_SPOP 26
#define LINEFOLD_CLASS_CLH 27
#define LINEFOLD_CLASS_CLSP 28
#define LINEFOLD_CLASS_CLHSP 29

/* Placeholder to indicate end of ordinal classes. */
#define LINEFOLD_CLASS_TABLESIZE 30

/*
 * Below from here shouldn't appear in linefold_lbpairs[][] table.
 */

/* SPACE */
#define LINEFOLD_CLASS_SP 30


/* Explicit break. */

#define LINEFOLD_CLASS_BK 31
#define LINEFOLD_CLASS_CR 32
#define LINEFOLD_CLASS_LF 33
#define LINEFOLD_CLASS_NL 34

/*
 * Custom classes.  These should be solved to any of classes above
 * by tailor_lbclass().
 */

#define LINEFOLD_CLASS_BKO 35
#define LINEFOLD_CLASS_IDSP 36
#define LINEFOLD_CLASS_NSK 37
#define LINEFOLD_CLASS_OPAL 38

extern void linefold_getprop_generic(unicode_char, int *, linefold_class *);
extern void linefold_getprop_C(unicode_char, int *, linefold_class *);
extern void linefold_getprop_G(unicode_char, int *, linefold_class *);
extern void linefold_getprop_J(unicode_char, int *, linefold_class *);
extern void linefold_getprop_K(unicode_char, int *, linefold_class *);

#endif /* LINEFOLDTAB_H */
