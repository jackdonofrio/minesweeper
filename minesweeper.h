/*
Minesweeper! for terminal.

TODO - flesh out documentation 
*/

#define EASY_SIZE 8
#define MID_SIZE 15
#define HARD_SIZE 22
#define EXTREME_SIZE 40

static void setup_ncurses();
static void parseArgs(const int argc, char* argv[], int* size);
static void dropMines(int** grid, int size);
static void reset(int** grid, bool** revealed, int size);
// static void showMapDebug(int** grid, bool** revealed, int size);
static bool reveal(int** grid, int size, int row, int column, bool** revealed, int* numNonMines);
static void grid_to_curse(int** grid, bool** revealed, int size);
static void init_grid_curse(int size);
static void put(int row, int col, char c, int color);
static inline int min(int a, int b);
static inline int max(int a, int b);
static inline int getNumMines(int size);
static inline int gridToCurse_col(int column);
static inline int gridToCurse_row(int row); 
