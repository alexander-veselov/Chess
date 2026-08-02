#pragma once

#include "chess/core/magic_bitboards.h"

#include "chess/core/attacks.h"
#include "chess/core/bitboard.h"
#include "chess/core/random.h"
#include "chess/core/square.h"

#include <array>
#include <mutex>
#include <unordered_map>

namespace chess {
namespace {

using namespace chess;

struct Magic {
  Bitboard mask;
  Bitboard magic;
};

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

constexpr auto kBishopPositions = 512;
constexpr auto kBishopShift = 64 - 9;
auto bishopAttacks = std::array<std::array<Bitboard, kBishopPositions>, Square::kSquareCount>{};
auto bishopMagics = std::array<Magic, Square::kSquareCount>{};
auto bishopInitFlag = std::once_flag{};

constexpr auto kRookPositions = 4096;
constexpr auto kRookShift = 64 - 12;
auto rookAttacks = std::array<std::array<Bitboard, kRookPositions>, Square::kSquareCount>{};
auto rookMagics = std::array<Magic, Square::kSquareCount>{};
auto rookInitFlag = std::once_flag{};

Bitboard BishopMask(Square square) {
  return SingleBishopAttacks(square, kEmptyBoard) & ~kEdges;
}

Bitboard RookMask(Square square) {
  const auto rank = GetRank(square);
  const auto file = GetFile(square);
  auto mask = ~kEdges;
  if (rank == _1) {
    mask ^= FillRank(_1) ^ BBFromSquare(A1) ^ BBFromSquare(H1);
  }
  if (rank == _8) {
    mask ^= FillRank(_8) ^ BBFromSquare(A8) ^ BBFromSquare(H8);
  }
  if (file == _A) {
    mask ^= FillFile(_A) ^ BBFromSquare(A1) ^ BBFromSquare(A8);
  }
  if (file == _H) {
    mask ^= FillFile(_H) ^ BBFromSquare(H1) ^ BBFromSquare(H8);
  }
  return SingleRookAttacks(square, kEmptyBoard) & mask;
}

Bitboard OccupancyFromIndex(I32 index, Bitboard mask) {
  auto occupancy = kEmptyBoard;
  while (mask) {
    const auto square = PopLSB(mask);
    if (index & 1) {
      occupancy |= BBFromSquare(square);
    }
    index >>= 1;
  }
  return occupancy;
}

template <size_t T>
bool TryMagic(Bitboard magic, const std::array<Bitboard, T>& attacks,
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

void InitializeBishopMagic() {
  for (auto square = 0; square < Square::kSquareCount; ++square) {
    auto attacks = std::array<Bitboard, kBishopPositions>{};
    const auto mask = BishopMask(Square(square));
    bishopMagics[square].mask = mask;
    auto occupancies = std::array<Bitboard, kBishopPositions>{};
    for (auto index = 0; index < kBishopPositions; ++index) {
      occupancies[index] = OccupancyFromIndex(index, mask);
      attacks[index] = SingleBishopAttacks(Square(square), occupancies[index]);
    }
    auto magic = kEmptyBoard;
    magic = magicBishopConstants[square];
    // while (!TryMagic(magic, attacks, occupancies, kMagicShift)) {
    //   magic = RandomU64() & RandomU64() & RandomU64();
    // }
    bishopMagics[square].magic = magic;
    for (size_t i = 0; i < occupancies.size(); ++i) {
      auto hash = (occupancies[i] * magic) >> kBishopShift;
      bishopAttacks[square][hash] = attacks[i];
    }
    // std::cout << magic << std::endl;
  }
}

void InitializeRookMagic() {
  for (auto square = 0; square < Square::kSquareCount; ++square) {
    auto attacks = std::array<Bitboard, kRookPositions>{};
    const auto mask = RookMask(Square(square));
    rookMagics[square].mask = mask;
    auto occupancies = std::array<Bitboard, kRookPositions>{};
    for (auto index = 0; index < kRookPositions; ++index) {
      occupancies[index] = OccupancyFromIndex(index, mask);
      attacks[index] = SingleRookAttacks(Square(square), occupancies[index]);
    }
    auto magic = kEmptyBoard;
    magic = magicRookConstants[square];
    // while (!TryMagic(magic, attacks, occupancies, kRookShift)) {
    //   magic = RandomU64() & RandomU64() & RandomU64();
    // }
    rookMagics[square].magic = magic;
    for (size_t i = 0; i < occupancies.size(); ++i) {
      auto hash = (occupancies[i] * magic) >> kRookShift;
      rookAttacks[square][hash] = attacks[i];
    }
    // std::cout << magic << std::endl;
  }
}

} // namespace

Bitboard MagicBishopAttacks(Square square, Bitboard occupancy) {
  std::call_once(bishopInitFlag, InitializeBishopMagic);
  occupancy &= bishopMagics[square].mask;
  occupancy *= bishopMagics[square].magic;
  occupancy >>= kBishopShift;
  return bishopAttacks[square][occupancy];
}

Bitboard MagicRookAttacks(Square square, Bitboard occupancy) {
  std::call_once(rookInitFlag, InitializeRookMagic);
  occupancy &= rookMagics[square].mask;
  occupancy *= rookMagics[square].magic;
  occupancy >>= kRookShift;
  return rookAttacks[square][occupancy];
}

Bitboard MagicQueenAttacks(Square square, Bitboard occupancy) {
  return MagicBishopAttacks(square, occupancy) |
         MagicRookAttacks(square, occupancy);
}

} // namespace chess
