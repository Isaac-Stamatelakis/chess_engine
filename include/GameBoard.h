//
// Created by Isaac on 2026-01-03.
//

#ifndef CHESSENGINE_GAMEBOARD_H
#define CHESSENGINE_GAMEBOARD_H
#include <array>

static constexpr int ROWS = 8;
static constexpr int BOARD_SIZE = ROWS*ROWS;

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
    Black
};

enum PieceMoveState {
    NotMoved,
    Moved
};

enum PieceProtectionState {
    NotProtected,
    Protected,
};

struct Piece {
    PieceType type;
    PieceColor color;
    PieceMoveState moveState;
    PieceProtectionState protectionState;
};

struct PiecePosition {
    short row;
    short col;

    int ToIndex() const {
        return row * ROWS + col;
    }

    bool OutOfBounds() const {
        return row >= BOARD_SIZE || col >= BOARD_SIZE || row < 0 || col < 0;
    }

    PiecePosition operator+(const PiecePosition& other) const {
        short newRow = row + other.row;
        short newCol = col + other.col;
        return {newRow, newCol};
    }

    PiecePosition operator-(const PiecePosition& other) const {
        short newRow = row + other.row;
        short newCol = col + other.col;
        return {newRow, newCol};
    }
};

struct PieceMove {
    Piece piece;
    PiecePosition Position;
};

class GameBoard {
public:
    Piece Pieces[BOARD_SIZE];
    PieceMove LastMove;
    bool IsOccupied(PiecePosition piecePosition) const;
};




#endif //CHESSENGINE_GAMEBOARD_H