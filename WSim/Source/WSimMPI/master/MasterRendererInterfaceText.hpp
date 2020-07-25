#pragma once

#include "Source/WSimCommon/MainLooper.hpp"
#include "Source/WSimMPI/master/MasterRendererInterface.hpp"

class TextRenderer : public MainLooper {
public:
    TextRenderer() : MainLooper(std::chrono::seconds(0)) {}

protected:
    void update(float deltaTime) override;
};

class MasterRendererInterfaceText : public MasterRendererInterface {
public:
    constexpr static size_t mainBufferTexelSize = 4 * sizeof(float);
    MasterRendererInterfaceText(Master &master);
    virtual void mainLoop() override;

private:
    TextRenderer textRenderer;
};
