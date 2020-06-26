#pragma once

#include "Source/WSimCommon/Logger.h"

#include <cstdlib>

#ifdef WSIM_ERROR_AS_FUNCTIONS
[[noreturn]] inline void wsimError() {
    Logger::get() << "wsimError called!\n";
    std::abort();
}

inline void wsimErrorIf(bool condition) {
    if (condition) {
        wsimError();
    }
}

inline void wsimErrorUnless(bool condition) {
    if (!condition) {
        wsimError();
    }
}
#else
#define wsimError()                                                                 \
    Logger::get() << "wsimError called at " << __FILE__ << ":" << __LINE__ << '\n'; \
    std::abort();

#define wsimErrorIf(condition) \
    {                          \
        if (condition) {       \
            wsimError();       \
        }                      \
    }

#define wsimErrorUnless(condition) \
    {                              \
        if (!(condition)) {        \
            wsimError();           \
        }                          \
    }
#endif