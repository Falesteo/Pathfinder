//
// Created by longo on 23/09/2022.
//

#include "Cell.h"

Cell::Cell(sf::Vector2i indexes, float side) : indexes(indexes), side(side) {
    pos = {indexes.x * side, indexes.y * side};

    cellRect.setPosition(pos.x, pos.y);
    cellRect.setSize({float(side), float(side)});
    cellRect.setOutlineThickness(1);
    cellRect.setOutlineColor(sf::Color(0, 24, 27, 64));
    cellRect.setFillColor(sf::Color(0, 0, 0, 0));

    obstacleRect.setPosition(pos.x + side / 2, pos.y + side / 2);
    obstacleRect.setSize({0.0f, 0.0f});
    obstacleRect.setOrigin({obstacleRect.getSize().x / 2, obstacleRect.getSize().y / 2});
    obstacleRect.setOutlineThickness(1);
    obstacleRect.setOutlineColor(sf::Color(0, 24, 27, 64));
    obstacleRect.setFillColor(sf::Color(0, 24, 27));
}

void Cell::update(sf::RenderWindow &window) {
    if (cellRect.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            obstacle = true;
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            obstacle = false;
}

void Cell::draw(sf::RenderWindow &window) {
    // draw the cell
    window.draw(cellRect);

    // and if the cell has an obstacle on it, draw that too
    if (obstacle) {
        if (obstacleRect.getSize().x < side)
            obstacleRect.setSize({obstacleRect.getSize().x + 1, obstacleRect.getSize().y + 1});
    } else {
        if (obstacleRect.getSize().x > 0)
            obstacleRect.setSize({obstacleRect.getSize().x - 1, obstacleRect.getSize().y - 1});
    }
    obstacleRect.setOrigin({obstacleRect.getSize().x / 2, obstacleRect.getSize().y / 2});
    window.draw(obstacleRect);
}

