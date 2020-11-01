#include "NodeSimulationInterfaceWater.hpp"

#include "Source/WSimCommon/LevelSetHelper.h"
#include "Source/WSimMPI/Node/Node.hpp"

NodeSimulationInterfaceWater::NodeSimulationInterfaceWater(Node &node, SimulationMode simulationMode)
    : NodeSimulationInterface(node),
      positionInGrid(createPositionInGrid(node)),
      simulation(0, 0, getNodeSize(node), simulationMode.isLevelSet(), node.getShareThickness(), positionInGrid),
      copier(positionInGrid, (cl_command_queue &)simulation.getCommandQueue(), (size_t)node.getShareThickness(), simulation.getSimulationSize()) {

    // Gravity
    simulation.setGravityForce(5.f);

    // For testing
    const auto nodeSize = node.getNodeSize();
    const auto centerX = nodeSize / 2.f - node.getXPosInGrid() * nodeSize;
    const auto centerY = nodeSize / 2.f - node.getYPosInGrid() * nodeSize;
    const auto centerZ = -1.f * node.getZPosInGrid() * nodeSize;
    Logger::get() << centerX << ", " << centerY << ", " << centerZ << std::endl;
    //simulation.applyForce(FloatVec3{centerX, centerY, centerZ}, FloatVec3{0, 5, 0}, 1f);

    if (simulationMode.isLevelSet()) {
        const float sphereRadius = static_cast<float>(simulation.getSimulationSize().x - 5) / 2;
        auto levelSet = std::make_unique<float[]>(simulation.getSimulationSize().getRequiredBufferSize(1));
        LevelSetHelper::initializeToSphere(levelSet.get(), simulation.getSimulationSize(), sphereRadius);
        simulation.writeColor(levelSet.get());

        simulation.setGravityForce(0.1f);
    }

    if (node.getXPosInGrid() == 0) {
        simulation.addObstacleWall(Dim::X, End::Lower);
    }
    if (node.getXPosInGrid() == node.getGridSize() - 1) {
        simulation.addObstacleWall(Dim::X, End::Higher);
    }
    if (node.getYPosInGrid() == 0) {
        simulation.addObstacleWall(Dim::Y, End::Lower);
    }
    if (node.getYPosInGrid() == node.getGridSize() - 1) {
        simulation.addObstacleWall(Dim::Y, End::Higher);
    }
    if (node.getZPosInGrid() == 0) {
        simulation.addObstacleWall(Dim::Z, End::Lower);
    }
    if (node.getZPosInGrid() == node.getGridSize() - 1) {
        simulation.addObstacleWall(Dim::Z, End::Higher);
    }
}

void NodeSimulationInterfaceWater::postReceiveFromMaster(const uint8_t *receivedArray) {
    const auto &commandQueue = simulation.getCommandQueue();
    const auto &image = simulation.getColor().getSource();
    const auto offset = simulation.getBorderOffset();
    const auto size = simulation.getSimulationSize();
//    OCL::enqueueWriteImage3D(commandQueue, image, CL_TRUE, offset, size, receivedArray);
}

void NodeSimulationInterfaceWater::preSendToMaster(uint8_t *arrayToSend) {
    const auto &commandQueue = simulation.getCommandQueue();
    const auto &image = simulation.getColor().getSource();
    const auto offset = simulation.getBorderOffset();
    const auto size = simulation.getSimulationSize();
    OCL::enqueueReadImage3D(commandQueue, image, CL_TRUE, offset, size, arrayToSend);
}

void NodeSimulationInterfaceWater::preShareCopy() {
    const auto &shareBuffers = node.getShareBuffers();
    const auto copy = [&shareBuffers, this](cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize) {
        copier.preShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_vertical_D_out, Dim::Y, End::Higher);
        copier.preShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_vertical_U_out, Dim::Y, End::Lower);
        copier.preShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_horizontal_R_out, Dim::X, End::Higher);
        copier.preShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_horizontal_L_out, Dim::X, End::Lower);
        copier.preShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_depth_B_out, Dim::Z, End::Higher);
        copier.preShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_depth_F_out, Dim::Z, End::Lower);

        copier.preShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_FUL_out, End::Lower, End::Lower, End::Lower);
        copier.preShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_BDR_out, End::Higher, End::Higher, End::Higher);
        copier.preShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_FUR_out, End::Lower, End::Lower, End::Higher);
        copier.preShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_BDL_out, End::Higher, End::Higher, End::Lower);
        copier.preShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_FDL_out, End::Lower, End::Higher, End::Lower);
        copier.preShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_BUR_out, End::Higher, End::Lower, End::Higher);
        copier.preShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_FDR_out, End::Lower, End::Higher, End::Higher);
        copier.preShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_BUL_out, End::Higher, End::Lower, End::Lower);

        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_UL_out, Dim::Y, End::Lower, Dim::X, End::Lower);
        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_DR_out, Dim::Y, End::Higher, Dim::X, End::Higher);
        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_UR_out, Dim::Y, End::Lower, Dim::X, End::Higher);
        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_DL_out, Dim::Y, End::Higher, Dim::X, End::Lower);

        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_FL_out, Dim::Z, End::Lower, Dim::X, End::Lower);
        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_BR_out, Dim::Z, End::Higher, Dim::X, End::Higher);
        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_FR_out, Dim::Z, End::Lower, Dim::X, End::Higher);
        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_BL_out, Dim::Z, End::Higher, Dim::X, End::Lower);

        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_FU_out, Dim::Z, End::Lower, Dim::Y, End::Lower);
        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_BD_out, Dim::Z, End::Higher, Dim::Y, End::Higher);
        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_FD_out, Dim::Z, End::Lower, Dim::Y, End::Higher);
        copier.preShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_BU_out, Dim::Z, End::Higher, Dim::Y, End::Lower);
    };

    copy(simulation.getColor().getSource(), 0, 0);
    copy(simulation.getVelocity().getSource(), 1, Simulation::colorVoxelSize);
    clFinish(simulation.getCommandQueue());
}

