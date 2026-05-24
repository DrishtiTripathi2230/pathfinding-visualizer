#include "UI.h"

bool UI::init(const std::string& fontPath)
{
    return m_font.openFromFile(fontPath);
}

void UI::draw(sf::RenderWindow& window)
{
    sf::RectangleShape panel(sf::Vector2f(230.f, 230.f));
    panel.setPosition(sf::Vector2f(10.f, 355.f));
    panel.setFillColor(sf::Color(0, 0, 0, 180));
    panel.setOutlineColor(sf::Color(80, 80, 80));
    panel.setOutlineThickness(1.f);
    window.draw(panel);

    sf::Text title(m_font, "CONTROLS", 13);
    title.setFillColor(sf::Color(220, 220, 220));
    title.setStyle(sf::Text::Bold);
    title.setPosition(sf::Vector2f(20.f, 363.f));
    window.draw(title);

    float startY = 390.f;
    float gap    = 26.f;

    drawLegendRow(window, "SPACE", "BFS",        20.f, startY + gap*0, sf::Color(100, 180, 255));
    drawLegendRow(window, "D",     "DFS",        20.f, startY + gap*1, sf::Color(100, 180, 255));
    drawLegendRow(window, "J",     "Dijkstra",   20.f, startY + gap*2, sf::Color(100, 180, 255));
    drawLegendRow(window, "A",     "A*",         20.f, startY + gap*3, sf::Color(100, 180, 255));
    drawLegendRow(window, "M",     "Maze",       20.f, startY + gap*4, sf::Color(150, 255, 150));
    drawLegendRow(window, "C",     "Clear",      20.f, startY + gap*5, sf::Color(255, 180, 100));
    drawLegendRow(window, "SHIFT", "Move Start", 20.f, startY + gap*6, sf::Color(255, 180, 100));
    drawLegendRow(window, "CTRL",  "Move End",   20.f, startY + gap*7, sf::Color(255, 180, 100));
}

void UI::drawLegendRow(sf::RenderWindow& window,
    const std::string& key, const std::string& action,
    float x, float y, sf::Color keyColor)
{
    sf::RectangleShape badge(sf::Vector2f(52.f, 18.f));
    badge.setPosition(sf::Vector2f(x, y));
    badge.setFillColor(sf::Color(50, 50, 50));
    badge.setOutlineColor(sf::Color(100, 100, 100));
    badge.setOutlineThickness(1.f);
    window.draw(badge);

    sf::Text keyText(m_font, key, 11);
    keyText.setFillColor(keyColor);
    keyText.setPosition(sf::Vector2f(x + 4.f, y + 2.f));
    window.draw(keyText);

    sf::Text actionText(m_font, action, 11);
    actionText.setFillColor(sf::Color(200, 200, 200));
    actionText.setPosition(sf::Vector2f(x + 62.f, y + 2.f));
    window.draw(actionText);
}