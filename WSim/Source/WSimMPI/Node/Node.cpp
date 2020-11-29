#include "Node.hpp"

#include "Source/WSimCommon/Logger.hpp"
#include "Source/WSimMPI/Node/NodeSimulationInterfaceWater.hpp"
#include "Source/WSimMPI/Utils.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

ShareBuffers::ShareBuffers(int shSideSize, int shCornerSize, int shEdgeSize) {
    // clang-format off
    sh_horizontal_L_in  = new uint8_t[shSideSize];
    sh_horizontal_L_out = new uint8_t[shSideSize];
    sh_horizontal_R_in  = new uint8_t[shSideSize];
    sh_horizontal_R_out = new uint8_t[shSideSize];

    sh_vertical_U_in  = new uint8_t[shSideSize];
    sh_vertical_U_out = new uint8_t[shSideSize];
    sh_vertical_D_in  = new uint8_t[shSideSize];
    sh_vertical_D_out = new uint8_t[shSideSize];

    sh_depth_F_in  = new uint8_t[shSideSize];
    sh_depth_F_out = new uint8_t[shSideSize];
    sh_depth_B_in  = new uint8_t[shSideSize];
    sh_depth_B_out = new uint8_t[shSideSize];

    sh_corner_FUL_in  = new uint8_t[shCornerSize];
    sh_corner_FUL_out = new uint8_t[shCornerSize];
    sh_corner_FUR_in  = new uint8_t[shCornerSize];
    sh_corner_FUR_out = new uint8_t[shCornerSize];
    sh_corner_FDL_in  = new uint8_t[shCornerSize];
    sh_corner_FDL_out = new uint8_t[shCornerSize];
    sh_corner_FDR_in  = new uint8_t[shCornerSize];
    sh_corner_FDR_out = new uint8_t[shCornerSize];
    sh_corner_BUL_in  = new uint8_t[shCornerSize];
    sh_corner_BUL_out = new uint8_t[shCornerSize];
    sh_corner_BUR_in  = new uint8_t[shCornerSize];
    sh_corner_BUR_out = new uint8_t[shCornerSize];
    sh_corner_BDL_in  = new uint8_t[shCornerSize];
    sh_corner_BDL_out = new uint8_t[shCornerSize];
    sh_corner_BDR_in  = new uint8_t[shCornerSize];
    sh_corner_BDR_out = new uint8_t[shCornerSize];

    sh_edge_UL_in  = new uint8_t[shEdgeSize];
    sh_edge_UL_out = new uint8_t[shEdgeSize];
    sh_edge_UR_in  = new uint8_t[shEdgeSize];
    sh_edge_UR_out = new uint8_t[shEdgeSize];
    sh_edge_DL_in  = new uint8_t[shEdgeSize];
    sh_edge_DL_out = new uint8_t[shEdgeSize];
    sh_edge_DR_in  = new uint8_t[shEdgeSize];
    sh_edge_DR_out = new uint8_t[shEdgeSize];
    sh_edge_FL_in  = new uint8_t[shEdgeSize];
    sh_edge_FL_out = new uint8_t[shEdgeSize];
    sh_edge_FR_in  = new uint8_t[shEdgeSize];
    sh_edge_FR_out = new uint8_t[shEdgeSize];
    sh_edge_FU_in  = new uint8_t[shEdgeSize];
    sh_edge_FU_out = new uint8_t[shEdgeSize];
    sh_edge_FD_in  = new uint8_t[shEdgeSize];
    sh_edge_FD_out = new uint8_t[shEdgeSize];
    sh_edge_BL_in  = new uint8_t[shEdgeSize];
    sh_edge_BL_out = new uint8_t[shEdgeSize];
    sh_edge_BR_in  = new uint8_t[shEdgeSize];
    sh_edge_BR_out = new uint8_t[shEdgeSize];
    sh_edge_BU_in  = new uint8_t[shEdgeSize];
    sh_edge_BU_out = new uint8_t[shEdgeSize];
    sh_edge_BD_in  = new uint8_t[shEdgeSize];
    sh_edge_BD_out = new uint8_t[shEdgeSize];
    // clang-format on
}

