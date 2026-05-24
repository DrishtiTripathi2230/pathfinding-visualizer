#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class UI
{
public:
    bool init(const std::string& fontPath);
    void draw(sf::RenderWindow& window);

private:
    sf::Font m_font;
    void drawLegendRow(sf::RenderWindow& window,
        const std::string& key, const std::string& action,
        float x, float y, sf::Color keyColor);
};