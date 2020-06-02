#include "Source/WSimSimulation/Utils/OpenCL.h"

#include <cstdint>
#include <iostream>

static void displayDeviceType(std::ostream &out, cl_device_type type) {
    switch (type) {
    case CL_DEVICE_TYPE_CPU:
        out << "CL_DEVICE_TYPE_CPU";
        break;
    case CL_DEVICE_TYPE_GPU:
        out << "CL_DEVICE_TYPE_GPU";
        break;
    case CL_DEVICE_TYPE_ACCELERATOR:
        out << "CL_DEVICE_TYPE_ACCELERATOR";
        break;
    case CL_DEVICE_TYPE_DEFAULT:
        out << "CL_DEVICE_TYPE_DEFAULT";
        break;
    default:
        out << "<unknown>";
        break;
    }
}

void displayDevice(cl_platform_id platform, cl_device_type deviceType, size_t index) {
    OCL::Device device = OCL::createDevice(platform, deviceType, index);
    OCL::DeviceInfo info = OCL::getDeviceInfo(device);
    std::cout << "\t\tdevice.name: " << info.name << '\n'
              << "\t\tdevice.profile: " << info.profile << '\n'
              << "\t\tdevice.deviceType: ";
    displayDeviceType(std::cout, info.deviceType);
    std::cout << '\n';
    std::cout << "\t\tdevice.image3DMaxSize: "
              << "{ " << info.image3DMaxSize.x << ", " << info.image3DMaxSize.y << ", " << info.image3DMaxSize.z << "}\n"
              << "\t\tdevice.maxComputeUnits: " << info.maxComputeUnits << '\n'
              << "\t\tdevice.maxWorkGroupSize: " << info.maxWorkGroupSize << '\n';
    //<< "\t\tdevice.extensions: " << info.extensions << '\n';
}

void displayPlatform(size_t index) {
    cl_platform_id platform = OCL::createPlatform(index);
    OCL::PlatformInfo info = OCL::getPlatformInfo(platform);
    std::cout << "\tplatform.name:" << info.name << '\n'
              << "\tplatform.vendor:" << info.vendor << '\n'
              << "\tplatform.version: " << info.version << '\n'
              << "\tplatform.profile: " << info.profile << '\n'
              //<< "\tplatform.extensions: " << info.extensions << '\n'
              << "\tplatform.devices: " << '\n';

    cl_uint devicesNum{};
    cl_int retVal = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, nullptr, &devicesNum);
    for (auto i = 0u; i < devicesNum; i++) {
        displayDevice(platform, CL_DEVICE_TYPE_ALL, i);
        std::cout << '\n';
    }
}

int main() {
    cl_uint platformsNum{};
    cl_int retVal = clGetPlatformIDs(0, nullptr, &platformsNum);

    std::cout << "Platforms detected: " << platformsNum << '\n';
    for (auto i = 0u; i < platformsNum; i++) {
        displayPlatform(i);
        std::cout << "\n\n";
    }
}
