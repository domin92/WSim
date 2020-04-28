#pragma once

#include "Source/WSimSimulation/Utils/OpenCL.h"
#include <unordered_map>

class KernelManager {
public:
    KernelManager(OCL::Device &device, OCL::Context &context);

    class ProgramEntry;
    ProgramEntry& operator[](const std::string &programSourceFile);

private:
    OCL::Device &device;
    OCL::Context &context;
    std::unordered_map<std::string, ProgramEntry> programEntries;
};

class KernelManager::ProgramEntry {
public:
    ProgramEntry() = default;
    ProgramEntry(OCL::Program program);
    OCL::Kernel operator[](const std::string &kernelName);

private:
    OCL::Program program{};
};
