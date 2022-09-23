//
// Created by longo on 23/09/2022.
//

#include "Grid.h"

Grid::Grid(sf::Vector2u windowSize) {
    // get window dimensions
    getMeasures(windowSize);

    // and create the grid accordingly
    createGrid();
    setCellsNeighbours();

    addObstacles(0.4);
    addStartAndEndCell();
    availableCells.push_back(start);
}

void Grid::createGrid() {
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
            cells[i][j].findNeighbours(*this);
}

void Grid::update(sf::RenderWindow &window) {
    // remove all the obstacles the grid
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        for (int i = 0; i < gridSize.x; i++)
            for (int j = 0; j < gridSize.y; j++)
                cells[i][j].obstacle = false;
    } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)) {
        createGrid();
        setCellsNeighbours();
        addObstacles(0.4);
        addStartAndEndCell();
        availableCells.push_back(start);
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

void Grid::getMeasures(sf::Vector2u windowSize) {
    float aspectRatio = float(windowSize.x) / float(windowSize.y);

    // calculate the game unit based on the screen resolution
    if (is_equal(aspectRatio, 16.0 / 9.0) ||
        is_equal(aspectRatio, 16.0 / 10.0) ||
        is_equal(aspectRatio, 4.0 / 3.0))
        gridSize = {48, 27};
    else if (is_equal(aspectRatio, 5.0 / 4.0))
        gridSize = {30, 24};
    else if (is_equal(aspectRatio, 21.0 / 9.0))
        gridSize = {42, 18};

    side = floor(windowSize.x / gridSize.x);
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
    start->state = "start";

    // set the start cell
    end = findFreeCell();
    end->state = "end";

    // calculate start cell costs
    start->gCost = 0;
    start->hCost = sqrt(pow(start->getPosX() - end->getPosX(), 2) + pow(start->getPosY() - end->getPosY(), 2));
    start->fCost = start->gCost + start->fCost;
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
        for (int i = 0; i < 1; i++) {
            current = findLowestCost(availableCells);
            availableCells.remove(current);
            evaluatedCells.push_back(current);
            if (current->state != "start" && current->state != "end")
                current->state = "evaluated";

            // if target has been found exit from the loop
            if (current == end)
                backtrackPath();

            // for every adjacent cell to current
            for (auto neighbour: current->neighbours) {
                // if neighbour is not traversable or is already evaluated
                if (neighbour->obstacle ||
                    std::find(evaluatedCells.begin(), evaluatedCells.end(), neighbour) != evaluatedCells.end())
                    // skip to the next neighbour
                    continue;

                // if new path through the current cell is shorter or neighbour has not been considered
                if (current->gCost + sqrt(pow(current->getPosX() - neighbour->getPosX(), 2) +
                                          pow(current->getPosY() - neighbour->getPosY(), 2)) < neighbour->gCost ||
                    std::find(availableCells.begin(), availableCells.end(), neighbour) == availableCells.end()) {
                    // update the g cost (distance from the start cell) and the neighbour parent and evaluate h and f costs
                    neighbour->gCost = current->gCost + sqrt(pow(current->getPosX() - neighbour->getPosX(), 2) +
                                                             pow(current->getPosY() - neighbour->getPosY(), 2));
                    neighbour->hCost = sqrt(pow(end->getPosX() - neighbour->getPosX(), 2) +
                                            pow(end->getPosY() - neighbour->getPosY(), 2));
                    neighbour->fCost = neighbour->gCost + neighbour->hCost;

                    // set parent to current
                    neighbour->parent = current;

                    // and, if it hasn't been considered, add the neighbours to the considered cells list
                    if (std::find(availableCells.begin(), availableCells.end(), neighbour) == availableCells.end()) {
                        availableCells.push_back(neighbour);
                        if (neighbour->state != "start" && neighbour->state != "end")
                            neighbour->state = "available";
                    }
                }
            }
        }
    }

    if (size(availableCells) == 0)
        pathFound = true;
}

void Grid::resetPath() {
    availableCells = {};
    evaluatedCells = {};
    pathCells = {};

    for (int i = 0; i < size(cells); i++) {
        for (int j = 0; j < size(cells[i]); j++) {
            cells[i][j].gCost = 0;
            cells[i][j].hCost = 0;
            cells[i][j].hCost = 0;

            if (cells[i][j].state != "start" && cells[i][j].state != "end")
                cells[i][j].state = "unknown";
        }
    }
}

Cell* Grid::findLowestCost(std::list<Cell*> availableCells) {
    float lowestCost = 9999;
    Cell* lowestCostCell;

    for (auto cell = availableCells.begin(); cell != availableCells.end(); cell++) {
        if ((*cell)->fCost < lowestCost) {
            lowestCost = (*cell)->fCost;
            lowestCostCell = *cell;
        } else if ((*cell)->fCost == lowestCost) {
            if ((*cell)->hCost < lowestCostCell->hCost) {
                lowestCost = (*cell)->fCost;
                lowestCostCell = *cell;
            }
        }
    }

    return lowestCostCell;
}

void Grid::backtrackPath() {
    Cell* currentPathCell = end;
    while (currentPathCell != start) {
        pathCells.push_back(currentPathCell);
        if (currentPathCell->state != "start" && currentPathCell->state != "end")
            currentPathCell->state = "path";
        currentPathCell = currentPathCell->parent;
    }

    pathFound = true;
}
