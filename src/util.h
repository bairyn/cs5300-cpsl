#ifndef CPSL_CC_UTIL_H
#define CPSL_CC_UTIL_H

#define RAWSTR(x) #x
#define STR(x) RAWSTR(x)

#define RAWSTRCAT(x, y) x##y
#define STRCAT(x, y) RAWSTRCAT(x, y)

// assertm is copied from https://en.cppreference.com/w/cpp/error/assert .
// Use (void) to silent unused warnings.
#define assertm(exp, msg) assert(((void)msg, exp))

#endif /* #ifndef CPSL_CC_UTIL_H */
