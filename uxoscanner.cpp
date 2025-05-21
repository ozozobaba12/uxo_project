using namespace std;
#include <emscripten/bind.h>
#include <vector>
#include <random>

struct Cell {
    bool hasBomb    = false;
    bool revealed   = false;
    int  adjBombs   = 0;
};

static vector<vector<Cell>> grid;

void initGrid(int n, int m, int B) {
    grid.assign(n, vector<Cell>(m));
    mt19937 rng(random_device{}());
    uniform_int_distribution<int> distR(0, n - 1);
    uniform_int_distribution<int> distC(0, m - 1);

    int placed = 0;
    while (placed < B) {
        int r = distR(rng);
        int c = distC(rng);
        if (!grid[r][c].hasBomb) {
            grid[r][c].hasBomb = true;
            placed++;
        }
    }

    int directions[8][2] = {
        {-1,-1}, {-1,0}, {-1,1},
        { 0,-1},          { 0,1},
        { 1,-1}, { 1,0}, { 1,1}
    };

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (grid[i][j].hasBomb) continue;
            int count = 0;
            for (int d = 0; d < 8; ++d) {
                int ni = i + directions[d][0];
                int nj = j + directions[d][1];
                if (ni >= 0 && ni < n && nj >= 0 && nj < m && grid[ni][nj].hasBomb) {
                    count++;
                }
            }
            grid[i][j].adjBombs = count;
        }
    }
}

vector<vector<int>> getGridState() {
    int n = static_cast<int>(grid.size());
    int m = (n > 0 ? static_cast<int>(grid[0].size()) : 0);
    vector<vector<int>> state(n, vector<int>(m));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            Cell &cell = grid[i][j];
            if (!cell.revealed) {
                state[i][j] = (cell.hasBomb ? -1 : -2);
            } else {
                state[i][j] = (cell.hasBomb ? -1 : cell.adjBombs);
            }
        }
    }
    return state;
}

void revealCell(int r, int c) {
    int n = static_cast<int>(grid.size());
    if (n == 0) return;
    int m = static_cast<int>(grid[0].size());
    if (r < 0 || r >= n || c < 0 || c >= m) return;

    Cell &cell = grid[r][c];
    if (cell.revealed) return;
    cell.revealed = true;
    if (cell.hasBomb || cell.adjBombs != 0) return;

    int directions[8][2] = {
        {-1,-1}, {-1,0}, {-1,1},
        { 0,-1},          { 0,1},
        { 1,-1}, { 1,0}, { 1,1}
    };
    for (int d = 0; d < 8; ++d) {
        revealCell(r + directions[d][0], c + directions[d][1]);
    }
}

EMSCRIPTEN_BINDINGS(bombscanner) {
    emscripten::function("initGrid", &initGrid);
    emscripten::function("getGridState", &getGridState);
    emscripten::function("revealCell", &revealCell);
}
