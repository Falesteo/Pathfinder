//
// Created by longo on 23/09/2022.
//

#ifndef PATHFINDER_GRID_H
#define PATHFINDER_GRID_H

#include <cmath>
#include <list>

#include "Cell.h"

using namespace std;

class Grid {
public:
    Grid(sf::Vector2u windowSize);

    void update(sf::RenderWindow &window);

    void draw(sf::RenderWindow &window);

    sf::Vector2i getGridSize() const { return gridSize; }
    void setGridSize(sf::Vector2i gridSize) { Grid::gridSize = gridSize; }

    Cell* findFreeCell();

    std::vector<std::vector<Cell>> cells;

    void findPath();

private:
    sf::Vector2i gridSize;
    float side;
    int width, height;

    void createGrid();

    void setCellsNeighbours();

    void getMeasures(sf::Vector2u windowSize);

    bool is_equal(float a, float b) { return fabs(a - b) <= 0.01; }

    void addObstacles(float obstaclesChance);

    void addStartAndEndCell();

    std::list<Cell*> availableCells;
    std::list<Cell*> evaluatedCells;
    std::list<Cell*> pathCells;

    Cell* start;
    Cell* end;
    Cell* current;

    Cell* findLowestCost(std::list<Cell*> cells);

    void backtrackPath();

    void resetPath();

    bool pathFound = false;
};


#endif //PATHFINDER_GRID_H
