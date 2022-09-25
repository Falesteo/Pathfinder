#include <SFML/Graphics.hpp>

#include <ctime>
#include <iostream>

#include "Grid.h"

int main() {
    // initialize the randomic number generator
    srand(time(nullptr));

    // load window in fullscreen mode
    sf::RenderWindow window(sf::VideoMode(), "Pathfinder", sf::Style::Fullscreen);

    // creation of the event
    sf::Event event;

    // create the grid
    Grid grid({window.getSize().x, window.getSize().y}, 3);

    // Game loop
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            // closing the window and ending the game
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    if (event.key.code == sf::Keyboard::Escape)
                        window.close();
                    break;
            }
        }

        // Clearing the old frame and preparing for drawing the new one
        window.clear(sf::Color::White);

        // project update and draw
        grid.update(window);
        grid.draw(window);

        // Bring to screen and display the new frame just drawn
        window.display();
    }

    // End of the Game
    return 0;
}