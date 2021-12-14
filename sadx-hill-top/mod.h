#pragma once

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())

extern const HelperFunctions* gHelperFunctions;