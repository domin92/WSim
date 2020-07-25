#ifndef NODE_HPP
#define NODE_HPP

#include <memory>

class NodeSimulationInterface;

struct ShareBuffers {
    ShareBuffers(int shHorizontalSize, int sh_vertical_size, int shDepthSize, int shCornerSize, int shEdgeSize);
    ~ShareBuffers();

    char *sh_horizontal_L_in; // Left
    char *sh_horizontal_L_out;
    char *sh_horizontal_R_in; // Right
    char *sh_horizontal_R_out;

    char *sh_vertical_U_in; // Up
    char *sh_vertical_U_out;
    char *sh_vertical_D_in; // Down
    char *sh_vertical_D_out;

    char *sh_depth_F_in; // Front
    char *sh_depth_F_out;
    char *sh_depth_B_in; // Back
    char *sh_depth_B_out;

    char *sh_corner_FUL_in; // Front Upper Left
    char *sh_corner_FUL_out;
    char *sh_corner_FUR_in; // Front Upper Right
    char *sh_corner_FUR_out;
    char *sh_corner_FDL_in; // Front Down Left
    char *sh_corner_FDL_out;
    char *sh_corner_FDR_in; // Front Down Right
    char *sh_corner_FDR_out;
    char *sh_corner_BUL_in; // Back Upper Left
    char *sh_corner_BUL_out;
    char *sh_corner_BUR_in; // Back Upper Right
    char *sh_corner_BUR_out;
    char *sh_corner_BDL_in; // Back Down Left
    char *sh_corner_BDL_out;
    char *sh_corner_BDR_in; // Back Down Right
    char *sh_corner_BDR_out;

    char *sh_edge_UL_in; // Up Left
    char *sh_edge_UL_out;
    char *sh_edge_UR_in; // Up Right
    char *sh_edge_UR_out;
    char *sh_edge_DL_in; // Down Left
    char *sh_edge_DL_out;
    char *sh_edge_DR_in; // Down Right
    char *sh_edge_DR_out;
    char *sh_edge_FL_in; // Front Left
    char *sh_edge_FL_out;
    char *sh_edge_FR_in; // Front Right
    char *sh_edge_FR_out;
    char *sh_edge_FU_in; // Front Up
    char *sh_edge_FU_out;
    char *sh_edge_FD_in; // Front Down
    char *sh_edge_FD_out;
    char *sh_edge_BL_in; // Back Left
    char *sh_edge_BL_out;
    char *sh_edge_BR_in; // Back Right
    char *sh_edge_BR_out;
    char *sh_edge_BU_in; // Back Up
    char *sh_edge_BU_out;
    char *sh_edge_BD_in; // Back Down
    char *sh_edge_BD_out;
};

class Node {
private:
    // Constants from SimulationInterface
    const int shareThickness; // Thickness of shared areas in bytes
    const int numberOfMainArrays;

    // Share buffers
    const int shHorizontalSize;
    const int shVerticalSize;
    const int shDepthSize;
    const int shCornerSize;
    const int shEdgeSize;
    const ShareBuffers shareBuffers; // buffers used for sharing with neighbouring nodes;

    // Grid information
    const int rank;
    const int gridSize; // Number of nodes in side of 3d grid
    const int nodeSize; // Size of the cude side in bytes
    const int nodeVolume;

    // Position in grid
    const int xPosInGrid;
    const int yPosInGrid;
    const int zPosInGrid;

    // Simulation
    std::unique_ptr<NodeSimulationInterface> simulationInterface;

    // Arrays containing all the pixels (including shared areas)
    const int mainArraySize; // Size of the 3D array
    char ***array[2];        // Two 3D arrays - input, output
    int currentArrayIndex;   // Index of the current input array
    char *sendArray;         // Buffer for communication with master

    // Helpers for checking neigbours
    int getNeighbourRank(int neighbourOffsetX, int neighbourOffsetY, int neighbourOffsetZ);
    bool isNeighbourInGrid(int neighbourOffsetX, int neighbourOffsetY, int neighbourOffsetZ);

    // Methods for sharing buffers beetween nodes
    void shareBuffer(bool condition, char *intput_buffer, char *output_buffer, int size, int in_x, int in_y, int in_z);
    void shareHorizontal();
    void shareVertical();
    void shareDepth();
    void shareCorners();
    void shareEdges();
    void share(); // shares everything

    // Methods for communication with master
    void receiveFromMaster();
    void sendToMaster();

public:
    Node(int rank, int gridSize, int nodeSize);
    ~Node();

    void main();

    // Getters
    auto &get_share_buffers() const { return shareBuffers; }
    auto get_node_size() const { return nodeSize; }
    auto get_share_thickness() const { return shareThickness; }
    auto get_current_array_idx() const { return currentArrayIndex; }
    auto get_main_array_size() const { return mainArraySize; }
    char ***get_main_array_input() const { return array[currentArrayIndex]; }
    char ***get_main_array_output() const { return array[currentArrayIndex ^ 1]; }
    auto get_x_pos_in_grid() const { return xPosInGrid; }
    auto get_y_pos_in_grid() const { return yPosInGrid; }
    auto get_z_pos_in_grid() const { return zPosInGrid; }
    auto get_grid_size() const { return gridSize; }
    auto get_rank() const { return rank; }
};

#endif
