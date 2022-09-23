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

    bool getObstacle() const { return obstacle; }
    void setObstacle(bool obstacle) { Cell::obstacle = obstacle; }

    bool getStart() const { return start; }
    void setStart(bool start) { Cell::start = start; }

    bool getEnd() const { return end; }
    void setEnd(bool end) { Cell::end = end; }

private:
    // cell coordinates in the grid
    sf::Vector2i indexes;
    sf::Vector2f pos;
    float side;

    // cell states
    bool obstacle = false;
    bool start = false;
    bool end = false;

    sf::RectangleShape cellRect, obstacleRect;
};


#endif //PATHFINDER_CELL_H
