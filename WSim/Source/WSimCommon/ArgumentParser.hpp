#pragma once

#include <string>
#include <vector>
#include <algorithm>

class ArgumentParser {
    template <typename T>
    using ConvertFunction = T (*)(const std::string &);

public:
    ArgumentParser(int argc, char **argv) {
        for (int i = 1; i < argc; i++) {
            this->args.push_back(argv[i]);
        }
    }

    // Lookup methods, varying implementation for different types
    template <typename ResultType>
    ResultType getArgumentValue(const std::vector<std::string> &names, const ResultType &defaultValue);
    template <typename ResultType>
    std::vector<ResultType> getArgumentValues(const std::vector<std::string> &names);

private:
    // Conversion from string to various types
    template <typename Integral, typename = std::enable_if_t<std::is_integral<Integral>::value>>
    static Integral convertFunction(const std::string &arg) {
        return static_cast<Integral>(std::stoll(arg));
    }
    template <typename String, typename = std::enable_if_t<std::is_same<String, std::string>::value>>
    static std::string convertFunction(const std::string &arg) {
        return arg;
    }

    // Arguments captures
    std::vector<std::string> args;
};

template <>
inline bool ArgumentParser::getArgumentValue<bool>(const std::vector<std::string> &names, const bool &defaultValue) {
    for (auto &name : names) {
        auto nameIt = std::find(this->args.begin(), this->args.end(), name);
        if (nameIt != this->args.end()) {
            return true;
        }
    }
    return defaultValue;
}

template <typename ResultType>
inline ResultType ArgumentParser::getArgumentValue(const std::vector<std::string> &names, const ResultType &defaultValue) {
    for (auto &name : names) {
        auto nameIt = std::find(this->args.begin(), this->args.end(), name);
        if (nameIt == this->args.end())
            continue;

        auto valueIt = nameIt + 1;
        if (valueIt == this->args.end())
            continue;

        return ArgumentParser::convertFunction<ResultType>(*valueIt);
    }
    return defaultValue;
}

template <typename ResultType>
inline std::vector<ResultType> ArgumentParser::getArgumentValues(const std::vector<std::string> &names) {
    std::vector<ResultType> result{};
    for (auto &name : names) {
        auto nameIt = this->args.begin();
        while (true) {
            nameIt = std::find(nameIt, this->args.end(), name);
            if (nameIt == this->args.end()) {
                break;
            }

            auto valueIt = nameIt + 1;
            if (valueIt == this->args.end()) {
                break;
            }

            result.push_back(ArgumentParser::convertFunction<ResultType>(*valueIt));
            nameIt += 2;
        }
    }
    return result;
}
