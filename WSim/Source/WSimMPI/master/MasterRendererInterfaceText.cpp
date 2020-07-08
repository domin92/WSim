#include "Source/WSimMPI/master/MasterRendererInterfaceText.h"

#include <mpi.h>

void TextRenderer::update(float deltaTime) {
    MPI_Barrier(MPI_COMM_WORLD);
}

MasterRendererInterfaceText::MasterRendererInterfaceText(Master &master)
    : MasterRendererInterface(master) {}

void MasterRendererInterfaceText::mainLoop() {
    textRenderer.mainLoop();
}
