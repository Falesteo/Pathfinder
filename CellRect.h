//
// Created by longo on 25/09/2022.
//

#ifndef PATHFINDER_CELLRECT_H
#define PATHFINDER_CELLRECT_H

#include "SFML/Graphics.hpp"
#include <iostream>

class CellRect {
public:
    CellRect(sf::Color color);

    void update(bool active, float side);

    void draw(sf::RenderWindow &window);

    void setColor(sf::Color color) { CellRect::color = color; }
    sf::Color getColor() { return color; }

    sf::RectangleShape rect;

private:
    sf::Vector2f pos;
    sf::Color color;
};



#endif //PATHFINDER_CELLRECT_H
