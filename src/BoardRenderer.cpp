//
// Created by Isaac on 2026-01-03.
//

#include "../include/BoardRenderer.h"

#include <iostream>

#include "../include/MoveSearcher.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

BoardRenderer::BoardRenderer() {
    LoadGrid();
    LoadTextures();
}

void BoardRenderer::LoadGameBoard(const std::unique_ptr<GameBoard>& gameboard, PieceColor viewColor) {
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            pieceSprites[row][col] = nullptr;

            const Piece& piece = gameboard->Pieces[row + col*GRID_SIZE];
            PieceTextureKey pieceTextureKey{piece.type,piece.color};
            if (!textures.pieceTextures.contains(pieceTextureKey)) continue;

            sf::Texture& texture = textures.pieceTextures[pieceTextureKey];
            std::unique_ptr<sf::Sprite> sprite = std::make_unique<sf::Sprite>(texture);

            int viewRow = row;
            int viewCol = col;
            if (viewColor == White) {
                viewCol = GRID_SIZE - viewCol - 1;
            }

            sf::Vector2f position {static_cast<float>(viewRow*TILE_SIZE),static_cast<float>(viewCol*TILE_SIZE)};

            sprite.get()->setPosition(position);
            pieceSprites[row][col] = std::move(sprite);
        }
    }
}

void BoardRenderer::OnMouseDown(sf::Mouse::Button button, sf::Vector2i mousePosition) {
    short px = mousePosition.x / TILE_SIZE;
    short py = mousePosition.y / TILE_SIZE;
    PiecePosition piecePosition {px,py};
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


    int selectedPositionIndex = GetSelectedPiecePositionIndex();

    for (int x = 0; x < GRID_SIZE; ++x)
    {
        for (int y = 0; y < GRID_SIZE; ++y)
        {
            sf::Color color;
            if ((x + y) % 2 == 0) {
                color = lightColor;
            }
            else {
                color = darkColor;
            }

            int idx = x *GRID_SIZE + y;
            if (idx == selectedPositionIndex) {
                squares[y][x].setFillColor(selectedColor);
            } else if (highlightedSquares.test(idx)) {
                squares[y][x].setFillColor(highlightedColor*color);
            } else {
                squares[y][x].setFillColor(color);
            }
            window->draw(squares[x][y]);
        }
    }
}

void BoardRenderer::RenderPieces(const std::unique_ptr<sf::RenderWindow>& window) {
    int selectedPositionIndex = GetSelectedPiecePositionIndex();
    for (int row = 0; row < GRID_SIZE; ++row) {
        for (int col = 0; col < GRID_SIZE; ++col) {
            if (!pieceSprites[row][col]) continue;

            int idx = row * GRID_SIZE + col;
            const std::unique_ptr<sf::Sprite>& sprite = pieceSprites[row][col];

            sf::Vector2i mousePos = sf::Mouse::getPosition(*window);
            if (idx == selectedPositionIndex && selectedPieceFollowState == Active) {
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
    highlightedSquares.reset();
    int idx = piecePosition.ToIndex();
    selectedPieceFollowState = Active;
    int selectedPositionIndex = GetSelectedPiecePositionIndex();
    if (idx == selectedPositionIndex) return;
    selectedPiecePosition = piecePosition;
}

void BoardRenderer::HighlightSquare(PiecePosition piecePosition) {
    selectedPiecePosition = std::nullopt;
    int idx = piecePosition.ToIndex();
    if (highlightedSquares.test(idx)) {
        highlightedSquares.reset(idx);
    } else {
        highlightedSquares.set(idx);
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

void BoardRenderer::RestoreSelectedPiecePosition() {
    if (!selectedPiecePosition.has_value()) return;

    PiecePosition piecePosition = selectedPiecePosition.value();
    if (piecePosition.OutOfBounds()) return;

    const std::unique_ptr<sf::Sprite>& sprite = pieceSprites[piecePosition.row][piecePosition.col];
    if (!sprite) return;

    sf::Vector2f position {static_cast<float>(piecePosition.row*TILE_SIZE),static_cast<float>(piecePosition.col*TILE_SIZE)};
    sprite.get()->setPosition(position);
}

int BoardRenderer::GetSelectedPiecePositionIndex() {
    if (selectedPiecePosition.has_value()) {
        return selectedPiecePosition.value().ToIndex();
    }
    return  -1;
}


void BoardRenderer::LoadGrid() {
    for (int x = 0; x < GRID_SIZE; ++x)
    {
        for (int y = 0; y < GRID_SIZE; ++y)
        {
            squares[y][x].setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            const sf::Vector2f location(x * TILE_SIZE, y * TILE_SIZE);
            squares[y][x].setPosition(location);
        }
    }
}

