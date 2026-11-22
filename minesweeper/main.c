#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 9
#define COLS 9
#define MINES 10

#define RESET "\033[0m"
#define BOLD "\033[1m"
#define DIM "\033[2m"

#define TITLE_COLOR "\033[1;38;5;220m"
#define HUD_COLOR "\033[38;5;252m"
#define PROMPT_COLOR "\033[1;38;5;214m"
#define ERROR_COLOR "\033[1;38;5;196m"
#define SUCCESS_COLOR "\033[1;38;5;46m"
#define FLAG_COLOR "\033[1;38;5;226m"
#define MINE_COLOR "\033[1;38;5;196m"

#define NUM1 "\033[1;38;5;39m"
#define NUM2 "\033[1;38;5;46m"
#define NUM3 "\033[1;38;5;226m"
#define NUM4 "\033[1;38;5;201m"
#define NUM5 "\033[1;38;5;202m"
#define NUM6 "\033[1;38;5;51m"
#define NUM7 "\033[1;38;5;231m"
#define NUM8 "\033[1;38;5;245m"

typedef struct {
  bool mine;
  bool flagged;
  bool revealed;
  int adjacent;
} Tiles;

typedef struct {
  int r;
  int c;
} Pos;

Tiles board[ROWS][COLS];

void initBoard(void) {
  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      board[r][c].mine = false;
      board[r][c].flagged = false;
      board[r][c].revealed = false;
      board[r][c].adjacent = 0;
    }
  }
}
void clearScreen(void) {
  // \033 means ESC in ansi
  // [2J & [2H => clear the screen and move it to the top
  printf("\033[2J\033[H");
  fflush(stdout);
}

bool validCell(int r, int c) {
  return r >= 0 && r < ROWS && c >= 0 && c < COLS;
}

int countAdjacentMines(int r, int c) {
  int count = 0;

  for (int dr = -1; dr <= 1; dr++) {
    for (int dc = -1; dc <= 1; dc++) {
      // this means we're on the same (r,c) from which we're checking the
      // adjacent mines to.
      if (dr == 0 && dc == 0) {
        continue;
      }

      int nr = r + dr;
      int nc = c + dc;

      if (validCell(nr, nc) && board[nr][nc].mine) {
        count++;
      }
    }
  }
  return count;
}

void calculatedAdjacents(void) {
  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      if (board[r][c].mine) {
        board[r][c].adjacent = 0;
      } else {
        board[r][c].adjacent = countAdjacentMines(r, c);
      }
    }
  }
}

void placeMines(int safeR, int safeC) {
  int placed = 0;

  while (placed < MINES) {
    int r = rand() % ROWS;
    int c = rand() % COLS;

    if (board[r][c].mine) {
      continue;
    }

    // for first click, we should not place any mine in any of the adjacent
    // cells of the clicked cell.
    if (abs(r - safeR) <= 1 && abs(c - safeC) <= 1) {
      continue;
    }

    board[r][c].mine = true;
    placed++;
  }

  calculatedAdjacents();
}

void revealCells(int r, int c) {
  if (!validCell(r, c)) {
    return;
  }
  Tiles *start = &board[r][c];

  if (start->flagged || start->revealed || start->mine) {
    return;
  }

  Pos stack[ROWS * COLS];
  int top = 0;

  start->revealed = true;
  stack[top].r = r;
  stack[top].c = c;
  top++;

  while (top) {
    Pos p = stack[--top];
    Tiles *t = &board[p.r][p.c];

    if (t->adjacent != 0) {
      continue;
    }
    for (int dr = -1; dr <= 1; dr++) {
      for (int dc = -1; dc <= 1; dc++) {
        if (dr == 0 && dc == 0) {
          continue;
        }

        int nr = p.r + dr;
        int nc = p.c + dc;

        if (!validCell(nr, nc)) {
          continue;
        }

        Tiles *nt = &board[nr][nc];
        if (nt->flagged || nt->revealed || nt->mine) {
          continue;
        }

        nt->revealed = true;

        if (top < ROWS * COLS) {
          stack[top].r = nr;
          stack[top].c = nc;
          top++;
        }
      }
    }
  }
}

int countFlagged(void) {
  int flagged = 0;
  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      if (board[r][c].flagged) {
        flagged++;
      }
    }
  }
  return flagged;
}

int countRevealedSafeCell(void) {
  int safeCell = 0;

  for (int r = 0; r < ROWS; r++) {
    for (int c = 0; c < COLS; c++) {
      if (!board[r][c].mine && board[r][c].revealed) {
        safeCell++;
      }
    }
  }
  return safeCell;
}

bool checkWin(void) { return countRevealedSafeCell() == ROWS * COLS - MINES; }

const char *numberColor(int n) {
  switch (n) {
  case 1:
    return NUM1;
  case 2:
    return NUM2;
  case 3:
    return NUM3;
  case 4:
    return NUM4;
  case 5:
    return NUM5;
  case 6:
    return NUM6;
  case 7:
    return NUM7;
  default:
    return NUM8;
  }
}

void printHeader(void) {
  printf("\n");
  printf(TITLE_COLOR "  ====================================\n" RESET);
  printf(TITLE_COLOR "        " MINE_COLOR " M I N E S W E E P E R" TITLE_COLOR
                     "\n" RESET);
  printf(TITLE_COLOR "  ====================================\n\n" RESET);
}

