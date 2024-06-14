#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 50
#define HEIGHT 50

// Functie care verifica daca exista 2048 pe tabla de joc
int check_end_game(int board[4][4]) {
  int line, col;
  for (line = 0; line < 4; line++) {
    for (col = 0; col < 4; col++) {
      if (board[line][col] == 2048) {
        return 1;
      }
    }
  }
  return 0;
}

// Functie pentru copiere unei matrice in alta
void matcpy(int board[4][4], int boardcpy[4][4]) {
  int line, col;
  for (line = 0; line < 4; line++) {
    for (col = 0; col < 4; col++) {
      boardcpy[line][col] = board[line][col];
    }
  }
}

// Functie care verifica daca o matrice este egala cu alta sau nu
bool matrix_compare(int board[4][4], int boardcpy[4][4]) {
  int line, col;
  for (line = 0; line < 4; line++) {
    for (col = 0; col < 4; col++)
      if (board[line][col] != boardcpy[line][col]) return false;
  }
  return true;
}

void add_lines_right(int board[4][4], int *score) {
  int line, col;
  for (line = 0; line < 4; line++) {
    for (col = 3; col > 0; col--) {
      if (board[line][col] == board[line][col - 1]) {
        board[line][col] = board[line][col] * 2;
        *score = *score + board[line][col];
        board[line][col - 1] = 0;
        col--;
      }
    }
  }
}

void add_lines_left(int board[4][4], int *score) {
  int line, col;
  for (line = 0; line < 4; line++) {
    for (col = 0; col < 3; col++) {
      if (board[line][col] == board[line][col + 1]) {
        board[line][col] = board[line][col] * 2;
        *score = *score + board[line][col];
        board[line][col + 1] = 0;
        col++;
      }
    }
  }
}

void add_columns_down(int board[4][4], int *score) {
  int line, col;
  for (col = 0; col < 4; col++) {
    for (line = 3; line > 0; line--)
      if (board[line][col] == board[line - 1][col]) {
        board[line - 1][col] = board[line - 1][col] * 2;
        *score = *score + board[line - 1][col];
        board[line][col] = 0;
        line--;
      }
  }
}

void add_columns_up(int board[4][4], int *score) {
  int line, col;
  for (col = 0; col < 4; col++) {
    for (line = 0; line < 3; line++)
      if (board[line][col] == board[line + 1][col]) {
        board[line][col] = board[line][col] * 2;
        *score = *score + board[line][col];
        board[line + 1][col] = 0;
        line++;
      }
  }
}

void random_value(int board[4][4]) {
  int line, col, ok_zeros = 0, aux;

  srand(time(NULL));
  for (line = 0; line < 4; line++)
    for (col = 0; col < 4; col++)
      if (board[line][col] == 0) ok_zeros = 1;

  if (ok_zeros == 0) return;

  line = rand() % 4;
  col = rand() % 4;

  // Ne asiguram ca nu punem valoarea intr o casuta deja ocupata
  while (board[line][col] != 0) {
    line = rand() % 4;
    col = rand() % 4;
  }
  aux = rand();

  // Ma folosesc de un numar random pentru a decide daca punem 2 sau 4
  if (aux % 2 == 0)
    board[line][col] = 2;
  else
    board[line][col] = 4;
}

void initialize_board(int board[4][4]) {
  int i = -1, j = 0, k = -1, l = 0, line, col;
  srand(time(NULL));
  for (line = 0; line < 4; line++)
    for (col = 0; col < 4; col++) board[line][col] = 0;

  // Ne asiguram ca nu punem 2 valori in aceeasi casuta
  while (i == k && j == l) {
    i = rand() % 4;
    j = rand() % 4;
    k = rand() % 4;
    l = rand() % 4;
  }
  for (line = 0; line < 4; line++) {
    for (col = 0; col < 4; col++) board[line][col] = 0;
  }
  board[i][j] = 2;
  board[k][l] = 4;
}

void print_corner(WINDOW *win, int board[4][4], int score) {
  struct tm *cur_time;
  time_t clk = time(NULL);
  cur_time = localtime(&clk);
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_BLACK);
  wattron(win, COLOR_PAIR(2));
  mvwprintw(win, 0, 0, "%02d:%02d:%02d", cur_time->tm_hour, cur_time->tm_min,
            cur_time->tm_sec);
  mvwprintw(win, 1, 0, "Current score: %d", score);
  mvwprintw(win, 2, 0, "Use ARROW KEYS to move the board");
  mvwprintw(win, 3, 0, "Press q to quit the game");
  mvwprintw(win, 4, 0, "Press / to undo the last move");
  wattroff(win, COLOR_PAIR(2));

  wrefresh(win);
}

