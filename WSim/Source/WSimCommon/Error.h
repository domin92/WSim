#pragma once

#include "Source/WSimCommon/Logger.h"

#include <cstdlib>

[[noreturn]] inline void wsimError() {
    Logger::get() << "wsimError called!\n";
    std::abort();
}

[[noreturn]] inline void wsimErrorIf(bool condition) {
    if (condition) {
        wsimError();
    }
}

[[noreturn]] inline void wsimErrorUnless(bool condition) {
    if (!condition) {
        wsimError();
    }
}
