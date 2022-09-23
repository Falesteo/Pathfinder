//
// Created by longo on 23/09/2022.
//

#ifndef PATHFINDER_GRID_H
#define PATHFINDER_GRID_H

#include <cmath>

#include "Cell.h"

using namespace std;

class Grid {
public:
    Grid(sf::Vector2u windowSize);

    void update(sf::RenderWindow &window);

    void draw(sf::RenderWindow &window);

    Cell* findFreeCell();

private:
    std::vector<std::vector<Cell>> cells;
    sf::Vector2i gridSize;
    float side;
    int width, height;

    void getMeasures(sf::Vector2u windowSize);

    bool is_equal(float a, float b) { return fabs(a - b) <= 0.01; }

    void addStartAndEndCell();
};


#endif //PATHFINDER_GRID_H
