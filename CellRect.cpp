//
// Created by longo on 25/09/2022.
//

#include "CellRect.h"

CellRect::CellRect(sf::Color color) : color(color) {
    rect.setSize({0, 0});
    rect.setFillColor(color);
}

void CellRect::update(bool active, float side) {
    for (int s = 0; s < 3; s++) {
        if (active) {
            if (rect.getSize().x < side)
                rect.setSize({rect.getSize().x + 1, rect.getSize().y + 1});
        } else {
            if (rect.getSize().x > 0)
                rect.setSize({rect.getSize().x - 1, rect.getSize().y - 1});
        }
        // change the origin point
        rect.setOrigin(rect.getSize().x / 2, rect.getSize().y / 2);
    }
}

void CellRect::draw(sf::RenderWindow &window) {
    window.draw(rect);
}