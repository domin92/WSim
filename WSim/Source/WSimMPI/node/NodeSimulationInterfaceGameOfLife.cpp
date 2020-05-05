#include "NodeSimulationInterfaceGameOfLife.hpp"

#include "Source/WSimMPI/node/node.hpp"

NodeSimulationInterfaceGameOfLife::NodeSimulationInterfaceGameOfLife(Node &node)
    : NodeSimulationInterface(node) {}

void NodeSimulationInterfaceGameOfLife::postReceiveFromMaster(const char *receivedArray) {
    const auto nodeSize = node.get_node_size();
    const auto shareThickness = node.get_share_thickness();
    char ***array = node.get_main_array_input();

    for (int z = 0; z < nodeSize; z++) {
        for (int y = 0; y < nodeSize; y++) {
            for (int x = 0; x < nodeSize; x++) {
                array[z + shareThickness][y + shareThickness][x + shareThickness] = receivedArray[z * (nodeSize * nodeSize) + y * nodeSize + x];
            }
        }
    }
}

void NodeSimulationInterfaceGameOfLife::preSendToMaster(char *arrayToSend) {
    const auto nodeSize = node.get_node_size();
    const auto shareThickness = node.get_share_thickness();
    char ***array = node.get_main_array_input();

    for (int z = 0; z < nodeSize; z++) {
        for (int y = 0; y < nodeSize; y++) {
            for (int x = 0; x < nodeSize; x++) {
                arrayToSend[z * (nodeSize * nodeSize) + y * nodeSize + x] = array[z + shareThickness][y + shareThickness][x + shareThickness];
            }
        }
    }
}

void NodeSimulationInterfaceGameOfLife::preShareCopy() {
    const auto &shareBuffers = node.get_share_buffers();
    const auto nodeSize = node.get_node_size();
    const auto shareThickness = node.get_share_thickness();

    preShareCopyBuffer(shareBuffers.sh_vertical_D_out, nodeSize, shareThickness, nodeSize, 0, 1, 0);
    preShareCopyBuffer(shareBuffers.sh_vertical_U_out, nodeSize, shareThickness, nodeSize, 0, -1, 0);
    preShareCopyBuffer(shareBuffers.sh_horizontal_R_out, shareThickness, nodeSize, nodeSize, 1, 0, 0);
    preShareCopyBuffer(shareBuffers.sh_horizontal_L_out, shareThickness, nodeSize, nodeSize, -1, 0, 0);
    preShareCopyBuffer(shareBuffers.sh_depth_B_out, nodeSize, nodeSize, shareThickness, 0, 0, 1);
    preShareCopyBuffer(shareBuffers.sh_depth_F_out, nodeSize, nodeSize, shareThickness, 0, 0, -1);

    preShareCopyBuffer(shareBuffers.sh_corner_FUL_out, shareThickness, shareThickness, shareThickness, -1, -1, -1);
    preShareCopyBuffer(shareBuffers.sh_corner_BDR_out, shareThickness, shareThickness, shareThickness, 1, 1, 1);
    preShareCopyBuffer(shareBuffers.sh_corner_FUR_out, shareThickness, shareThickness, shareThickness, 1, -1, -1);
    preShareCopyBuffer(shareBuffers.sh_corner_BDL_out, shareThickness, shareThickness, shareThickness, -1, 1, 1);
    preShareCopyBuffer(shareBuffers.sh_corner_FDL_out, shareThickness, shareThickness, shareThickness, -1, 1, -1);
    preShareCopyBuffer(shareBuffers.sh_corner_BUR_out, shareThickness, shareThickness, shareThickness, 1, -1, 1);
    preShareCopyBuffer(shareBuffers.sh_corner_FDR_out, shareThickness, shareThickness, shareThickness, 1, 1, -1);
    preShareCopyBuffer(shareBuffers.sh_corner_BUL_out, shareThickness, shareThickness, shareThickness, -1, -1, 1);

    preShareCopyBuffer(shareBuffers.sh_edge_UL_out, shareThickness, shareThickness, nodeSize, -1, -1, 0);
    preShareCopyBuffer(shareBuffers.sh_edge_DR_out, shareThickness, shareThickness, nodeSize, 1, 1, 0);
    preShareCopyBuffer(shareBuffers.sh_edge_UR_out, shareThickness, shareThickness, nodeSize, 1, -1, 0);
    preShareCopyBuffer(shareBuffers.sh_edge_DL_out, shareThickness, shareThickness, nodeSize, -1, 1, 0);

    preShareCopyBuffer(shareBuffers.sh_edge_FL_out, shareThickness, nodeSize, shareThickness, -1, 0, -1);
    preShareCopyBuffer(shareBuffers.sh_edge_BR_out, shareThickness, nodeSize, shareThickness, 1, 0, 1);
    preShareCopyBuffer(shareBuffers.sh_edge_FR_out, shareThickness, nodeSize, shareThickness, 1, 0, -1);
    preShareCopyBuffer(shareBuffers.sh_edge_BL_out, shareThickness, nodeSize, shareThickness, -1, 0, 1);

    preShareCopyBuffer(shareBuffers.sh_edge_FU_out, nodeSize, shareThickness, shareThickness, 0, -1, -1);
    preShareCopyBuffer(shareBuffers.sh_edge_BD_out, nodeSize, shareThickness, shareThickness, 0, 1, 1);
    preShareCopyBuffer(shareBuffers.sh_edge_FD_out, nodeSize, shareThickness, shareThickness, 0, 1, -1);
    preShareCopyBuffer(shareBuffers.sh_edge_BU_out, nodeSize, shareThickness, shareThickness, 0, -1, 1);
}