void printTile(int r, int c, bool revealAll) {
  Tiles *t = &board[r][c];

  if (revealAll && t->mine) {
    printf(" " MINE_COLOR "*" RESET);
    return;
  }

  if (!t->revealed) {
    if (t->flagged) {
      printf(" " FLAG_COLOR "F" RESET);
    } else {
      printf(" " DIM "." RESET);
    }
    return;
  }

  if (t->mine) {
    printf(" " MINE_COLOR "*" RESET);
    return;
  }

  if (t->adjacent == 0) {
    printf("  ");
    return;
  }

  printf(" %s%d%s", numberColor(t->adjacent), t->adjacent, RESET);
}

void printBoard(bool revealAll) {
  printHeader();

  printf(HUD_COLOR "    " RESET);

  for (int c = 0; c < COLS; c++) {
    printf(HUD_COLOR "%d " RESET, c + 1);
  }

  printf("\n");

  for (int r = 0; r < ROWS; r++) {
    printf(HUD_COLOR " %2d" RESET, r + 1);

    for (int c = 0; c < COLS; c++) {
      printTile(r, c, revealAll);
    }
    printf("\n");
  }
  printf("\n");
}

void printStatus(void) {
  int flags = countFlagged();
  int revealed = countRevealedSafeCell();
  int minesLeft = MINES - flags;

  if (minesLeft < 0) {
    minesLeft = 0;
  }

  printf(HUD_COLOR " Mines Left: " RESET BOLD "%d" RESET HUD_COLOR
                   " Flags: " RESET BOLD "%d" RESET HUD_COLOR
                   " Safe revealed: " RESET BOLD "%d" RESET HUD_COLOR
                   "\n\n" RESET,
         minesLeft, flags, revealed);
}

void printHelp(void) {
  printf(HUD_COLOR " Reveal: " RESET BOLD "row col" RESET HUD_COLOR HUD_COLOR
                   " Flag: " RESET BOLD "f row col" RESET HUD_COLOR HUD_COLOR
                   " Quit: " RESET BOLD "q" RESET HUD_COLOR HUD_COLOR
                   "\n\n" RESET);
}

bool askPlayAgain(void) {
  char line[32];
  printf(SUCCESS_COLOR " Play again? (y/n): " RESET);
  fflush(stdout);

  if (fgets(line, sizeof(line), stdin) == NULL) {
    return false;
  }

  return line[0] == 'y' || line[0] == 'Y';
}

int main(int argc, char **argv) {
  srand((unsigned int)time(NULL));

  char input[64];
  bool play = true;

  clearScreen();

  printf(TITLE_COLOR " Presss Enter to start..." RESET);
  fflush(stdout);

  if (fgets(input, sizeof(input), stdin) == NULL) {
    return 0;
  }

  while (play) {
    initBoard();

    bool minesPlaced = false;
    bool gameOver = false;
    bool won = false;
    char message[128] = "";

    while (!gameOver) {
      clearScreen();
      printBoard(false);

      if (message[0] != '\0') {
        printf(ERROR_COLOR " %s\n\n" RESET, message);
      }

      printStatus();
      printHelp();

      printf(PROMPT_COLOR "  > " RESET);
      fflush(stdout);

      if (fgets(input, sizeof(input), stdin) == NULL) {
        return 0;
      }

      char *p = input;
      while (*p == ' ' || *p == '\t') {
        p++;
      }

      if (*p == 'q' || *p == 'Q') {
        clearScreen();
        printf(TITLE_COLOR "  Thanks for playing Minesweeper!\n\n" RESET);
        return 0;
      }

      int r = 0, c = 0;
      if (*p == 'f' || *p == 'F') {
        if (sscanf(p + 1, "%d %d", &r, &c) == 2) {
          r--, c--;

          if (!validCell(r, c)) {
            snprintf(message, sizeof(message),
                     "Row and column must be between 1 and %d", ROWS);
          } else if (board[r][c].revealed) {
            snprintf(message, sizeof(message),
                     "That cell is already revealed.");
          } else {
            board[r][c].flagged = !board[r][c].flagged;
            message[0] = '\0';
          }
        } else {
          snprintf(message, sizeof(message), "Flag format: f row col");
        }
      } else {
        if (sscanf(p, "%d %d", &r, &c) == 2) {
          r--, c--;

          if (!validCell(r, c)) {
            snprintf(message, sizeof(message),
                     "Row and column must be between 1 and %d", ROWS);
          } else if (board[r][c].flagged) {
            snprintf(message, sizeof(message),
                     "That cell is flagged. Use f %d %d to unflag it first",
                     r + 1, c + 1);
          } else {
            message[0] = '\0';

            if (!minesPlaced) {
              placeMines(r, c);
              minesPlaced = true;
            }

            revealCells(r, c);

            if (board[r][c].mine) {
              gameOver = true;
              won = false;
            } else if (checkWin()) {
              gameOver = true;
              won = true;
            }
          }
        } else {
          snprintf(message, sizeof(message), "Use: row col, f row col, or q");
        }
      }
    }
    clearScreen();
    printBoard(true);

    if (won) {
      printf(SUCCESS_COLOR " You cleared the board!\n\n" RESET);
    } else {
      printf(ERROR_COLOR " BOOM! You hit a mine.\n\n" RESET);
    }

    if (!askPlayAgain()) {
      play = false;
    }

    clearScreen();
  }

  printf(TITLE_COLOR "  Thanks for playing Minesweeper!\n\n" RESET);
  return 0;
}
