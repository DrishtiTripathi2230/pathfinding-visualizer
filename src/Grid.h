#pragma once
#include <vector>
#include <queue>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "Cell.h"

using DijkstraPQ = std::priority_queue<std::tuple<int,int,int>, std::vector<std::tuple<int,int,int>>, std::greater<std::tuple<int,int,int>>>;

class Grid
{
public:
    Grid(int rows, int cols, int cellSize);

    void draw(sf::RenderWindow& window);

    void toggleWall(int row, int col);
    void setWall(int row, int col);
    void eraseCell(int row, int col);
    void moveStart(int row, int col);
    void moveEnd(int row, int col);

    void startBFS();
    void stepBFS();
    bool isBFSRunning() const { return m_bfsRunning; }
    bool isBFSDone()    const { return m_bfsDone; }

    void startDFS();
    void stepDFS();
    bool isDFSRunning() const { return m_dfsRunning; }
    bool isDFSDone()    const { return m_dfsDone; }

    void startDijkstra();
    void stepDijkstra();
    bool isDijkstraRunning() const { return m_dijkstraRunning; }
    bool isDijkstraDone()    const { return m_dijkstraDone; }

    void clearSearch();

    int  pixelToRow(int y) const;
    int  pixelToCol(int x) const;
    bool inBounds(int row, int col) const;

    int   getRows() const { return m_rows; }
    int   getCols() const { return m_cols; }
    Cell& getCell(int row, int col) { return m_cells[row][col]; }
    

    // A*
    void startAStar();
    void stepAStar();
    bool isAStarRunning() const { return m_astarRunning; }
    bool isAStarDone()    const { return m_astarDone; }
    

    void generateMaze();
    bool isMazeGenerating() const { return m_mazeRunning; }
    void stepMaze();

private:
    void reconstructPath();
    void reconstructDFSPath();
    void reconstructDijkstraPath();
    void reconstructAStarPath(); 

    int heuristic(int row, int col) const;  // Manhattan distance
    int m_rows, m_cols, m_cellSize;
    int m_startRow, m_startCol;
    int m_endRow,   m_endCol;

    std::vector<std::vector<Cell>> m_cells;

    bool m_bfsRunning = false;
    bool m_bfsDone    = false;
    bool m_pathFound  = false;
    std::queue<std::pair<int,int>> m_bfsQueue;
    std::unordered_map<int,int> m_parent;

    bool m_dfsRunning   = false;
    bool m_dfsDone      = false;
    bool m_dfsPathFound = false;
    std::stack<std::pair<int,int>> m_dfsStack;
    std::unordered_map<int,int> m_dfsParent;

    bool m_dijkstraRunning   = false;
    bool m_dijkstraDone      = false;
    bool m_dijkstraPathFound = false;
    DijkstraPQ m_dijkstraQueue;
    std::unordered_map<int,int> m_dijkstraParent;
    std::unordered_map<int,int> m_dijkstraDist;

    // A* state
    bool m_astarRunning   = false;
    bool m_astarDone      = false;
    bool m_astarPathFound = false;
    DijkstraPQ m_astarQueue;   // reuse same type — {f, row, col}
    std::unordered_map<int,int> m_astarParent;
    std::unordered_map<int,int> m_astarG;  // actual cost from start

    // Maze generation state
    bool m_mazeRunning = false;
    std::stack<std::pair<int,int>> m_mazeStack;
    std::vector<std::vector<bool>> m_mazeVisited;

    void initMaze();
};

