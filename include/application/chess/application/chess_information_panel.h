#pragma once

#include <string>

namespace chess {

class Chess;
class Analysis;

class ChessInformationPanel {
public:
  void OnUIRender(Chess& chess, Analysis& analysis);

private:
  std::string fen_;
  std::string history_;
  std::string analysisLine_;
};

}; // namespace chess