// Functie care printeaza 2048 in ascii art
void print_2048(WINDOW *win) {
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  wattron(win, COLOR_PAIR(1));
  mvwprintw(win, 0, 0, "#####    ###   #        #####");
  mvwprintw(win, 1, 0, "#     #  #   #  #    #  #     #");
  mvwprintw(win, 2, 0, "      # #     # #    #  #     #");
  mvwprintw(win, 3, 0, " #####  #     # #    #   #####");
  mvwprintw(win, 4, 0, "#       #     # ####### #     #");
  mvwprintw(win, 5, 0, "#        #   #       #  #     #");
  mvwprintw(win, 6, 0, "#######   ###        #   #####");
  wattroff(win, COLOR_PAIR(1));
  wrefresh(win);
}

// Functie care printeaza tabla de joc cu tot cu chenar
void print_board(int board[4][4], WINDOW *board_win) {
  int i, j;
  start_color();

  init_pair(2, COLOR_RED, COLOR_BLACK);
  init_pair(4, COLOR_GREEN, COLOR_BLACK);
  init_pair(8, COLOR_YELLOW, COLOR_BLACK);
  init_pair(16, COLOR_BLUE, COLOR_BLACK);
  init_pair(32, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(64, COLOR_CYAN, COLOR_BLACK);
  init_pair(128, COLOR_RED, COLOR_BLACK);
  init_pair(256, COLOR_GREEN, COLOR_BLACK);
  init_pair(512, COLOR_YELLOW, COLOR_BLACK);
  init_pair(1024, COLOR_BLUE, COLOR_BLACK);
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      if (j % 2 == 0) {
        mvwprintw(board_win, i * 3, j * 5, "+----+");
      } else {
        mvwprintw(board_win, i * 3, j * 5, "+----+");
      }
    }
    mvwprintw(board_win, i * 3 + 1, 0, "\n");
    for (j = 0; j < 4; j++) {
      mvwprintw(board_win, i * 3 + 1, j * 5, "|");
      if (board[i][j] != 0) {
        wattron(board_win, COLOR_PAIR(board[i][j]));
        mvwprintw(board_win, i * 3 + 1, j * 5 + 1, "%d", board[i][j]);
        wattroff(board_win, COLOR_PAIR(board[i][j]));
      } else
        mvwprintw(board_win, i * 3 + 1, j * 5 + 1, "    ");
      mvwprintw(board_win, i * 3 + 1, j * 5 + 5, "|");
    }
    mvwprintw(board_win, i * 3 + 2, 0, "\n");
    for (j = 0; j < 4; j++) {
      mvwprintw(board_win, i * 3 + 2, j * 5, "+----+");
    }
    mvwprintw(board_win, i * 3 + 3, 0, "\n");
  }
  wrefresh(board_win);
}

void shift_down(int board[4][4]) {
  int line, col;
  int occupied_pos, empty_pos;
  for (col = 0; col < 4; col++) {
    empty_pos = 3;
    for (line = 3; line >= 0; line--) {
      while (board[empty_pos][col] != 0 && empty_pos >= 0) {
        empty_pos--;
      }
      occupied_pos = empty_pos - 1;
      while (board[occupied_pos][col] == 0 && occupied_pos >= 0) {
        occupied_pos--;
      }
      if (occupied_pos >= 0) {
        board[empty_pos][col] = board[occupied_pos][col];
        board[occupied_pos][col] = 0;
      }
    }
  }
}

void shift_up(int board[4][4]) {
  int line, col;
  int occupied_pos, empty_pos;
  for (col = 0; col < 4; col++) {
    empty_pos = 0;
    for (line = 0; line < 4; line++) {
      while (board[empty_pos][col] != 0 && empty_pos < 4) {
        empty_pos++;
      }
      occupied_pos = empty_pos + 1;
      while (board[occupied_pos][col] == 0 && occupied_pos < 4) {
        occupied_pos++;
      }
      if (occupied_pos < 4) {
        board[empty_pos][col] = board[occupied_pos][col];
        board[occupied_pos][col] = 0;
      }
    }
  }
}

void shift_left(int board[4][4]) {
  int line, col;
  int occupied_pos, empty_pos;
  for (line = 0; line < 4; line++) {
    empty_pos = 0;
    for (col = 0; col < 4; col++) {
      while (board[line][empty_pos] != 0 && empty_pos < 4) {
        empty_pos++;
      }
      occupied_pos = empty_pos + 1;
      while (board[line][occupied_pos] == 0 && occupied_pos < 4) {
        occupied_pos++;
      }
      if (occupied_pos < 4) {
        board[line][empty_pos] = board[line][occupied_pos];
        board[line][occupied_pos] = 0;
      }
    }
  }
}

