//
// Created by Isaac on 2026-01-03.
//

#include "../include/GameBoard.h"

void GameBoard::GetValidMoves(int piecePosition, PieceMoveQuery moveQuery) const {
    Piece piece = Pieces[piecePosition];

    switch (piece.type) {
        case None:
            moveQuery.MoveCount = 0;
            break;
        case King:
            break;
        case Queen:
            break;
        case Rook:
            break;
        case Knight:
            break;
        case Bishop:
            break;
        case Pawn:
            break;
    }
}