void NodeSimulationInterfaceWater::iter() {
    simulation.stepSimulation(0.1f); // TODO: what this should be?
}

void NodeSimulationInterfaceWater::postShareCopy() {
    const auto &shareBuffers = node.getShareBuffers();
    const auto copy = [&shareBuffers, this](cl_mem image, size_t numberOfImagesToSkip, size_t skipPixelSize) {
        copier.postShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_vertical_D_in, Dim::Y, End::Higher);
        copier.postShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_vertical_U_in, Dim::Y, End::Lower);
        copier.postShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_horizontal_R_in, Dim::X, End::Higher);
        copier.postShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_horizontal_L_in, Dim::X, End::Lower);
        copier.postShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_depth_B_in, Dim::Z, End::Higher);
        copier.postShareCopySide(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_depth_F_in, Dim::Z, End::Lower);

        copier.postShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_FUL_in, End::Lower, End::Lower, End::Lower);
        copier.postShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_BDR_in, End::Higher, End::Higher, End::Higher);
        copier.postShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_FUR_in, End::Lower, End::Lower, End::Higher);
        copier.postShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_BDL_in, End::Higher, End::Higher, End::Lower);
        copier.postShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_FDL_in, End::Lower, End::Higher, End::Lower);
        copier.postShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_BUR_in, End::Higher, End::Lower, End::Higher);
        copier.postShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_FDR_in, End::Lower, End::Higher, End::Higher);
        copier.postShareCopyCorner(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_corner_BUL_in, End::Higher, End::Lower, End::Lower);

        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_UL_in, Dim::Y, End::Lower, Dim::X, End::Lower);
        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_DR_in, Dim::Y, End::Higher, Dim::X, End::Higher);
        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_UR_in, Dim::Y, End::Lower, Dim::X, End::Higher);
        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_DL_in, Dim::Y, End::Higher, Dim::X, End::Lower);

        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_FL_in, Dim::Z, End::Lower, Dim::X, End::Lower);
        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_BR_in, Dim::Z, End::Higher, Dim::X, End::Higher);
        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_FR_in, Dim::Z, End::Lower, Dim::X, End::Higher);
        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_BL_in, Dim::Z, End::Higher, Dim::X, End::Lower);

        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_FU_in, Dim::Z, End::Lower, Dim::Y, End::Lower);
        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_BD_in, Dim::Z, End::Higher, Dim::Y, End::Higher);
        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_FD_in, Dim::Z, End::Lower, Dim::Y, End::Higher);
        copier.postShareCopyEdge(image, numberOfImagesToSkip, skipPixelSize, shareBuffers.sh_edge_BU_in, Dim::Z, End::Higher, Dim::Y, End::Lower);
    };

    copy(simulation.getColor().getSource(), 0, 0);
    copy(simulation.getVelocity().getSource(), 1, Simulation::colorVoxelSize);
}

PositionInGrid NodeSimulationInterfaceWater::createPositionInGrid(Node &node) {
    const Vec3 position = {
        static_cast<size_t>(node.getXPosInGrid()),
        static_cast<size_t>(node.getYPosInGrid()),
        static_cast<size_t>(node.getZPosInGrid()),
    };
    const Vec3 size = {
        static_cast<size_t>(node.getGridSize()),
        static_cast<size_t>(node.getGridSize()),
        static_cast<size_t>(node.getGridSize()),
    };
    return PositionInGrid{position, size};
}

Vec3 NodeSimulationInterfaceWater::getNodeSize(Node &node) {
    return Vec3{
        static_cast<size_t>(node.getNodeSize()),
        static_cast<size_t>(node.getNodeSize()),
        static_cast<size_t>(node.getNodeSize()),
    };
}
