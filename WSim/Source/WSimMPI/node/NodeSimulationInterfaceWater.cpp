#include "NodeSimulationInterfaceWater.hpp"

#include "Source/WSimMPI/node/node.hpp"

NodeSimulationInterfaceWater::NodeSimulationInterfaceWater(Node &node)
    : NodeSimulationInterface(node),
      positionInGrid(createPositionInGrid(node)),
      simulation(0, 0, getNodeSize(node), node.get_share_thickness(), positionInGrid),
      copier(positionInGrid, (cl_command_queue &)simulation.getCommandQueue(), (size_t)node.get_share_thickness(), simulation.getSimulationSize()) {

    // Gravity
    simulation.setGravityForce(5.f);

    // For testing
    const auto nodeSize = node.get_node_size();
    const auto centerX = nodeSize / 2.f - node.get_x_pos_in_grid() * nodeSize;
    const auto centerY = nodeSize / 2.f - node.get_y_pos_in_grid() * nodeSize;
    const auto centerZ = -1.f * node.get_z_pos_in_grid() * nodeSize;
    Logger::get() << centerX << ", " << centerY << ", " << centerZ << std::endl;
    simulation.applyForce(FloatVec3{centerX, centerY, centerZ}, FloatVec3{50, 50, 0}, 10);

    if (node.get_x_pos_in_grid() == 0) {
        simulation.addObstacleWall(Dim::X, End::Lower);
    }
    if (node.get_x_pos_in_grid() == node.get_grid_size() - 1) {
        simulation.addObstacleWall(Dim::X, End::Higher);
    }
    if (node.get_y_pos_in_grid() == 0) {
        simulation.addObstacleWall(Dim::Y, End::Lower);
    }
    if (node.get_y_pos_in_grid() == node.get_grid_size() - 1) {
        simulation.addObstacleWall(Dim::Y, End::Higher);
    }
    if (node.get_z_pos_in_grid() == 0) {
        simulation.addObstacleWall(Dim::Z, End::Lower);
    }
    if (node.get_z_pos_in_grid() == node.get_grid_size() - 1) {
        simulation.addObstacleWall(Dim::Z, End::Higher);
    }

}

void NodeSimulationInterfaceWater::postReceiveFromMaster(const char *receivedArray) {
    const auto &commandQueue = simulation.getCommandQueue();
    const auto &image = simulation.getColor().getSource();
    const auto offset = simulation.getBorderOffset();
    const auto size = simulation.getSimulationSize();
    OCL::enqueueWriteImage3D(commandQueue, image, CL_TRUE, offset, size, receivedArray);
}

void NodeSimulationInterfaceWater::preSendToMaster(char *arrayToSend) {
    const auto &commandQueue = simulation.getCommandQueue();
    const auto &image = simulation.getColor().getSource();
    const auto offset = simulation.getBorderOffset();
    const auto size = simulation.getSimulationSize();
    OCL::enqueueReadImage3D(commandQueue, image, CL_TRUE, offset, size, arrayToSend);
}

void NodeSimulationInterfaceWater::preShareCopy() {
    const auto &shareBuffers = node.get_share_buffers();
    const auto copy = [&shareBuffers, this](cl_mem image, size_t imageIndex) {
        copier.preShareCopySide(image, imageIndex, shareBuffers.sh_vertical_D_out, Dim::Y, End::Higher);
        copier.preShareCopySide(image, imageIndex, shareBuffers.sh_vertical_U_out, Dim::Y, End::Lower);
        copier.preShareCopySide(image, imageIndex, shareBuffers.sh_horizontal_R_out, Dim::X, End::Higher);
        copier.preShareCopySide(image, imageIndex, shareBuffers.sh_horizontal_L_out, Dim::X, End::Lower);
        copier.preShareCopySide(image, imageIndex, shareBuffers.sh_depth_B_out, Dim::Z, End::Higher);
        copier.preShareCopySide(image, imageIndex, shareBuffers.sh_depth_F_out, Dim::Z, End::Lower);

        copier.preShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_FUL_out, End::Lower, End::Lower, End::Lower);
        copier.preShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_BDR_out, End::Higher, End::Higher, End::Higher);
        copier.preShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_FUR_out, End::Lower, End::Lower, End::Higher);
        copier.preShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_BDL_out, End::Higher, End::Higher, End::Lower);
        copier.preShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_FDL_out, End::Lower, End::Higher, End::Lower);
        copier.preShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_BUR_out, End::Higher, End::Lower, End::Higher);
        copier.preShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_FDR_out, End::Lower, End::Higher, End::Higher);
        copier.preShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_BUL_out, End::Higher, End::Lower, End::Lower);

        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_UL_out, Dim::Y, End::Lower, Dim::X, End::Lower);
        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_DR_out, Dim::Y, End::Higher, Dim::X, End::Higher);
        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_UR_out, Dim::Y, End::Lower, Dim::X, End::Higher);
        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_DL_out, Dim::Y, End::Higher, Dim::X, End::Lower);

        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_FL_out, Dim::Z, End::Lower, Dim::X, End::Lower);
        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_BR_out, Dim::Z, End::Higher, Dim::X, End::Higher);
        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_FR_out, Dim::Z, End::Lower, Dim::X, End::Higher);
        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_BL_out, Dim::Z, End::Higher, Dim::X, End::Lower);

        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_FU_out, Dim::Z, End::Lower, Dim::Y, End::Lower);
        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_BD_out, Dim::Z, End::Higher, Dim::Y, End::Higher);
        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_FD_out, Dim::Z, End::Lower, Dim::Y, End::Higher);
        copier.preShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_BU_out, Dim::Z, End::Higher, Dim::Y, End::Lower);
    };

    copy(simulation.getColor().getSource(), 0);
    copy(simulation.getVelocity().getSource(), 1);
    clFinish(simulation.getCommandQueue());
}

