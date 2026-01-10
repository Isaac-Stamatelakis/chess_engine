//
// Created by Isaac on 2026-01-03.
//

#include "../include/GameBoard.h"

#include <iostream>
#include <ostream>
#include <stdexcept>


void GameBoard::LoadDefaultBoard() {
    ClearBoard();

    std::vector<PieceDeclaration> pieceDeclarations = {
        {PieceType::Rook},
        {PieceType::Knight},
        {PieceType::Bishop},
        {PieceType::King},
        {PieceType::Queen},
        {PieceType::Bishop},
        {PieceType::Knight},
        {PieceType::Rook},
    };

    std::vector<PieceDeclaration> pawns;
    pawns.reserve(GRID_SIZE);
    for (int i = 0; i < GRID_SIZE; i++) {
        pawns.push_back(PieceDeclaration{PieceType::Pawn});
    }

    LoadPieceDeclarations(pieceDeclarations,PieceColor::White, 0);
    LoadPieceDeclarations(pawns,PieceColor::White, 1);

    LoadPieceDeclarations(pieceDeclarations,PieceColor::Black, 0);
    LoadPieceDeclarations(pawns,PieceColor::Black, 1);

    Piece emptyPiece = {PieceType::None,PieceColor::Black}; // Lets white go first
    PieceMove emptyMove = {};
    pieceMoveHistory = {emptyMove,emptyPiece};

    whiteBitBoard = CalculateBitBoards(PieceColor::White);
    blackBitBoard = CalculateBitBoards(PieceColor::Black);
}

void GameBoard::ClearBoard() {
    for (auto& row : pieces)
    {
        for (Piece& piece : row)
        {
            piece.type = PieceType::None;
            piece.moveState = PieceMoveState::NotMoved;
        }
    }
    for (int i = 0; i < BOARD_SIZE; i++) {

    }
}

Piece &GameBoard::GetPiece(PiecePosition position) {
    return pieces[position.col][position.row];
}

void GameBoard::MovePiece(PiecePosition from, PiecePosition to) {
    Piece currentPiece = GetPiece(from);
    currentPiece.moveState = PieceMoveState::Moved;
    pieces[to.col][to.row] = currentPiece;
    pieces[from.col][from.row] = Piece{PieceType::None};
}

void GameBoard::ExecuteMove(PieceMove move, PiecePosition piecePosition) {
    Piece movePiece = GetPiece(piecePosition);
    switch (move.type) {
        case MoveType::Standard:
        case MoveType::DoublePawnPush:
            MovePiece(piecePosition, move.position);
            break;
        case MoveType::EnPassant: {
            MovePiece(piecePosition, move.position);
            PiecePosition adjacentPawnPosition = move.position;
            if (movePiece.color == PieceColor::Black) {
                adjacentPawnPosition.row++;
            } else {
                adjacentPawnPosition.row--;
            }
            pieces[adjacentPawnPosition.col][adjacentPawnPosition.row] = {PieceType::None};
            break;
        }
        case MoveType::Promotion: {
            MovePiece(piecePosition, move.position);
            Piece& piece = pieces[move.position.col][move.position.row];
            if (move.promotion == PieceType::None) {
                move.promotion = PieceType::Queen;
            }
            piece.type = move.promotion;
            break;
        }
        case MoveType::ShortCastle: {
            MovePiece(piecePosition, move.position);
            auto rookInitialPosition = PiecePosition(move.position.row, move.position.col-1);
            pieces[rookInitialPosition.col][rookInitialPosition.row] = {PieceType::None};
            auto rookEndPosition = PiecePosition(move.position.row, move.position.col+1);
            pieces[rookEndPosition.col][rookEndPosition.row] = {PieceType::Rook,movePiece.color, PieceMoveState::Moved};
        }
            break;
        case MoveType::LongCastle: {
            MovePiece(piecePosition, move.position);
            auto rookInitialPosition = PiecePosition(move.position.row, move.position.col+2);
            pieces[rookInitialPosition.col][rookInitialPosition.row] = {PieceType::None};
            auto rookEndPosition = PiecePosition(move.position.row, move.position.col-1);
            pieces[rookEndPosition.col][rookEndPosition.row] = {PieceType::Rook,movePiece.color, PieceMoveState::Moved};
            break;
        }

    }
    SetLastMove(move, movePiece);

    // Naive
    whiteBitBoard = CalculateBitBoards(PieceColor::White);
    blackBitBoard = CalculateBitBoards(PieceColor::Black);
}

void GameBoard::SetLastMove(PieceMove move, Piece piece) {
    pieceMoveHistory = {move,piece};
}

PieceMoveHistory & GameBoard::GetLastMove() {
    return pieceMoveHistory;
}

void GameBoard::LoadPieceDeclarations(const std::vector<PieceDeclaration> &pieceDeclarations, PieceColor pieceColor, short row) {
    short col = 0;
    short placementRow = pieceColor == PieceColor::White ? row : GRID_SIZE-row-1;
    for (const auto& declaration : pieceDeclarations) {
        PiecePosition piecePosition{placementRow,col};
        if (piecePosition.OutOfBounds()) {
            throw std::runtime_error("Piece declaration out of bounds");
        }

        Piece piece{declaration.type,pieceColor, PieceMoveState::NotMoved};
        pieces[piecePosition.col][piecePosition.row] = piece;
        col++;
    }
}

ColorBitBoards & GameBoard::GetColorBitBoards(PieceColor pieceColor) {
    switch (pieceColor) {
        case PieceColor::Black:
            return blackBitBoard;
        case PieceColor::White:
            return whiteBitBoard;
        default:
            throw std::runtime_error("Invalid PieceColor in GetColorBitBoards");
    }
}

ColorBitBoards GameBoard::CalculateBitBoards(PieceColor pieceColor) {
    uint64_t occupied = 0;

    for (int col = 0; col < GRID_SIZE; ++col) {
        for (int row = 0; row < GRID_SIZE; ++row) {
            PiecePosition piecePosition(row,col);
            Piece& piece = GetPiece(piecePosition);

            if (pieceColor == piece.color && piece.type != PieceType::None) {
                uint64_t mask = piecePosition.GetBitMapMask();
                occupied |= mask;
            }
        }
    }
    return ColorBitBoards{occupied,0,0};
}

bool GameBoard::RowOccupied(PiecePosition initialPosition, int direction, int checkCount) {
    for (int i = 1; i <= checkCount; i++) {
        PiecePosition position(initialPosition.row,initialPosition.col+direction*i);
        if (position.OutOfBounds()) return true;

        Piece piece = GetPiece(position);
        if (piece.type != PieceType::None) return true;
    }
    return false;
}
