#include "Source/WSimRenderer/VolumeRenderer.hpp"
#include "Source/WSimCommon/ArgumentParser.hpp"
#include <fstream>
#include <thread>
#include <chrono>

int my_cbrt(int a) {
    switch (a) {
    case 1:
        return 1;
    case 8:
        return 2;
    case 27:
        return 3;
    case 64:
        return 4;
    case 125:
        return 5;
    default:
        return 0;
    }
}

class VolumeRendererCallbacksImpl : public VolumeRendererCallbacks {
private:
    std::ifstream *inputFiles;
    uint8_t *volumeBuffers;
    int nodeSizeInVoxels;
    int gridSizeInNodes;
    int nodeVolumeInBytes;
    int gridVolume;

public:
    VolumeRendererCallbacksImpl(int nodeSizeInVoxels, int gridSizeInNodes) {
        this->nodeSizeInVoxels = nodeSizeInVoxels;
        this->gridSizeInNodes = gridSizeInNodes;
        gridVolume = gridSizeInNodes * gridSizeInNodes * gridSizeInNodes;
        nodeVolumeInBytes = nodeSizeInVoxels * nodeSizeInVoxels * nodeSizeInVoxels * sizeof(float);

        inputFiles = new std::ifstream[gridVolume];
        for (int i = 0; i < gridVolume; i++) {
            std::string filename = "outputFile" + std::to_string(i+1); // Starting from 1
            inputFiles[i].open(filename, std::ios::in | std::ios::binary);
        }

        volumeBuffers = new uint8_t[nodeVolumeInBytes * gridVolume];
    }

    ~VolumeRendererCallbacksImpl() {
        for (int i = 0; i < gridVolume; i++) {
            inputFiles[i].close();
        }
        delete[] inputFiles;
        delete[] volumeBuffers;
    }

    void stepSimulation(float deltaTimeSeconds){
        for (int i = 0; i < gridVolume; i++) {
            if (!inputFiles[i]) {
                continue;
            }
            int addressOffset = i * nodeVolumeInBytes;
            inputFiles[i].read(((char *)volumeBuffers) + addressOffset, nodeVolumeInBytes);
        }
    }

    uint8_t *getVolumeBuffers(){
        return volumeBuffers;
    }
    
};

void main(int argc, char **argv) {
    // Parse arguments
    ArgumentParser argumentParser{argc, argv};
    int procCount = argumentParser.getArgumentValue<int>({"-n", "--numberOfProcesses"}, 2); // Size of the edge of the simulation cube
    int fullSize = argumentParser.getArgumentValue<int>({"-s", "--simulationSize"}, 60);    // Size of the edge of the simulation cube
    int gridSize = my_cbrt(procCount - 1);
    int nodeSize = fullSize / gridSize;

    VolumeRendererCallbacksImpl callbacks(nodeSize, gridSize);
    VolumeRenderer renderer(callbacks, nodeSize, gridSize, 1000);
    renderer.mainLoop();
}