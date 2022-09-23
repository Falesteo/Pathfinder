//
// Created by longo on 23/09/2022.
//

#include "Cell.h"

Cell::Cell(sf::Vector2i indexes, float side) : indexes(indexes), side(side) {
    pos = {indexes.x * side, indexes.y * side};

    cellBackgroundRect.setPosition(pos.x, pos.y);
    cellBackgroundRect.setSize({side, side});
    cellBackgroundRect.setOutlineThickness(1);
    cellBackgroundRect.setOutlineColor(sf::Color(0, 24, 27, 64));
    cellBackgroundRect.setFillColor(sf::Color(239, 254, 255));

    cellStateRect.setPosition(pos.x + side / 2, pos.y + side / 2);
    cellStateRect.setSize({0, 0});

    state = "free";
}

void Cell::update(sf::RenderWindow &window) {
    // add/remove obstacles
    if (cellBackgroundRect.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window)))) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && state == "free")
            state = "obstacle";
        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && state == "obstacle")
            state = "free";
    }

    // set state rectangle color
    if (state == "obstacle")
        cellStateRect.setFillColor(sf::Color(0, 24, 27));
    else if (state == "start")
        cellStateRect.setFillColor(sf::Color(94, 211, 0));
    else if (state == "end")
        cellStateRect.setFillColor(sf::Color(211, 28, 0));

    // resize the state rectangle
    if (state != "free") {
        if (cellStateRect.getSize().x < side)
            cellStateRect.setSize({cellStateRect.getSize().x + 1, cellStateRect.getSize().y + 1});
    } else {
        if (cellStateRect.getSize().x > 0)
            cellStateRect.setSize({cellStateRect.getSize().x - 1, cellStateRect.getSize().y - 1});
    }

    // change the origin point
    cellStateRect.setOrigin(cellStateRect.getSize().x / 2, cellStateRect.getSize().y / 2);
}

void Cell::draw(sf::RenderWindow &window) {
    // draw the cell
    window.draw(cellBackgroundRect);
    window.draw(cellStateRect);
}

