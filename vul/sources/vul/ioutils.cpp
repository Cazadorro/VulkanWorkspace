//
// Created by Shae Bolt on 9/2/2021.
//

#include "ioutils.h"
#include <fstream>

std::vector<std::uint32_t> vul::readSPIRV(const std::string &filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    if(fileSize % 4 != 0){
        throw std::runtime_error("file not word aligned!");
    }
    std::vector<std::uint32_t> buffer(fileSize / 4);

    file.seekg(0);
    file.read(reinterpret_cast<char *>(buffer.data()), fileSize);
    file.close();

    return buffer;
}
