//
// Created by longo on 23/09/2022.
//

#include "Cell.h"
#include "Grid.h"

Cell::Cell(sf::Vector2i indexes, float side) : indexes(indexes), side(side) {
    pos = {indexes.x * side, indexes.y * side};

    cellBackgroundRect.setPosition(pos.x, pos.y);
    cellBackgroundRect.setSize({side, side});
    cellBackgroundRect.setOutlineThickness(1);
    cellBackgroundRect.setOutlineColor(sf::Color(0, 24, 27, 64));
    cellBackgroundRect.setFillColor(sf::Color(239, 254, 255));

    cellAvailableRect.rect.setPosition({pos.x + side / 2, pos.y + side / 2});
    cellEvaluatedRect.rect.setPosition({pos.x + side / 2, pos.y + side / 2});
    cellPathRect.rect.setPosition({pos.x + side / 2, pos.y + side / 2});
    cellObstacleRect.rect.setPosition({pos.x + side / 2, pos.y + side / 2});
    cellStartRect.rect.setPosition({pos.x + side / 2, pos.y + side / 2});
    cellEndRect.rect.setPosition({pos.x + side / 2, pos.y + side / 2});

    parentArrow.setRadius(side / 10);
    parentArrow.setFillColor(sf::Color(0, 24, 27));
    parentArrow.setOrigin({side / 10, side / 10});
}

void Cell::update(Grid &grid, sf::RenderWindow &window) {
    // add/remove obstacles
    if (cellBackgroundRect.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (state != "start" && state != "end")) {
            obstacle = true;
            // if you block the path all the path cells after the one blocked have to be removed
            if (state == "path")
                grid.cutPath(this);
        } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && obstacle)
            obstacle = false;
    }

    cellAvailableRect.update((state == "available" || state == "evaluated" || state == "path"), side);
    cellEvaluatedRect.update((state == "evaluated" || state == "path"), side);
    cellPathRect.update((state == "path"), side);
    cellStartRect.update(start, side);
    cellEndRect.update(end, side);
    cellObstacleRect.update(obstacle, side);

    if (parent != nullptr) {
        if (parent->getPosX() - pos.x == side)
            parentArrow.setPosition(pos.x + side / 2 + side / 4, pos.y + side / 2);
        if (parent->getPosX() - pos.x == -side)
            parentArrow.setPosition(pos.x + side / 2 - side / 4, pos.y + side / 2);
        if (parent->getPosY() - pos.y == side)
            parentArrow.setPosition(pos.x + side / 2, pos.y + side / 2 + side / 4);
        if (parent->getPosY() - pos.y == -side)
            parentArrow.setPosition(pos.x + side / 2, pos.y + side / 2 - side / 4);
    }
}

void Cell::draw(sf::RenderWindow &window) {
    // draw the cell
    window.draw(cellBackgroundRect);
    cellAvailableRect.draw(window);
    cellEvaluatedRect.draw(window);
    cellPathRect.draw(window);
    cellObstacleRect.draw(window);
    cellStartRect.draw(window);
    cellEndRect.draw(window);

    // if (parent != nullptr)
        // window.draw(parentArrow);
}

void Cell::findNeighbours(Grid &grid, bool diagonals) {
    neighbours = {};
    if (indexes.x - 1 >= 0)
        neighbours.push_back(&grid.cells[indexes.x - 1][indexes.y]);
    if (indexes.x + 1 < grid.getGridSize().x)
        neighbours.push_back(&grid.cells[indexes.x + 1][indexes.y]);
    if (indexes.y - 1 >= 0)
        neighbours.push_back(&grid.cells[indexes.x][indexes.y - 1]);
    if (indexes.y + 1 < grid.getGridSize().y)
        neighbours.push_back(&grid.cells[indexes.x][indexes.y + 1]);

    if (diagonals) {
        if (indexes.x - 1 >= 0 && indexes.y - 1 >= 0)
            neighbours.push_back(&grid.cells[indexes.x - 1][indexes.y - 1]);
        if (indexes.x - 1 >= 0 && indexes.y + 1 < grid.getGridSize().y)
            neighbours.push_back(&grid.cells[indexes.x - 1][indexes.y + 1]);
        if (indexes.x + 1 < grid.getGridSize().x && indexes.y - 1 >= 0)
            neighbours.push_back(&grid.cells[indexes.x + 1][indexes.y - 1]);
        if (indexes.x + 1 < grid.getGridSize().x && indexes.y + 1 < grid.getGridSize().y)
            neighbours.push_back(&grid.cells[indexes.x + 1][indexes.y + 1]);
    }
}

void Cell::evaluateNeighbours(Grid &grid) {
    // for every adjacent cell to current
    for (auto neighbour: neighbours) {
        // if neighbour is not traversable or is already evaluated
        if (neighbour->obstacle || std::find(grid.evaluatedCells.begin(), grid.evaluatedCells.end(), neighbour) != grid.evaluatedCells.end())
            // skip to the next neighbour
            continue;

        // if new path through the current cell is shorter or neighbour has not been considered
        if (gCost + sqrt(pow(getPosX() - neighbour->getPosX(), 2) +
                                  pow(getPosY() - neighbour->getPosY(), 2)) < neighbour->gCost ||
            std::find(grid.availableCells.begin(), grid.availableCells.end(), neighbour) == grid.availableCells.end()) {
            // update the g cost (distance from the start cell) and the neighbour parent and evaluate h and f costs
            neighbour->gCost = gCost + sqrt(pow(getPosX() - neighbour->getPosX(), 2) +
                                                     pow(getPosY() - neighbour->getPosY(), 2));
            neighbour->hCost = sqrt(pow(grid.end->getPosX() - neighbour->getPosX(), 2) +
                                    pow(grid.end->getPosY() - neighbour->getPosY(), 2));
            neighbour->fCost = neighbour->gCost + neighbour->hCost;

            // set parent to current
            neighbour->parent = this;
            children.push_back(neighbour);

            // and, if it hasn't been considered, add the neighbours to the considered cells list
            if (std::find(grid.availableCells.begin(), grid.availableCells.end(), neighbour) == grid.availableCells.end()) {
                if (neighbour->state != "start" && neighbour->state != "end")
                    neighbour->setState(grid, "available");
            }
        }
    }
}

void Cell::removeChildren() {
    gCost = 999;
    hCost = 999;
    fCost = 999;
    for (auto child : children) {
        if (child->state != "end")
            child->state = "unknown";
        child->parent = nullptr;
        child->removeChildren();
    }
}

void Cell::setState(Grid &grid, std::string state) {
    if (state == "path") {
        if (this->state == "evaluated") {
            grid.evaluatedCells.remove(this);
        } else if (this->state == "available")
            grid.availableCells.remove(this);
    }

    if (state == "available") {
        if (this->state == "evaluated") {
            grid.evaluatedCells.remove(this);
            grid.availableCells.push_back(this);
        } else if (this->state == "unknown")
            grid.availableCells.push_back(this);
    }

    if (state == "evaluated") {
        if (this->state == "available") {
            grid.availableCells.remove(this);
            grid.evaluatedCells.push_back(this);
        } else if (this->state == "unknown")
            grid.evaluatedCells.push_back(this);
    }

    if (state == "unknown") {
        grid.availableCells.remove(this);
        grid.evaluatedCells.remove(this);
    }

    this->state = state;
}

void Cell::reset() {
    gCost = 999;
    hCost = 999;
    fCost = 999;
    parent = nullptr;
    obstacle = false;
    start = false;
    end = false;
}
