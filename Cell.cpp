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
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (!start && !end)) {
            obstacle = true;
            // if you block the path all the path cells after the one blocked have to be removed
            setState(grid, "unknown");
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
            parentArrow.setPosition(pos.x + side / 2 + side / 3, pos.y + side / 2);
        if (parent->getPosX() - pos.x == -side)
            parentArrow.setPosition(pos.x + side / 2 - side / 3, pos.y + side / 2);
        if (parent->getPosY() - pos.y == side)
            parentArrow.setPosition(pos.x + side / 2, pos.y + side / 2 + side / 3);
        if (parent->getPosY() - pos.y == -side)
            parentArrow.setPosition(pos.x + side / 2, pos.y + side / 2 - side / 3);
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
        // if neighbour is not traversable or is already evaluated skip to the next neighbour
        if (neighbour->obstacle || neighbour->state == "evaluated") {}

        // else if new path through the current cell is shorter or neighbour has not been considered
        else if ((gCost + sqrt(pow(getPosX() - neighbour->getPosX(), 2) + pow(getPosY() - neighbour->getPosY(), 2)) <
                 neighbour->gCost && neighbour->state == "available") || neighbour->state == "unknown") {
            // update the g cost (distance from the start cell) and the neighbour parent and evaluate h and f costs
            neighbour->gCost = gCost + sqrt(pow(getPosX() - neighbour->getPosX(), 2) +
                                            pow(getPosY() - neighbour->getPosY(), 2));
            neighbour->hCost = sqrt(pow(grid.end->getPosX() - neighbour->getPosX(), 2) +
                                    pow(grid.end->getPosY() - neighbour->getPosY(), 2));
            neighbour->fCost = neighbour->gCost + neighbour->hCost;

            // if the neighbour has already a parent, remove it from its parent list of children
            if (neighbour->parent != nullptr)
                neighbour->parent->children.remove(neighbour);

            // and set its new parent to current
            neighbour->parent = this;
            children.push_back(neighbour);

            // and, if it's a not yet considered cell, add the neighbour to the available cells list
            if (neighbour->state == "unknown")
                neighbour->setState(grid, "available");
        }
    }
}

void Cell::setState(Grid &grid, std::string state) {
    if (state == "available") {
        if (this->state == "evaluated") {
            grid.evaluatedCells.remove(this);
        } else if (this->state == "path") {
            auto it = find(grid.pathCells.begin(), grid.pathCells.end(), this);
            if (it != grid.pathCells.end())
                grid.pathCells.erase(it);
        }

        grid.availableCells.push_back(this);
    }

    else if (state == "evaluated") {
        if (this->state == "available") {
            grid.availableCells.remove(this);
            evaluateNeighbours(grid);
        } else if (this->state == "path") {
            auto it = find(grid.pathCells.begin(), grid.pathCells.end(), this);
            if (it != grid.pathCells.end())
                grid.pathCells.erase(it);
        }

        grid.evaluatedCells.push_back(this);
    }

    else if (state == "path") {
        if (this->state == "evaluated")
            grid.evaluatedCells.remove(this);
        else if (this->state == "available")
            grid.availableCells.remove(this);

        grid.pathCells.push_back(this);
    }

    else if (state == "unknown") {
        // reset cell
        for (auto child: children)
            child->reset(grid);

        children.clear();

        // remove this cell from the of available, evaluated and path cells lists
        if (this->state == "available")
            grid.availableCells.remove(this);
        else if (this->state == "evaluated")
            grid.evaluatedCells.remove(this);
        else if (this->state == "path") {
            for (int i = size(grid.pathCells) - 1; i >= 0; i--)
                grid.pathCells[i]->setState(grid, "evaluated");
            grid.pathFound = false;
        }

        // reset all the values
        gCost = grid.windowDiagonal;
        hCost = grid.windowDiagonal;
        fCost = gCost + hCost;

        parent = nullptr;

        if (this->state == "path" || this->state == "evaluated") {
            for (auto cell: grid.evaluatedCells)
                cell->evaluateNeighbours(grid);
        }
    }

    this->state = state;
}

void Cell::reset(Grid &grid) {
    for (auto child: children)
        child->reset(grid);

    children.clear();

    // remove this cell from the of available, evaluated and path cells lists
    if (this->state == "available")
        grid.availableCells.remove(this);
    else if (this->state == "evaluated")
        grid.evaluatedCells.remove(this);
    else if (this->state == "path") {
        auto it = find(grid.pathCells.begin(), grid.pathCells.end(), this);
        if (it != grid.pathCells.end())
            grid.pathCells.erase(it);
    }

    this->state = "unknown";

    // reset all the values
    gCost = grid.windowDiagonal;
    hCost = grid.windowDiagonal;
    fCost = gCost + hCost;

    parent = nullptr;
}
