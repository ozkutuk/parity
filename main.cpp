#include <string>
#include <algorithm>
#include <ncurses.h>

struct Timer {
    bool started;
    int time;
};

void drawMain() {
    // set-up main window
    const int titlePad = 4;
    box(stdscr,0,0);
    mvprintw(0, titlePad, " parity timer ");
}

void drawTimer(WINDOW * win, Timer & timer) {
    int timerRow, timerCol;
    getmaxyx(win, timerRow, timerCol);
    box(win, 0, 0);

    // TODO
    if (timer.started) {
        std::string startMsg("START!");
        attron(A_STANDOUT | A_BOLD);
        mvwprintw( win, 
                   timerRow / 2,
                  (timerCol - startMsg.length()) / 2,
                   startMsg.c_str());
        attroff(A_STANDOUT | A_BOLD);
    }
    else {
        std::string startMsg("Press SPACE to start");
        mvwprintw( win, 
                   timerRow / 2,
                  (timerCol - startMsg.length()) / 2,
                   startMsg.c_str());
    }
}

WINDOW * initTimer() {
    int row, col;
    getmaxyx(stdscr, row, col);

    int height, width, start_x, start_y;
    const int max_height = 18;
    const int max_width = 100;

    height = std::min(max_height, static_cast<int>(0.6 * row));
    width  = std::min(max_width,  static_cast<int>(0.8 * col));

    start_x = (row - height) / 2;
    start_y = (col - width)  / 2;
           
    WINDOW * timerWindow = newwin(height, width, start_x, start_y);
    return timerWindow;
}

int main() {

    // ncurses initialization
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    Timer timer;
    timer.started = false;

    drawMain();
    WINDOW * timerWindow = initTimer();
    drawTimer(timerWindow, timer);
    refresh();
    wrefresh(timerWindow);

    // int row, col;
    // getmaxyx(stdscr, row, col);

    int keyPressed;

    while ((keyPressed = getch()) != 'q') {
        if (keyPressed == KEY_RESIZE) {
            clear();
            timerWindow = initTimer();
            drawMain();
            drawTimer(timerWindow, timer);
            refresh();
            wrefresh(timerWindow);
            // getmaxyx(stdscr, row, col);
        }

        else if (keyPressed == ' ') {
            wclear(timerWindow);
            if (timer.started)
                timer.started = false;
            else
                timer.started = true;
            drawTimer(timerWindow, timer);
            wrefresh(timerWindow);
            // attron(A_BOLD);
            // mvprintw(row/2, col/2 - 3, "START!");
            // attroff(A_BOLD);
        }

    }

    endwin();

    return 0;
}