void NodeSimulationInterfaceGameOfLife::iter() {
    const auto nodeSize = node.get_node_size();
    const auto shareThickness = node.get_share_thickness();
    const auto currentArrayIndex = node.get_current_array_idx();
    const auto mainArraySize = node.get_main_array_size();
    char ***input_array = node.get_main_array_input();
    char ***output_array = node.get_main_array_output();

    for (int z = shareThickness; z < nodeSize + shareThickness; z++) {
        for (int y = shareThickness; y < nodeSize + shareThickness; y++) {
            for (int x = shareThickness; x < nodeSize + shareThickness; x++) {
                int val = 0;

                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        for (int k = -1; k <= 1; k++) {
                            if (i == 0 && j == 0 && z == 0) {
                                continue;
                            }

                            val += input_array[z + i][y + j][x + k] == 1;
                        }
                    }
                }

                if (input_array[z][y][x] == 0) {
                    if ((val >= 13 && val <= 14) || val == 17 || val == 18 || val == 19) {
                        output_array[z][y][x] = 1;
                    } else {
                        output_array[z][y][x] = 0;
                    }
                } else {
                    if (val >= 13 && val <= 26) {
                        output_array[z][y][x] = 1;
                    } else {
                        output_array[z][y][x] = 0;
                    }
                }
            }
        }
    }
}

