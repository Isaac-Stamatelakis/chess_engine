//
// Created by Isaac on 2026-01-03.
//

#ifndef CHESSENGINE_GAMEBOARD_H
#define CHESSENGINE_GAMEBOARD_H
#include <array>
#include <iosfwd>
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

enum OccuputationState {
    NotProtected,
    Protected,
};

struct Piece {
    PieceType type;
    PieceColor color;
    PieceMoveState moveState;
    OccuputationState protectionState;
};

enum Axis {
    Horizontal,
    Vertical
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

    bool operator==(const PiecePosition & piece_position) const {
        return piece_position.row == row && piece_position.col == col;
    };

    void InvertAxis(Axis axis) {
        switch (axis) {
            case Horizontal:
                col = GRID_SIZE - col - 1;
                break;
            case Vertical:
                row = GRID_SIZE - row - 1;
                break;
        }
    }
};

enum MoveType {
    Standard,
    DoublePawnPush,
    ShortCastle,
    LongCastle,
    Promotion,
    EnPassant
};

struct PieceMove {
    MoveType type;
    PiecePosition position;
    PieceType promotion;
};


struct PieceMoveHistory {
    PieceMove move;
    Piece piece;
};

struct PieceDeclaration {
    PieceType type;
};

class GameBoard {
public:
    void LoadDefaultBoard();
    void ClearBoard();
    Piece GetPiece(PiecePosition position);
    void MovePiece(PiecePosition from, PiecePosition to);
    void ExecuteMove(PieceMove move, PiecePosition piecePosition);
    void SetLastMove(PieceMove move, Piece piece);
    PieceMoveHistory& GetLastMove();
    bool RowOccupied(PiecePosition initialPosition, int direction, int checkCount);


private:
    void LoadPieceDeclarations(const std::vector<PieceDeclaration>& pieceDeclarations, PieceColor pieceColor, short row);
    Piece pieces[GRID_SIZE][GRID_SIZE];

    PieceMoveHistory pieceMoveHistory;

};




#endif //CHESSENGINE_GAMEBOARD_H