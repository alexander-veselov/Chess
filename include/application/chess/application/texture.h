#pragma once

#include <string>

typedef unsigned int GLuint;

namespace chess {

GLuint LoadTextureFromSVG(const std::string& file, int width, int height);

} // namespace chess