void NodeSimulationInterfaceGameOfLife::postShareCopy() {
    const auto &shareBuffers = node.get_share_buffers();
    const auto nodeSize = node.get_node_size();
    const auto shareThickness = node.get_share_thickness();

    postShareCopyBuffer(shareBuffers.sh_vertical_U_in, nodeSize, shareThickness, nodeSize, 0, -1, 0);
    postShareCopyBuffer(shareBuffers.sh_vertical_D_in, nodeSize, shareThickness, nodeSize, 0, 1, 0);
    postShareCopyBuffer(shareBuffers.sh_horizontal_L_in, shareThickness, nodeSize, nodeSize, -1, 0, 0);
    postShareCopyBuffer(shareBuffers.sh_horizontal_R_in, shareThickness, nodeSize, nodeSize, 1, 0, 0);
    postShareCopyBuffer(shareBuffers.sh_depth_B_in, nodeSize, nodeSize, shareThickness, 0, 0, 1);
    postShareCopyBuffer(shareBuffers.sh_depth_F_in, nodeSize, nodeSize, shareThickness, 0, 0, -1);

    postShareCopyBuffer(shareBuffers.sh_corner_FUL_in, shareThickness, shareThickness, shareThickness, -1, -1, -1);
    postShareCopyBuffer(shareBuffers.sh_corner_BDR_in, shareThickness, shareThickness, shareThickness, 1, 1, 1);
    postShareCopyBuffer(shareBuffers.sh_corner_FUR_in, shareThickness, shareThickness, shareThickness, 1, -1, -1);
    postShareCopyBuffer(shareBuffers.sh_corner_BDL_in, shareThickness, shareThickness, shareThickness, -1, 1, 1);
    postShareCopyBuffer(shareBuffers.sh_corner_FDL_in, shareThickness, shareThickness, shareThickness, -1, 1, -1);
    postShareCopyBuffer(shareBuffers.sh_corner_BUR_in, shareThickness, shareThickness, shareThickness, 1, -1, 1);
    postShareCopyBuffer(shareBuffers.sh_corner_FDR_in, shareThickness, shareThickness, shareThickness, 1, 1, -1);
    postShareCopyBuffer(shareBuffers.sh_corner_BUL_in, shareThickness, shareThickness, shareThickness, -1, -1, 1);

    postShareCopyBuffer(shareBuffers.sh_edge_UL_in, shareThickness, shareThickness, nodeSize, -1, -1, 0);
    postShareCopyBuffer(shareBuffers.sh_edge_DR_in, shareThickness, shareThickness, nodeSize, 1, 1, 0);
    postShareCopyBuffer(shareBuffers.sh_edge_UR_in, shareThickness, shareThickness, nodeSize, 1, -1, 0);
    postShareCopyBuffer(shareBuffers.sh_edge_DL_in, shareThickness, shareThickness, nodeSize, -1, 1, 0);

    postShareCopyBuffer(shareBuffers.sh_edge_FL_in, shareThickness, nodeSize, shareThickness, -1, 0, -1);
    postShareCopyBuffer(shareBuffers.sh_edge_BR_in, shareThickness, nodeSize, shareThickness, 1, 0, 1);
    postShareCopyBuffer(shareBuffers.sh_edge_FR_in, shareThickness, nodeSize, shareThickness, 1, 0, -1);
    postShareCopyBuffer(shareBuffers.sh_edge_BL_in, shareThickness, nodeSize, shareThickness, -1, 0, 1);

    postShareCopyBuffer(shareBuffers.sh_edge_FU_in, nodeSize, shareThickness, shareThickness, 0, -1, -1);
    postShareCopyBuffer(shareBuffers.sh_edge_BD_in, nodeSize, shareThickness, shareThickness, 0, 1, 1);
    postShareCopyBuffer(shareBuffers.sh_edge_FD_in, nodeSize, shareThickness, shareThickness, 0, 1, -1);
    postShareCopyBuffer(shareBuffers.sh_edge_BU_in, nodeSize, shareThickness, shareThickness, 0, -1, 1);
}

inline void NodeSimulationInterfaceGameOfLife::preShareCopyBuffer(char *output_buffer, int size_x, int size_y, int size_z, int out_x, int out_y, int out_z) {
    const auto nodeSize = node.get_node_size();
    const auto shareThickness = node.get_share_thickness();
    char ***inputArray = node.get_main_array_input();

    if (node.node_in_grid(out_x, out_y, out_z)) {
        for (int z = 0; z < size_z; z++) {
            for (int y = 0; y < size_y; y++) {
                for (int x = 0; x < size_x; x++) {
                    int x_offset = (out_x == 1 ? nodeSize : shareThickness);
                    int y_offset = (out_y == 1 ? nodeSize : shareThickness);
                    int z_offset = (out_z == 1 ? nodeSize : shareThickness);
                    output_buffer[z * size_y * size_x + y * size_x + x] = inputArray[z_offset + z][y_offset + y][x_offset + x];
                }
            }
        }
    }
}

inline void NodeSimulationInterfaceGameOfLife::postShareCopyBuffer(char *input_buffer, int size_x, int size_y, int size_z, int out_x, int out_y, int out_z) {
    const auto nodeSize = node.get_node_size();
    const auto shareThickness = node.get_share_thickness();
    char ***inputArray = node.get_main_array_input();

    if (node.node_in_grid(out_x, out_y, out_z)) {
        for (int z = 0; z < size_z; z++) {
            for (int y = 0; y < size_y; y++) {
                for (int x = 0; x < size_x; x++) {
                    int x_offset = (out_x == 1 ? nodeSize + shareThickness : (out_x == 0 ? shareThickness : 0));
                    int y_offset = (out_y == 1 ? nodeSize + shareThickness : (out_y == 0 ? shareThickness : 0));
                    int z_offset = (out_z == 1 ? nodeSize + shareThickness : (out_z == 0 ? shareThickness : 0));
                    inputArray[z_offset + z][y_offset + y][x_offset + x] = input_buffer[z * size_y * size_x + y * size_x + x];
                }
            }
        }
    }
}
