#include <SFML/Graphics.hpp>
#include "Grid.h"
#include "UI.h"

int main()
{
    const int CELL_SIZE = 20;
    const int ROWS      = 30;
    const int COLS      = 40;

    sf::RenderWindow window(
        sf::VideoMode({
            static_cast<unsigned>(COLS * CELL_SIZE),
            static_cast<unsigned>(ROWS * CELL_SIZE)
        }),
        "Pathfinding Visualizer",
        sf::Style::Close
    );

    window.setFramerateLimit(60);

    Grid grid(ROWS, COLS, CELL_SIZE);
    UI ui;
    ui.init("assets/JetBrainsMono-Regular.ttf");
    bool isDraggingWall  = false;
    bool isDraggingErase = false;
    bool isMovingStart   = false;
    bool isMovingEnd     = false;

    // BFS animation timing
    sf::Clock clock;
    const float BFS_STEP_INTERVAL = 0.01f;  // seconds between each BFS step

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* key = event->getIf<sf::Event::KeyPressed>())
            {
                // ESC to quit
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();

                // Space to start BFS
                if (key->code == sf::Keyboard::Key::Space)
                    grid.startBFS();

                // C to clear search
                if (key->code == sf::Keyboard::Key::C)
                    grid.clearSearch();

                // D to start DFS
                if (key->code == sf::Keyboard::Key::D)
                    grid.startDFS();

                // J to start Dijkstra
                if (key->code == sf::Keyboard::Key::J)
                    grid.startDijkstra();

                // A to start A*
                if (key->code == sf::Keyboard::Key::A)
                    grid.startAStar();

                // M to generate maze
                if (key->code == sf::Keyboard::Key::M)
                {
                   grid.generateMaze();
                   clock.restart();
                }
            }


            if (const auto* mb = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (grid.isBFSRunning()) continue;  // block input while running

                int row = grid.pixelToRow(mb->position.y);
                int col = grid.pixelToCol(mb->position.x);

                if (!grid.inBounds(row, col)) continue;

                if (mb->button == sf::Mouse::Button::Left)
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift))
                        isMovingStart = true;
                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
                        isMovingEnd = true;
                    else
                    {
                        isDraggingWall = true;
                        grid.toggleWall(row, col);
                    }
                }

                if (mb->button == sf::Mouse::Button::Right)
                {
                    isDraggingErase = true;
                    grid.eraseCell(row, col);
                }
            }

            if (event->is<sf::Event::MouseButtonReleased>())
            {
                isDraggingWall  = false;
                isDraggingErase = false;
                isMovingStart   = false;
                isMovingEnd     = false;
            }

            if (const auto* mm = event->getIf<sf::Event::MouseMoved>())
            {
                if (grid.isBFSRunning()) continue;

                int row = grid.pixelToRow(mm->position.y);
                int col = grid.pixelToCol(mm->position.x);

                if (!grid.inBounds(row, col)) continue;

                if (isDraggingWall)  grid.setWall(row, col);
                if (isDraggingErase) grid.eraseCell(row, col);
                if (isMovingStart)   grid.moveStart(row, col);
                if (isMovingEnd)     grid.moveEnd(row, col);
            }
        }

        // ── BFS STEP (time-based, not frame-based) ───────────
        if (grid.isBFSRunning())
        {
            if (clock.getElapsedTime().asSeconds() >= BFS_STEP_INTERVAL)
            {
                grid.stepBFS();
                clock.restart();
            }
        }

        if (grid.isDFSRunning())
        {
            if (clock.getElapsedTime().asSeconds() >= BFS_STEP_INTERVAL)
            {
                grid.stepDFS();
                clock.restart();
            }
        }

        if (grid.isDijkstraRunning())
       {
            if (clock.getElapsedTime().asSeconds() >= BFS_STEP_INTERVAL)
            {
                 grid.stepDijkstra();
                 clock.restart();
            }
        }


        if (grid.isAStarRunning())
        {
            if (clock.getElapsedTime().asSeconds() >= BFS_STEP_INTERVAL)
            {
                grid.stepAStar();
                clock.restart();
            }
        }
        if (grid.isMazeGenerating())
        {
            if (clock.getElapsedTime().asSeconds() >= BFS_STEP_INTERVAL)
            {
                grid.stepMaze();
                clock.restart();
            }
        }

        window.clear(sf::Color(18, 18, 18));
        grid.draw(window);
        ui.draw(window);
        window.display();
    }

    return 0;
}