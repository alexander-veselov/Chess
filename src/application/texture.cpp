#include "chess/application/texture.h"

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <lunasvg.h>

namespace chess {

bool LoadTextureFromSVG(const std::string& file, GLuint* out_texture, int width,
                        int height) {

  auto document = lunasvg::Document::loadFromFile(file);
  if (!document) {
    return false;
  }

  auto bounds = document->boundingBox();

  if (bounds.w <= 0 || bounds.h <= 0) {
    return false;
  }

  const auto bitmap = document->renderToBitmap(width, height);

  const unsigned char* pixels = bitmap.data();

  glGenTextures(1, out_texture);
  glBindTexture(GL_TEXTURE_2D, *out_texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, pixels);

  return true;
}

} // namespace chess