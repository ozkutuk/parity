#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread> // sleep_for
#include <ncurses.h>

static constexpr auto SLEEP_DURATION = std::chrono::milliseconds(1);
static constexpr int TIMER_PRECISION = 2;

class Timer {

public:
    bool started;
    std::string elapsed();
    std::string avgOfFive();
    std::string totalAverage();
    // std::string avgOfTwelve();

    Timer();
    void start(); // start resets the timer automatically
    void stop();
    void update();

private:
    std::chrono::steady_clock::time_point startTime;
    std::chrono::milliseconds time;
    std::vector<std::chrono::milliseconds> times;

    std::string formatTime(std::chrono::milliseconds time);
    // std::chrono::milliseconds avgTimes(const std::vector<std::chrono::milliseconds> & timeVec);
    std::chrono::milliseconds avgTimes(const std::vector<std::chrono::milliseconds>::iterator & first,
                                              const std::vector<std::chrono::milliseconds>::iterator & last );
    std::chrono::milliseconds ao5();
    std::chrono::milliseconds avg();
    
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
    times.push_back(time);
}

void Timer::update() {
    if (started)
        time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime);
}

std::chrono::milliseconds Timer::avgTimes(const std::vector<std::chrono::milliseconds>::iterator & first,
                                          const std::vector<std::chrono::milliseconds>::iterator & last ) {
    std::chrono::milliseconds accumulated(0);
    std::size_t size;
    std::for_each(first, last, [&](std::chrono::milliseconds x){ accumulated += x; size++; });
    return accumulated / size;
}

std::chrono::milliseconds Timer::avg() {
    if(times.size() == 0)
        return std::chrono::milliseconds::zero(); // TODO: return N/A maybe?
    return avgTimes(times.begin(), times.end());
}

std::string Timer::totalAverage() {
    return formatTime(avg());
}


std::chrono::milliseconds Timer::ao5() {
    // TODO: handle when times.size() < 5
    return avgTimes(times.begin()+(times.size()-5), times.end());
}

std::string Timer::formatTime(std::chrono::milliseconds time) {
    auto min = std::chrono::duration_cast<std::chrono::minutes>(time);
    auto sec = std::chrono::duration_cast<std::chrono::seconds>(time - min);
    auto msec = (time - min) - sec;

    /* 3 -> 1
     * 2 -> 10
     * 1 -> 100
     * 0 -> invalid
     * 10^(3 - TIMER_PRECISION)
     */
    const int precision = std::pow(10, 3-TIMER_PRECISION);
    
    std::ostringstream resultStream;
    resultStream << std::setfill('0') << std::setw(2) << min.count() << ":"
                                      << std::setw(2) << sec.count() << "."
                                      << std::setw(TIMER_PRECISION) << msec.count() / precision;

    return resultStream.str();
}

std::string Timer::elapsed() {
    // update();
    return formatTime(this -> time);
}

void drawMain() {
    // set-up main window
    clear();
    const int titlePad = 4;
    box(stdscr,0,0);
    mvprintw(0, titlePad, " parity timer ");
    refresh();
}

void drawCenteredString(WINDOW * win, const std::string & s) {
    int timerRow, timerCol;
    getmaxyx(win, timerRow, timerCol);
    box(win, 0, 0);

    mvwprintw( win, 
               timerRow / 2,
              (timerCol - s.length()) / 2,
               s.c_str());
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

WINDOW * initAvgs(WINDOW *timerWindow) {
    int height, width, start_x, start_y;
    getmaxyx(timerWindow, height, width);
    getbegyx(timerWindow, start_x, start_y);

    WINDOW * avgsWindow = newwin(3, width, start_x + height + 1, start_y);
    nodelay(avgsWindow, TRUE); // for the non-blocking wgetch
    return avgsWindow;
}

int main(void) {

    // ncurses initialization
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    Timer timer;

    drawMain();
    WINDOW * timerWindow = initTimer();
    drawCenteredString(timerWindow, timer.elapsed());

    WINDOW * avgsWindow = initAvgs(timerWindow);
    drawCenteredString(avgsWindow, timer.totalAverage());

    int keyPressed;
    bool running = true;

    while (running) {
        timer.update();
        drawCenteredString(timerWindow, timer.elapsed());
        drawCenteredString(avgsWindow, timer.totalAverage());

        keyPressed = wgetch(timerWindow);

        switch (keyPressed) {
            case 'Q': // fall-through
            case 'q':
                running = false;
                break;

            case KEY_RESIZE:
                timerWindow = initTimer();
                drawMain();
                drawCenteredString(timerWindow, timer.elapsed());
                break;

            case ' ':
                if (timer.started)
                    timer.stop();
                else
                    timer.start();
                break;
        }

        std::this_thread::sleep_for(SLEEP_DURATION);
    }

    endwin();

    return 0;
}
