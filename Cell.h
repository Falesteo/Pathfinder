//
// Created by longo on 23/09/2022.
//

#ifndef PATHFINDER_CELL_H
#define PATHFINDER_CELL_H

#include "CellRect.h"
#include <list>

class Grid;
class Cell {
public:
    Cell(sf::Vector2i indexes, float side);
    ~Cell() = default;

    void update(Grid &grid, sf::RenderWindow &window);

    void draw(sf::RenderWindow &window);

    sf::Vector2f getPosition() const { return pos; }
    void setPosition(sf::Vector2f pos) { Cell::pos = pos; }
    float getPosX() const { return pos.x; }
    void setPosX(float posX) { Cell::pos.x = posX; }
    float getPosY() const { return pos.y; }
    void setPosY(float posY) { Cell::pos.y = posY; }

    std::string getState() { return state; };
    void setState(Grid &grid, std::string state);

    bool obstacle = false;
    bool start = false;
    bool end = false;

    void findNeighbours(Grid &grid, bool diagonals);
    void evaluateNeighbours(Grid &grid);

    std::vector<Cell*> neighbours;
    float gCost = 999;
    float hCost = 999;
    float fCost = 999;
    Cell* parent = nullptr;
    std::list<Cell*> children;

    void reset(Grid &grid);
    void removeChildren(Grid &grid);

private:
    std::string state = "unknown";

    sf::RectangleShape cellBackgroundRect;
    // normal
    CellRect cellAvailableRect = {sf::Color(38, 207, 238)};
    CellRect cellEvaluatedRect = {sf::Color(3, 77, 133)};
    CellRect cellPathRect = {sf::Color(232, 219, 20)};
    CellRect cellObstacleRect = {sf::Color(0, 24, 27)};
    CellRect cellStartRect = {sf::Color(94, 211, 0)};
    CellRect cellEndRect = {sf::Color(211, 28, 0)};

    sf::CircleShape parentArrow;

    // cell coordinates in the grid
    sf::Vector2i indexes;
    sf::Vector2f pos;
    float side;
};


#endif //PATHFINDER_CELL_H
