#include <string>
#include <algorithm>
#include <chrono>
#include <ncurses.h>

class Timer {

public:
    bool started;
    std::string elapsed();
    Timer();
    void start(); // start resets the timer automatically
    void stop();
    void update();

private:
    std::chrono::steady_clock::time_point startTime;
    std::chrono::milliseconds time;
};

Timer::Timer() : started (false),
                 time (0) { }

void Timer::start() {
    // TODO: check if timer has already started
    startTime = std::chrono::steady_clock::now();
    started = true;
}

void Timer::stop() {
    update();
    started = false;
}

void Timer::update() {
    time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime);
}

std::string Timer::elapsed() {
    // TODO: does not work properly
    if (!started)
        return std::string("0:0.000");
    update();
    auto min = std::chrono::duration_cast<std::chrono::minutes>(time);
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(time - min);
    auto msec = (time - min) - sec;
    
    std::string result(std::to_string(min.count()) + ":" + 
                      std::to_string(sec.count()) + "." + 
                      std::to_string(msec.count()));
    return result;

}

void drawMain() {
    // set-up main window
    clear();
    const int titlePad = 4;
    box(stdscr,0,0);
    mvprintw(0, titlePad, " parity timer ");
    refresh();
}

void drawTimer(WINDOW * win, Timer & timer) {
    // TODO: do not need to calculate these every draw

    wclear(win);
    int timerRow, timerCol;
    getmaxyx(win, timerRow, timerCol);
    box(win, 0, 0);

    std::string startMsg(timer.elapsed());
    // wattron(win, A_STANDOUT | A_BOLD);
    mvwprintw( win, 
               timerRow / 2,
              (timerCol - startMsg.length()) / 2,
               startMsg.c_str());
    // wattroff(win, A_STANDOUT | A_BOLD);

    wrefresh(win);
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
    // timer.started = false;

    drawMain();
    // refresh();
    WINDOW * timerWindow = initTimer();
    drawTimer(timerWindow, timer);
    // wrefresh(timerWindow);

    // int row, col;
    // getmaxyx(stdscr, row, col);

    int keyPressed;

    while ((keyPressed = wgetch(timerWindow)) != 'q') {
        timer.update();
        drawTimer(timerWindow, timer);
        // wrefresh(timerWindow);
        if (keyPressed == KEY_RESIZE) {
            // clear();
            timerWindow = initTimer();
            drawMain();
            drawTimer(timerWindow, timer);
            // refresh();
            // getmaxyx(stdscr, row, col);
        }

        else if (keyPressed == ' ') {
            // wclear(timerWindow);
            if (timer.started)
                // timer.started = false;
                timer.stop();
            else
                // timer.started = true;
                timer.start();
            drawTimer(timerWindow, timer);
            // wrefresh(timerWindow);
            // attron(A_BOLD);
            // mvprintw(row/2, col/2 - 3, "START!");
            // attroff(A_BOLD);
        }

    }

    endwin();

    return 0;
}
