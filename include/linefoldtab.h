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

extern const linefold_class LINEFOLD_CLASS_OP;
extern const linefold_class LINEFOLD_CLASS_CL;
extern const linefold_class LINEFOLD_CLASS_QU;
extern const linefold_class LINEFOLD_CLASS_GL;
extern const linefold_class LINEFOLD_CLASS_NS;
extern const linefold_class LINEFOLD_CLASS_EX;
extern const linefold_class LINEFOLD_CLASS_SY;
extern const linefold_class LINEFOLD_CLASS_IS;
extern const linefold_class LINEFOLD_CLASS_PR;
extern const linefold_class LINEFOLD_CLASS_PO;
extern const linefold_class LINEFOLD_CLASS_NU;
extern const linefold_class LINEFOLD_CLASS_AL;
extern const linefold_class LINEFOLD_CLASS_ID;
extern const linefold_class LINEFOLD_CLASS_IN;
extern const linefold_class LINEFOLD_CLASS_HY;
extern const linefold_class LINEFOLD_CLASS_BA;
extern const linefold_class LINEFOLD_CLASS_BB;
extern const linefold_class LINEFOLD_CLASS_B2;
extern const linefold_class LINEFOLD_CLASS_ZW;
extern const linefold_class LINEFOLD_CLASS_CM;
extern const linefold_class LINEFOLD_CLASS_WJ;
extern const linefold_class LINEFOLD_CLASS_H2;
extern const linefold_class LINEFOLD_CLASS_H3;
extern const linefold_class LINEFOLD_CLASS_JL;
extern const linefold_class LINEFOLD_CLASS_JV;
extern const linefold_class LINEFOLD_CLASS_JT;
extern const linefold_class LINEFOLD_CLASS_IDSP;
extern const linefold_class LINEFOLD_CLASS_SPOP;
extern const linefold_class LINEFOLD_CLASS_CLH;
extern const linefold_class LINEFOLD_CLASS_CLSP;
extern const linefold_class LINEFOLD_CLASS_CLHSP;

/* Placeholder to indicate end of ordinal classes. */
extern const linefold_class LINEFOLD_CLASS_TABLESIZE;

/*
 * Below from here shouldn't appear in linefold_lbpairs[][] table.
 */

/* SPACE */
extern const linefold_class LINEFOLD_CLASS_SP;

/* Explicit break. */

extern const linefold_class LINEFOLD_CLASS_BK;
extern const linefold_class LINEFOLD_CLASS_CR;
extern const linefold_class LINEFOLD_CLASS_LF;
extern const linefold_class LINEFOLD_CLASS_NL;

/*
 * Custom classes.  These should be solved to any of classes above
 * by tailor_lbclass().
 */

#define LINEFOLD_CLASS_BKFF 36
#define LINEFOLD_CLASS_BKVT 37
#define LINEFOLD_CLASS_INB2 38
#define LINEFOLD_CLASS_NSEX 39
#define LINEFOLD_CLASS_NSK 40
#define LINEFOLD_CLASS_OPAL 41

/* User defined class. */

extern const linefold_class LINEFOLD_CLASS_USERDEFINED;

extern void linefold_getprop_generic(linefold_char,
                                     linefold_width *, linefold_class *);
extern void linefold_getprop_C(linefold_char,
                               linefold_width *, linefold_class *);
extern void linefold_getprop_G(linefold_char,
                               linefold_width *, linefold_class *);
extern void linefold_getprop_J(linefold_char,
                               linefold_width *, linefold_class *);
extern void linefold_getprop_K(linefold_char,
                               linefold_width *, linefold_class *);

#endif /* LINEFOLDTAB_H */
