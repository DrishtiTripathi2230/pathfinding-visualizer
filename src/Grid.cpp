#include "Grid.h"
#include <cstdlib>
#include <algorithm>
#include <ctime>

Grid::Grid(int rows, int cols, int cellSize)
    : m_rows(rows), m_cols(cols), m_cellSize(cellSize),
      m_startRow(5), m_startCol(5),
      m_endRow(24),  m_endCol(34)
{
    m_cells.resize(m_rows, std::vector<Cell>(m_cols));

    for (int r = 0; r < m_rows; r++)
        for (int c = 0; c < m_cols; c++)
        {
            m_cells[r][c].row = r;
            m_cells[r][c].col = c;
        }

    // Set default start/end
    m_cells[m_startRow][m_startCol].state = CellState::START;
    m_cells[m_endRow][m_endCol].state     = CellState::END;
}

void Grid::draw(sf::RenderWindow& window)
{
    sf::RectangleShape rect(sf::Vector2f(
        m_cellSize - 1.f,
        m_cellSize - 1.f
    ));

    for (int r = 0; r < m_rows; r++)
        for (int c = 0; c < m_cols; c++)
        {
            rect.setPosition(sf::Vector2f(c * m_cellSize, r * m_cellSize));
            rect.setFillColor(m_cells[r][c].getColor());
            window.draw(rect);
        }
}

void Grid::toggleWall(int row, int col)
{
    if (!inBounds(row, col)) return;
    Cell& cell = m_cells[row][col];
    if (cell.state == CellState::START || cell.state == CellState::END) return;

    cell.state = (cell.state == CellState::WALL)
        ? CellState::EMPTY : CellState::WALL;
}

void Grid::setWall(int row, int col)
{
    if (!inBounds(row, col)) return;
    Cell& cell = m_cells[row][col];
    if (cell.state == CellState::START || cell.state == CellState::END) return;
    cell.state = CellState::WALL;
}

void Grid::eraseCell(int row, int col)
{
    if (!inBounds(row, col)) return;
    Cell& cell = m_cells[row][col];
    if (cell.state == CellState::START || cell.state == CellState::END) return;
    cell.state = CellState::EMPTY;
}

void Grid::moveStart(int row, int col)
{
    if (!inBounds(row, col)) return;
    if (m_cells[row][col].state == CellState::END) return;

    // Clear old start
    m_cells[m_startRow][m_startCol].state = CellState::EMPTY;

    // Place new start
    m_startRow = row;
    m_startCol = col;
    m_cells[m_startRow][m_startCol].state = CellState::START;
}

void Grid::moveEnd(int row, int col)
{
    if (!inBounds(row, col)) return;
    if (m_cells[row][col].state == CellState::START) return;

    // Clear old end
    m_cells[m_endRow][m_endCol].state = CellState::EMPTY;

    // Place new end
    m_endRow = row;
    m_endCol = col;
    m_cells[m_endRow][m_endCol].state = CellState::END;
}

int Grid::pixelToRow(int y) const { return y / m_cellSize; }
int Grid::pixelToCol(int x) const { return x / m_cellSize; }

bool Grid::inBounds(int row, int col) const
{
    return row >= 0 && row < m_rows && col >= 0 && col < m_cols;
}

void Grid::clearSearch()
{
    for (int r = 0; r < m_rows; r++)
        for (int c = 0; c < m_cols; c++)
        {
            Cell& cell = m_cells[r][c];
            if (cell.state == CellState::VISITED ||
                cell.state == CellState::PATH)
                cell.state = CellState::EMPTY;
        }
}

void Grid::startBFS()
{
    // Clear any previous search first
    clearSearch();

    // Reset state
    m_bfsRunning = true;
    m_bfsDone    = false;
    m_pathFound  = false;

    // Clear queue and parent map
    while (!m_bfsQueue.empty()) m_bfsQueue.pop();
    m_parent.clear();

    // Encode position as single int for the map
    int startKey = m_startRow * m_cols + m_startCol;
    m_parent[startKey] = -1;  // start has no parent

    m_bfsQueue.push({m_startRow, m_startCol});
}

