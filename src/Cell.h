#pragma once
#include <SFML/Graphics.hpp>

// enum class is strongly typed — you can't accidentally mix up
// CellState::WALL with some random integer in your code
enum class CellState
{
    EMPTY,
    WALL,
    START,
    END,
    VISITED,
    PATH
};

struct Cell
{
    int row;
    int col;
    CellState state = CellState::EMPTY;

    // Returns the color this cell should be drawn with
    sf::Color getColor() const
    {
        switch (state)
        {
            case CellState::EMPTY:   return sf::Color(30, 30, 30);
            case CellState::WALL:    return sf::Color(68, 68, 65);
            case CellState::START:   return sf::Color(29, 158, 117);
            case CellState::END:     return sf::Color(216, 90, 48);
            case CellState::VISITED: return sf::Color(181, 212, 244);
            case CellState::PATH:    return sf::Color(250, 199, 117);
            default:                 return sf::Color::White;
        }
    }
};