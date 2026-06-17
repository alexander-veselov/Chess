#include "chess/application/texture.h"

#include <GLFW/glfw3.h>
#include <lunasvg.h>

namespace chess {

GLuint LoadTextureFromSVG(const std::string& file, int width, int height) {
  auto document = lunasvg::Document::loadFromFile(file);
  if (!document) {
    return 0;
  }

  auto bounds = document->boundingBox();

  if (bounds.w <= 0 || bounds.h <= 0) {
    return 0;
  }

  const auto bitmap = document->renderToBitmap(width, height);
  const auto pixels = static_cast<unsigned char*>(bitmap.data());

  auto texture = GLuint{0};
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, pixels);

  return texture;
}

} // namespace chess