void Grid::stepBFS()
{
    if (!m_bfsRunning || m_bfsQueue.empty())
    {
        // Queue empty = no path found
        m_bfsRunning = false;
        m_bfsDone    = true;
        return;
    }

    // Process one cell per step (for animation)
    auto [row, col] = m_bfsQueue.front();
    m_bfsQueue.pop();

    // Did we reach the end?
    if (row == m_endRow && col == m_endCol)
    {
        m_bfsRunning = false;
        m_bfsDone    = true;
        m_pathFound  = true;
        reconstructPath();
        return;
    }

    // Explore 4 neighbors (up, down, left, right)
    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = { 0, 0,-1, 1};

    for (int i = 0; i < 4; i++)
    {
        int nr = row + dr[i];
        int nc = col + dc[i];

        if (!inBounds(nr, nc)) continue;

        Cell& neighbor = m_cells[nr][nc];
        int   nKey     = nr * m_cols + nc;

        // Skip if already visited or is a wall
        if (m_parent.count(nKey)) continue;
        if (neighbor.state == CellState::WALL) continue;

        // Mark as visited and record parent
        m_parent[nKey] = row * m_cols + col;
        m_bfsQueue.push({nr, nc});

        // Don't overwrite start/end colors
        if (neighbor.state == CellState::EMPTY)
            neighbor.state = CellState::VISITED;
    }
}

void Grid::reconstructPath()
{
    // Walk backwards from end to start using parent map
    int key = m_endRow * m_cols + m_endCol;

    while (key != -1)
    {
        int r = key / m_cols;
        int c = key % m_cols;

        // Don't overwrite start/end
        if (m_cells[r][c].state == CellState::EMPTY ||
            m_cells[r][c].state == CellState::VISITED)
            m_cells[r][c].state = CellState::PATH;

        key = m_parent.count(key) ? m_parent[key] : -1;
    }
}


void Grid::startDFS()
{
    clearSearch();

    m_dfsRunning    = false;
    m_dfsDone       = false;
    m_dfsPathFound  = false;

    while (!m_dfsStack.empty()) m_dfsStack.pop();
    m_dfsParent.clear();

    int startKey = m_startRow * m_cols + m_startCol;
    m_dfsParent[startKey] = -1;

    m_dfsStack.push({m_startRow, m_startCol});
    m_dfsRunning = true;
}

void Grid::stepDFS()
{
    if (!m_dfsRunning || m_dfsStack.empty())
    {
        m_dfsRunning = false;
        m_dfsDone    = true;
        return;
    }

    // Stack instead of queue — only this line differs from BFS!
    auto [row, col] = m_dfsStack.top();
    m_dfsStack.pop();

    if (row == m_endRow && col == m_endCol)
    {
        m_dfsRunning   = false;
        m_dfsDone      = true;
        m_dfsPathFound = true;
        reconstructDFSPath();
        return;
    }

    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = { 0, 0,-1, 1};

    for (int i = 0; i < 4; i++)
    {
        int nr = row + dr[i];
        int nc = col + dc[i];

        if (!inBounds(nr, nc)) continue;

        Cell& neighbor = m_cells[nr][nc];
        int   nKey     = nr * m_cols + nc;

        if (m_dfsParent.count(nKey)) continue;
        if (neighbor.state == CellState::WALL) continue;

        m_dfsParent[nKey] = row * m_cols + col;
        m_dfsStack.push({nr, nc});

        if (neighbor.state == CellState::EMPTY)
            neighbor.state = CellState::VISITED;
    }
}

void Grid::reconstructDFSPath()
{
    int key = m_endRow * m_cols + m_endCol;

    while (key != -1)
    {
        int r = key / m_cols;
        int c = key % m_cols;

        if (m_cells[r][c].state == CellState::EMPTY ||
            m_cells[r][c].state == CellState::VISITED)
            m_cells[r][c].state = CellState::PATH;

        key = m_dfsParent.count(key) ? m_dfsParent[key] : -1;
    }
}



