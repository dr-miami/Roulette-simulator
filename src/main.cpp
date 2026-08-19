#include "functions.hpp"

#define MA_ENABLE_PULSEAUDIO
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

std::string getSelectionText(int id) {
    if (id == 0) return "0 (Green)";
    if (id >= 1 && id <= 36) return "Number " + std::to_string(id);
    if (id == 38) return "Top Row (3,6,9...36)";
    if (id == 39) return "Middle Row (2,5,8...35)";
    if (id == 40) return "Bottom Row (1,4,7...34)";
    if (id == 41) return "1st 12";
    if (id == 42) return "2nd 12";
    if (id == 43) return "3rd 12";
    if (id >= 44 && id <= 49) {
        std::string names[] = {"1-18", "Even", "RED", "BLACK", "Odd", "19-36"};
        return names[id - 44];
    }
    return "None";
}

int evaluateBet(const BetEntry& b, const Pocket& res) {
    bool won = false;
    int mult = 0;
    int cur = b.cursor;

    if (cur <= 36) { if (res.number == cur) { won = true; mult = 36; } }
    else if (cur == 38 && res.number != 0 && res.number % 3 == 0) { won = true; mult = 3; }
    else if (cur == 39 && res.number != 0 && res.number % 3 == 2) { won = true; mult = 3; }
    else if (cur == 40 && res.number != 0 && res.number % 3 == 1) { won = true; mult = 3; }
    else if (cur == 41 && res.number >= 1  && res.number <= 12)   { won = true; mult = 3; }
    else if (cur == 42 && res.number >= 13 && res.number <= 24)   { won = true; mult = 3; }
    else if (cur == 43 && res.number >= 25 && res.number <= 36)   { won = true; mult = 3; }
    else if (cur == 44 && res.number >= 1  && res.number <= 18)   { won = true; mult = 2; }
    else if (cur == 49 && res.number >= 19 && res.number <= 36)   { won = true; mult = 2; }
    else if (cur == 46 && res.getColor() == "Red")   { won = true; mult = 2; }
    else if (cur == 47 && res.getColor() == "Black") { won = true; mult = 2; }
    else if (cur == 45 && res.isEven()) { won = true; mult = 2; }
    else if (cur == 48 && res.isOdd())  { won = true; mult = 2; }

    return won ? b.amount * mult : 0;
}

inline int gridCol(int num) { return (num - 1) / 3; }
inline int gridRow(int num) { return (num - 1) % 3 + 1; }
inline int gridNum(int col, int row) { return col * 3 + row; }

int moveCursorLeft(int cursor) {
    if (cursor >= 1 && cursor <= 36) {
        int col = gridCol(cursor);
        int row = gridRow(cursor);
        if (col == 0) return 0;
        return gridNum(col - 1, row);
    }
    if (cursor == 38) return gridNum(11, 3);
    if (cursor == 39) return gridNum(11, 2);
    if (cursor == 40) return gridNum(11, 1);
    if (cursor >= 41 && cursor <= 49) {
        return (cursor == 41) ? 40 : cursor - 1;
    }
    return cursor;
}

int moveCursorRight(int cursor) {
    if (cursor >= 1 && cursor <= 36) {
        int col = gridCol(cursor);
        int row = gridRow(cursor);
        if (col == 11) {
            if (row == 3) return 38;
            if (row == 2) return 39;
            return 40;
        }
        return gridNum(col + 1, row);
    }
    if (cursor == 38 || cursor == 39 || cursor == 40) return 41;
    if (cursor >= 41 && cursor <= 48) return cursor + 1;
    if (cursor == 49) return 0;
    if (cursor == 0) return gridNum(0, 2);
    return cursor;
}