Node::Node(int rank, int gridSize, int nodeSize, SimulationMode simulationMode, bool benchmark)
    : shareThickness(15),
      // clang-format off
      shSideSize   (shareThickness * nodeSize       * nodeSize       * (Simulation::colorVoxelSize + Simulation::velocityVoxelSize)),
      shEdgeSize   (shareThickness * shareThickness * nodeSize       * (Simulation::colorVoxelSize + Simulation::velocityVoxelSize)),
      shCornerSize (shareThickness * shareThickness * shareThickness * (Simulation::colorVoxelSize + Simulation::velocityVoxelSize)),
      // clang-format on
      shareBuffers(shSideSize, shCornerSize, shEdgeSize),
      rank(rank),
      gridSize(gridSize),
      nodeSize(nodeSize),
      nodeVolume(nodeSize * nodeSize * nodeSize * Simulation::colorVoxelSize), // Only color representation of simulation (TODO level set values)
      xPosInGrid(convertTo3DRankX(rank, gridSize)),
      yPosInGrid(convertTo3DRankY(rank, gridSize)),
      zPosInGrid(convertTo3DRankZ(rank, gridSize)),
      simulationInterface(new NodeSimulationInterfaceWater(*this, simulationMode)),
      sendArray(new uint8_t[nodeVolume]),
      simulationMode(simulationMode),
      benchmark(benchmark),
      lastFrameTime(static_cast<double>(clock())) {
    Logger::get() << "My rank: " << rank << std::endl;
    Logger::get() << "My 3D coords: " << xPosInGrid << ", " << yPosInGrid << ", " << zPosInGrid << std::endl;

    if (simulationMode.value == SimulationMode::Enum::Text) {
        std::string filename = "outputFile" + std::to_string(rank);
        outputFile.open(filename, std::ios::out | std::ios::binary);
    }
}

ShareBuffers::~ShareBuffers() {
    delete[] sh_horizontal_L_in;
    delete[] sh_horizontal_L_out;
    delete[] sh_horizontal_R_in;
    delete[] sh_horizontal_R_out;
    delete[] sh_vertical_U_in;
    delete[] sh_vertical_U_out;
    delete[] sh_vertical_D_in;
    delete[] sh_vertical_D_out;
    delete[] sh_depth_F_in;
    delete[] sh_depth_F_out;
    delete[] sh_depth_B_in;
    delete[] sh_depth_B_out;
    delete[] sh_corner_FUL_in;
    delete[] sh_corner_FUL_out;
    delete[] sh_corner_FUR_in;
    delete[] sh_corner_FUR_out;
    delete[] sh_corner_FDL_in;
    delete[] sh_corner_FDL_out;
    delete[] sh_corner_FDR_in;
    delete[] sh_corner_FDR_out;
    delete[] sh_corner_BUL_in;
    delete[] sh_corner_BUL_out;
    delete[] sh_corner_BUR_in;
    delete[] sh_corner_BUR_out;
    delete[] sh_corner_BDL_in;
    delete[] sh_corner_BDL_out;
    delete[] sh_corner_BDR_in;
    delete[] sh_corner_BDR_out;
    delete[] sh_edge_UL_in;
    delete[] sh_edge_UL_out;
    delete[] sh_edge_UR_in;
    delete[] sh_edge_UR_out;
    delete[] sh_edge_DL_in;
    delete[] sh_edge_DL_out;
    delete[] sh_edge_DR_in;
    delete[] sh_edge_DR_out;
    delete[] sh_edge_FL_in;
    delete[] sh_edge_FL_out;
    delete[] sh_edge_FR_in;
    delete[] sh_edge_FR_out;
    delete[] sh_edge_FU_in;
    delete[] sh_edge_FU_out;
    delete[] sh_edge_FD_in;
    delete[] sh_edge_FD_out;
    delete[] sh_edge_BL_in;
    delete[] sh_edge_BL_out;
    delete[] sh_edge_BR_in;
    delete[] sh_edge_BR_out;
    delete[] sh_edge_BU_in;
    delete[] sh_edge_BU_out;
    delete[] sh_edge_BD_in;
    delete[] sh_edge_BD_out;
}

Node::~Node() {
    delete[] sendArray;
    outputFile.close();
}

bool Node::isNeighbourInGrid(int neighbourOffsetX, int neighbourOffsetY, int neighbourOffsetZ) {
    const bool xInGrid = (xPosInGrid + neighbourOffsetX >= 0) && (xPosInGrid + neighbourOffsetX < gridSize);
    const bool yInGrid = (yPosInGrid + neighbourOffsetY >= 0) && (yPosInGrid + neighbourOffsetY < gridSize);
    const bool zInGrid = (zPosInGrid + neighbourOffsetZ >= 0) && (zPosInGrid + neighbourOffsetZ < gridSize);
    return xInGrid & yInGrid & zInGrid;
}

inline int Node::getNeighbourRank(int neighbourOffsetX, int neighbourOffsetY, int neighbourOffsetZ) {
    return rank + neighbourOffsetZ * gridSize * gridSize + neighbourOffsetY * gridSize + neighbourOffsetX;
}

