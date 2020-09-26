#pragma once

#include "Source/WSimCommon/Error.hpp"

#include <memory>
#include <string>

struct SimulationMode {
    enum class Enum {
        Graphical2D,
        Graphical3D,
        Text,
    } value;
    SimulationMode(Enum value) : value(value) {}

    static std::unique_ptr<SimulationMode> fromString(const std::string &modeString) {
        if (modeString == "graphical2d") {
            return std::make_unique<SimulationMode>(Enum::Graphical2D);
        } else if (modeString == "graphical3d") {
            return std::make_unique<SimulationMode>(Enum::Graphical3D);
        } else if (modeString == "text") {
            return std::make_unique<SimulationMode>(Enum::Text);
        } else {
            return nullptr;
        }
    }

    bool is2D() {
        switch (value) {
        case Enum::Graphical2D:
        case Enum::Text:
            return true;
        default:
            return false;
        }
    }

    std::string toString() const {
        switch (value) {
        case Enum::Graphical2D:
            return "Graphical2D";
        case Enum::Graphical3D:
            return "Graphical3D";
        case Enum::Text:
            return "Text";
        default:
            wsimError();
        }
    }
};
