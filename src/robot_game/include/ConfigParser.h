#pragma once
#include "Config.h"
#include <string>
#include <stdexcept>


class ConfigException : public std::exception {
public:
    explicit ConfigException(const std::string& msg) : msg_(msg) {}
    const char* what() const noexcept override { return msg_.c_str(); }
private:
    std::string msg_;
};

class ConfigParser {
public:
    static GameConfig load(const std::string& filename);
};