int moveCursorUp(int cursor) {
    if (cursor >= 1 && cursor <= 36) {
        int col = gridCol(cursor);
        int row = gridRow(cursor);
        if (row == 3) return 0;
        return gridNum(col, row + 1);
    }
    if (cursor == 0) return cursor;
    if (cursor >= 38 && cursor <= 40) return 0;
    if (cursor >= 41 && cursor <= 43) {
        int col = (cursor == 41) ? 3 : (cursor == 42) ? 7 : 11;
        return gridNum(col, 1);
    }
    if (cursor >= 44 && cursor <= 49) return 41 + ((cursor - 44) / 2);
    return cursor;
}

int moveCursorDown(int cursor) {
    if (cursor >= 1 && cursor <= 36) {
        int col = gridCol(cursor);
        int row = gridRow(cursor);
        if (row == 1) {
            if (col < 4) return 41;
            if (col < 8) return 42;
            return 43;
        }
        return gridNum(col, row - 1);
    }
    if (cursor == 0) return gridNum(5, 3);
    if (cursor == 38) return 43;
    if (cursor == 39) return 42;
    if (cursor == 40) return 41;
    if (cursor == 41) return 44;
    if (cursor == 42) return 46;
    if (cursor == 43) return 48;
    if (cursor >= 44 && cursor <= 49) return cursor;
    return cursor;
}

int getKeyPress() {
    int key = _getch();
    if (key == 27) { 
        int next1 = _getch();
        if (next1 == '[') {
            int next2 = _getch();
            if (next2 == 'A') return 72; // UP
            if (next2 == 'B') return 80; // DOWN
            if (next2 == 'D') return 75; // LEFT
            if (next2 == 'C') return 77; // RIGHT
        }
    }
    else if (key == 224) {
        return _getch();
    }
    return key;
}

