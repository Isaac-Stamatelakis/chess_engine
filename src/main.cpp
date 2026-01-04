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

    std::unique_ptr<BoardRenderer> boardRenderer = std::make_unique<BoardRenderer>(gameBoard, Black);

    while (window->isOpen())
    {
        while (const std::optional event = window->pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                window->close();
            }
            else if (const auto* mousePress = event->getIf<sf::Event::MouseButtonPressed>()) {
                boardRenderer->OnMouseDown(mousePress->button, mousePress->position);
            }  else if (const auto* mouseRelease = event->getIf<sf::Event::MouseButtonReleased>()) {
                boardRenderer->OnMouseRelease(mouseRelease->button, mouseRelease->position);
            }
            else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                // May need later
            }
        }

        window->clear();

        boardRenderer->Render(window);
        window->display();
    }
}
