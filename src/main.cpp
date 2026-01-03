#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>

#include "../include/BoardRenderer.h"

int main() {
    std::unique_ptr<GameBoard> gameBoard = std::make_unique<GameBoard>();
    gameBoard->LoadDefaultBoard();

    // May want to increase window size later when ui is needed
    std::unique_ptr<sf::RenderWindow> window = std::make_unique<sf::RenderWindow>(sf::VideoMode({GRID_SIZE*TILE_SIZE, GRID_SIZE*TILE_SIZE}), "Chess");
    window->setFramerateLimit(144);

    std::unique_ptr<BoardRenderer> boardRenderer = std::make_unique<BoardRenderer>();
    boardRenderer->LoadGameBoard(gameBoard);

    while (window->isOpen())
    {
        while (const std::optional event = window->pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window->close();
            }
        }
        window->clear();

        boardRenderer->Render(window);
        window->display();
    }
}
