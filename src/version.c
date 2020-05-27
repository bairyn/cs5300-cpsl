#include "version.h"

const uint64_t  cpsl_cc_version_major          = CPSL_CC_VERSION_MAJOR;
const uint64_t  cpsl_cc_version_minor          = CPSL_CC_VERSION_MINOR;
const uint64_t  cpsl_cc_version_patch          = CPSL_CC_VERSION_PATCH;
const size_t    cpsl_cc_version_num_components = CPSL_CC_VERSION_NUM_COMPONENTS;
const uint64_t  cpsl_cc_version_components[CPSL_CC_VERSION_NUM_COMPONENTS] = {
	cpsl_cc_version_major,
	cpsl_cc_version_minor,
	cpsl_cc_version_patch,
};
const char     *cpsl_cc_version_string = CPSL_CC_VERSION_STRING;
