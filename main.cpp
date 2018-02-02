#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>
#include <ncurses.h>

static constexpr auto SLEEP_DURATION = std::chrono::milliseconds(10);

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
    if (started)
        time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime);
}

std::string Timer::elapsed() {
    update();
    auto min = std::chrono::duration_cast<std::chrono::minutes>(time);
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(time - min);
    auto msec = (time - min) - sec;
    
    std::ostringstream resultStream;
    resultStream << std::setfill('0') << std::setw(2) << min.count() << ":"
                                      << std::setw(2) << sec.count() << "."
                                      << std::setw(3) << msec.count();

    return resultStream.str();
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
    nodelay(timerWindow, TRUE); // for the non-blocking wgetch
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

    drawMain();
    WINDOW * timerWindow = initTimer();
    drawTimer(timerWindow, timer);

    int keyPressed;
    bool running = true;

    while (running) {
        timer.update();
        drawTimer(timerWindow, timer);

        keyPressed = wgetch(timerWindow);

        switch (keyPressed) {
            case 'Q': // fall-through
            case 'q':
                running = false;
                break;

            case KEY_RESIZE:
                timerWindow = initTimer();
                drawMain();
                drawTimer(timerWindow, timer);
                break;

            case ' ':
                if (timer.started)
                    timer.stop();
                else
                    timer.start();
                break;
        }

            // drawTimer(timerWindow, timer);
            std::this_thread::sleep_for(SLEEP_DURATION);
    }

    endwin();

    return 0;
}
