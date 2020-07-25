#include "node.hpp"

#include "Source/WSimCommon/Logger.hpp"
#include "Source/WSimMPI/Utils.hpp"
#include "Source/WSimMPI/node/NodeSimulationInterfaceWater.hpp"

#include <cstdlib>
#include <iostream>
#include <mpi.h>

ShareBuffers::ShareBuffers(int shHorizontalSize, int shVerticalSize, int shDepthSize, int shCornerSize, int shEdgeSize) {
    sh_horizontal_L_in = new char[shHorizontalSize];
    sh_horizontal_L_out = new char[shHorizontalSize];
    sh_horizontal_R_in = new char[shHorizontalSize];
    sh_horizontal_R_out = new char[shHorizontalSize];

    sh_vertical_U_in = new char[shVerticalSize];
    sh_vertical_U_out = new char[shVerticalSize];
    sh_vertical_D_in = new char[shVerticalSize];
    sh_vertical_D_out = new char[shVerticalSize];

    sh_depth_F_in = new char[shDepthSize];
    sh_depth_F_out = new char[shDepthSize];
    sh_depth_B_in = new char[shDepthSize];
    sh_depth_B_out = new char[shDepthSize];

    sh_corner_FUL_in = new char[shCornerSize];
    sh_corner_FUL_out = new char[shCornerSize];
    sh_corner_FUR_in = new char[shCornerSize];
    sh_corner_FUR_out = new char[shCornerSize];
    sh_corner_FDL_in = new char[shCornerSize];
    sh_corner_FDL_out = new char[shCornerSize];
    sh_corner_FDR_in = new char[shCornerSize];
    sh_corner_FDR_out = new char[shCornerSize];
    sh_corner_BUL_in = new char[shCornerSize];
    sh_corner_BUL_out = new char[shCornerSize];
    sh_corner_BUR_in = new char[shCornerSize];
    sh_corner_BUR_out = new char[shCornerSize];
    sh_corner_BDL_in = new char[shCornerSize];
    sh_corner_BDL_out = new char[shCornerSize];
    sh_corner_BDR_in = new char[shCornerSize];
    sh_corner_BDR_out = new char[shCornerSize];

    sh_edge_UL_in = new char[shEdgeSize];
    sh_edge_UL_out = new char[shEdgeSize];
    sh_edge_UR_in = new char[shEdgeSize];
    sh_edge_UR_out = new char[shEdgeSize];
    sh_edge_DL_in = new char[shEdgeSize];
    sh_edge_DL_out = new char[shEdgeSize];
    sh_edge_DR_in = new char[shEdgeSize];
    sh_edge_DR_out = new char[shEdgeSize];
    sh_edge_FL_in = new char[shEdgeSize];
    sh_edge_FL_out = new char[shEdgeSize];
    sh_edge_FR_in = new char[shEdgeSize];
    sh_edge_FR_out = new char[shEdgeSize];
    sh_edge_FU_in = new char[shEdgeSize];
    sh_edge_FU_out = new char[shEdgeSize];
    sh_edge_FD_in = new char[shEdgeSize];
    sh_edge_FD_out = new char[shEdgeSize];
    sh_edge_BL_in = new char[shEdgeSize];
    sh_edge_BL_out = new char[shEdgeSize];
    sh_edge_BR_in = new char[shEdgeSize];
    sh_edge_BR_out = new char[shEdgeSize];
    sh_edge_BU_in = new char[shEdgeSize];
    sh_edge_BU_out = new char[shEdgeSize];
    sh_edge_BD_in = new char[shEdgeSize];
    sh_edge_BD_out = new char[shEdgeSize];
}

