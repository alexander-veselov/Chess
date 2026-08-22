#pragma once

#include "chess/core/castling_rights.h"
#include "chess/core/color.h"
#include "chess/core/hash.h"
#include "chess/core/piece.h"
#include "chess/core/square.h"

namespace chess {

struct State;
Hash CalculateHash(const State& state);
void UpdatePiece(Hash& hash, Square square, Piece piece);
void UpdateTurn(Hash& hash);
void UpdateCastlingRights(Hash& hash, CastlingRightsMask mask);
void UpdateEnPassant(Hash& hash, File file);

}