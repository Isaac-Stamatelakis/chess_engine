//
// Created by Isaac on 2026-01-03.
//

#include "../include/BoardRenderer.h"

#include <iostream>

#include "../include/MoveSearcher.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

BoardRenderer::BoardRenderer(std::unique_ptr<GameBoard>& gameboard, PieceColor viewColor) : gameBoard(gameboard) {
    LoadGrid();
    LoadTextures();
    LoadGameBoard(viewColor);
}

void BoardRenderer::LoadGameBoard(PieceColor viewColor) {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            PiecePosition piecePosition(row, col);
            pieceSprites[col][row] = nullptr;

            const Piece& piece = gameBoard->GetPiece(piecePosition);
            PieceTextureKey pieceTextureKey{piece.type,piece.color};
            if (!textures.pieceTextures.contains(pieceTextureKey)) continue;

            sf::Texture& texture = textures.pieceTextures[pieceTextureKey];
            std::unique_ptr<sf::Sprite> sprite = std::make_unique<sf::Sprite>(texture);

            int viewRow = row;
            int viewCol = col;
            if (viewColor == White) {
                viewRow = GRID_SIZE - row - 1;
            }

            sf::Vector2f position {static_cast<float>(viewCol*TILE_SIZE),static_cast<float>(viewRow*TILE_SIZE)};

            sprite.get()->setPosition(position);
            pieceSprites[col][row] = std::move(sprite);
        }
    }
}

void BoardRenderer::OnMouseDown(sf::Mouse::Button button, sf::Vector2i mousePosition) {
    short col = mousePosition.x / TILE_SIZE;
    short row = mousePosition.y / TILE_SIZE;
    PiecePosition piecePosition {row,col};
    if (piecePosition.OutOfBounds()) return;

    switch (button) {
        case sf::Mouse::Button::Left:
            SelectSquare(piecePosition);
            break;
        case sf::Mouse::Button::Right:
            HighlightSquare(piecePosition);
            break;
    }
}

void BoardRenderer::OnMouseRelease(sf::Mouse::Button button, sf::Vector2i mousePosition) {
    switch (button) {
        case sf::Mouse::Button::Left:
            selectedPieceFollowState = Inactive;
            RestoreSelectedPiecePosition();
            break;
    }
}


void BoardRenderer::Render(const std::unique_ptr<sf::RenderWindow>& window) {
    RenderGrid(window);
    RenderPieces(window);
    RenderMovePositions(window);
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
            if (selectedPieceFollowState == Active && selectedPiecePosition.has_value() && selectedPiecePosition.value().row == row && selectedPiecePosition.value().col == col) {
                sf::Vector2f vectorFloat{static_cast<float>(mousePos.x - TILE_SIZE/2),static_cast<float>(mousePos.y - TILE_SIZE/2)};
                sprite->setPosition(vectorFloat);
            }
            window->draw(*sprite);
        }
    }
}

void BoardRenderer::RenderMovePositions(const std::unique_ptr<sf::RenderWindow> &window) {
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

    const Piece& piece = gameBoard->GetPiece(piecePosition);
    if (piece.type == None) {
        selectedPiecePosition = std::nullopt;
        selectedPieceFollowState = Inactive;
        return;
    }


    selectedPieceFollowState = Active;
    selectedPiecePosition = piecePosition;
}

void BoardRenderer::HighlightSquare(PiecePosition piecePosition) {
    selectedPiecePosition = std::nullopt;

    if (highlightedSquares.IsHighlighted(piecePosition)) {
        highlightedSquares.UnHighlight(piecePosition);
    } else {
        highlightedSquares.Highlight(piecePosition);
    }

}

sf::Texture BoardRenderer::LoadTexture(const std::string &spriteName) {
    std::string path = std::string("../assets/") + spriteName + ".png";
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        throw std::runtime_error("Failed to load texture: " + path);
    }
    return texture;
}

void BoardRenderer::SetMoveSprites(PiecePosition position) {

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

