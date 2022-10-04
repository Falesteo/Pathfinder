//
// Created by longo on 23/09/2022.
//

#include "Grid.h"

Grid::Grid(sf::Vector2u windowSize) {
    // get window dimensions
    scale = 3;
    getGridMeasures(windowSize, scale);

    // and initialize cells matrix accordingly
    initializeGrid();

    // calculate cells neighbours
    diagonals = false;
    setCellsNeighbours();

    // add obstacles to the grid
    obstacleChance = 0.3;
    addObstacles(obstacleChance);

    // add start and end cell
    addStartAndEndCell();

    speed = 1;
}

void Grid::getGridMeasures(sf::Vector2u windowSize, int scale) {
    float aspectRatio = float(windowSize.x) / float(windowSize.y);

    // calculate the game unit based on the screen resolution
    if (is_equal(aspectRatio, 16.0 / 9.0) ||
        is_equal(aspectRatio, 16.0 / 10.0) ||
        is_equal(aspectRatio, 4.0 / 3.0))
        gridSize = {16 * scale, 9 * scale};
    else if (is_equal(aspectRatio, 5.0 / 4.0))
        gridSize = {15 * scale, 12 * scale};
    else if (is_equal(aspectRatio, 21.0 / 9.0))
        gridSize = {21 * scale, 9 * scale};

    side = floor(windowSize.x / gridSize.x);

    windowDiagonal = sqrtf(pow(windowSize.x, 2) + pow(windowSize.y, 2));
}

void Grid::initializeGrid() {
    cells = {};
    for (int i = 0; i < gridSize.x; i++) {
        cells.emplace_back();
        for (int j = 0; j < gridSize.y; j++) {
            Cell cell({i, j}, side);
            cells[i].emplace_back(cell);
        }
    }
}

void Grid::setCellsNeighbours() {
    for (int i = 0; i < size(cells); i++)
        for (int j = 0; j < size(cells[i]); j++)
            cells[i][j].findNeighbours(*this, diagonals);
}

void Grid::addObstacles(float obstacleChance) {
    for (int i = 0; i < gridSize.x; i++)
        for (int j = 0; j < gridSize.y; j++)
            if (rand() / (RAND_MAX + 1.0f) < obstacleChance)
                cells[i][j].obstacle = true;
}

void Grid::addStartAndEndCell() {
    // set the start cell
    start = findFreeCell();
    start->start = true;

    // set the start cell
    end = findFreeCell();
    end->end = true;

    // calculate start cell costs
    start->setState(*this, "available");
    start->gCost = 0;
    start->hCost = sqrt(pow(start->getPosX() - end->getPosX(), 2) + pow(start->getPosY() - end->getPosY(), 2));
    start->fCost = start->gCost + start->hCost;

    pathFound = false;
    newAvailableCells = true;
}

Cell* Grid::findFreeCell() {
    int i = round(rand() % gridSize.x);
    int j = round(rand() % gridSize.y);
    if (!cells[i][j].obstacle)
        return &cells[i][j];
    else
        return findFreeCell();
}

void Grid::update(sf::RenderWindow &window) {
    // remove all the obstacles the grid
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        for (int i = 0; i < gridSize.x; i++)
            for (int j = 0; j < gridSize.y; j++)
                cells[i][j].obstacle = false;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N) && (pathFound || !newAvailableCells)) {
        resetGrid();
        addObstacles(obstacleChance);
        addStartAndEndCell();
    }

    for (int i = 0; i < gridSize.x; i++)
        for (int j = 0; j < gridSize.y; j++)
            cells[i][j].update(*this, window);

    findPath();
}

void Grid::draw(sf::RenderWindow &window) {
    for (int i = 0; i < gridSize.x; i++)
        for (int j = 0; j < gridSize.y; j++)
            cells[i][j].draw(window);
}

void Grid::findPath() {
    // if the path has not been found yet and there's more cells available to be evaluated
    if (!pathFound && newAvailableCells) {
        for (int s = 0; s < speed; s++) {
            // set the available cell with the lowest cost to the current cell
            current = findLowestCost(availableCells);

            // if there's not a new cell to be evaluated it means that there's no more available cell
            if (current == nullptr) {
                newAvailableCells = false;
                return;
            }

            // if the new cell is the goal cell go back on the path to the start cell
            else if (current == end) {
                backtrackPath();
                return;
            }

            // if the new cell is a normal cell, evaluate it and it's neighbours
            current->setState(*this, "evaluated");
        }
    }
}

Cell* Grid::findLowestCost(std::list<Cell*> availableCells) {
    float lowestCost = RAND_MAX;
    Cell* lowestCostCell = nullptr;

    for (auto cell = availableCells.begin(); cell != availableCells.end(); cell++) {
        if ((*cell)->fCost < lowestCost) {
            lowestCost = (*cell)->fCost;
            lowestCostCell = *cell;
        } else if ((*cell)->fCost == lowestCost && (*cell)->hCost < lowestCostCell->hCost) {
            lowestCost = (*cell)->fCost;
            lowestCostCell = *cell;
        }
    }

    return lowestCostCell;
}

void Grid::backtrackPath() {
    Cell* currentPathCell = end;
    while (currentPathCell != start) {
        currentPathCell->setState(*this, "path");
        currentPathCell = currentPathCell->parent;
    }

    pathFound = true;
}

void Grid::resetGrid() {
    // reset all cells
    for (int i = 0; i < gridSize.x; i++) {
        for (int j = 0; j < gridSize.y; j++) {
            // make it an empty cell
            cells[i][j].obstacle = false;
            cells[i][j].start = false;
            cells[i][j].end = false;
        }
    }

    start->setState(*this, "unknown");

    pathFound = false;
}