inline void Node::shareBuffer(bool condition, uint8_t *intput_buffer, uint8_t *output_buffer, int size, int in_x, int in_y, int in_z) {
    if (condition) {
        if (isNeighbourInGrid(in_x, in_y, in_z)) {
            MPI_Recv(intput_buffer, size, MPI_CHAR, getNeighbourRank(in_x, in_y, in_z), 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } else {
        if (isNeighbourInGrid(-in_x, -in_y, -in_z)) {
            MPI_Send(output_buffer, size, MPI_CHAR, getNeighbourRank(-in_x, -in_y, -in_z), 1, MPI_COMM_WORLD);
        }
    }
}

void Node::shareSide() {
    shareBuffer(yPosInGrid % 2 == 0, shareBuffers.sh_vertical_U_in, shareBuffers.sh_vertical_D_out, shSideSize, 0, -1, 0);
    shareBuffer(yPosInGrid % 2 == 0, shareBuffers.sh_vertical_D_in, shareBuffers.sh_vertical_U_out, shSideSize, 0, 1, 0);

    shareBuffer(yPosInGrid % 2 == 1, shareBuffers.sh_vertical_U_in, shareBuffers.sh_vertical_D_out, shSideSize, 0, -1, 0);
    shareBuffer(yPosInGrid % 2 == 1, shareBuffers.sh_vertical_D_in, shareBuffers.sh_vertical_U_out, shSideSize, 0, 1, 0);

    shareBuffer(xPosInGrid % 2 == 0, shareBuffers.sh_horizontal_L_in, shareBuffers.sh_horizontal_R_out, shSideSize, -1, 0, 0);
    shareBuffer(xPosInGrid % 2 == 0, shareBuffers.sh_horizontal_R_in, shareBuffers.sh_horizontal_L_out, shSideSize, 1, 0, 0);

    shareBuffer(xPosInGrid % 2 == 1, shareBuffers.sh_horizontal_L_in, shareBuffers.sh_horizontal_R_out, shSideSize, -1, 0, 0);
    shareBuffer(xPosInGrid % 2 == 1, shareBuffers.sh_horizontal_R_in, shareBuffers.sh_horizontal_L_out, shSideSize, 1, 0, 0);

    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_depth_F_in, shareBuffers.sh_depth_B_out, shSideSize, 0, 0, -1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_depth_B_in, shareBuffers.sh_depth_F_out, shSideSize, 0, 0, 1);

    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_depth_F_in, shareBuffers.sh_depth_B_out, shSideSize, 0, 0, -1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_depth_B_in, shareBuffers.sh_depth_F_out, shSideSize, 0, 0, 1);
}

void Node::shareCorners() {
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_corner_FUL_in, shareBuffers.sh_corner_BDR_out, shCornerSize, -1, -1, -1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_corner_BDR_in, shareBuffers.sh_corner_FUL_out, shCornerSize, 1, 1, 1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_corner_FUR_in, shareBuffers.sh_corner_BDL_out, shCornerSize, 1, -1, -1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_corner_BDL_in, shareBuffers.sh_corner_FUR_out, shCornerSize, -1, 1, 1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_corner_FDL_in, shareBuffers.sh_corner_BUR_out, shCornerSize, -1, 1, -1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_corner_BUR_in, shareBuffers.sh_corner_FDL_out, shCornerSize, 1, -1, 1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_corner_FDR_in, shareBuffers.sh_corner_BUL_out, shCornerSize, 1, 1, -1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_corner_BUL_in, shareBuffers.sh_corner_FDR_out, shCornerSize, -1, -1, 1);

    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_corner_FUL_in, shareBuffers.sh_corner_BDR_out, shCornerSize, -1, -1, -1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_corner_BDR_in, shareBuffers.sh_corner_FUL_out, shCornerSize, 1, 1, 1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_corner_FUR_in, shareBuffers.sh_corner_BDL_out, shCornerSize, 1, -1, -1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_corner_BDL_in, shareBuffers.sh_corner_FUR_out, shCornerSize, -1, 1, 1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_corner_FDL_in, shareBuffers.sh_corner_BUR_out, shCornerSize, -1, 1, -1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_corner_BUR_in, shareBuffers.sh_corner_FDL_out, shCornerSize, 1, -1, 1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_corner_FDR_in, shareBuffers.sh_corner_BUL_out, shCornerSize, 1, 1, -1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_corner_BUL_in, shareBuffers.sh_corner_FDR_out, shCornerSize, -1, -1, 1);
}

