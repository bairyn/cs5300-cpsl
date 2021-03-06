#ifndef CPSL_CC_VERSION_H
#define CPSL_CC_VERSION_H

#include <stddef.h>  // size_t
#include <stdint.h>  // uint64_t

#include "util.h"

#define CPSL_CC_VERSION_MAJOR 0
#define CPSL_CC_VERSION_MINOR 3
#define CPSL_CC_VERSION_PATCH 0

#define CPSL_CC_VERSION_NUM_COMPONENTS 3

#define CPSL_CC_VERSION_RAW_STRING STRCAT_5( \
	CPSL_CC_VERSION_MAJOR, ., \
	CPSL_CC_VERSION_MINOR, ., \
	CPSL_CC_VERSION_PATCH \
)
#define CPSL_CC_VERSION_QUOTED_STRING STRQUOTE(CPSL_CC_VERSION_RAW_STRING)

extern const uint64_t  cpsl_cc_version_major;
extern const uint64_t  cpsl_cc_version_minor;
extern const uint64_t  cpsl_cc_version_patch;
extern const size_t    cpsl_cc_version_num_components;
extern const uint64_t  cpsl_cc_version_components[];
extern const char     *cpsl_cc_version_string;

#endif /* #ifndef CPSL_CC_VERSION_H */
