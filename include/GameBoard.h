//
// Created by Isaac on 2026-01-03.
//

#ifndef CHESSENGINE_GAMEBOARD_H
#define CHESSENGINE_GAMEBOARD_H
#include <array>

static constexpr int BOARD_SIZE = 64;

enum PieceType {
    None = 0,
    King = 1,
    Queen = 2,
    Rook = 3,
    Knight = 4,
    Bishop = 5,
    Pawn = 6
};

enum PieceColor {
    White,
    Black,
    NoColor
};

enum PieceMoveState {
    NotMoved,
    Moved
};

struct Piece {
    PieceType type;
    PieceColor color;
    PieceMoveState MoveState;
};

struct PieceMove {
    Piece piece;
    int Position;
};

struct PieceMoveQuery {
    std::array<int, BOARD_SIZE> moves;
    int MoveCount;
};

class GameBoard {
public:
    Piece Pieces[BOARD_SIZE];
    PieceMove LastMove;

    void GetValidMoves(int piecePosition, PieceMoveQuery moveQuery) const;
};




#endif //CHESSENGINE_GAMEBOARD_H