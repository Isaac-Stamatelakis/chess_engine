//
// Created by Isaac on 2026-01-03.
//

#ifndef CHESSENGINE_GAMEBOARD_H
#define CHESSENGINE_GAMEBOARD_H
#include <array>
#include <optional>
#include <vector>

static constexpr int GRID_SIZE = 8;
static constexpr int BOARD_SIZE = GRID_SIZE*GRID_SIZE;

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

enum MoveType {
    Standard,
    Castle
};
struct PieceMove {
    MoveType type;
    PiecePosition Position;
};


struct PieceDeclaration {
    PieceType type;
};

class GameBoard {
public:
    PieceMove LastMove;
    void LoadDefaultBoard();
    void ClearBoard();
    Piece GetPiece(PiecePosition position);

private:
    void LoadPieceDeclarations(const std::vector<PieceDeclaration>& pieceDeclarations, PieceColor pieceColor, short row);
    Piece pieces[GRID_SIZE][GRID_SIZE];
};




#endif //CHESSENGINE_GAMEBOARD_H