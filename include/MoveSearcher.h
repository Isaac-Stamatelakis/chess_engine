//
// Created by Isaac on 2026-01-03.
//

#ifndef CHESSENGINE_MOVESEARCHER_H
#define CHESSENGINE_MOVESEARCHER_H
#include <memory>

#include "GameBoard.h"

struct PieceMoveQuery {
    std::array<PieceMove, BOARD_SIZE> moves;
    int moveCount;
};

class MoveSearcher {
public:
    static void GetValidMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard);

private:
    static void GetKingMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece);
    static void GetQueenMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece);
    static void GetRookMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece);

    static void GetKnightMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece);

    static void GetBishopMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece);

    static void GetPawnMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece);

    static void GenerateSlidingMoves(const Piece& piece, PiecePosition piecePosition,PieceMoveQuery &moveQuery,const std::unique_ptr<GameBoard> &gameBoard,const int directions[][2], int directionCount);

    static void AddPawnPushMove(const Piece& piece, PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, int movement, int& idx, int direction);
};


#endif //CHESSENGINE_MOVESEARCHER_H