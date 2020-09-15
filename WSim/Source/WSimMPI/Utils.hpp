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

inline int convertTo3DRankX(int linearRank, int gridSize) {
    const int adjusted_rank = linearRank - 1; // Rank excluding master
    return (adjusted_rank) % gridSize;
}

inline int convertTo3DRankY(int linearRank, int gridSize) {
    const int adjusted_rank = linearRank - 1; // Rank excluding master
    return ((adjusted_rank) % (gridSize * gridSize)) / gridSize;
}
inline int convertTo3DRankZ(int linearRank, int gridSize) {
    const int adjusted_rank = linearRank - 1; // Rank excluding master
    return (adjusted_rank) / (gridSize * gridSize);
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