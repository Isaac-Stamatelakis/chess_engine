//
// Created by Isaac on 2026-01-03.
//

#ifndef CHESSENGINE_GAMEBOARD_H
#define CHESSENGINE_GAMEBOARD_H
#include <array>
#include <cstdint>
#include <iosfwd>
#include <locale>
#include <memory>
#include <optional>
#include <vector>

class GameBoard;
static constexpr int GRID_SIZE = 8;
static constexpr int BOARD_SIZE = GRID_SIZE*GRID_SIZE;

enum class PieceType {
    None = 0,
    King = 1,
    Queen = 2,
    Rook = 3,
    Knight = 4,
    Bishop = 5,
    Pawn = 6
};

enum class PieceColor {
    White,
    Black
};

enum class PieceMoveState {
    NotMoved,
    Moved
};

enum class OccuputationState {
    NotProtected,
    Protected,
};

struct Piece {
    PieceType type;
    PieceColor color;
    PieceMoveState moveState;
    OccuputationState protectionState;
};

enum class Axis {
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
            case Axis::Horizontal:
                col = GRID_SIZE - col - 1;
                break;
            case Axis::Vertical:
                row = GRID_SIZE - row - 1;
                break;
        }
    }

    int GetBitMapPosition() const {
        return row * GRID_SIZE + col;
    }

    uint64_t GetBitMapMask() const {
        int position = GetBitMapPosition();
        return 1ULL << position;
    }
};

enum class MoveType {
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

struct KingCheckData {
    std::vector<PiecePosition> interceptPositions;
    int attackers;
};

enum class ColorBitBoardType {
    None = 0,
    Occupied = 1,
    Attacked = 2,
    Pinned = 3,
};

struct ColorBitBoards {
    uint64_t occupiedSquares;
    uint64_t attackedSquares;
    uint64_t pinnedPieces;

    bool IsActive(PiecePosition piecePosition, ColorBitBoardType bitBoard) const {
        uint64_t bitmap = GetBitMap(bitBoard);
        int pos = piecePosition.GetBitMapPosition();
        return (bitmap & (1ULL << pos)) != 0;
    }

    uint64_t GetBitMap(ColorBitBoardType bitBoard) const {
        switch (bitBoard) {
            case ColorBitBoardType::Attacked:
                return attackedSquares;
            case ColorBitBoardType::Occupied:
                return occupiedSquares;
            case ColorBitBoardType::Pinned:
                return pinnedPieces;
            default:
                return 0;
        }
    }
};




class GameBoard {
public:
    void LoadDefaultBoard();
    void ClearBoard();

    Piece &GetPiece(PiecePosition position);
    void MovePiece(PiecePosition from, PiecePosition to);
    void ExecuteMove(PieceMove move, PiecePosition piecePosition);
    void SetLastMove(PieceMove move, Piece piece);
    PieceMoveHistory& GetLastMove();
    bool RowOccupied(PiecePosition initialPosition, int direction, int checkCount);
    ColorBitBoards& GetColorBitBoards(PieceColor pieceColor);
    ColorBitBoards CalculateBitBoards(PieceColor pieceColor);

private:
    void LoadPieceDeclarations(const std::vector<PieceDeclaration>& pieceDeclarations, PieceColor pieceColor, short row);

    Piece pieces[GRID_SIZE][GRID_SIZE] = {};
    PieceMoveHistory pieceMoveHistory = {};
    ColorBitBoards whiteBitBoard = {};
    ColorBitBoards blackBitBoard = {};
};


#endif //CHESSENGINE_GAMEBOARD_H