#pragma once

#include "chess/core/move.h"

#include <string>
#include <vector>
#include <map>

typedef void* HANDLE;

namespace chess {
namespace test {

class Stockfish {
public:
  explicit Stockfish(const std::string& exePath);
  ~Stockfish();

  void Position(const std::string& fen, const std::vector<Move>& moves);
  std::map<std::string, uint64_t> Perft(uint32_t depth);
  void Uci();

private:
  void Send(const std::string& command) const;
  std::string ReadLine() const;
  std::vector<std::string> ReadUntil(const std::string& marker) const;

  HANDLE process_{};
  HANDLE thread_{};
  HANDLE stdin_{};
  HANDLE stdout_{};
};

} // namespace test
} // namespace chess