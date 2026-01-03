#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>

#include "../include/BoardRenderer.h"

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({640, 640}), "Chess");
    window.setFramerateLimit(144);

    BoardRenderer boardRenderer(window);
    GameBoard gameBoard;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }
        window.clear();

        // Draw all squares
        boardRenderer.Render(gameBoard);
        window.display();
    }
}