void shift_right(int board[4][4]) {
  int line, col;
  int occupied_pos, empty_pos;
  for (line = 0; line < 4; line++) {
    empty_pos = 3;
    for (col = 3; col >= 0; col--) {
      while (board[line][empty_pos] != 0 && empty_pos >= 0) {
        empty_pos--;
      }
      occupied_pos = empty_pos - 1;
      while (board[line][occupied_pos] == 0 && occupied_pos >= 0) {
        occupied_pos--;
      }
      if (occupied_pos >= 0) {
        board[line][empty_pos] = board[line][occupied_pos];
        board[line][occupied_pos] = 0;
      }
    }
  }
}

void move_board(int board[4][4], int direction, int *score) {
  int boardcpy[4][4], line, col;
  matcpy(board, boardcpy);

  // Pentru fiecare directie, mai intai le shiftam, apoi le adunam iar apoi le
  // shiftam din nou pentru a ne asigura ca nu raman spatii goale intre numere
  if (direction == KEY_RIGHT) {
    shift_right(board);
    add_lines_right(board, score);
    shift_right(board);
  }
  if (direction == KEY_LEFT) {
    shift_left(board);
    add_lines_left(board, score);
    shift_left(board);
  }
  if (direction == KEY_UP) {
    shift_up(board);
    add_columns_up(board, score);
    shift_up(board);
  }
  if (direction == KEY_DOWN) {
    shift_down(board);
    add_columns_down(board, score);
    shift_down(board);
  }

  // Vom pune o valoare random DOAR daca s-a facut o mutare valida, cand boardul
  // mutat e diferit de cel initial
  for (line = 0; line < 4; line++) {
    for (col = 0; col < 4; col++) {
      if (board[line][col] != boardcpy[line][col]) {
        random_value(board);
        return;
      }
    }
  }
}

// Functie care returneaza maximul dintre 4 numere (folosita la best_move)
int max_number(int right_boxes, int left_boxes, int up_boxes, int down_boxes) {
  int max = -1;
  if (right_boxes > max) max = right_boxes;
  if (left_boxes > max) max = left_boxes;
  if (up_boxes > max) max = up_boxes;
  if (down_boxes > max) max = down_boxes;
  return max;
}
// Functie care verifica cate casute libere sunt pe tabla de joc
int check_boxes(int board[4][4]) {
  int line, col, empty_boxes = 0;
  for (line = 0; line < 4; line++)
    for (col = 0; col < 4; col++)
      if (board[line][col] == 0) empty_boxes++;
  return empty_boxes;
}

// Functie care verifica care este cea mai buna mutare
int best_move(int board[4][4]) {
  int right_boxes = 0, left_boxes = 0, up_boxes = 0, down_boxes = 0;
  int boardcpy[4][4];
  int dummy_score = 0, max = -1;
  // Booleane pentru a verifica daca o mutare este posibila sau nu
  bool changed_right = false, changed_left = false, changed_up = false,
       changed_down = false;

  // Luam pe rand fiecare caz de miscare si vedem cate casute sunt eliberate
  matcpy(board, boardcpy);
  move_board(boardcpy, KEY_RIGHT, &dummy_score);
  right_boxes = check_boxes(boardcpy);
  changed_right = matrix_compare(board, boardcpy);
  matcpy(board, boardcpy);
  move_board(boardcpy, KEY_LEFT, &dummy_score);
  left_boxes = check_boxes(boardcpy);
  changed_left = matrix_compare(board, boardcpy);
  matcpy(board, boardcpy);
  move_board(boardcpy, KEY_UP, &dummy_score);
  up_boxes = check_boxes(boardcpy);
  changed_up = matrix_compare(board, boardcpy);
  matcpy(board, boardcpy);
  move_board(boardcpy, KEY_DOWN, &dummy_score);
  down_boxes = check_boxes(boardcpy);
  changed_down = matrix_compare(board, boardcpy);

  // Verificam care mutare a fost valida
  if (changed_right == true) right_boxes = 0;
  if (changed_left == true) left_boxes = 0;
  if (changed_up == true) up_boxes = 0;
  if (changed_down == true) down_boxes = 0;

  // Aflam care e mutarea cu cele mai multe casute libere
  max = max_number(right_boxes, left_boxes, up_boxes, down_boxes);

  // Returnam arrowkeyul care ne duce la mutarea cu cele mai multe casute libere
  if (max == right_boxes && !changed_right) {
    return KEY_RIGHT;
  } else if (max == left_boxes && !changed_left) {
    return KEY_LEFT;
  } else if (max == up_boxes && !changed_up) {
    return KEY_UP;
  } else if (max == down_boxes && !changed_down) {
    return KEY_DOWN;
  }
  return 0;
}