void Grid::startDijkstra()
{
    clearSearch();

    m_dijkstraRunning   = false;
    m_dijkstraDone      = false;
    m_dijkstraPathFound = false;

    // Clear the priority queue by swapping with empty one
    m_dijkstraQueue = {};
    m_dijkstraParent.clear();
    m_dijkstraDist.clear();

    int startKey = m_startRow * m_cols + m_startCol;

    m_dijkstraDist[startKey]   = 0;
    m_dijkstraParent[startKey] = -1;

    // Push {cost, row, col}
    m_dijkstraQueue.push({0, m_startRow, m_startCol});
    m_dijkstraRunning = true;
}

void Grid::stepDijkstra()
{
    if (!m_dijkstraRunning || m_dijkstraQueue.empty())
    {
        m_dijkstraRunning = false;
        m_dijkstraDone    = true;
        return;
    }

    // Always get the CHEAPEST cell first
    auto [cost, row, col] = m_dijkstraQueue.top();
    m_dijkstraQueue.pop();

    int key = row * m_cols + col;

    // If we already found a cheaper path to this cell, skip it
    // This handles duplicate entries in the priority queue
    if (m_dijkstraDist.count(key) && cost > m_dijkstraDist[key])
        return;

    // Did we reach the end?
    if (row == m_endRow && col == m_endCol)
    {
        m_dijkstraRunning   = false;
        m_dijkstraDone      = true;
        m_dijkstraPathFound = true;
        reconstructDijkstraPath();
        return;
    }

    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = { 0, 0,-1, 1};

    for (int i = 0; i < 4; i++)
    {
        int nr = row + dr[i];
        int nc = col + dc[i];

        if (!inBounds(nr, nc)) continue;

        Cell& neighbor = m_cells[nr][nc];
        int   nKey     = nr * m_cols + nc;

        if (neighbor.state == CellState::WALL) continue;

        // Each cell costs 1 to enter (we'll add weights later)
        int newCost = cost + 1;

        // Only add to queue if we found a cheaper path
        if (!m_dijkstraDist.count(nKey) || newCost < m_dijkstraDist[nKey])
        {
            m_dijkstraDist[nKey]   = newCost;
            m_dijkstraParent[nKey] = key;
            m_dijkstraQueue.push({newCost, nr, nc});

            if (neighbor.state == CellState::EMPTY)
                neighbor.state = CellState::VISITED;
        }
    }
}

void Grid::reconstructDijkstraPath()
{
    int key = m_endRow * m_cols + m_endCol;

    while (key != -1)
    {
        int r = key / m_cols;
        int c = key % m_cols;

        if (m_cells[r][c].state == CellState::EMPTY ||
            m_cells[r][c].state == CellState::VISITED)
            m_cells[r][c].state = CellState::PATH;

        key = m_dijkstraParent.count(key) ? m_dijkstraParent[key] : -1;
    }
}


int Grid::heuristic(int row, int col) const
{
    // Manhattan distance to end node
    return std::abs(row - m_endRow) + std::abs(col - m_endCol);
}

void Grid::startAStar()
{
    clearSearch();

    m_astarRunning   = false;
    m_astarDone      = false;
    m_astarPathFound = false;

    m_astarQueue  = {};
    m_astarParent.clear();
    m_astarG.clear();

    int startKey = m_startRow * m_cols + m_startCol;

    m_astarG[startKey]      = 0;
    m_astarParent[startKey] = -1;

    // Priority = g + h (f score)
    int f = heuristic(m_startRow, m_startCol);
    m_astarQueue.push({f, m_startRow, m_startCol});
    m_astarRunning = true;
}