using UsedSimulationInterface = NodeSimulationInterfaceWater;
Node::Node(int rank, int gridSize, int nodeSize)
    : shareThickness(UsedSimulationInterface::shareThickness),
      numberOfMainArrays(UsedSimulationInterface::mainArraysCount),
      shHorizontalSize(nodeSize * nodeSize * shareThickness * numberOfMainArrays * UsedSimulationInterface::texelSize),
      shVerticalSize(nodeSize * nodeSize * shareThickness * numberOfMainArrays * UsedSimulationInterface::texelSize),
      shDepthSize(nodeSize * nodeSize * shareThickness * numberOfMainArrays * UsedSimulationInterface::texelSize),
      shCornerSize(shareThickness * shareThickness * shareThickness * numberOfMainArrays * UsedSimulationInterface::texelSize),
      shEdgeSize(shareThickness * shareThickness * nodeSize * numberOfMainArrays * UsedSimulationInterface::texelSize),
      shareBuffers(shHorizontalSize, shVerticalSize, shDepthSize, shCornerSize, shEdgeSize),
      rank(rank),
      gridSize(gridSize),
      nodeSize(nodeSize),
      nodeVolume(nodeSize * nodeSize * nodeSize * UsedSimulationInterface::texelSize),
      xPosInGrid(convertTo3DRankX(rank, gridSize)),
      yPosInGrid(convertTo3DRankY(rank, gridSize)),
      zPosInGrid(convertTo3DRankZ(rank, gridSize)),
      simulationInterface(new UsedSimulationInterface(*this)),
      mainArraySize(nodeSize + 2 * shareThickness) {

    // Log position in frid
    Logger::get() << "My 3D coords: " << xPosInGrid << ", " << yPosInGrid << ", " << zPosInGrid << std::endl;

    // Creating two 3D arrays
    array[0] = new char **[mainArraySize * numberOfMainArrays];
    for (int i = 0; i < mainArraySize * numberOfMainArrays; i++) {

        array[0][i] = new char *[mainArraySize];
        for (int j = 0; j < mainArraySize; j++) {

            array[0][i][j] = new char[mainArraySize];
            for (int k = 0; k < mainArraySize; k++) {

                array[0][i][j][k] = 0;
            }
        }
    }

    array[1] = new char **[mainArraySize * numberOfMainArrays];
    for (int i = 0; i < mainArraySize * numberOfMainArrays; i++) {

        array[1][i] = new char *[mainArraySize];
        for (int j = 0; j < mainArraySize; j++) {

            array[1][i][j] = new char[mainArraySize];
            for (int k = 0; k < mainArraySize; k++) {

                array[1][i][j][k] = 0;
            }
        }
    }

    currentArrayIndex = 0;

    sendArray = new char[nodeVolume];
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

    for (int i = 0; i < mainArraySize * numberOfMainArrays; i++) {
        for (int j = 0; j < mainArraySize; j++) {
            delete[] array[0][i][j];
            delete[] array[1][i][j];
        }
        delete[] array[0][i];
        delete[] array[1][i];
    }

    delete[] array[0];
    delete[] array[1];
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

inline void Node::shareBuffer(bool condition, char *intput_buffer, char *output_buffer, int size, int in_x, int in_y, int in_z) {
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

void Node::shareVertical() {
    shareBuffer(yPosInGrid % 2 == 0, shareBuffers.sh_vertical_U_in, shareBuffers.sh_vertical_D_out, shVerticalSize, 0, -1, 0);
    shareBuffer(yPosInGrid % 2 == 0, shareBuffers.sh_vertical_D_in, shareBuffers.sh_vertical_U_out, shVerticalSize, 0, 1, 0);

    shareBuffer(yPosInGrid % 2 == 1, shareBuffers.sh_vertical_U_in, shareBuffers.sh_vertical_D_out, shVerticalSize, 0, -1, 0);
    shareBuffer(yPosInGrid % 2 == 1, shareBuffers.sh_vertical_D_in, shareBuffers.sh_vertical_U_out, shVerticalSize, 0, 1, 0);
}

void Node::shareHorizontal() {
    shareBuffer(xPosInGrid % 2 == 0, shareBuffers.sh_horizontal_L_in, shareBuffers.sh_horizontal_R_out, shHorizontalSize, -1, 0, 0);
    shareBuffer(xPosInGrid % 2 == 0, shareBuffers.sh_horizontal_R_in, shareBuffers.sh_horizontal_L_out, shHorizontalSize, 1, 0, 0);

    shareBuffer(xPosInGrid % 2 == 1, shareBuffers.sh_horizontal_L_in, shareBuffers.sh_horizontal_R_out, shHorizontalSize, -1, 0, 0);
    shareBuffer(xPosInGrid % 2 == 1, shareBuffers.sh_horizontal_R_in, shareBuffers.sh_horizontal_L_out, shHorizontalSize, 1, 0, 0);
}

void Node::shareDepth() {
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_depth_F_in, shareBuffers.sh_depth_B_out, shDepthSize, 0, 0, -1);
    shareBuffer(zPosInGrid % 2 == 0, shareBuffers.sh_depth_B_in, shareBuffers.sh_depth_F_out, shDepthSize, 0, 0, 1);

    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_depth_F_in, shareBuffers.sh_depth_B_out, shDepthSize, 0, 0, -1);
    shareBuffer(zPosInGrid % 2 == 1, shareBuffers.sh_depth_B_in, shareBuffers.sh_depth_F_out, shDepthSize, 0, 0, 1);
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

void Node::sendToMaster() {
#ifdef WSIM_TEXT_ONLY
    MPI_Barrier(MPI_COMM_WORLD)
#else
    simulationInterface->preSendToMaster(sendArray);
    MPI_Gather(sendArray, nodeVolume, MPI_CHAR, MPI_IN_PLACE, 0, MPI_CHAR, 0, MPI_COMM_WORLD);
#endif
}

void Node::share() {
    shareVertical();
    shareHorizontal();
    shareDepth();
    shareCorners();
    shareEdges();
}

void Node::main() {
    receiveFromMaster();
    while (true) {
        // Sharing
        simulationInterface->preShareCopy();
        share();
        simulationInterface->postShareCopy();

        // Step iteration
        simulationInterface->iter();
        currentArrayIndex ^= 1;

        // Gather results in master
        sendToMaster();
    }
}
