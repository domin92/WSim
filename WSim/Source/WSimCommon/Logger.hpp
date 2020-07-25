#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <memory>

class Logger {
    static inline std::unique_ptr<Logger> logger = {};
    std::ostream *const stream;
    const bool ownsStream;

    Logger(std::ostream &existingStream) : stream(&existingStream), ownsStream(false) {}
    Logger(const std::string &fileName) : stream(new std::ofstream(fileName, std::ios::out)), ownsStream(true) {}

public:
    ~Logger() {
        if (ownsStream) {
            delete stream;
        }
    }

    template <typename T>
    std::ostream &operator<<(T &&arg) {
        (*stream) << std::forward<T>(arg);
        stream->flush();
        return *stream;
    }

    static Logger &get() {
        if (logger == nullptr) {
            createStderrLogger();
        }
        return *logger;
    }

    static void createFileLogger(const std::string &fileName) {
        logger.reset(new Logger(fileName));
    }

    static void createFileLogger(const std::string &fileName, int suffix) {
        createFileLogger(fileName + std::to_string(suffix));
    }

    static void createStdoutLogger() {
        logger.reset(new Logger(std::cout));
    }

    static void createStderrLogger() {
        logger.reset(new Logger(std::cerr));
    }
};
