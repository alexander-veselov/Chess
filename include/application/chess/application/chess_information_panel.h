#pragma once

#include <string>

namespace chess {

class Chess;

class ChessInformationPanel {
public:
  void OnUIRender(Chess& chess);

private:
  std::string fen_;
  std::string history_;
};

}; // namespace chess