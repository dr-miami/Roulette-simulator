#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <random>
#include <ctime>
#include <thread>
#include <chrono>

// Linux/POSIX non-blocking keyboard input
#if defined(__linux__) || defined(__APPLE__)
    #include <termios.h>
    #include <unistd.h>
    #include <sys/select.h>

    inline int _getch() {
        struct termios oldt, newt;
        int ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
#else
    #include <conio.h>
    #include <windows.h>
#endif

// ANSI Colors
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define BOLD    "\033[1m"

// Globals
inline int playerBalance = 1000;

// --- Bet Entry ---
struct BetEntry {
    int cursor;
    int amount;
    std::string label;
};

// --- TIMING UTILS ---
inline void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// --- Roulette Classes ---
class Pocket {
public:
    int number;
    Pocket(int n) : number(n) {}
    virtual std::string getColor() const = 0;
    bool isEven() const { return (number != 0 && number % 2 == 0); }
    bool isOdd() const { return (number != 0 && number % 2 != 0); }
    virtual ~Pocket() {}
};

class RedPocket : public Pocket {
public: using Pocket::Pocket; std::string getColor() const override { return "Red"; }
};

class BlackPocket : public Pocket {
public: using Pocket::Pocket; std::string getColor() const override { return "Black"; }
};

class GreenPocket : public Pocket {
public: using Pocket::Pocket; std::string getColor() const override { return "Green"; }
};

class RouletteWheel {
private:
    std::vector<std::unique_ptr<Pocket>> pockets;
    std::mt19937 rng;
public:
    RouletteWheel() : rng(static_cast<unsigned>(std::time(0))) {
        for (int i = 0; i <= 36; ++i) {
            if (i == 0) pockets.push_back(std::make_unique<GreenPocket>(i));
            else {
                bool isRed = ((i >= 1 && i <= 10) || (i >= 19 && i <= 28)) ? (i % 2 != 0) : (i % 2 == 0);
                if (isRed) pockets.push_back(std::make_unique<RedPocket>(i));
                else pockets.push_back(std::make_unique<BlackPocket>(i));
            }
        }
    }
    const Pocket& spin() {
        std::uniform_int_distribution<int> dist(0, 36);
        return *pockets[dist(rng)];
    }
};

// --- Cross-Platform UI Utilities ---
inline void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

inline void hideCursor() {
    std::cout << "\033[?25l";
}

inline void showCursor() {
    std::cout << "\033[?25h";
}

inline void printTitle() {
    std::cout << RED << BOLD << R"(
$$$$$$$\                      $$\            $$\     $$\               
$$  __$$\                     $$ |           $$ |    $$ |              
$$ |  $$ | $$$$$$\  $$\   $$\ $$ | $$$$$$\ $$$$$$\ $$$$$$\    $$$$$$\  
$$$$$$$  |$$  __$$\ $$ |  $$ |$$ |$$  __$$\\_$$  _|\_$$  _|  $$  __$$\ 
$$  __$$< $$ /  $$ |$$ |  $$ |$$ |$$$$$$$$ | $$ |    $$ |    $$$$$$$$ |
$$ |  $$ |$$ |  $$ |$$ |  $$ |$$ |$$   ____| $$ |$$\ $$ |$$\ $$   ____|
$$ |  $$ |\$$$$$$  |\$$$$$$  |$$ |\$$$$$$$\  \$$$$  |\$$$$  |\$$$$$$$\ 
\__|  \__| \______/  \______/ \__| \_______|  \____/  \____/  \_______|                                                                    
    )" << RESET << std::endl;
}

inline void Booting() {
    clearScreen();
    std::cout << CYAN << BOLD << "[SYSTEM]: INITIALIZING ROULETTE..." << RESET << "\n";
    sleepMs(700);
    clearScreen();
    std::cout << CYAN << BOLD << "[SYSTEM]: LOADING ASSETS..." << RESET << "\n";
    sleepMs(700);
    clearScreen();
    printTitle();
    sleepMs(1000);
}

inline void printRouletteTable(int selection = -1) {
    std::cout << "\n              " << (selection == 0 ? MAGENTA : GREEN) << "  [ 0 ]  " << RESET << "\n";

    for (int row = 3; row >= 1; row--) {
        std::cout << "    ";
        for (int col = 0; col < 12; col++) {
            int num = (col * 3) + row;
            bool isRed = ((num >= 1 && num <= 10) || (num >= 19 && num <= 28)) ? (num % 2 != 0) : (num % 2 == 0);
            
            if (num == selection) {
                std::cout << MAGENTA << "[" << (num < 10 ? "0" : "") << num << "]" << RESET;
            } else {
                std::cout << (isRed ? RED : BOLD) << "[" << (num < 10 ? "0" : "") << num << "]" << RESET;
            }
        }
        
        int colBetId = (row == 3) ? 38 : (row == 2 ? 39 : 40);
        std::cout << (selection == colBetId ? MAGENTA : GREEN) << " (2to1)" << RESET << "\n";
    }

    std::cout << "    " << (selection == 41 ? MAGENTA : GREEN) << "    [  1st 12  ]" << RESET 
              << (selection == 42 ? MAGENTA : GREEN) << "[  2nd 12  ]" << RESET 
              << (selection == 43 ? MAGENTA : GREEN) << "[  3rd 12  ]" << RESET << "\n";

    std::cout << "    " << (selection == 44 ? MAGENTA : GREEN) << "    [1-18]" << RESET 
              << (selection == 45 ? MAGENTA : GREEN) << "[Even]" << RESET 
              << (selection == 46 ? MAGENTA : RED)   << "[ RED ]" << RESET 
              << (selection == 47 ? MAGENTA : BOLD)  << "[BLACK]" << RESET 
              << (selection == 48 ? MAGENTA : GREEN) << "[ Odd ]" << RESET 
              << (selection == 49 ? MAGENTA : GREEN) << "[19-36]" << RESET << "\n\n";
}

inline void printBetQueue(const std::vector<BetEntry>& bets, int totalWagered) {
    if (bets.empty()) {
        std::cout << CYAN << "  Bet Queue: (empty)  " << RESET << "                              \n";
    } else {
        std::cout << CYAN << BOLD << "  Bet Queue:" << RESET << "                              \n";
        for (size_t i = 0; i < bets.size(); ++i) {
            std::cout << "    " << YELLOW << "[" << (i + 1) << "]" << RESET
                      << " $" << bets[i].amount
                      << " on " << BOLD << bets[i].label << RESET
                      << "                    \n";
        }
        std::cout << CYAN << "  Total wagered: $" << totalWagered << RESET << "                    \n";
    }
}

inline void rollAnimation(RouletteWheel& wheel) {
    std::cout << YELLOW << BOLD << "\nSPINNING THE WHEEL..." << RESET << "\n[ ";
    for (int i = 0; i < 20; i++) {
        const Pocket& temp = wheel.spin();
        std::string color = (temp.getColor() == "Red") ? RED : (temp.getColor() == "Black" ? BOLD : GREEN);
        std::cout << color << temp.number << RESET << " ";
        std::cout.flush();
        sleepMs(50 + (i * 10));
    }
    std::cout << "]\n";
}