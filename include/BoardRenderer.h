//
// Created by Isaac on 2026-01-03.
//

#ifndef CHESSENGINE_BOARDRENDERER_H
#define CHESSENGINE_BOARDRENDERER_H
#include <bitset>

#include "GameBoard.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include <unordered_map>

#include "SFML/Graphics/Sprite.hpp"

struct PieceTextureKey {
    PieceType pieceType;
    PieceColor color;
    bool operator==(const PieceTextureKey& other) const {
        return pieceType == other.pieceType && color == other.color;
    }
};
struct PieceKeyHash {
    std::size_t operator()(const PieceTextureKey& pieceTextureKey) const {
        return static_cast<std::size_t>(pieceTextureKey.color) * 128 + static_cast<std::size_t>(pieceTextureKey.pieceType);
    }
};

struct PieceTextureLoadData {
    PieceType pieceType;
    PieceColor color;
    std::string spriteName;
};

static constexpr int TILE_SIZE = 60;
static constexpr int NO_POSITION_SELECTED = -1;

class BoardRenderer {
public:
    BoardRenderer();
    void Render(const std::unique_ptr<sf::RenderWindow>& window);
    void LoadGameBoard(const std::unique_ptr<GameBoard>& gameboard, PieceColor viewColor);
    void OnMouseEvent(sf::Mouse::Button button, sf::Vector2i mousePosition);
private:
    sf::RectangleShape squares[GRID_SIZE][GRID_SIZE];
    std::unique_ptr<sf::Sprite> pieceSprites[GRID_SIZE][GRID_SIZE];
    std::unordered_map<PieceTextureKey, sf::Texture, PieceKeyHash> pieceTextures;
    std::bitset<GRID_SIZE*GRID_SIZE> highlightedSquares;
    int selectedPositionIndex = NO_POSITION_SELECTED;
    void LoadGrid();
    void RenderGrid(const std::unique_ptr<sf::RenderWindow>& window);
    void RenderPieces(const std::unique_ptr<sf::RenderWindow>& window);
    void LoadTextures();
    void LoadTexture(PieceType piece, PieceColor pieceColor, const std::string &spriteName);
    void SelectSquare(PiecePosition piecePosition);
    void HighlightSquare(PiecePosition piecePosition);
};



#endif //CHESSENGINE_BOARDRENDERER_H