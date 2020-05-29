#ifndef CPSL_CC_UTIL_H
#define CPSL_CC_UTIL_H

#define RAWSTRQUOTE(x) #x
#define STRQUOTE(x) RAWSTRQUOTE(x)

#define RAWSTRCAT_2(x, y) x##y
#define STRCAT_2(x, y) RAWSTRCAT_2(x, y)

#define STRCAT_3(s0, s1, s2)                     STRCAT_2(s0, STRCAT_2(s1, s2))
#define STRCAT_4(s0, s1, s2, s3)                 STRCAT_2(s0, STRCAT_3(s1, s2, s3))
#define STRCAT_5(s0, s1, s2, s3, s4)             STRCAT_2(s0, STRCAT_4(s1, s2, s3, s4))
#define STRCAT_6(s0, s1, s2, s3, s4, s5)         STRCAT_2(s0, STRCAT_5(s1, s2, s3, s4, s5))
#define STRCAT_7(s0, s1, s2, s3, s4, s5, s6)     STRCAT_2(s0, STRCAT_6(s1, s2, s3, s4, s5, s6))
#define STRCAT_8(s0, s1, s2, s3, s4, s5, s6, s7) STRCAT_2(s0, STRCAT_7(s1, s2, s3, s4, s5, s6, s7))

// assertm is copied from https://en.cppreference.com/w/cpp/error/assert .
// Use (void) to silent unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))

#endif /* #ifndef CPSL_CC_UTIL_H */
