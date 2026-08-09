#pragma once

#include <array>

namespace chess {

class Chess;

class ChessInformationPanel {
public:
  void OnUIRender(Chess& chess);

private:
  std::array<char, 100> fenTextInputData_;
};

}; // namespace chess