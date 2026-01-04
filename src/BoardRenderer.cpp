//
// Created by Isaac on 2026-01-03.
//

#include "../include/BoardRenderer.h"

#include <iostream>

#include "../include/MoveSearcher.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

BoardRenderer::BoardRenderer(std::unique_ptr<GameBoard>& gameboard, PieceColor viewColor) : gameBoard(gameboard), viewColor(viewColor) {
    LoadGrid();
    LoadTextures();
    LoadGameBoard();
}

void BoardRenderer::LoadGameBoard() {
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            pieceSprites[col][row] = nullptr;
        }
    }

    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            PiecePosition piecePosition(row, col);
            LoadPieceSprite(piecePosition, FlipIfWhite);
        }
    }
}


void BoardRenderer::LoadPieceSprite(PiecePosition piecePosition, PieceLoadFlipMode flipMode) {
    const Piece& piece = gameBoard->GetPiece(piecePosition);

    if (flipMode == FlipIfWhite && viewColor == White) {
        piecePosition.InvertAxis(Vertical);
    }
    PieceTextureKey pieceTextureKey{piece.type,piece.color};
    if (!textures.pieceTextures.contains(pieceTextureKey)) return;;

    sf::Texture& texture = textures.pieceTextures[pieceTextureKey];
    std::unique_ptr<sf::Sprite> sprite = std::make_unique<sf::Sprite>(texture);



    sf::Vector2f position {static_cast<float>(piecePosition.col*TILE_SIZE),static_cast<float>(piecePosition.row*TILE_SIZE)};

    sprite.get()->setPosition(position);
    pieceSprites[piecePosition.col][piecePosition.row] = std::move(sprite);
}

void BoardRenderer::ClearSelectedPiece() {
    selectedPiecePosition = std::nullopt;
    selectedPieceFollowState = Inactive;
    ClearMoveSprites();
}


PieceMoveResult BoardRenderer::TryMoveToPosition(PiecePosition piecePosition) {
    for (int i = 0; i < pieceMoveQuery.moveCount; i++) {
        const PieceMove& move = pieceMoveQuery.moves[i];
        if (move.position == piecePosition) {
            MoveSelectedPiece(move);
            return MoveSuccess;
        }
    }
    return MoveFail;
}

void BoardRenderer::OnMouseDown(sf::Mouse::Button button, sf::Vector2i mousePosition) {
    short col = mousePosition.x / TILE_SIZE;
    short row = mousePosition.y / TILE_SIZE;
    PiecePosition piecePosition {row,col};
    if (piecePosition.OutOfBounds()) return;

    switch (button) {
        case sf::Mouse::Button::Left: {
            PieceMoveResult moveResult = TryMoveToPosition(piecePosition);
            if (moveResult == MoveFail) {
                SelectSquare(piecePosition);
            }
            break;
        }
        case sf::Mouse::Button::Right: {
            HighlightSquare(piecePosition);
            break;
        }

    }
}

void BoardRenderer::OnMouseRelease(sf::Mouse::Button button, sf::Vector2i mousePosition) {
    short col = mousePosition.x / TILE_SIZE;
    short row = mousePosition.y / TILE_SIZE;
    PiecePosition piecePosition {row,col};

    switch (button) {
        case sf::Mouse::Button::Left:
            PieceMoveResult moveResult = TryMoveToPosition(piecePosition);
            if (moveResult == MoveFail) {
                RestoreSelectedPiecePosition();
            }
            if (selectedPieceFollowState == DoubleClick) {
                ClearSelectedPiece();
            }

            selectedPieceFollowState = Inactive;
            break;
    }
}



void BoardRenderer::Render(const std::unique_ptr<sf::RenderWindow>& window) {
    RenderGrid(window);
    RenderMovePositions(window);
    RenderPieces(window);
}

void BoardRenderer::RenderGrid(const std::unique_ptr<sf::RenderWindow>& window) {
    sf::Color lightColor(240, 217, 181); // light beige
    sf::Color darkColor(181, 136, 99);   // dark brown

    sf::Color selectedColor(246, 246, 105);    // soft yellow (selected square)
    sf::Color highlightedColor(255, 80, 80);   // soft red (valid moves / attacks)

    for (int row = 0; row < GRID_SIZE; ++row)
    {
        for (int col = 0; col < GRID_SIZE; ++col)
        {
            PiecePosition piecePosition(row, col);

            sf::Color color;
            if ((row + col) % 2 == 0) {
                color = lightColor;
            }
            else {
                color = darkColor;
            }

            if (selectedPiecePosition.has_value() && selectedPiecePosition.value().row == piecePosition.row && selectedPiecePosition.value().col == piecePosition.col) {
                squares[col][row].setFillColor(selectedColor);
            } else if (highlightedSquares.IsHighlighted(piecePosition)) {
                squares[col][row].setFillColor(highlightedColor*color);
            } else {
                squares[col][row].setFillColor(color);
            }
            window->draw(squares[row][col]);
        }
    }
}

void BoardRenderer::RenderPieces(const std::unique_ptr<sf::RenderWindow>& window) {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            const std::unique_ptr<sf::Sprite>& sprite = pieceSprites[col][row];
            if (!sprite) continue;

            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
            if (selectedPieceFollowState != Inactive && selectedPiecePosition.has_value() && selectedPiecePosition.value().row == row && selectedPiecePosition.value().col == col) {
                sf::Vector2f vectorFloat{static_cast<float>(mousePos.x - TILE_SIZE/2),static_cast<float>(mousePos.y - TILE_SIZE/2)};
                sprite->setPosition(vectorFloat);
            }
            window->draw(*sprite);
        }
    }
}

