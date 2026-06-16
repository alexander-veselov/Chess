#pragma once

#include <string>

typedef unsigned int GLuint;

namespace chess {

bool LoadTextureFromSVG(const std::string& file, GLuint* out_texture, int width,
                        int height);

} // namespace chess