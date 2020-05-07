#pragma once

#include <cstdlib>

[[noreturn]] inline void wsimError() {
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