int main() {
    hideCursor();
    Booting();

    ma_engine engine;
    ma_sound bgm;
    bool audio = false;

    if (ma_engine_init(NULL, &engine) == MA_SUCCESS) {
        if (ma_sound_init_from_file(&engine, "sound.wav", MA_SOUND_FLAG_STREAM, NULL, NULL, &bgm) == MA_SUCCESS) {
            ma_sound_set_looping(&bgm, true);
            ma_sound_start(&bgm);
            audio = true;
        }
    }

    RouletteWheel wheel;
    bool keepPlaying = true;

    while (keepPlaying) {
        int cursor = 0;
        std::vector<BetEntry> betQueue;
        int totalWagered = 0;

        if (playerBalance <= 0) {
            playerBalance = 1000;
        }

        while (playerBalance > 0) {
            clearScreen();
            printTitle();
            printRouletteTable(cursor);

            std::cout << YELLOW << "BALANCE: $" << playerBalance << RESET
                      << " | " << MAGENTA << "WASD / ARROWS: move" << RESET
                      << " | " << GREEN   << "ENTER: queue bet" << RESET
                      << " | " << YELLOW  << "SPACE: spin all" << RESET
                      << " | " << RED     << "BKSP: undo last" << RESET
                      << " | " << BOLD    << "'X': cash out" << RESET
                      << "          \n";
            std::cout << "Selection: " << BOLD << getSelectionText(cursor) << RESET
                      << "                    \n";

            printBetQueue(betQueue, totalWagered);

            int key = getKeyPress();

            if (key == 72 || key == 'w' || key == 'W') cursor = moveCursorUp(cursor);
            else if (key == 80 || key == 's' || key == 'S') cursor = moveCursorDown(cursor);
            else if (key == 75 || key == 'a' || key == 'A') cursor = moveCursorLeft(cursor);
            else if (key == 77 || key == 'd' || key == 'D') cursor = moveCursorRight(cursor);
            else if (key == 'x' || key == 'X') {
                keepPlaying = false;
                break;
            }
            else if (key == 8 || key == 127) { // Backspace
                if (!betQueue.empty()) {
                    totalWagered -= betQueue.back().amount;
                    playerBalance += betQueue.back().amount;
                    betQueue.pop_back();
                }
                continue;
            }
            else if (key == 13 || key == 10) { // Enter
                if (cursor >= 1 && cursor <= 36) {
                    bool alreadyBet[37] = {};
                    for (const auto& b : betQueue)
                        if (b.cursor >= 1 && b.cursor <= 36)
                            alreadyBet[b.cursor] = true;

                    int uniqueNumberBets = 0;
                    for (int i = 1; i <= 36; ++i)
                        if (alreadyBet[i]) ++uniqueNumberBets;

                    if (!alreadyBet[cursor] && uniqueNumberBets >= 18) {
                        std::cout << RED << BOLD
                                  << "\n  Max 18 number bets reached! Remove a bet first."
                                  << RESET << "          \n";
                        sleepMs(1200);
                        continue;
                    }
                }

                showCursor();
                int availableBalance = playerBalance - totalWagered;
                std::cout << "\nBet amount for [" << getSelectionText(cursor)
                          << "] (available: $" << availableBalance << "): $";
                int bet = 0;
                if (!(std::cin >> bet)) {
                    std::cin.clear();
                    std::cin.ignore(10000, '\n');
                } else {
                    std::cin.ignore(10000, '\n');
                    if (bet > 0 && bet <= availableBalance) {
                        totalWagered += bet;
                        playerBalance -= bet;
                        betQueue.push_back({ cursor, bet, getSelectionText(cursor) });
                    }
                }

                hideCursor();
                continue;
            }
            else if (key == ' ') {
                if (betQueue.empty()) continue;

                if (audio) ma_engine_play_sound(&engine, "spin.wav", NULL);
                rollAnimation(wheel);
                const Pocket& res = wheel.spin();

                if (audio) ma_sound_stop(&bgm);

                int totalWon = 0;
                bool anyWin = false;

                std::cout << "\n" << BOLD << "Result: " << res.number << " " << res.getColor() << RESET << "\n";
                std::cout << "─────────────────────────────────────\n";

                for (const auto& b : betQueue) {
                    int winnings = evaluateBet(b, res);
                    if (winnings > 0) {
                        anyWin = true;
                        totalWon += winnings;
                        std::cout << GREEN << " WIN " << winnings
                                  << " on " << b.label << RESET << "\n";
                    } else {
                        std::cout << RED << " LOSS " << b.amount
                                  << " on " << b.label << RESET << "\n";
                    }
                }

                std::cout << "─────────────────────────────────────\n";
                playerBalance += totalWon;

                if (anyWin) {
                    std::cout << GREEN << BOLD << "Net result: +"
                              << (totalWon - totalWagered) << "  Balance: $"
                              << playerBalance << RESET << "\n";
                } else {
                    std::cout << RED << BOLD << "Net result: -" << totalWagered
                              << "  Balance: $" << playerBalance << RESET << "\n";
                }

                // Play win/loss sound directly using ma_engine
                if (audio) {
                    const char* soundFile = anyWin ? "win.wav" : "loss.wav";
                    ma_engine_play_sound(&engine, soundFile, NULL);
                    int delay = anyWin ? 3500 : 4000;
                    for (int elapsed = 0; elapsed < delay; elapsed += 100) {
                        sleepMs(100);
                    }
                    ma_sound_start(&bgm);
                }

                betQueue.clear();
                totalWagered = 0;
                std::cout << "\nPress ANY key to return to betting...";
                _getch();
                continue;
            }
        }

        if (playerBalance <= 0 && keepPlaying) {
            if (audio) { 
                ma_sound_stop(&bgm); 
                ma_engine_play_sound(&engine, "loss.wav", NULL);
                for (int elapsed = 0; elapsed < 4000; elapsed += 100) {
                    sleepMs(100);
                }
            }
            clearScreen();
            std::cout << RED << BOLD << "\nBANKRUPT! " << RESET << "You have run out of chips.\n";
            std::cout << "Press " << YELLOW << "'R'" << RESET << " to play again, or any other key to quit.\n";
            int restartKey = getKeyPress();
            if (restartKey != 'r' && restartKey != 'R') {
                keepPlaying = false;
            }
        }
    }

    showCursor();
    if (audio) { ma_sound_stop(&bgm); ma_engine_uninit(&engine); }
    return 0;
}