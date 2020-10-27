#include "KernelManager.hpp"

KernelManager::KernelManager(OCL::Device &device, OCL::Context &context, const std::string &sourcePrefix)
    : device(device),
      context(context),
      sourcePrefix(sourcePrefix) {}

KernelManager::ProgramEntry &KernelManager::operator[](const std::string &programSourceFile) {
    auto it = programEntries.find(programSourceFile);
    if (it != programEntries.end()) {
        return it->second;
    }

    OCL::Program program = OCL::createProgramFromFile(device, context, programSourceFile, true, sourcePrefix);
    this->programEntries[programSourceFile] = ProgramEntry(std::move(program));
    return this->programEntries[programSourceFile];
}

KernelManager::ProgramEntry::ProgramEntry(OCL::Program program) : program(std::move(program)) {}

OCL::Kernel KernelManager::ProgramEntry::operator[](const std::string &kernelName) {
    return OCL::createKernel(program, kernelName.c_str());
}
