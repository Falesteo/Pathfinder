//
// Created by longo on 23/09/2022.
//

#include "Grid.h"

Grid::Grid(sf::Vector2u windowSize) {
    // get window dimensions
    getMeasures(windowSize);

    // and create the grid accordingly
    for (int i = 0; i < gridSize.x; i++) {
        cells.emplace_back();
        for (int j = 0; j < gridSize.y; j++) {
            Cell cell({i, j}, side);
            cells[i].emplace_back(cell);
        }
    }
}

void Grid::update(sf::RenderWindow &window) {
    for (int i = 0; i < gridSize.x; i++)
        for (int j = 0; j < gridSize.y; j++)
            cells[i][j].update(window);

    // reset the grid
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
        for (int i = 0; i < gridSize.x; i++)
            for (int j = 0; j < gridSize.y; j++)
                cells[i][j].setObstacle(false);
    }
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
        gridSize = {32, 18};
    else if (is_equal(aspectRatio, 5.0 / 4.0))
        gridSize = {30, 24};
    else if (is_equal(aspectRatio, 21.0 / 9.0))
        gridSize = {42, 18};

    side = floor(windowSize.x / gridSize.x);

    // saves window dimensions
    width = windowSize.x;
    height = windowSize.y;
}
