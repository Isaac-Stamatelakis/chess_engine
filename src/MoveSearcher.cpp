//
// Created by Isaac on 2026-01-03.
//

#include "../include/MoveSearcher.h"

#include <iostream>
#include <memory>

void MoveSearcher::GetValidMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard) {
    const Piece& piece = gameBoard->GetPiece(piecePosition);

    switch (piece.type) {
        case None:
            moveQuery.moveCount = 0;
            break;
        case King:
            GetKingMoves(piecePosition, moveQuery, gameBoard, piece);
            break;
        case Queen:
            GetQueenMoves(piecePosition, moveQuery, gameBoard, piece);
            break;
        case Rook:
            GetRookMoves(piecePosition, moveQuery, gameBoard, piece);
            break;
        case Knight:
            GetKnightMoves(piecePosition, moveQuery, gameBoard, piece);
            break;
        case Bishop:
            GetBishopMoves(piecePosition, moveQuery, gameBoard, piece);
            break;
        case Pawn:
            GetPawnMoves(piecePosition, moveQuery, gameBoard, piece);
            break;
    }
}

void MoveSearcher::GetKingMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard,const Piece& piece) {
    int idx = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dx == 0) continue;
            PiecePosition movePosition(piecePosition.row + dx, piecePosition.col + dx);
            if (movePosition.OutOfBounds()) continue;
            const Piece& otherPiece = gameBoard->GetPiece(movePosition);
            if (otherPiece.type != PieceType::None && otherPiece.color == piece.color) continue;
            if (otherPiece.protectionState == PieceProtectionState::Protected) continue;
            moveQuery.moves[idx] = PieceMove{ Standard,movePosition};
            idx++;
        }
    }
    moveQuery.moveCount = idx;

    // TODO Castling
}

void MoveSearcher::GetQueenMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery,const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece) {
    const int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},  // straight lines
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // diagonals
    };
    GenerateSlidingMoves(piece, piecePosition, moveQuery, gameBoard, directions, 8);
}

void MoveSearcher::GetRookMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery,const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece) {
    const int directions[4][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };
    GenerateSlidingMoves(piece, piecePosition, moveQuery, gameBoard, directions, 4);
}

void MoveSearcher::GetKnightMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery,const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece) {
    const int knightOffsets[8][2] = {
        {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
        {-2,-1}, {-1,-2}, {1,-2}, {2,-1}
    };

    int idx = 0;
    for (int i = 0; i < 8; i++) {
        PiecePosition movePosition(piecePosition.row + knightOffsets[i][0],piecePosition.col + knightOffsets[i][1]);

        if (movePosition.OutOfBounds()) continue;
        const Piece& targetPiece = gameBoard->GetPiece(movePosition);

        // Skip friendly pieces
        if (targetPiece.type != PieceType::None && targetPiece.color == piece.color) continue;

        moveQuery.moves[idx++] = PieceMove{ Standard,movePosition};
    }

    moveQuery.moveCount = idx;

}

void MoveSearcher::GetBishopMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery,const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece) {
        const int directions[4][2] = {
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // diagonals
        };
        GenerateSlidingMoves(piece, piecePosition, moveQuery, gameBoard, directions, 4);
}

void MoveSearcher::GetPawnMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const std::unique_ptr<GameBoard> &gameBoard, const Piece& piece) {
    const int captureSquares[2][2] = {
        {1, 1}, {-1, 1}
    };

    int idx = 0;
    int direction = piece.color == PieceColor::White ? 1 : -1;

    for (int i = 0; i < 2; i++) {
        PiecePosition movePos(piecePosition.row + captureSquares[i][1]*direction,piecePosition.col + captureSquares[i][0]*direction);

        if (movePos.OutOfBounds()) continue;

        const Piece& targetPiece = gameBoard->GetPiece(movePos);

        // Skip friendly pieces
        if (targetPiece.type == PieceType::None || targetPiece.color == piece.color) continue;

        moveQuery.moves[idx++] = PieceMove{ Standard,movePos};
    }

    // TODO en-passent

    AddPawnPushMove(piece,piecePosition,moveQuery,gameBoard,1,idx, direction);
    if (piece.moveState == PieceMoveState::NotMoved) {
        AddPawnPushMove(piece,piecePosition,moveQuery,gameBoard,2,idx, direction);
    }

    moveQuery.moveCount = idx;

}

void MoveSearcher::GenerateSlidingMoves(const Piece &piece, PiecePosition piecePosition, PieceMoveQuery &moveQuery,const std::unique_ptr<GameBoard> &gameBoard, const int directions[][2], int directionCount) {
    int idx = 0;
    for (int d = 0; d < 8; d++) {
        int dx = directions[d][0];
        int dy = directions[d][1];

        PiecePosition currentPos = piecePosition;

        while (true) {
            currentPos.row += dx;
            currentPos.col += dy;

            if (currentPos.OutOfBounds()) break;

            const Piece& targetPiece = gameBoard->GetPiece(currentPos);

            // Same color piece
            if (targetPiece.type != PieceType::None && targetPiece.color == piece.color)
                break;

            moveQuery.moves[idx] = PieceMove{ Standard,currentPos};
            idx = idx + 1;

            // Hit enemy piece
            if (targetPiece.type != PieceType::None && targetPiece.color != piece.color)break;
        }
    }

    moveQuery.moveCount = idx;
}

void MoveSearcher::AddPawnPushMove(const Piece &piece, PiecePosition piecePosition, PieceMoveQuery &moveQuery,
    const std::unique_ptr<GameBoard> &gameBoard, int movement, int &idx, int direction) {
    PiecePosition movePosition(piecePosition.row + direction * movement, piecePosition.col);

    if (movePosition.OutOfBounds()) return;

    const Piece& targetPiece = gameBoard->GetPiece(movePosition);
    if (targetPiece.type != PieceType::None) return;


    for (int i = 1; i < movement; i++) {
        PiecePosition betweenPosition(piecePosition.row + direction * i, piecePosition.col);
        if (betweenPosition.OutOfBounds()) return;

        const Piece& betweenPiece = gameBoard->GetPiece(betweenPosition);
        std::cout<<betweenPiece.type<<std::endl;
        if (betweenPiece.type != PieceType::None) return;
    }
    std::cout << movement << std::endl;
    moveQuery.moves[idx++] = PieceMove{ Standard,movePosition};
}

