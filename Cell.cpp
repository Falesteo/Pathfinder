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

    cellStateRect.setPosition(pos.x + side / 2, pos.y + side / 2);
    cellStateRect.setSize({0, 0});

    cellObstacleRect.setPosition(pos.x + side / 2, pos.y + side / 2);
    cellObstacleRect.setSize({0, 0});
    cellObstacleRect.setFillColor(sf::Color(0, 24, 27));
}

void Cell::update(Grid &grid, sf::RenderWindow &window) {
    // add/remove obstacles
    if (cellBackgroundRect.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (state != "start" && state != "end")) {
            if (state != "unknown")
                grid.findPath();
            obstacle = true;
        } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && obstacle)
            obstacle = false;
    }

    // set state rectangle color
    if (state == "start")
        cellStateRect.setFillColor(sf::Color(94, 211, 0));
    else if (state == "end")
        cellStateRect.setFillColor(sf::Color(211, 28, 0));
    else if (state == "available")
        cellStateRect.setFillColor(sf::Color(38, 207, 238));
    else if (state == "evaluated")
        cellStateRect.setFillColor(sf::Color(3, 77, 133));
    else if (state == "path")
        cellStateRect.setFillColor(sf::Color(232, 219, 20));

    // resize the state rectangle
    if (state != "unknown") {
        if (cellStateRect.getSize().x < side)
            cellStateRect.setSize({cellStateRect.getSize().x + 1, cellStateRect.getSize().y + 1});
    } else {
        if (cellStateRect.getSize().x > 0)
            cellStateRect.setSize({cellStateRect.getSize().x - 1, cellStateRect.getSize().y - 1});
    }
    // change the origin point
    cellStateRect.setOrigin(cellStateRect.getSize().x / 2, cellStateRect.getSize().y / 2);

    if (obstacle) {
        if (cellObstacleRect.getSize().x < side)
            cellObstacleRect.setSize({cellObstacleRect.getSize().x + 1, cellObstacleRect.getSize().y + 1});
    } else {
        if (cellObstacleRect.getSize().x > 0)
            cellObstacleRect.setSize({cellObstacleRect.getSize().x - 1, cellObstacleRect.getSize().y - 1});
    }
    // change the origin point
    cellObstacleRect.setOrigin(cellObstacleRect.getSize().x / 2, cellObstacleRect.getSize().y / 2);
}

void Cell::draw(sf::RenderWindow &window) {
    // draw the cell
    window.draw(cellBackgroundRect);
    window.draw(cellStateRect);
    window.draw(cellObstacleRect);
}

void Cell::findNeighbours(Grid &grid) {
    neighbours = {};
    if (indexes.x - 1 >= 0)
        neighbours.push_back(&grid.cells[indexes.x - 1][indexes.y]);
    if (indexes.x + 1 < grid.getGridSize().x)
        neighbours.push_back(&grid.cells[indexes.x + 1][indexes.y]);
    if (indexes.y - 1 >= 0)
        neighbours.push_back(&grid.cells[indexes.x][indexes.y - 1]);
    if (indexes.y + 1 < grid.getGridSize().y)
        neighbours.push_back(&grid.cells[indexes.x][indexes.y + 1]);
}

void Cell::evaluateNeighbours() {

}

