//
// Created by Isaac on 2026-01-03.
//

#include "../include/MoveSearcher.h"

void MoveSearcher::GetValidMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const GameBoard &gameBoard) const {
    const int boardPosition = piecePosition.ToIndex();
    const Piece& piece = gameBoard.Pieces[boardPosition];

    switch (piece.type) {
        case None:
            moveQuery.moveCount = 0;
            break;
        case King:
            GetKingMoves(piecePosition, moveQuery, gameBoard);
            break;
        case Queen:
            GetQueenMoves(piecePosition, moveQuery, gameBoard);
            break;
        case Rook:
            GetRookMoves(piecePosition, moveQuery, gameBoard);
            break;
        case Knight:
            GetKnightMoves(piecePosition, moveQuery, gameBoard);
            break;
        case Bishop:
            GetBishopMoves(piecePosition, moveQuery, gameBoard);
            break;
        case Pawn:
            GetPawnMoves(piecePosition, moveQuery, gameBoard);
            break;
    }
}

void MoveSearcher::GetKingMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery,
    const GameBoard &gameBoard) const {
    const Piece& piece = gameBoard.Pieces[piecePosition.ToIndex()];

    int idx = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            PiecePosition movePosition(piecePosition.row + dx, piecePosition.col + dy);
            if (movePosition.OutOfBounds()) continue;
            const Piece& otherPiece = gameBoard.Pieces[movePosition.ToIndex()];
            if (otherPiece.type != PieceType::None && otherPiece.color == piece.color) continue;
            if (otherPiece.protectionState == PieceProtectionState::Protected) continue;
            moveQuery.moves[idx] = movePosition;
            idx++;
        }
    }
    moveQuery.moveCount = idx;

    // TODO Castling
}

void MoveSearcher::GetQueenMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery,
    const GameBoard &gameBoard) const {
    const int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},  // straight lines
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // diagonals
    };
    const Piece& piece = gameBoard.Pieces[piecePosition.ToIndex()];
    GenerateSlidingMoves(piece, piecePosition, moveQuery, gameBoard, directions, 8);
}

void MoveSearcher::GetRookMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery,
    const GameBoard &gameBoard) const {
    const int directions[4][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1}
    };
    const Piece& piece = gameBoard.Pieces[piecePosition.ToIndex()];
    GenerateSlidingMoves(piece, piecePosition, moveQuery, gameBoard, directions, 4);
}

void MoveSearcher::GetKnightMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery,
    const GameBoard &gameBoard) const {
    const int knightOffsets[8][2] = {
        {2, 1}, {1, 2}, {-1, 2}, {-2, 1},
        {-2,-1}, {-1,-2}, {1,-2}, {2,-1}
    };

    const Piece& piece = gameBoard.Pieces[piecePosition.ToIndex()];
    int idx = 0;
    for (int i = 0; i < 8; i++) {
        PiecePosition movePos(piecePosition.row + knightOffsets[i][0],piecePosition.col + knightOffsets[i][1]);

        if (movePos.OutOfBounds()) continue;

        int targetIdx = movePos.ToIndex();
        const Piece& targetPiece = gameBoard.Pieces[targetIdx];

        // Skip friendly pieces
        if (targetPiece.type != PieceType::None && targetPiece.color == piece.color) continue;

        moveQuery.moves[idx++] = movePos;
    }

    moveQuery.moveCount = idx;

}

void MoveSearcher::GetBishopMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery,
    const GameBoard &gameBoard) const {
        const int directions[4][2] = {
            {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // diagonals
        };
        const Piece& piece = gameBoard.Pieces[piecePosition.ToIndex()];
        GenerateSlidingMoves(piece, piecePosition, moveQuery, gameBoard, directions, 4);
}

void MoveSearcher::GetPawnMoves(PiecePosition piecePosition, PieceMoveQuery &moveQuery, const GameBoard &gameBoard) const {
    const int captureSquares[2][2] = {
        {1, 1}, {-1, 1}
    };
    const Piece& piece = gameBoard.Pieces[piecePosition.ToIndex()];
    int idx = 0;

    int direction = piece.color == PieceColor::White ? 1 : -1;

    for (int i = 0; i < 2; i++) {
        PiecePosition movePos(piecePosition.row + captureSquares[i][0]*direction,piecePosition.col + captureSquares[i][1]*direction);

        if (movePos.OutOfBounds()) continue;

        int targetIdx = movePos.ToIndex();
        const Piece& targetPiece = gameBoard.Pieces[targetIdx];

        // Skip friendly pieces
        if (targetPiece.type != PieceType::None && targetPiece.color == piece.color) continue;

        moveQuery.moves[idx++] = movePos;
    }

    // TODO en-passent

    AddPawnPushMove(piece,piecePosition,moveQuery,gameBoard,1,idx, direction);
    if (piece.moveState == PieceMoveState::NotMoved) {
        AddPawnPushMove(piece,piecePosition,moveQuery,gameBoard,2,idx, direction);
    }


    moveQuery.moveCount = idx;

}

void MoveSearcher::GenerateSlidingMoves(const Piece &piece, PiecePosition piecePosition, PieceMoveQuery &moveQuery,const GameBoard &gameBoard, const int directions[][2], int directionCount) const {
    int idx = 0;
    for (int d = 0; d < 8; d++) {
        int dx = directions[d][0];
        int dy = directions[d][1];

        PiecePosition currentPos = piecePosition;

        while (true) {
            currentPos.row += dx;
            currentPos.col += dy;

            if (currentPos.OutOfBounds()) break;

            int targetIdx = currentPos.ToIndex();
            const Piece& targetPiece = gameBoard.Pieces[targetIdx];

            // Same color piece
            if (targetPiece.type != PieceType::None && targetPiece.color == piece.color)
                break;

            moveQuery.moves[idx] = currentPos;
            idx = idx + 1;

            // Hit enemy piece
            if (targetPiece.type != PieceType::None && targetPiece.color != piece.color)break;
        }
    }

    moveQuery.moveCount = idx;
}

void MoveSearcher::AddPawnPushMove(const Piece &piece, PiecePosition piecePosition, PieceMoveQuery &moveQuery, const GameBoard &gameBoard, int movement, int& idx, int direction) const {
    PiecePosition movePosition(piecePosition.row + direction * movement, piecePosition.col);
    if (movePosition.OutOfBounds()) return;

    int targetIdx = movePosition.ToIndex();
    const Piece& targetPiece = gameBoard.Pieces[targetIdx];
    if (targetPiece.type != PieceType::None) return;

    for (int i = 0; i < movement-1; i++) {
        PiecePosition betweenPosition(piecePosition.row + direction * i, piecePosition.col);
        if (betweenPosition.OutOfBounds()) return;
        int targetIdx = betweenPosition.ToIndex();
        const Piece& betweenPiece = gameBoard.Pieces[targetIdx];
        if (betweenPiece.type != PieceType::None) return;
    }
    moveQuery.moves[idx++] = movePosition;
}

