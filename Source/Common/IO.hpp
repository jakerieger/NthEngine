// Author: Jake Rieger
// Created: 11/29/25.
//

#pragma once

#include <fstream>

namespace Nth::IO {
    inline static std::optional<std::vector<uint8_t>> ReadBytes(const std::filesystem::path& filename) {
        if (!exists(filename)) { return {}; }
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) { return {}; }
        const std::streamsize fileSize = file.tellg();
        std::vector<uint8_t> bytes(fileSize);
        file.seekg(0, std::ios::beg);
        if (!file.read(reinterpret_cast<char*>(bytes.data()), fileSize)) { return {}; }
        file.close();

        return bytes;
    }

    inline static std::string ReadString(const std::filesystem::path& filename) {
        if (!exists(filename)) { return {}; }
        const std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
}  // namespace Nth::IO