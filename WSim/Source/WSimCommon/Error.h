#pragma once

#include "Source/WSimCommon/Logger.h"

#include <cstdlib>

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
