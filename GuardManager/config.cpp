#include "config.h"

std::vector<FileAction> ConfigReader::read_config(const std::string& filename)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::vector<FileAction> fileActions;

    // Read JSON file
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return fileActions;
    }

    json config;
    try {
        config = json::parse(ifs);
    }
    catch (json::parse_error& e) {
        std::cerr << "Parse error: " << e.what() << std::endl;
        return fileActions;
    }

    // Parse JSON objects
    for (const auto& obj : config) {
        std::wstring file = converter.from_bytes(obj["file"]);
        std::string action_str = obj["action"];
        ActionType action = parse_action(action_str);
        fileActions.push_back({ file, action });
    }

    return fileActions;
}

ActionType ConfigReader::parse_action(const std::string& action_string)
{
    if (action_string == "ACCESS_DENIED") {
        return ActionType::ACCESS_DENIED;
    }
    else if (action_string == "NOTIFY") {
        return ActionType::NOTIFY;
    }
    else {
        std::cerr << "Unknown action: " << action_string << std::endl;
        return ActionType::NOTIFY; // Default action
    }
}
