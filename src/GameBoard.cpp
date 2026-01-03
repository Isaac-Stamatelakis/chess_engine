//
// Created by Isaac on 2026-01-03.
//

#include "../include/GameBoard.h"


bool GameBoard::IsOccupied(PiecePosition piecePosition) const {
    const Piece& piece = Pieces[piecePosition.ToIndex()];
    return piece.type != PieceType::None;
}
