//
// Created by longo on 23/09/2022.
//

#ifndef PATHFINDER_CELL_H
#define PATHFINDER_CELL_H

#include "SFML/Graphics.hpp"

class Grid;
class Cell {
public:
    Cell(sf::Vector2i indexes, float side);

    void update(Grid &grid, sf::RenderWindow &window);

    void draw(sf::RenderWindow &window);

    sf::Vector2f getPosition() const { return pos; }
    void setPosition(sf::Vector2f pos) { Cell::pos = pos; }
    float getPosX() const { return pos.x; }
    void setPosX(float posX) { Cell::pos.x = posX; }
    float getPosY() const { return pos.y; }
    void setPosY(float posY) { Cell::pos.y = posY; }

    std::string state = "unknown";
    bool obstacle = false;

    void findNeighbours(Grid &grid);
    void evaluateNeighbours();

    std::vector<Cell*> neighbours;
    float gCost, hCost, fCost;
    Cell* parent;

private:
    sf::RectangleShape cellBackgroundRect;
    sf::RectangleShape cellStateRect;
    sf::RectangleShape cellObstacleRect;

    // cell coordinates in the grid
    sf::Vector2i indexes;
    sf::Vector2f pos;
    float side;
};


#endif //PATHFINDER_CELL_H
