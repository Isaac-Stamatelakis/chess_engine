#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>

int main() {
    auto window = sf::RenderWindow(sf::VideoMode({640, 640}), "Chess Engine");
    window.setFramerateLimit(144);

    sf::Texture texture;
    if (!texture.loadFromFile("../assets/king_black.png")) {
        return -1;
    }


    const int TILE_SIZE = 64;
    const int BOARD_SIZE = 8;
    sf::Sprite sprite(texture);

    sf::RectangleShape squares[BOARD_SIZE][BOARD_SIZE];

    for (int y = 0; y < BOARD_SIZE; ++y)
    {
        for (int x = 0; x < BOARD_SIZE; ++x)
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
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            for (int x = 0; x < BOARD_SIZE; ++x)
            {
                window.draw(squares[y][x]);
            }
        }

        window.draw(sprite);
        window.display();
    }
}
