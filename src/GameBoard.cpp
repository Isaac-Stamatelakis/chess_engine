//
// Created by Isaac on 2026-01-03.
//

#include "../include/GameBoard.h"

#include <stdexcept>


void GameBoard::LoadDefaultBoard() {
    ClearBoard();

    std::vector<PieceDeclaration> pieceDeclarations = {
        {Rook},
        {Knight},
        {Bishop},
        {King},
        {Queen},
        {Bishop},
        {Knight},
        {Rook},
    };

    std::vector<PieceDeclaration> pawns;
    pawns.reserve(GRID_SIZE);
    for (int i = 0; i < GRID_SIZE; i++) {
        pawns.push_back(PieceDeclaration{Pawn});
    }

    LoadPieceDeclarations(pieceDeclarations,White, 0);
    LoadPieceDeclarations(pawns,White, 1);

    LoadPieceDeclarations(pieceDeclarations,Black, 0);
    LoadPieceDeclarations(pawns,Black, 1);
}

void GameBoard::ClearBoard() {
    for (auto& row : pieces)
    {
        for (Piece& piece : row)
        {
            piece.type = None;
            piece.moveState = NotMoved;
        }
    }
    for (int i = 0; i < BOARD_SIZE; i++) {

    }
}

Piece GameBoard::GetPiece(PiecePosition position) {
    return pieces[position.col][position.row];
}

void GameBoard::LoadPieceDeclarations(const std::vector<PieceDeclaration> &pieceDeclarations, PieceColor pieceColor, short row) {
    short col = 0;
    short placementRow = pieceColor == White ? row : GRID_SIZE-row-1;
    for (const auto& declaration : pieceDeclarations) {
        PiecePosition piecePosition{placementRow,col};
        if (piecePosition.OutOfBounds()) {
            throw std::runtime_error("Piece declaration out of bounds");
        }

        Piece piece{declaration.type,pieceColor,NotMoved};
        pieces[piecePosition.col][piecePosition.row] = piece;
        col++;
    }
}
