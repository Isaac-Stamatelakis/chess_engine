//
// Created by Isaac on 2026-01-03.
//

#include "../include/BoardRenderer.h"

#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

BoardRenderer::BoardRenderer(sf::RenderWindow& window) : window(window){
    LoadGrid();
    LoadTextures();
}

void BoardRenderer::Render(const GameBoard &gameboard) {
    RenderGrid();
    RenderBoard(gameboard);
}

void BoardRenderer::RenderGrid() {
    for (int y = 0; y < ROWS; ++y)
    {
        for (int x = 0; x < ROWS; ++x)
        {
            window.draw(squares[y][x]);
        }
    }
}

void BoardRenderer::RenderBoard(const GameBoard &gameboard) {
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
        LoadTexture(data.pieceType, data.color, data.spriteName);
    }
}

void BoardRenderer::LoadTexture(PieceType piece, PieceColor pieceColor, std::string spriteName) {
    sf::Texture texture;

    std::string path = std::string("../assets/") + spriteName + ".png";
    if (!texture.loadFromFile(path)) {
        throw std::runtime_error("Failed to load texture: " + path);
    }

    const PieceTextureKey textureKey{piece, pieceColor};
    pieceTextures[textureKey] = texture;
}


void BoardRenderer::LoadGrid() {
    for (int y = 0; y < ROWS; ++y)
    {
        for (int x = 0; x < ROWS; ++x)
        {
            squares[y][x].setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
            const sf::Vector2f location(x * TILE_SIZE, y * TILE_SIZE);
            squares[y][x].setPosition(location);

            // Alternate colors
            if ((x + y) % 2 == 0)
                squares[y][x].setFillColor(sf::Color::White);
            else
                squares[y][x].setFillColor(sf::Color::Black);
        }
    }
}

