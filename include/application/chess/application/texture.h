#pragma once

#include <string>

typedef unsigned int GLuint;

namespace chess {

bool LoadTextureFromSVG(GLuint& out_texture, const std::string& file, int width,
                        int height);

} // namespace chess