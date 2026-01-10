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

#include "Debug.h"
#include "MoveSearcher.h"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Texture.hpp"

class MoveSearcher;

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

struct RenderTextures {
    std::unordered_map<PieceTextureKey, sf::Texture, PieceKeyHash> pieceTextures;
    sf::Texture moveTexture;
    sf::Texture captureTexture;
};

struct HighlightedSquares {
    std::array<std::bitset<GRID_SIZE>, GRID_SIZE> bitSetList; // all bits initialized to 0

    bool IsHighlighted(PiecePosition piecePosition) {
        if (piecePosition.OutOfBounds()) return false;
        return bitSetList[piecePosition.row].test(piecePosition.col);
    }

    void Highlight(PiecePosition piecePosition) {
         bitSetList[piecePosition.row].set(piecePosition.col);
    }

    void UnHighlight(PiecePosition piecePosition) {
        bitSetList[piecePosition.row].reset(piecePosition.col);
    }

    void Clear() {
        for (std::bitset<GRID_SIZE>& bitSet : bitSetList) {
            bitSet.reset();
        }
    }
};

enum SelectedPieceFollowState {
    Active,
    Inactive,
    DoubleClick
};

enum PieceMoveResult {
    MoveSuccess,
    MoveFail,
};

enum PieceLoadFlipMode {
    NoFlip,
    FlipIfWhite
};

static constexpr int TILE_SIZE = 60;

class BoardRenderer {
public:
    BoardRenderer(std::unique_ptr<GameBoard> &gameboard, PieceColor viewColor,DebugOptions debugOptions);

    void Render(const std::unique_ptr<sf::RenderWindow>& window);
    void OnMouseDown(sf::Mouse::Button button, sf::Vector2i mousePosition);
    void OnMouseRelease(sf::Mouse::Button button, sf::Vector2i mousePosition);
    void LoadChessIcon(const std::unique_ptr<sf::RenderWindow> &window);
private:
    sf::RectangleShape squares[GRID_SIZE][GRID_SIZE];
    std::unique_ptr<sf::Sprite> pieceSprites[GRID_SIZE][GRID_SIZE];
    std::vector<std::unique_ptr<sf::Sprite>> movePositionSprites;
    HighlightedSquares highlightedSquares;
    RenderTextures textures;
    std::optional<PiecePosition> selectedPiecePosition;
    SelectedPieceFollowState selectedPieceFollowState = Inactive;
    std::unique_ptr<GameBoard>& gameBoard;
    PieceMoveQuery pieceMoveQuery;
    PieceColor viewColor;
    DebugOptions debugOptions;

    void LoadGrid();
    void RenderGrid(const std::unique_ptr<sf::RenderWindow>& window);

    void RenderDebug(ColorBitBoardType bitBoardType, sf::Color &debugWhiteColor, sf::Color &debugBlackColor, PiecePosition &piecePosition, int
                     col, int row);

    void RenderPieces(const std::unique_ptr<sf::RenderWindow>& window);
    void RenderMovePositions(const std::unique_ptr<sf::RenderWindow>& window) const;
    void LoadTextures();
    void LoadPieceTexture(PieceType piece, PieceColor pieceColor, const std::string &spriteName);
    void SelectSquare(PiecePosition piecePosition);
    void HighlightSquare(PiecePosition piecePosition);

    void LoadGameBoard();
    sf::Texture LoadTexture(const std::string &path);
    void LoadMoveSprites(PiecePosition position);
    void RestoreSelectedPiecePosition() const;
    void ClearMoveSprites();
    void MoveSelectedPiece(const PieceMove& move);
    void LoadPieceSprite(PiecePosition piecePosition, PieceLoadFlipMode flipMode);
    void ClearSelectedPiece();
    PieceMoveResult TryMoveToPosition(PiecePosition piecePosition);
};



#endif //CHESSENGINE_BOARDRENDERER_H