void Grid::stepAStar()
{
    if (!m_astarRunning || m_astarQueue.empty())
    {
        m_astarRunning = false;
        m_astarDone    = true;
        return;
    }

    auto [f, row, col] = m_astarQueue.top();
    m_astarQueue.pop();

    int key = row * m_cols + col;

    // Skip if we already found a cheaper path to this cell
    if (m_astarG.count(key) && f > m_astarG[key] + heuristic(row, col))
        return;

    // Reached the end?
    if (row == m_endRow && col == m_endCol)
    {
        m_astarRunning   = false;
        m_astarDone      = true;
        m_astarPathFound = true;
        reconstructAStarPath();
        return;
    }

    const int dr[] = {-1, 1, 0, 0};
    const int dc[] = { 0, 0,-1, 1};

    for (int i = 0; i < 4; i++)
    {
        int nr = row + dr[i];
        int nc = col + dc[i];

        if (!inBounds(nr, nc)) continue;

        Cell& neighbor = m_cells[nr][nc];
        int   nKey     = nr * m_cols + nc;

        if (neighbor.state == CellState::WALL) continue;

        int newG = m_astarG[key] + 1;  // cost to reach neighbor

        if (!m_astarG.count(nKey) || newG < m_astarG[nKey])
        {
            m_astarG[nKey]      = newG;
            m_astarParent[nKey] = key;

            // f = g + h — this is the ONLY line different from Dijkstra
            int newF = newG + heuristic(nr, nc);
            m_astarQueue.push({newF, nr, nc});

            if (neighbor.state == CellState::EMPTY)
                neighbor.state = CellState::VISITED;
        }
    }
}

void Grid::reconstructAStarPath()
{
    int key = m_endRow * m_cols + m_endCol;

    while (key != -1)
    {
        int r = key / m_cols;
        int c = key % m_cols;

        if (m_cells[r][c].state == CellState::EMPTY ||
            m_cells[r][c].state == CellState::VISITED)
            m_cells[r][c].state = CellState::PATH;

        key = m_astarParent.count(key) ? m_astarParent[key] : -1;
    }
}


void Grid::initMaze()
{
    // Fill everything with walls
    for (int r = 0; r < m_rows; r++)
        for (int c = 0; c < m_cols; c++)
        {
            Cell& cell = m_cells[r][c];
            if (cell.state != CellState::START &&
                cell.state != CellState::END)
                cell.state = CellState::WALL;
        }

    // Reset visited grid
    m_mazeVisited.assign(m_rows, std::vector<bool>(m_cols, false));
}

void Grid::generateMaze()
{
    initMaze();

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Maze works on odd cells only — start from (1,1)
    // This ensures walls between passages always exist
    int startR = 1;
    int startC = 1;

    m_mazeVisited[startR][startC] = true;
    m_cells[startR][startC].state = CellState::EMPTY;

    m_mazeStack.push({startR, startC});
    m_mazeRunning = true;
}

void Grid::stepMaze()
{
    if (!m_mazeRunning || m_mazeStack.empty())
    {
        m_mazeRunning = false;

        // Restore start and end on top of maze
        m_cells[m_startRow][m_startCol].state = CellState::START;
        m_cells[m_endRow][m_endCol].state     = CellState::END;
        return;
    }

    auto [row, col] = m_mazeStack.top();

    // Collect unvisited neighbors 2 steps away
    // (step of 2 so there's always a wall cell between passages)
    std::vector<std::pair<int,int>> neighbors;

    const int dr[] = {-2, 2,  0, 0};
    const int dc[] = { 0, 0, -2, 2};

    for (int i = 0; i < 4; i++)
    {
        int nr = row + dr[i];
        int nc = col + dc[i];

        if (!inBounds(nr, nc)) continue;
        if (m_mazeVisited[nr][nc])  continue;

        neighbors.push_back({nr, nc});
    }

    if (!neighbors.empty())
    {
        // Pick a random unvisited neighbor
        int idx = std::rand() % neighbors.size();
        auto [nr, nc] = neighbors[idx];

        // Carve passage — remove the wall between current and neighbor
        int wallR = (row + nr) / 2;
        int wallC = (col + nc) / 2;

        m_cells[wallR][wallC].state = CellState::EMPTY;
        m_cells[nr][nc].state       = CellState::EMPTY;

        m_mazeVisited[nr][nc] = true;
        m_mazeStack.push({nr, nc});
    }
    else
    {
        // Dead end — backtrack
        m_mazeStack.pop();
    }
}