void BoardRenderer::RenderMovePositions(const std::unique_ptr<sf::RenderWindow> &window) const {
    for (const std::unique_ptr<sf::Sprite>& sprite : movePositionSprites) {
        window->draw(*sprite);
    }
}

void BoardRenderer::LoadTextures() {
    PieceTextureLoadData textureData[] = {
        {PieceType::King, PieceColor::White, "white_king"},
        {PieceType::Queen, PieceColor::White, "white_queen"},
        {PieceType::Rook, PieceColor::White, "white_rook"},
        {PieceType::Bishop, PieceColor::White, "white_bishop"},
        {PieceType::Knight, PieceColor::White, "white_knight"},
        {PieceType::Pawn, PieceColor::White, "white_pawn"},
        {PieceType::King, PieceColor::Black, "black_king"},
        {PieceType::Queen, PieceColor::Black, "black_queen"},
        {PieceType::Rook, PieceColor::Black, "black_rook"},
        {PieceType::Bishop, PieceColor::Black, "black_bishop"},
        {PieceType::Knight, PieceColor::Black, "black_knight"},
        {PieceType::Pawn, PieceColor::Black, "black_pawn"}
    };
    for (const auto& data : textureData) {
        LoadPieceTexture(data.pieceType, data.color, data.spriteName);
    }

    textures.moveTexture = LoadTexture("move_position");
    textures.captureTexture = LoadTexture("capture_position");
}

void BoardRenderer::LoadPieceTexture(PieceType piece, PieceColor pieceColor, const std::string &spriteName) {
    sf::Texture texture = LoadTexture(spriteName);
    const PieceTextureKey textureKey{piece, pieceColor};
    textures.pieceTextures[textureKey] = texture;
}

void BoardRenderer::SelectSquare(PiecePosition piecePosition) {
    highlightedSquares.Clear();
    if (piecePosition.OutOfBounds()) return;

    if (selectedPiecePosition == piecePosition) {
        selectedPieceFollowState = DoubleClick;
        return;
    }

    const Piece& piece = gameBoard->GetPiece(piecePosition);
    if (piece.type == None) {
        ClearSelectedPiece();
        return;
    }

    selectedPieceFollowState = Active;
    selectedPiecePosition = piecePosition;
    LoadMoveSprites(piecePosition);
}

void BoardRenderer::HighlightSquare(PiecePosition piecePosition) {
    selectedPiecePosition = std::nullopt;

    if (highlightedSquares.IsHighlighted(piecePosition)) {
        highlightedSquares.UnHighlight(piecePosition);
    } else {
        highlightedSquares.Highlight(piecePosition);
    }
    ClearMoveSprites();
}

sf::Texture BoardRenderer::LoadTexture(const std::string &spriteName) {
    std::string path = std::string("../assets/") + spriteName + ".png";
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        throw std::runtime_error("Failed to load texture: " + path);
    }
    return texture;
}

void BoardRenderer::LoadMoveSprites(PiecePosition position) {
    MoveSearcher::GetValidMoves(position, pieceMoveQuery, gameBoard);

    ClearMoveSprites();
    movePositionSprites.reserve(pieceMoveQuery.moveCount);

    for (int i = 0; i < pieceMoveQuery.moveCount; i++) {
        PieceMove pieceMove = pieceMoveQuery.moves[i];

        const Piece& piece = gameBoard->GetPiece(pieceMove.position);
        const sf::Texture& texture = piece.type == None ? textures.moveTexture : textures.captureTexture;

        std::unique_ptr<sf::Sprite> sprite = std::make_unique<sf::Sprite>(texture);
        float rowPosition = TILE_SIZE * pieceMove.position.row;
        float columnPosition = TILE_SIZE * pieceMove.position.col;

        sf::Vector2f position{columnPosition, rowPosition};
        sprite->setPosition(position);
        movePositionSprites.push_back(std::move(sprite));
    }
}




void BoardRenderer::RestoreSelectedPiecePosition() const {
    if (!selectedPiecePosition.has_value()) return;

    PiecePosition piecePosition = selectedPiecePosition.value();
    if (piecePosition.OutOfBounds()) return;

    const std::unique_ptr<sf::Sprite>& sprite = pieceSprites[piecePosition.col][piecePosition.row];
    if (!sprite) return;

    sf::Vector2f position {static_cast<float>(piecePosition.col*TILE_SIZE),static_cast<float>(piecePosition.row*TILE_SIZE)};
    sprite.get()->setPosition(position);
}

void BoardRenderer::ClearMoveSprites() {
    movePositionSprites.clear();
}

void BoardRenderer::MoveSelectedPiece(const PieceMove &move) {
    if (!selectedPiecePosition.has_value()) return;

    gameBoard->ExecuteMove(move, selectedPiecePosition.value());
    //RenderMove(move);
    LoadGameBoard();
    ClearSelectedPiece();
    ClearMoveSprites();
    pieceMoveQuery.moveCount = 0;
}


void BoardRenderer::RenderMove(const PieceMove &move) {
    switch (move.type) {
        case Standard:
            pieceSprites[selectedPiecePosition->col][selectedPiecePosition->row] = nullptr;
            pieceSprites[move.position.col][move.position.row] = nullptr;
            LoadPieceSprite(move.position, NoFlip);
            break;
        case EnPassant:
            break;

    }
}


void BoardRenderer::LoadGrid() {
    for (int col = 0; col < GRID_SIZE; ++col)
    {
        for (int row = 0; row < GRID_SIZE; ++row)
        {
            squares[col][row].setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            const sf::Vector2f location(col * TILE_SIZE, row * TILE_SIZE);
            squares[col][row].setPosition(location);
        }
    }
}

