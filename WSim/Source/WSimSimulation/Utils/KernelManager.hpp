#pragma once

#include "Source/WSimSimulation/Utils/OpenCL.hpp"

#include <string>
#include <unordered_map>

class KernelManager {
public:
    KernelManager(OCL::Device &device, OCL::Context &context, const std::string &sourcePrefix);

    class ProgramEntry {
    public:
        ProgramEntry() = default;
        ProgramEntry(OCL::Program program);
        OCL::Kernel operator[](const std::string &kernelName);

    private:
        OCL::Program program{};
    };
    ProgramEntry &operator[](const std::string &programSourceFile);

private:
    OCL::Device &device;
    OCL::Context &context;
    std::unordered_map<std::string, ProgramEntry> programEntries;
    const std::string sourcePrefix;
};
