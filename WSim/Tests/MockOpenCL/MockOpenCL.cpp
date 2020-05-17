#include "MockOpenCL.h"

namespace MockOpenCL {
void *mockedBodies[int(MockedCall::COUNT)] = {};

void initialize() {
    for (int i = 0; i < int(MockedCall::COUNT); i++) {
        mockedBodies[i] = nullptr;
    }
}

} // namespace MockOpenCL
