//
// Created by ocean on 2018/11/8.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include <fstream>
#include "HTRenderPipline.hpp"

HTRenderPipline::HTRenderPipline(const char *vertexShaderPath, const char *fragmentShaderPath) {

}

HTRenderPipline::~HTRenderPipline() {
}

std::vector<unsigned char> HTRenderPipline::readFileContent(const char *path) {
    std::ifstream file;
    file.open(path, std::ios_base::binary | std::ios_base::ate);
}

