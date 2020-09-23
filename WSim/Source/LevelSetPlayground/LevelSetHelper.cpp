#include "LevelSetHelper.h"

#include <iostream>
#include <queue>
#include <unordered_set>
#include <vector>

void LevelSetHelper::initializeToSphere(float *levelSet, Vec3 levelSetSize, float sphereRadius) {
    const FloatVec3 origin{
        static_cast<float>(levelSetSize.x - 1) / 2,
        static_cast<float>(levelSetSize.y - 1) / 2,
        static_cast<float>(levelSetSize.z - 1) / 2,
    };
    initializeToSphere(levelSet, levelSetSize, origin, sphereRadius);
}

void LevelSetHelper::initializeToSphere(float *levelSet, Vec3 levelSetSize, FloatVec3 sphereOrigin, float sphereRadius) {
    for (size_t z = 0u; z < levelSetSize.z; z++) {
        for (size_t y = 0u; y < levelSetSize.y; y++) {
            for (size_t x = 0u; x < levelSetSize.x; x++) {
                const float distanceFromOriginX = x - sphereOrigin.x;
                const float distanceFromOriginY = y - sphereOrigin.y;
                const float distanceFromOriginZ = z - sphereOrigin.z;
                const float distanceFromOrigin = std::sqrtf(distanceFromOriginX * distanceFromOriginX + distanceFromOriginY * distanceFromOriginY);
                const float levelSetValue = distanceFromOrigin - sphereRadius;

                const auto indexInLevelSet = IndexHelper::idx(x, y, z, levelSetSize);
                levelSet[indexInLevelSet] = levelSetValue;
            }
        }
    }
}

void LevelSetHelper::reinitializeLevelSet(float *levelSet, size_t size) {
    /*
    struct CandidatePoint {
        size_t index;
        size_t distance;
    };
    struct CandidatePointComparator {
        bool operator()(const CandidatePoint &a, const CandidatePoint &b) {
            return a.distance > b.distance;
        }
    };

    std::unordered_set<size_t> knownPoints{};
    std::priority_queue<CandidatePoint, std::vector<CandidatePoint>, CandidatePointComparator> candidatePoints{};
    auto addNeighboursToCandidatePoints = [&candidatePoints, &knownPoints, size](size_t x, size_t y, bool checkIfKnown) {
        auto tryAdd = [&candidatePoints, &knownPoints, &checkIfKnown](size_t index) {
            if (!checkIfKnown || knownPoints.find(index) == knownPoints.end()) {
                candidatePoints.push(CandidatePoint{});
            }
        };

        const size_t index = IndexHelper::idx(x, y, size);
        if (x > 0) {
            if (knownPoints.find(index) != knownPoints.end()) {
            }
            candidatePoints.push({index - 1, 1});
        }
        if (x < size - 1) {
            candidatePoints.push({index + 1, 1});
        }

        if (y > 0) {
            candidatePoints.push({index - size, 2});
        }
        if (y < size - 1) {
            candidatePoints.push({index + size, 1});
        }
    };

    // Initialize surfaces, get neighbours
    for (size_t y = 0; y < size; y++) {
        for (size_t x = 0; x < size; x++) {
            const size_t index = IndexHelper::idx(x, y, size);
            if (levelSet[index] == 0) {
                knownPoints.insert(index);
                //    addNeighboursToCandidatePoints(x, y);
            }
        }
    }

    // DEBUG --------------------
    while (!candidatePoints.empty()) {
        CandidatePoint p = candidatePoints.top();
        candidatePoints.pop();

        std::cout << "Candidate: [" << IndexHelper::getX(p.index, size) << ", " << IndexHelper::getY(p.index, size) << "], distance=" << p.distance << ", value=" << levelSet[p.index] << '\n';
    }
    // DEBUG --------------------

    while (!candidatePoints.empty()) {
        CandidatePoint p = candidatePoints.top();
        candidatePoints.pop();

        const auto insertionResult = knownPoints.insert(p.index);
        if (!insertionResult.second) {
            continue;
        }

        //addNeighboursToCandidatePoints(IndexHelper::getX(p.index, size), IndexHelper::getY(p.index, size));
    }
*/
}
