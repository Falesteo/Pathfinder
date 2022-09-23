//
// Created by longo on 23/09/2022.
//

#ifndef PATHFINDER_CELL_H
#define PATHFINDER_CELL_H

#include "SFML/Graphics.hpp"

class Cell {
public:
    Cell(sf::Vector2i indexes, float side);

    void update(sf::RenderWindow &window);

    void draw(sf::RenderWindow &window);

    sf::Vector2f getPosition() const { return pos; }
    void setPosition(sf::Vector2f pos) { Cell::pos = pos; }

    std::string state;

private:
    sf::RectangleShape cellBackgroundRect;
    sf::RectangleShape cellStateRect;

    // cell coordinates in the grid
    sf::Vector2i indexes;
    sf::Vector2f pos;
    float side;
};


#endif //PATHFINDER_CELL_H
