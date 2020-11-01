#pragma once

#include "Source/WSimCommon/SimulationMode.h"
#include <cstdint>
#include <memory>
#include <mpi.h>

class NodeSimulationInterface;

struct ShareBuffers {
    ShareBuffers(int shSideSize, int shCornerSize, int shEdgeSize);
    ~ShareBuffers();

    uint8_t *sh_horizontal_L_in; // Left
    uint8_t *sh_horizontal_L_out;
    uint8_t *sh_horizontal_R_in; // Right
    uint8_t *sh_horizontal_R_out;

    uint8_t *sh_vertical_U_in; // Up
    uint8_t *sh_vertical_U_out;
    uint8_t *sh_vertical_D_in; // Down
    uint8_t *sh_vertical_D_out;

    uint8_t *sh_depth_F_in; // Front
    uint8_t *sh_depth_F_out;
    uint8_t *sh_depth_B_in; // Back
    uint8_t *sh_depth_B_out;

    uint8_t *sh_corner_FUL_in; // Front Upper Left
    uint8_t *sh_corner_FUL_out;
    uint8_t *sh_corner_FUR_in; // Front Upper Right
    uint8_t *sh_corner_FUR_out;
    uint8_t *sh_corner_FDL_in; // Front Down Left
    uint8_t *sh_corner_FDL_out;
    uint8_t *sh_corner_FDR_in; // Front Down Right
    uint8_t *sh_corner_FDR_out;
    uint8_t *sh_corner_BUL_in; // Back Upper Left
    uint8_t *sh_corner_BUL_out;
    uint8_t *sh_corner_BUR_in; // Back Upper Right
    uint8_t *sh_corner_BUR_out;
    uint8_t *sh_corner_BDL_in; // Back Down Left
    uint8_t *sh_corner_BDL_out;
    uint8_t *sh_corner_BDR_in; // Back Down Right
    uint8_t *sh_corner_BDR_out;

    uint8_t *sh_edge_UL_in; // Up Left
    uint8_t *sh_edge_UL_out;
    uint8_t *sh_edge_UR_in; // Up Right
    uint8_t *sh_edge_UR_out;
    uint8_t *sh_edge_DL_in; // Down Left
    uint8_t *sh_edge_DL_out;
    uint8_t *sh_edge_DR_in; // Down Right
    uint8_t *sh_edge_DR_out;
    uint8_t *sh_edge_FL_in; // Front Left
    uint8_t *sh_edge_FL_out;
    uint8_t *sh_edge_FR_in; // Front Right
    uint8_t *sh_edge_FR_out;
    uint8_t *sh_edge_FU_in; // Front Up
    uint8_t *sh_edge_FU_out;
    uint8_t *sh_edge_FD_in; // Front Down
    uint8_t *sh_edge_FD_out;
    uint8_t *sh_edge_BL_in; // Back Left
    uint8_t *sh_edge_BL_out;
    uint8_t *sh_edge_BR_in; // Back Right
    uint8_t *sh_edge_BR_out;
    uint8_t *sh_edge_BU_in; // Back Up
    uint8_t *sh_edge_BU_out;
    uint8_t *sh_edge_BD_in; // Back Down
    uint8_t *sh_edge_BD_out;
};

class Node {
private:
    // Constants from SimulationInterface
    const int shareThickness; // Thickness of shared areas in bytes

    // Share buffers
    const int shSideSize;
    const int shCornerSize;
    const int shEdgeSize;
    const ShareBuffers shareBuffers; // buffers used for sharing with neighbouring nodes;

    // Grid information
    const int rank;
    const int gridSize; // Number of nodes in side of 3d grid
    const int nodeSize; // Size of the cude side in bytes
    const int nodeVolume;

    // Position in grid
    const size_t xPosInGrid;
    const size_t yPosInGrid;
    const size_t zPosInGrid;

    // Simulation
    std::unique_ptr<NodeSimulationInterface> simulationInterface;
    SimulationMode::Enum simulationMode;

    // Arrays containing all the pixels (including shared areas)
    uint8_t *sendArray; // Buffer for communication with master

    // MPI_Request used to block Igather
    MPI_Request igatherRequest;

    // Helpers for checking neigbours
    int getNeighbourRank(int neighbourOffsetX, int neighbourOffsetY, int neighbourOffsetZ);
    bool isNeighbourInGrid(int neighbourOffsetX, int neighbourOffsetY, int neighbourOffsetZ);

    // Methods for sharing buffers beetween nodes
    void shareBuffer(bool condition, uint8_t *intput_buffer, uint8_t *output_buffer, int size, int in_x, int in_y, int in_z);
    void shareSide();
    void shareCorners();
    void shareEdges();
    void share(); // shares everything

    void dumpArrayToFile();
    std::ofstream outputFile;

    // Methods for communication with master
    void receiveFromMaster();
    void sendToMaster();

public:
    Node(int rank, int gridSize, int nodeSize, SimulationMode simulationMode);
    ~Node();

    void main();

    // Getters
    auto &getShareBuffers() const { return shareBuffers; }
    auto getNodeSize() const { return nodeSize; }
    auto getShareThickness() const { return shareThickness; }
    auto getXPosInGrid() const { return xPosInGrid; }
    auto getYPosInGrid() const { return yPosInGrid; }
    auto getZPosInGrid() const { return zPosInGrid; }
    auto getGridSize() const { return gridSize; }
};
