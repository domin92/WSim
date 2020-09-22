#pragma once

// Calculating node position in 3D space - VERY IMPORTANT!
// For gridSize = 2 we have:
//
// Rank | x | y | z
// 1    | 0 | 0 | 0
// 2    | 1 | 0 | 0
// 3    | 0 | 1 | 0
// 4    | 1 | 1 | 0
// 5    | 0 | 0 | 1
// 6    | 1 | 0 | 1
// 7    | 0 | 1 | 1
// 8    | 1 | 1 | 1
//

inline auto convertTo3DRankX(size_t linearRank, size_t gridSize) {
    wsimErrorIf(linearRank == 0);
    const auto adjustedRank = linearRank - 1; // Rank excluding master
    return (adjustedRank) % gridSize;
}

inline auto convertTo3DRankY(size_t linearRank, size_t gridSize) {
    wsimErrorIf(linearRank == 0);
    const auto adjustedRank = linearRank - 1; // Rank excluding master
    return ((adjustedRank) % (gridSize * gridSize)) / gridSize;
}
inline auto convertTo3DRankZ(size_t linearRank, size_t gridSize) {
    wsimErrorIf(linearRank == 0);
    const auto adjustedRank = linearRank - 1; // Rank excluding master
    return (adjustedRank) / (gridSize * gridSize);
}

#ifdef _WIN32
#include <Windows.h>
inline int getProcessId() {
    return GetCurrentProcessId();
}

#else
#include <sys/types.h>
#include <unistd.h>
inline int getProcessId() {
    return getpid();
}
#endif
