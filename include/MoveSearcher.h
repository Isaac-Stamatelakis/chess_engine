//
// Created by Isaac on 2026-01-03.
//

#ifndef CHESSENGINE_MOVESEARCHER_H
#define CHESSENGINE_MOVESEARCHER_H
#include <memory>

#include "GameBoard.h"

struct PieceMoveQuery {
    std::array<PiecePosition, BOARD_SIZE> moves;
    int moveCount;
};

class MoveSearcher {
public:
    void GetValidMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard) const;

private:
    void GetKingMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece) const;
    void GetQueenMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece) const;
    void GetRookMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece) const;
    void GetKnightMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece) const;
    void GetBishopMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece) const;
    void GetPawnMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece) const;
    void GenerateSlidingMoves(const Piece& piece, PiecePosition piecePosition,PieceMoveQuery &moveQuery,const std::unique_ptr<GameBoard> &gameBoard,const int directions[][2], int directionCount) const;
    void AddPawnPushMove(const Piece& piece, PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, int movement, int& idx, int direction) const;
};


#endif //CHESSENGINE_MOVESEARCHER_H