void Node::shareEdges() {
    shareBuffer(yPosInGrid % 2 == 0, shareBuffers.sh_edge_UL_in, shareBuffers.sh_edge_DR_out, shEdgeSize, -1, -1, 0);
    shareBuffer(yPosInGrid % 2 == 0, shareBuffers.sh_edge_DR_in, shareBuffers.sh_edge_UL_out, shEdgeSize, 1, 1, 0);
    shareBuffer(yPosInGrid % 2 == 0, shareBuffers.sh_edge_UR_in, shareBuffers.sh_edge_DL_out, shEdgeSize, 1, -1, 0);
    shareBuffer(yPosInGrid % 2 == 0, shareBuffers.sh_edge_DL_in, shareBuffers.sh_edge_UR_out, shEdgeSize, -1, 1, 0);

    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_edge_FL_in, shareBuffers.sh_edge_BR_out, shEdgeSize, -1, 0, -1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_edge_BR_in, shareBuffers.sh_edge_FL_out, shEdgeSize, 1, 0, 1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_edge_FR_in, shareBuffers.sh_edge_BL_out, shEdgeSize, 1, 0, -1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_edge_BL_in, shareBuffers.sh_edge_FR_out, shEdgeSize, -1, 0, 1);

    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_edge_FU_in, shareBuffers.sh_edge_BD_out, shEdgeSize, 0, -1, -1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_edge_BD_in, shareBuffers.sh_edge_FU_out, shEdgeSize, 0, 1, 1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_edge_FD_in, shareBuffers.sh_edge_BU_out, shEdgeSize, 0, 1, -1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_edge_BU_in, shareBuffers.sh_edge_FD_out, shEdgeSize, 0, -1, 1);

    shareBuffer(yPosInGrid % 2 == 1, shareBuffers.sh_edge_UL_in, shareBuffers.sh_edge_DR_out, shEdgeSize, -1, -1, 0);
    shareBuffer(yPosInGrid % 2 == 1, shareBuffers.sh_edge_DR_in, shareBuffers.sh_edge_UL_out, shEdgeSize, 1, 1, 0);
    shareBuffer(yPosInGrid % 2 == 1, shareBuffers.sh_edge_UR_in, shareBuffers.sh_edge_DL_out, shEdgeSize, 1, -1, 0);
    shareBuffer(yPosInGrid % 2 == 1, shareBuffers.sh_edge_DL_in, shareBuffers.sh_edge_UR_out, shEdgeSize, -1, 1, 0);

    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_edge_FL_in, shareBuffers.sh_edge_BR_out, shEdgeSize, -1, 0, -1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_edge_BR_in, shareBuffers.sh_edge_FL_out, shEdgeSize, 1, 0, 1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_edge_FR_in, shareBuffers.sh_edge_BL_out, shEdgeSize, 1, 0, -1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_edge_BL_in, shareBuffers.sh_edge_FR_out, shEdgeSize, -1, 0, 1);

    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_edge_FU_in, shareBuffers.sh_edge_BD_out, shEdgeSize, 0, -1, -1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_edge_BD_in, shareBuffers.sh_edge_FU_out, shEdgeSize, 0, 1, 1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_edge_FD_in, shareBuffers.sh_edge_BU_out, shEdgeSize, 0, 1, -1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_edge_BU_in, shareBuffers.sh_edge_FD_out, shEdgeSize, 0, -1, 1);
}

void Node::receiveFromMaster() {
    MPI_Scatter(MPI_IN_PLACE, 0, MPI_CHAR, sendArray, nodeVolume, MPI_CHAR, 0, MPI_COMM_WORLD);
    simulationInterface->postReceiveFromMaster(sendArray);
}

void Node::dumpArrayToFile() {
    outputFile.write((const char *)sendArray, nodeVolume);
    outputFile.flush();
}

void Node::sendToMaster() {
    simulationInterface->preSendToMaster(sendArray);
    if (simulationMode.value == SimulationMode::Enum::Text) {
        dumpArrayToFile();
        //MPI_Barrier(MPI_COMM_WORLD);
    } else {
        MPI_Gather(sendArray, nodeVolume, MPI_CHAR, MPI_IN_PLACE, 0, MPI_CHAR, 0, MPI_COMM_WORLD);
    }
}

void Node::share() {
    shareSide();
    shareCorners();
    shareEdges();
}

void Node::main() {
    while (true) {

        if (benchmark) {
            startSimTime = static_cast<double>(clock());
        }

        // Sharing
        simulationInterface->preShareCopy();
        share();
        simulationInterface->postShareCopy();

        // Step iteration
        simulationInterface->iter();

        if (benchmark) {
            double currentTime = static_cast<double>(clock());
            double deltaSimTime = currentTime - startSimTime;
            double deltaFrameTime = currentTime - lastFrameTime;
            lastFrameTime = static_cast<double>(clock());
            Logger::get() << "Simulation time (ms): " << deltaSimTime << std::endl;
            Logger::get() << "Frame time (ms): " << deltaFrameTime << std::endl;
        }

        // Gather results in master
        sendToMaster();
    }
}
