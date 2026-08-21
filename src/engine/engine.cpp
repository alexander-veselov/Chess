#include "chess/engine/engine.h"

#include "chess/core/attacks.h"
#include "chess/core/bits.h"
#include "chess/core/game.h"
#include "chess/core/random.h"
#include "chess/engine/pv_table.h"
#include "chess/engine/transposition_table.h"

#include <algorithm>
#include <array>
#include <chrono>

namespace chess {
namespace {

constexpr auto kPieceValues = [] {
  std::array<Score, kPieceCount> values{};
  values[kNone]         =    0;
  values[kWhiteKing]    =    0;
  values[kWhiteQueen]   = +900;
  values[kWhiteRook]    = +500;
  values[kWhiteBishop]  = +325;
  values[kWhiteKnight]  = +310;
  values[kWhitePawn]    = +100;
  values[kBlackKing]    =    0;
  values[kBlackQueen]   = -900;
  values[kBlackRook]    = -500;
  values[kBlackBishop]  = -325;
  values[kBlackKnight]  = -310;
  values[kBlackPawn]    = -100;
  return values;
}();

constexpr auto kBasePieceCount = static_cast<size_t>(BasePiece::kBasePieceCount);
constexpr auto kMvvLva = [] {
  auto table = std::array<std::array<Score, kBasePieceCount>, kBasePieceCount>{};
  for (auto attacker = 0; attacker < kBasePieceCount; ++attacker) {
    for (auto victim = 0; victim < kBasePieceCount; ++victim) {
      const auto victimPiece = static_cast<BasePiece>(victim);
      if (victimPiece == BasePiece::kNone || victimPiece == BasePiece::kKing) {
        continue;
      }
      table[attacker][victim] = kPieceValues[victim] * 10 - kPieceValues[attacker];
    }
  }
  return table;
}();

Score MvvLva(const Board& board, Move move) {
  const auto fromPiece = GetBasePiece(board[GetFrom(move)]);
  const auto toPiece = GetBasePiece(board[GetTo(move)]);
  return kMvvLva[static_cast<size_t>(fromPiece)][static_cast<size_t>(toPiece)];
}

Bitboard GetPiecesOfColor(const State& state, Color color) {
  auto pieces = Bitboard{};
  if (color == Color::kWhite) {
    pieces |= state.bitboards[kWhiteKing];
    pieces |= state.bitboards[kWhiteQueen];
    pieces |= state.bitboards[kWhiteRook];
    pieces |= state.bitboards[kWhiteBishop];
    pieces |= state.bitboards[kWhiteKnight];
    pieces |= state.bitboards[kWhitePawn];
  } else if (color == Color::kBlack) {
    pieces |= state.bitboards[kBlackKing];
    pieces |= state.bitboards[kBlackQueen];
    pieces |= state.bitboards[kBlackRook];
    pieces |= state.bitboards[kBlackBishop];
    pieces |= state.bitboards[kBlackKnight];
    pieces |= state.bitboards[kBlackPawn];
  }
  return pieces;
}

I8 AttackCount(Piece piece, Square square, Bitboard mask, Bitboard occupancy) {
  switch (GetBasePiece(piece)) {
  case BasePiece::kKing:
    return PopCount(KingAttacks(square) & mask);
  case BasePiece::kKnight:
    return PopCount(KnightAttacks(square) & mask);
  case BasePiece::kQueen:
    return PopCount(QueenAttacks(square, occupancy) & mask);
  case BasePiece::kBishop:
    return PopCount(BishopAttacks(square, occupancy) & mask);
  case BasePiece::kRook:
    return PopCount(RookAttacks(square, occupancy) & mask);
  case BasePiece::kPawn:
    return 0;
  }
  return 0;
}

Score EvaluatePiece(Piece piece, Bitboard bitboard) {
  const auto baseValue = kPieceValues[piece];
  const auto pieceCount = PopCount(bitboard);
  const auto value = baseValue * pieceCount;
  return value;
}

Score EvaluateBoard2(const State& state) {
  auto value = 0;
  const auto nonWhite = ~GetPiecesOfColor(state, Color::kWhite);
  const auto nonBlack = ~GetPiecesOfColor(state, Color::kBlack);
  const auto occupancy = ~state.bitboards[Piece::kNone];
  for (auto squareIndex = 0; squareIndex < kBoardTotalSize; ++squareIndex) {
    const auto piece = state.board[squareIndex];
    value += kPieceValues[piece];
    const auto mask = GetPieceColor(piece) == Color::kWhite ? nonWhite : nonBlack;
    const auto attacks = AttackCount(piece, (Square)squareIndex, mask, occupancy);
    value += GetPieceColor(piece) == Color::kWhite ? attacks : -attacks;
  }
  return value;
}

Score EvaluateBoard(const State& state) {
  auto value = 0;
  for (auto pieceIndex = 1; pieceIndex < kPieceCount; ++pieceIndex) {
    const auto piece = static_cast<Piece>(pieceIndex);
    value += EvaluatePiece(piece, state.bitboards[piece]);
  }
  return value;
}

Score EvaluateState(const State& state) {
  const auto score = EvaluateBoard(state);
  return state.turn == Color::kWhite ? score : -score;
}

void OrderMoves(const State& state, Moves& moves, TTEntry const* ttEntry = nullptr,
                bool shuffle = false) {
  if (moves.empty()) {
    return;
  }
  if (shuffle) {
    std::shuffle(moves.begin(), moves.end(), GetRNG());
  }
  if (ttEntry) {
    auto it = std::find(moves.begin(), moves.end(), ttEntry->move);
    if (it != moves.end()) {
      std::swap(*moves.begin(), *it);
    }
  }
  std::sort(moves.begin() + 1, moves.end(), [&state](Move move1, Move move2) {
    return MvvLva(state.board, move1) > MvvLva(state.board, move2);
  });
}

Score Quiesce(const State& state, Score alpha, Score beta, U32 ply, U64& nodes,
              TranspositionTable& transpotisionTable) {

  ++nodes;

  const auto* ttEntry = transpotisionTable.GetEntry(state.hash);
  if (const auto ttScore = transpotisionTable.Probe(ttEntry, ply, 0, alpha, beta)) {
    return ttScore.value();
  }

  auto moves = Moves{};
  auto bestValue = Score{};
  if (IsInCheck(state, state.turn)) {
    bestValue = Score(-kInfinity);
    GetLegalMoves(state, moves);
    if (moves.empty()) {
      const auto value = MatedIn(ply);
      transpotisionTable.Record(state.hash, ply, 0, value, TTEntryType::kExact, kInvalidMove);
      return value;
    }
  } else {
    const auto staticEval = EvaluateState(state);
    bestValue = staticEval;
    if (bestValue >= beta) {
      transpotisionTable.Record(state.hash, ply, 0, bestValue, TTEntryType::kLowerBound,
                                kInvalidMove);
      return bestValue;
    }
    if (bestValue > alpha) {
      alpha = bestValue;
    }
    GetCaptures(state, moves); // TODO: add promotions
  }

  OrderMoves(state, moves, ttEntry);

  auto bestMove = kInvalidMove;
  auto ttEntryType = TTEntryType::kUpperBound;
  for (const auto& move : moves) {
    auto childState = State{state};
    MakeMove(childState, move);

    const auto value = -Quiesce(childState, -beta, -alpha, ply + 1, nodes, transpotisionTable);
    if (value > bestValue) {
      bestValue = value;
      bestMove = move;
    }
    if (value > alpha) {
      alpha = value;
      ttEntryType = TTEntryType::kExact;
    }
    if (alpha >= beta) {
      ttEntryType = TTEntryType::kLowerBound;
      break;
    }
  }

  transpotisionTable.Record(state.hash, ply, 0, bestValue, ttEntryType, bestMove);

  return bestValue;
}

Score Negamax(const State& state, Score alpha, Score beta, U32 ply, U32 depth, U64& nodes,
              TranspositionTable& transpotisionTable, PVTable& pvTable, std::stop_token stopToken) {
  if (stopToken.stop_requested()) {
    return 0;
  }

  ++nodes;

  if (depth == 0) {
    return Quiesce(state, alpha, beta, ply, nodes, transpotisionTable);
  }

  if (Is50MoveRuleDraw(state)) {
    return kDrawScore;
  }

  if (IsThreefoldRepetition(state)) {
    return kDrawScore;
  }

  const auto* ttEntry = transpotisionTable.GetEntry(state.hash);
  if (const auto ttScore = transpotisionTable.Probe(ttEntry, ply, depth, alpha, beta)) {
    if (ttEntry && ttEntry->type == TTEntryType::kExact) {
      pvTable.Update(ply, depth, ttEntry->move);
    }
    return ttScore.value(); // TODO: fix threefold repetition bug
  }

  auto moves = Moves{};
  GetLegalMoves(state, moves);
  if (moves.empty()) {
    if (IsInCheck(state, state.turn)) {
      return MatedIn(ply);
    } else {
      return kDrawScore;
    }
  }

  OrderMoves(state, moves, ttEntry);

  auto bestValue = Score(-kInfinity);
  auto bestMove = moves[0];
  auto ttEntryType = TTEntryType::kUpperBound;
  for (const auto& move : moves) {
    auto childState = State{state};
    MakeMove(childState, move);

    const auto value = -Negamax(childState, -beta, -alpha, ply + 1, depth - 1, nodes,
                                transpotisionTable, pvTable, stopToken);
    if (value > bestValue) {
      bestValue = value;
      bestMove = move;
    }
    if (value > alpha) {
      alpha = value;
      ttEntryType = TTEntryType::kExact;
      pvTable.Update(ply, depth, move);
    }
    if (alpha >= beta) {
      ttEntryType = TTEntryType::kLowerBound;
      break;
    }
  }

  if (!stopToken.stop_requested()) {
    transpotisionTable.Record(state.hash, ply, depth, bestValue, ttEntryType, bestMove);
  }

  return bestValue;
}

void RecostructFullLine(std::vector<Move>& moves, U32 depth, const State& state, const TranspositionTable& tt) {
  auto currentState = State{state};
  for (const auto move : moves) {
    MakeMove(currentState, move);
  }
  while (moves.size() < depth) {
    if (IsGameOver(GetStatus(currentState))) {
      return;
    }
    auto entry = TTEntry{};
    if (!tt.GetEntry(currentState.hash) || entry.move == kInvalidMove) {
      return;
    }
    MakeMove(currentState, entry.move); // TODO: check if move is valid
    moves.push_back(entry.move);
  }
}

} // namespace

SearchInfo BestMove(const State& state, U32 maxDepth, SearchInfoCallback callback,
                    std::stop_token stopToken) {
  static auto transpotisionTable = TranspositionTable{};
  auto start = std::chrono::steady_clock::now();
  auto nodes = U64{0};
  auto searchInfo = SearchInfo{};
  for (auto depth = 1; depth <= maxDepth; ++depth) {
    auto pvTable = PVTable{};
    const auto score = Negamax(state, -kInfinity, kInfinity, 0, depth, nodes, transpotisionTable,
                               pvTable, stopToken);
    if (stopToken.stop_requested()) {
      break;
    }
    const auto duration = std::chrono::steady_clock::now() - start;
    const auto lineSize = IsMateInN(score) ? GetMatePly(score) : depth;
    searchInfo.hash = state.hash;
    searchInfo.score = state.turn == Color::kWhite ? score : -score;
    searchInfo.line = pvTable.GetLine(lineSize);
    RecostructFullLine(searchInfo.line, depth, state, transpotisionTable);
    searchInfo.depth = depth;
    searchInfo.nodes = nodes;
    searchInfo.nodesPerSecond = nodes / std::chrono::duration<double>(duration).count();
    if (callback) {
      callback(searchInfo);
    }
  }
  return searchInfo;
}


} // namespace chess