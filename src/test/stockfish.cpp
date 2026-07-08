#include "chess/test/stockfish.h"
#include "chess/test/utils.h"

// TODO: make this file compile Windows only
#include <Windows.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace chess {
namespace test {

Stockfish::Stockfish(const std::string& exePath) {
  SECURITY_ATTRIBUTES sa{};
  sa.nLength = sizeof(sa);
  sa.bInheritHandle = TRUE;

  HANDLE childStdoutRead = nullptr;
  HANDLE childStdoutWrite = nullptr;
  HANDLE childStdinRead = nullptr;
  HANDLE childStdinWrite = nullptr;

  if (!CreatePipe(&childStdoutRead, &childStdoutWrite, &sa, 0)) {
    throw std::runtime_error("CreatePipe failed");
  }

  if (!SetHandleInformation(childStdoutRead, HANDLE_FLAG_INHERIT, 0)) {
    throw std::runtime_error("SetHandleInformation failed");
  }

  if (!CreatePipe(&childStdinRead, &childStdinWrite, &sa, 0)) {
    throw std::runtime_error("CreatePipe failed");
  }

  if (!SetHandleInformation(childStdinWrite, HANDLE_FLAG_INHERIT, 0)) {
    throw std::runtime_error("SetHandleInformation failed");
  }

  STARTUPINFOA si{};
  si.cb = sizeof(si);
  si.dwFlags = STARTF_USESTDHANDLES;
  si.hStdInput = childStdinRead;
  si.hStdOutput = childStdoutWrite;
  si.hStdError = childStdoutWrite;

  PROCESS_INFORMATION pi{};

  std::string cmd = exePath;

  if (!CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr,
                      nullptr, &si, &pi)) {
    throw std::runtime_error("CreateProcess failed");
  }

  CloseHandle(childStdoutWrite);
  CloseHandle(childStdinRead);

  process_ = pi.hProcess;
  thread_ = pi.hThread;
  stdout_ = childStdoutRead;
  stdin_ = childStdinWrite;

  Uci();
}

Stockfish::~Stockfish() {
  if (stdin_) {
    CloseHandle(stdin_);
  }
  if (stdout_) {
    CloseHandle(stdout_);
  }

  if (process_) {
    TerminateProcess(process_, 0);
    CloseHandle(process_);
  }

  if (thread_) {
    CloseHandle(thread_);
  }
}

void Stockfish::Send(const std::string& command) const {
  DWORD written;

  std::string line(command);
  line += '\n';

  WriteFile(stdin_, line.data(), static_cast<DWORD>(line.size()), &written, nullptr);
}

std::string Stockfish::ReadLine() const {
  std::string line;

  char c;
  DWORD read;

  while (true) {
    if (!ReadFile(stdout_, &c, 1, &read, nullptr) || read == 0) {
      break;
    }

    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      break;
    }

    line += c;
  }

  return line;
}

std::vector<std::string> Stockfish::ReadUntil(const std::string& marker) const {
  std::vector<std::string> lines;

  while (true) {
    auto line = ReadLine();

    if (line.empty()) {
      continue;
    }

    lines.push_back(line);

    if (line.find(marker) != std::string::npos) {
      break;
    }
  }

  return lines;
}

void Stockfish::Uci() {
  Send("uci");
  ReadUntil("uciok");

  Send("isready");
  ReadUntil("readyok");
}

void Stockfish::Position(const std::string& fen, const std::vector<Move>& moves) {
  auto command = std::string{"position fen "};
  command += fen + " ";
  if (!moves.empty()) {
    command += "moves";
    for (const auto& move : moves) {
      command += " " + MoveToString(move);
    }
  }
  Send(command);
}

std::map<std::string, uint64_t> Stockfish::Perft(uint32_t depth) {
  Send(std::string{"go perft "} + std::to_string(depth));

  auto result = std::map<std::string, uint64_t>{};
  for (const auto& line : ReadUntil("Nodes searched:")) {
    if (line.rfind("info", 0) == 0) {
      continue;
    }

    constexpr auto kSeparator = std::string_view{": "};
    const auto separatorPosition = line.find(kSeparator);
    if (separatorPosition == std::string::npos) {
      continue;
    }

    const auto moveString = line.substr(0, separatorPosition);
    const auto countString = line.substr(separatorPosition + kSeparator.size());

    auto move = Move{};
    if (ParseMove(moveString, move)) {
      result[MoveToString(move)] = std::stoull(countString.c_str());
    }
  }

  return result;
}

} // namespace test
} // namespace chess