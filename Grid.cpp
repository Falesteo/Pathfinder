//
// Created by longo on 23/09/2022.
//

#include "Grid.h"

Grid::Grid(sf::Vector2u windowSize, int scale) {
    // get window dimensions
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

    pathFound = false;
}

void Grid::setCellsNeighbours() {
    for (int i = 0; i < size(cells); i++)
        for (int j = 0; j < size(cells[i]); j++)
            cells[i][j].findNeighbours(*this, diagonals);
}

void Grid::update(sf::RenderWindow &window) {
    // remove all the obstacles the grid
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        for (int i = 0; i < gridSize.x; i++)
            for (int j = 0; j < gridSize.y; j++)
                cells[i][j].obstacle = false;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N) && pathFound) {
        resetGrid();
        setCellsNeighbours();
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
}

Cell* Grid::findFreeCell() {
    int i = round(rand() % gridSize.x);
    int j = round(rand() % gridSize.y);
    if (!cells[i][j].obstacle)
        return &cells[i][j];
    else
        return findFreeCell();
}

void Grid::findPath() {
    // and start the loop
    if (!pathFound) {
        for (int s = 0; s < speed; s++) {
            current = findLowestCost(availableCells);
            if (current == nullptr) {
                pathFound = true;
                return;
            } else if (current == end) {
                backtrackPath();
                return;
            }

            current->setState(*this, "evaluated");

            current->evaluateNeighbours(*this);
        }
    }
}

void Grid::resetGrid() {
    for (int i = 0; i < gridSize.x; i++) {
        for (int j = 0; j < gridSize.y; j++) {
            cells[i][j].reset();
            cells[i][j].setState(*this, "unknown");
        }
    }

    availableCells = {};
    evaluatedCells = {};
    pathCells = {};

    pathFound = false;
}

Cell* Grid::findLowestCost(std::list<Cell*> availableCells) {
    float lowestCost = 9999.9f;
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
        pathCells.push_back(currentPathCell);
        if (currentPathCell->getState() != "start" && currentPathCell->getState() != "end")
            currentPathCell->setState(*this, "path");

        currentPathCell = currentPathCell->parent;
    }

    pathFound = true;
}

void Grid::cutPath(Cell* deletedCell) {
    deletedCell->setState(*this, "unknown");
    deletedCell->removeChildren();

    int i;
    for (i = 1; pathCells[i] != deletedCell; i++)
        pathCells[i]->setState(*this, "unknown");
    for (i = i; i < size(pathCells); i++)
        pathCells[i]->setState(*this, "evaluated");

    pathCells = {};
    for (auto cell : evaluatedCells)
        cell->evaluateNeighbours(*this);

    pathFound = false;
}