void NodeSimulationInterfaceWater::iter() {
    simulation.stepSimulation(0.1f); // TODO: what this should be?
}

void NodeSimulationInterfaceWater::postShareCopy() {
    const auto &shareBuffers = node.get_share_buffers();
    const auto copy = [&shareBuffers, this](cl_mem image, size_t imageIndex) {
        copier.postShareCopySide(image, imageIndex, shareBuffers.sh_vertical_D_in, Dim::Y, End::Higher);
        copier.postShareCopySide(image, imageIndex, shareBuffers.sh_vertical_U_in, Dim::Y, End::Lower);
        copier.postShareCopySide(image, imageIndex, shareBuffers.sh_horizontal_R_in, Dim::X, End::Higher);
        copier.postShareCopySide(image, imageIndex, shareBuffers.sh_horizontal_L_in, Dim::X, End::Lower);
        copier.postShareCopySide(image, imageIndex, shareBuffers.sh_depth_B_in, Dim::Z, End::Higher);
        copier.postShareCopySide(image, imageIndex, shareBuffers.sh_depth_F_in, Dim::Z, End::Lower);

        copier.postShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_FUL_in, End::Lower, End::Lower, End::Lower);
        copier.postShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_BDR_in, End::Higher, End::Higher, End::Higher);
        copier.postShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_FUR_in, End::Lower, End::Lower, End::Higher);
        copier.postShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_BDL_in, End::Higher, End::Higher, End::Lower);
        copier.postShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_FDL_in, End::Lower, End::Higher, End::Lower);
        copier.postShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_BUR_in, End::Higher, End::Lower, End::Higher);
        copier.postShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_FDR_in, End::Lower, End::Higher, End::Higher);
        copier.postShareCopyCorner(image, imageIndex, shareBuffers.sh_corner_BUL_in, End::Higher, End::Lower, End::Lower);

        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_UL_in, Dim::Y, End::Lower, Dim::X, End::Lower);
        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_DR_in, Dim::Y, End::Higher, Dim::X, End::Higher);
        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_UR_in, Dim::Y, End::Lower, Dim::X, End::Higher);
        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_DL_in, Dim::Y, End::Higher, Dim::X, End::Lower);

        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_FL_in, Dim::Z, End::Lower, Dim::X, End::Lower);
        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_BR_in, Dim::Z, End::Higher, Dim::X, End::Higher);
        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_FR_in, Dim::Z, End::Lower, Dim::X, End::Higher);
        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_BL_in, Dim::Z, End::Higher, Dim::X, End::Lower);

        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_FU_in, Dim::Z, End::Lower, Dim::Y, End::Lower);
        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_BD_in, Dim::Z, End::Higher, Dim::Y, End::Higher);
        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_FD_in, Dim::Z, End::Lower, Dim::Y, End::Higher);
        copier.postShareCopyEdge(image, imageIndex, shareBuffers.sh_edge_BU_in, Dim::Z, End::Higher, Dim::Y, End::Lower);
    };

    copy(simulation.getColor().getSource(), 0);
    copy(simulation.getVelocity().getSource(), 1);
}

PositionInGrid NodeSimulationInterfaceWater::createPositionInGrid(Node &node) {
    const Vec3 position = {
        static_cast<size_t>(node.get_x_pos_in_grid()),
        static_cast<size_t>(node.get_y_pos_in_grid()),
        static_cast<size_t>(node.get_z_pos_in_grid()),
    };
    const Vec3 size = {
        static_cast<size_t>(node.get_grid_size()),
        static_cast<size_t>(node.get_grid_size()),
        static_cast<size_t>(node.get_grid_size()),
    };
    return PositionInGrid{position, size};
}

Vec3 NodeSimulationInterfaceWater::getNodeSize(Node &node) {
    return Vec3{
        static_cast<size_t>(node.get_node_size()),
        static_cast<size_t>(node.get_node_size()),
        static_cast<size_t>(node.get_node_size()),
    };
}
