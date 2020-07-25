#include "KernelManager.hpp"

KernelManager::KernelManager(OCL::Device &device, OCL::Context &context)
    : device(device),
      context(context) {}

KernelManager::ProgramEntry &KernelManager::operator[](const std::string &programSourceFile) {
    auto it = programEntries.find(programSourceFile);
    if (it != programEntries.end()) {
        return it->second;
    }

    OCL::Program program = OCL::createProgramFromFile(device, context, programSourceFile, true);
    this->programEntries[programSourceFile] = ProgramEntry(std::move(program));
    return this->programEntries[programSourceFile];
}

KernelManager::ProgramEntry::ProgramEntry(OCL::Program program) : program(std::move(program)) {}

OCL::Kernel KernelManager::ProgramEntry::operator[](const std::string &kernelName) {
    return OCL::createKernel(program, kernelName.c_str());
}
