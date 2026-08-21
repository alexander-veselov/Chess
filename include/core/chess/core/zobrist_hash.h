#pragma once

#include "chess/core/castling_rights.h"
#include "chess/core/color.h"
#include "chess/core/piece.h"
#include "chess/core/square.h"
#include "chess/core/types.h"

namespace chess {

struct State;

using Hash = U64;
Hash CalculateHash(const State& state);
void UpdatePiece(Hash& hash, Square square, Piece piece);
void UpdateTurn(Hash& hash);
void UpdateCastlingRights(Hash& hash, CastlingRightsMask mask);
void UpdateEnPassant(Hash& hash, File file);

}