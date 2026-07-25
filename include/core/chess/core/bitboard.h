#pragma once

#include "chess/core/board.h"
#include "chess/core/square.h"
#include "chess/core/types.h"

#include <bit>
#include <unordered_map>
#include <random>

namespace chess {

using Bitboard = U64;

constexpr Square LSB(const Bitboard& bitboard) {
  return static_cast<Square>(std::countr_zero(bitboard));
}

constexpr Square PopLSB(Bitboard& bitboard) {
  auto square = LSB(bitboard);
  bitboard &= bitboard - 1;
  return square;
}

constexpr U64 ClearBit(U64 value, U8 position) {
  return value & ~(1ULL << position);
}

constexpr U64 SetBit(U64 value, U8 position) {
  return value | (1ULL << position);
}

constexpr U64 InvertBit(U64 value, U8 position) {
  return value ^ (1ULL << position);
}

static U64 SetBit(U64 value, U8 position, U8 bit) {
  if (bit == 0) {
    return ClearBit(value, position);
  } else {
    return SetBit(value, position);
  }
}

// NW   N   NE
//   \  |  /
// W ------- E
//   /  |  \
// SW   S   SE

enum Direction : I8 {
  SW = -9,
  S  = -8,
  SE = -7,
  W  = -1,
  E  = +1,
  NW = +7,
  N  = +8,
  NE = +9
};

constexpr Bitboard AFile =
    (1ULL << A1) |
    (1ULL << A2) |
    (1ULL << A3) |
    (1ULL << A4) |
    (1ULL << A5) |
    (1ULL << A6) |
    (1ULL << A7) |
    (1ULL << A8);

constexpr Bitboard _1Rank = 0b11111111;

constexpr Bitboard FillFile(File file) {
  return AFile << file;
}

constexpr Bitboard FillRank(Rank rank) {
  return _1Rank << (8 * rank);
}

constexpr Bitboard _2Rank = FillRank(_2);
constexpr Bitboard _7Rank = FillRank(_7);
constexpr Bitboard NotAFile = ~FillFile(_A);
constexpr Bitboard NotHFile = ~FillFile(_H);
constexpr Bitboard NotABFile = ~(FillFile(_A) | FillFile(_B));
constexpr Bitboard NotGHFile = ~(FillFile(_G) | FillFile(_H));
constexpr Bitboard Edges = FillFile(_A) | FillFile(_H) | FillRank(_1) | FillRank(_8);
constexpr Bitboard Corners = (1ULL << A1) | (1ULL << A8) | (1ULL << H1) | (1ULL << H8);

constexpr Bitboard KnightAttacks(Bitboard bitboard) {
  return ((bitboard & NotABFile) << (+NW + W)) |
         ((bitboard & NotAFile ) << (+NW + N)) |
         ((bitboard & NotHFile ) << (+NE + N)) |
         ((bitboard & NotGHFile) << (+NE + E)) |
         ((bitboard & NotGHFile) >> (-SE - E)) |
         ((bitboard & NotHFile ) >> (-SE - S)) |
         ((bitboard & NotAFile ) >> (-SW - S)) |
         ((bitboard & NotABFile) >> (-SW - W));
}

constexpr Bitboard WhitePawnAttacks(Bitboard bitboard) {
  return ((bitboard & NotAFile) << NW) |
         ((bitboard & NotHFile) << NE);
}

constexpr Bitboard BlackPawnAttacks(Bitboard bitboard) {
  return ((bitboard & NotAFile) >> -SW) |
         ((bitboard & NotHFile) >> -SE);
}

constexpr Bitboard WhitePawnSinglePushes(Bitboard bitboard, Bitboard occupancy) {
  return (bitboard << N) & ~occupancy;
}

constexpr Bitboard WhitePawnDoublePushes(Bitboard bitboard, Bitboard occupancy) {
  return WhitePawnSinglePushes(WhitePawnSinglePushes(bitboard & _2Rank, occupancy), occupancy);
}

constexpr Bitboard BlackPawnSinglePushes(Bitboard bitboard, Bitboard occupancy) {
  return (bitboard >> -S) & ~occupancy;
}

constexpr Bitboard BlackPawnDoublePushes(Bitboard bitboard, Bitboard occupancy) {
  return BlackPawnSinglePushes(BlackPawnSinglePushes(bitboard & _7Rank, occupancy), occupancy);
}

constexpr bool ValidSquare(Square square) {
  return A1 <= square && square <= H8;
}

constexpr Bitboard ShiftSquare(Square square, Direction direction) {
  const auto shifted = Square(square + direction);
  return ValidSquare(shifted) && std::abs(GetFile(square) - GetFile(shifted)) <= 1 ? 1ULL << shifted
                                                                                   : 0ULL;
}

constexpr Bitboard SlidingAttacks(Square square, Bitboard occupancy, Direction direction) {
  auto attacks = Bitboard(0ULL);

  auto shifted = square;
  auto attack = Bitboard(0ULL);
  while (attack = ShiftSquare(shifted, direction)) {
    shifted = Square(shifted + direction);
    attacks |= attack;
    if (attack & occupancy) {
      break;
    }
  }

  return attacks;
}

constexpr Bitboard SingleBishopAttacks(Square square, Bitboard occupancy) {
  return SlidingAttacks(square, occupancy, NW) |
         SlidingAttacks(square, occupancy, NE) |
         SlidingAttacks(square, occupancy, SE) |
         SlidingAttacks(square, occupancy, SW);
}

constexpr Bitboard SingleRookAttacks(Square square, Bitboard occupancy) {
  return SlidingAttacks(square, occupancy, N) |
         SlidingAttacks(square, occupancy, E) |
         SlidingAttacks(square, occupancy, S) |
         SlidingAttacks(square, occupancy, W);
}

static Bitboard SingleQueenAttacks(Square square, Bitboard occupancy) {
  return SingleBishopAttacks(square, occupancy) |
         SingleRookAttacks(square, occupancy);
}

struct Magic {
  Bitboard mask;
  Bitboard magic;
};

constexpr Bitboard BishopMask(Square square) {
  return SingleBishopAttacks(square, 0ULL) & ~Edges;
}

constexpr Bitboard RookMask(Square square) {
  const auto rank = GetRank(square);
  const auto file = GetFile(square);
  auto mask = ~Edges;
  if (rank == _1) {
    mask ^= FillRank(_1) ^ (1ULL << A1) ^ (1ULL << H1);
  }
  if (rank == _8) {
    mask ^= FillRank(_8) ^ (1ULL << A8) ^ (1ULL << H8);
  }
  if (file == _A) {
    mask ^= FillFile(_A) ^ (1ULL << A1) ^ (1ULL << A8);
  }
  if (file == _H) {
    mask ^= FillFile(_H) ^ (1ULL << H1) ^ (1ULL << H8);
  }
  return SingleRookAttacks(square, 0ULL) & mask;
}

constexpr Bitboard OccupancyFromIndex(I32 index, Bitboard mask) {
  auto occupancy = Bitboard{0ULL};
  while (mask) {
    const auto square = PopLSB(mask);
    if (index & 1) {
      occupancy |= (1ULL << square);
    }
    index >>= 1;
  }
  return occupancy;
}

template <size_t T>
static bool TryMagic(Bitboard magic, const std::array<Bitboard, T>& attacks,
                     const std::array<Bitboard, T>& occupancies, U8 shift) {
  auto hashes = std::unordered_map<I32, Bitboard>{};
  for (auto index = 0; index < occupancies.size(); ++index) {
    const auto hash = (occupancies[index] * magic) >> shift;
    if (!hashes.contains(hash)) {
      hashes[hash] = attacks[index];
    } else if (hashes[hash] != attacks[index]) {
      return false;
    }
  }
  return true;
}

constexpr Bitboard magicBishopConstants[] = {
    73607976316960896,    10696144142237698,    1445664843955724353, 10416834769134305288,
    27347087582560832,    54117281592115200,    722845612092948992,  5219683238353601026,
    30470233437766149,    36296769277440,       72128036971496448,   13907190417453285384,
    90074768304275456,    2233492111360,        144137199817375760,  577586660850696320,
    13857648623947089920, 1339856943405076496,  5189272809689350656, 2315417625590763528,
    1585337457108484112,  2306977852324593920,  146437426563322368,  4400799211538,
    11529919288114689538, 580612503113858,      1154052422143311876, 1127001568053632,
    1180088240061816837,  178138067767537,      2305896629804728832, 1153484465298540552,
    9332656079609344000,  4983251754546495552,  72068614926173185,   9223374238042817152,
    220678582375415938,   4504157973219360,     2324424772916175648, 6593552428992300064,
    576751160824958986,   1297124726660927808,  1747486824535752968, 36601659759002624,
    4639838198753656864,  10377437171397169664, 1162069514381164816, 145030126172635152,
    11822056412875031,    1152939268658823552,  140808489861124,     10380797141626979040,
    1211468583235289360,  551936983040,         2306476551267681312, 3026471734810312836,
    5358470229541144,     1731229506305393153,  577309575353501772,  8796768699392,
    1155213582642872832,  70403426893952,       74890245503455744,   1156303605091803146};

constexpr Bitboard magicRookConstants[] = {
    2630103351689248801,  36063983542730760,    1161946296333107272,  648520618389539848,
    2738197373862817826,  45040399699675144,    18019346324390656,    36030446316972288,
    505529616534877185,   668926765695000,      613087687945095169,   360569513913136168,
    281479306283008,      576751025524900104,   1157442698603397184,  2310381797575532608,
    292734113223344416,   9071239376978,        106661219992465,      74907562545251462,
    2328784319612454917,  4917966664672215556,  63337642741466120,    722845349561125120,
    90072063414386976,    6922032903220109312,  4612249002941892673,  1153009534795514882,
    147501735563821072,   90353469679997956,    10451781684814454785, 35463545024642,
    459367230713365540,   4684042817070170120,  595618643455770632,   153265873666179104,
    42784203439997184,    20271146133881217,    1271319447539716,     162165872113222340,
    35185446895620,       10737852547903000576, 143233137115140,      9299933239647568385,
    282883726147714,      1152939105383354432,  11529505321437536264, 287762849796,
    9232493587466293792,  2306133555163430976,  361490837017731588,   11540493104064768,
    13835622107037322368, 9223398438020055808,  246299203470344,      6917531364107882500,
    2341912559029911619,  3940722690497666,     9799841860402028545,  6919791823729068066,
    1153494369761239050,  2969265531785226,     1152923987173511940,  144121939768672514};

static U64 GenerateRandomU64() {
  static auto rng = std::mt19937{std::random_device{}()};
  auto dist = std::uniform_int_distribution<U64>{};
  return dist(rng);
}

static Bitboard MagicBishopAttacks(Square square, Bitboard occupancy) {
  constexpr auto kPositions = 512;
  constexpr auto kShift = 64 - 9;
  static auto bishopAttacks = std::array<std::array<Bitboard, kPositions>, Square::kSquareCount>{};
  static auto magics = std::array<Magic, Square::kSquareCount>{};
  static auto once = false;
  if (!once) {
    for (auto square = 0; square < Square::kSquareCount; ++square) {
      auto attacks = std::array<Bitboard, kPositions>{};
      const auto mask = BishopMask(Square(square));
      magics[square].mask = mask;
      auto occupancies = std::array<Bitboard, kPositions>{};
      for (auto index = 0; index < kPositions; ++index) {
        occupancies[index] = OccupancyFromIndex(index, mask);
        attacks[index] = SingleBishopAttacks(Square(square), occupancies[index]);
      }
      auto magic = Bitboard(0ULL);
      magic = magicBishopConstants[square];
      //while (!TryMagic(magic, attacks, occupancies, kShift)) {
      //  magic = GenerateRandomU64() & GenerateRandomU64() & GenerateRandomU64();
      //}
      magics[square].magic = magic;
      for (size_t i = 0; i < occupancies.size(); ++i) {
        auto hash = (occupancies[i] * magic) >> kShift;
        bishopAttacks[square][hash] = attacks[i];
      }
      //std::cout << magic << std::endl;
    }
    once = true;
  }
  occupancy &= magics[square].mask;
  occupancy *= magics[square].magic;
  occupancy >>= kShift;
  return bishopAttacks[square][occupancy];
}

static Bitboard MagicRookAttacks(Square square, Bitboard occupancy) {
  constexpr auto kPositions = 4096;
  constexpr auto kShift = 64 - 12;
  static auto rookAttacks = std::array<std::array<Bitboard, kPositions>, Square::kSquareCount>{};
  static auto magics = std::array<Magic, Square::kSquareCount>{};
  static auto once = false;
  if (!once) {
    for (auto square = 0; square < Square::kSquareCount; ++square) {
      auto attacks = std::array<Bitboard, kPositions>{};
      const auto mask = RookMask(Square(square));
      magics[square].mask = mask;
      auto occupancies = std::array<Bitboard, kPositions>{};
      for (auto index = 0; index < kPositions; ++index) {
        occupancies[index] = OccupancyFromIndex(index, mask);
        attacks[index] = SingleRookAttacks(Square(square), occupancies[index]);
      }
      auto magic = Bitboard(0ULL);
      magic = magicRookConstants[square];
      //while (!TryMagic(magic, attacks, occupancies, kShift)) {
      //  magic = GenerateRandomU64() & GenerateRandomU64() & GenerateRandomU64();
      //}
      magics[square].magic = magic;
      for (size_t i = 0; i < occupancies.size(); ++i) {
        auto hash = (occupancies[i] * magic) >> kShift;
        rookAttacks[square][hash] = attacks[i];
      }
      //std::cout << magic << std::endl;
    }
    once = true;
  }
  occupancy &= magics[square].mask;
  occupancy *= magics[square].magic;
  occupancy >>= kShift;
  return rookAttacks[square][occupancy];
}

static Bitboard BishopAttacks(Bitboard bitboard, Bitboard occupancy) {
  auto attacks = Bitboard{0ULL};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= MagicBishopAttacks(from, occupancy);
  }
  return attacks;
}

static Bitboard RookAttacks(Bitboard bitboard, Bitboard occupancy) {
  auto attacks = Bitboard{0ULL};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= MagicRookAttacks(from, occupancy);
  }
  return attacks;
}

static Bitboard QueenAttacks(Bitboard bitboard, Bitboard occupancy) {
  auto attacks = Bitboard{0ULL};
  while (bitboard) {
    const auto from = PopLSB(bitboard);
    attacks |= SingleQueenAttacks(from, occupancy);
  }
  return attacks;
}

constexpr Bitboard KingAttacks(Bitboard bitboard) {
  return ((bitboard & NotAFile) >> -SW) |
         ((bitboard           ) >> -S ) |
         ((bitboard & NotHFile) >> -SE) |
         ((bitboard & NotAFile) >> -W) |
         ((bitboard & NotHFile) << +E ) |
         ((bitboard & NotAFile) << +NW) |
         ((bitboard           ) << +N ) |
         ((bitboard & NotHFile) << +NE);
}

} // namespace chess
