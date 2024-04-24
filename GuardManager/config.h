#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <codecvt>

#include "third_party/nlohmann/json.hpp"

using json = nlohmann::json;

enum class ActionType {
    ACCESS_DENIED,
    NOTIFY
};

struct FileAction {
    std::wstring file;
    ActionType action;
};

class ConfigReader {
public:
    static std::vector<FileAction> read_config(const std::string& filename);

private:
    static ActionType parse_action(const std::string& action_string);
};