void start_game(int board[4][4], bool resume, int *score) {
  int perfect_move, boardcpy[4][4], undo_score = 0;

  keypad(stdscr, true);
  curs_set(0);
  noecho();
  //Cat dureaza pana la mutare automata
  timeout(10000);
  int command;
  if (resume == false) initialize_board(board);
  matcpy(board, boardcpy);

  // Windowuri pentru tabla de joc si scor/panou cu legenda
  WINDOW *board_win = newwin(21, 21, 0, 0);
  WINDOW *score_win = newwin(30, 35, 0, 25);
  while (1) {
    print_board(board, board_win);
    wrefresh(board_win);
    print_corner(score_win, board, *score);
    wrefresh(score_win);
    perfect_move = best_move(board);

    command = getch();

    // Daca a trecut destul timp fara sa fie apasata o tasta, programul face
    //  cea mai buna mutare
    if (command == ERR) {
      perfect_move = best_move(board);
      if (perfect_move == 0) {
        delwin(board_win);
        return;
      } else {
        matcpy(board, boardcpy);
        undo_score = *score;
        move_board(board, perfect_move, score);
      }
      // Daca a fost apasat q, se iese din joc
    } else if (command == 113) {
      delwin(board_win);
      clear();
      refresh();
      return;
      // Verificam daca a fost apasata o tasta valida
    } else if (command == KEY_UP || command == KEY_DOWN ||
               command == KEY_LEFT || command == KEY_RIGHT) {
      matcpy(board, boardcpy);
      undo_score = *score;
      move_board(board, command, score);
      // Daca a fost apasat / se revine la starea de acum o mutare
    } else if (command == 47) {
      matcpy(boardcpy, board);
      *score = undo_score;
      wrefresh(score_win);
      wrefresh(board_win);
    }

    // Verificam daca s-a ajuns la 2048
    if (check_end_game(board) == 1) {
      attron(COLOR_PAIR(3));

      wclear(score_win);
      wrefresh(score_win);
      mvprintw(3, 50, "YOU WON!!!!");
      mvprintw(4, 50, "Press any key to continue");
      attroff(COLOR_PAIR(3));
      getch();
      delwin(board_win);
      return;
    }

    // Verificam daca ai pierdut, cand nu mai poti face nici o mutare
    if (perfect_move == 0) {
      init_pair(3, COLOR_RED, COLOR_BLACK);
      attron(COLOR_PAIR(3));
      wclear(score_win);
      wrefresh(score_win);
      mvprintw(3, 50, "GAME OVER");
      mvprintw(4, 50, "Press any key to continue");
      attroff(COLOR_PAIR(3));
      getch();
      refresh();
      delwin(board_win);
      return;
    }
  }
  clear();
}

void initialize_menu() {
  refresh();
  noecho();
  curs_set(0);

  int board[4][4];
  bool has_started = false;
  // Facem 2 ferestrea , una meniul propriu-zis iar a doua fereastra pentru 2048
  // ul mare
  WINDOW *menuwin = newwin(6, 12, 10, 100);
  WINDOW *win_2048 = newwin(8, 40, 0, 100);
  box(menuwin, 0, 0);

  wrefresh(menuwin);

  keypad(menuwin, true);

  char *choices[3] = {"New Game", "Load Game", "Exit"};
  int choice;
  int highlight = 0;
  int score = 0;

  while (1) {
    int i;
    print_2048(win_2048);
    wrefresh(win_2048);

    // Verificam daca este highlight pe o anumita alegere
    for (i = 0; i < 3; i++) {
      if (i == highlight) wattron(menuwin, A_REVERSE);
      mvwprintw(menuwin, i + 1, 1, "%s", choices[i]);
      wattroff(menuwin, A_REVERSE);
    }
    choice = wgetch(menuwin);

    // Verificam daca a fost apasata o tasta valida, caz in care se schimba
    // highlightul
    switch (choice) {
      case KEY_UP:
        highlight--;
        if (highlight == -1) highlight = 0;
        break;
      case KEY_DOWN:
        if (highlight == 2) highlight = 1;
        highlight++;
        break;
      default:
        break;
    }
    // Daca a fost apasat enter pe o anumita alegere, se executa functia
    // corespunzatoare

    // Functia de load game
    if (choice == 10 && highlight == 0) {
      has_started = true;
      score = 0;
      wclear(menuwin);
      wrefresh(menuwin);
      start_game(board, false, &score);
      clear();
      refresh();
    }

    // Functia de start game
    if (choice == 10 && highlight == 1 && has_started == true) {
      wclear(menuwin);
      wrefresh(menuwin);
      start_game(board, true, &score);
      clear();
      refresh();
    }
    if (choice == 10 && highlight == 2) break;
    if (choice == 113) break;
  }
  delwin(menuwin);
}

int main(void) {
  initscr();
  initialize_menu();
  endwin();
  return 0;
}