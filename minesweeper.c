/*
 * Minesweeper by Jack Donofrio
 * usage: ./minesweeper 0|1|2
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <ncurses.h>

static void setup_ncurses();
static void parseArgs(const int argc, char* argv[], int* size);
static void dropMines(int** grid, int size);
// static void showMapDebug(int** grid, bool** revealed, int size);
static bool reveal(int** grid, int size, int row, int column, bool** revealed, int* numNonMines);
static void grid_to_curse(int** grid, bool** revealed, int size);
static void init_grid_curse(int size);
static void put(int row, int col, char c, int color);
static inline int min(int a, int b);
static inline int max(int a, int b);
static inline int getNumMines(int size);

static const int EASY_SIZE = 8;
static const int MID_SIZE = 15;
static const int HARD_SIZE = 22;

int main(const int argc, char* argv[])
{
  int size;
  parseArgs(argc, argv, &size);
  srand(getpid());

  int** grid = malloc(sizeof(int*) * size);
  bool** revealed = malloc(sizeof(bool*) * size);
  for (int i = 0; i < size; i++) {
    grid[i] = malloc(sizeof(int) * size);
    revealed[i] = malloc(sizeof(bool) * size);
    for (int j = 0; j < size; j++) {
      grid[i][j] = 0;
      revealed[i][j] = false;
    }
  }
  dropMines(grid, size);
  setup_ncurses();

  init_grid_curse(size);
  attron(COLOR_PAIR(2));
  mvprintw(size + 1, 0, "Minesweeper");
  mvprintw(size + 2, 0, "wasd to move, r: reveal tile, f: toggle flag, q: quit");
  attroff(COLOR_PAIR(2));

  int playerRow = 0;
  int playerCol = 0;
  move(playerRow, playerCol);
  int numNonMines = size * size - getNumMines(size);

  int revealResult = 0;  
  char c = '\0';
  while (c != 'q') {
    c = getch(); // cannot put assignment in loop condition, for game over to break properly
    switch (c) {
      case 'q':
        break;
      case 'a':
        playerCol = max(playerCol - 2, 0);
        break;
      case 'd':
        playerCol = min(playerCol + 2, 2 * (size - 1));
        break;
      case 'w':
        playerRow = max(playerRow - 1, 0);
        break;
      case 's':
        playerRow = min(playerRow + 1, size - 1);
        break;
      case 'r':
        revealResult = reveal(grid, size, playerRow, playerCol / 2, revealed, &numNonMines);
        grid_to_curse(grid, revealed, size);
        if (revealResult) { // if we stepped on a bomb
          attron(COLOR_PAIR(3));
          mvprintw(size + 3, 0, "You stepped on a mine! Game Over!\nPress q to quit.");
          attroff(COLOR_PAIR(3));
        } else {
          if (numNonMines == 0) {
            attron(COLOR_PAIR(2));
            mvprintw(size + 3, 0, "You won!\nPress q to quit!");
            attroff(COLOR_PAIR(2));
          }
        }
        refresh();
        if (revealResult || numNonMines == 0) {
          while ((c = getch()) != 'q')
            ;
        }
        break;
      case 'f':
        if (revealed[playerRow][playerCol / 2]) break;
        if ((mvinch(playerRow, playerCol) & A_CHARTEXT) == '?') {
          put(playerRow, playerCol, '#', '#');
        } else {
          put(playerRow, playerCol, '?', '?');
        }
        refresh();
        break;
    }
    move(playerRow, playerCol);
  }

  // showMapDebug(grid, revealed, size);

  for (int i = 0; i < size; i++) {
    free(grid[i]);
    free(revealed[i]);
  }
  free(grid);
  free(revealed);
  endwin();
}

static void parseArgs(const int argc, char* argv[], int* size)
{
  if (argc == 2) {
    switch(argv[1][0]) {
      case '0':
        *size = EASY_SIZE;
        break;
      case '1':
        *size = MID_SIZE;
        break;
      case '2':
        *size = HARD_SIZE;
        break;
      default:
        fprintf(stderr, "usage: minesweeper 0|1|2\n");
        exit(1);
    }
  } else {
    fprintf(stderr, "usage: minesweeper 0|1|2\n");
    exit(1);
  }
}

static void setup_ncurses()
{
  initscr();
  cbreak();
  noecho();
  start_color();
  // change white to gray
  init_color(COLOR_WHITE, 700, 700, 700);
  init_pair('#', COLOR_YELLOW, COLOR_BLACK);
  init_pair(0, COLOR_WHITE, COLOR_BLACK);
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_RED, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair('!', COLOR_BLACK, COLOR_RED);
  init_pair('?', COLOR_BLUE, COLOR_GREEN);
}

static void init_grid_curse(int size) {
  attron(COLOR_PAIR('#'));
  int i, j;
  for (i = 0; i < size; i++) {
    for (j = 0; j < size; j++) {
      mvaddch(i, 2 * j, '#');
    }
    mvaddch(i, 2 * j, '|');
  }
  for (j = 0; j < size * 2; j++) {
    mvaddch(i, j, '-');
  }
  mvaddch(i, j, '+');
  attroff(COLOR_PAIR('#'));
}

static void grid_to_curse(int** grid, bool** revealed, int size)
{
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (revealed[i][j]) {
        if (grid[i][j] == -1) {
          put(i, 2 * j, '!', '!');
        } else {
          int num = grid[i][j];
          switch (num) {
            case 0:
              put(i, 2 * j, ' ', 0);
              break;
            case 1:
              put(i, 2 * j, '1', 1);
              break;
            case 2:
              put(i, 2 * j, '2', 2);
              break;
            case 3:
              put(i, 2 * j, '3', 3);
              break;
            case 4:
              put(i, 2 * j, '4', 4);
              break;
            default:
              mvaddch(i, 2 * j, '0' + num);
              break;
          }
        }
      }
    }
  }
}


static void dropMines(int** grid, int size)
{
  if (grid == NULL) return;

  int mines = getNumMines(size);
  while (mines > 0) {
    int randRow = rand() % size;
    int randCol = rand() % size;
    if (grid[randRow][randCol] == -1) continue;

    grid[randRow][randCol] = -1;
    
    for (int row = max(randRow - 1, 0); row < min(randRow + 2, size); row++) {
      for (int col = max(randCol - 1, 0); col < min(randCol + 2, size); col++) {
        if (grid[row][col] != -1)
          grid[row][col]++;
      }
    }
    mines--;
  }
}

// return true if we reveal a mine
static bool 
reveal(int** grid, int size, int row, int col, bool** revealed, int* numNonMines)
{
  if (grid == NULL || revealed == NULL || row < 0 || row >= size || col < 0 || col >= size ||
    grid[row] == NULL || revealed[row] == NULL) {
    return false;
  }
  if (grid[row][col] == -1) {
    revealed[row][col] = true;
    return true; // game over
  }
  if (revealed[row][col]) {
    return false;
  }
  if (grid[row][col] != 0) {
    revealed[row][col] = true;
    (*numNonMines)--;
    return false;
  }
  revealed[row][col] = true;
  (*numNonMines)--;
  for (int r = max(row - 1, 0); r < min(row + 2, size); r++) {
    for (int c = max(col - 1, 0); c < min(col + 2, size); c++) {
      if (!revealed[r][c]) {
        reveal(grid, size, r, c, revealed, numNonMines);
      }
    }
  }
  return false;
}

static void put(int row, int col, char c, int color)
{
  attron(COLOR_PAIR(color));
  mvaddch(row, col, c);
  attroff(COLOR_PAIR(color));
}

static inline int min(int a, int b) 
{
  return a > b ? b : a;
}

static inline int max(int a, int b)
{
  return a > b ? a : b;
}

static inline int getNumMines(int size)
{
  if (size == EASY_SIZE) return 10;
  else if (size ==  MID_SIZE) return 40;
  else if (size == HARD_SIZE) return 99;
  return 0;
}

// static void showMapDebug(int** grid, bool** revealed, int size)
// {
//   for (int r = 0; r < size; r++) {
//     for (int c = 0; c < size; c++) {
//       if (revealed[r][c]) {
//         printf("#");
//       } else {
//         printf("%d", grid[r][c]);
//       }
//     }
//     putchar('\n');
//    }
// }
