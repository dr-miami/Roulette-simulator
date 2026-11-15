#define MA_ENABLE_PULSEAUDIO
#define MA_ENABLE_ALSA
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "functions.hpp"
#include <limits>

int main()
{
    ma_engine engine;
    ma_sound bgSound, spinSound, winSound, lossSound;
    bool audioEnabled = false;

    if (ma_engine_init(NULL, &engine) == MA_SUCCESS) {
        bool bgmOk  = (ma_sound_init_from_file(&engine, "sound.wav", MA_SOUND_FLAG_STREAM, NULL, NULL, &bgSound) == MA_SUCCESS);
        bool spinOk = (ma_sound_init_from_file(&engine, "spin.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &spinSound) == MA_SUCCESS);
        bool winOk  = (ma_sound_init_from_file(&engine, "win.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &winSound) == MA_SUCCESS);
        bool lossOk = (ma_sound_init_from_file(&engine, "loss.wav", MA_SOUND_FLAG_DECODE, NULL, NULL, &lossSound) == MA_SUCCESS);

        if (bgmOk && spinOk && winOk && lossOk) {
            ma_sound_set_looping(&bgSound, true);
            ma_sound_start(&bgSound);
            audioEnabled = true;
        }
    }

    clearScreen();
    Title();

    int balance = 1000;
    std::cout << GREEN << "Balance: $" << balance << RESET << "\n\n";

    while (balance > 0)
    {
        int bet;
        while (true)
        {
            std::cout << GREEN << "How much do you want to bet? " << RESET;
            if (!(std::cin >> bet))
            {
                std::cout << RED << "Invalid input!\n" << RESET;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            if (bet <= 0 || bet > balance)
            {
                std::cout << RED << "Invalid bet!\n" << RESET;
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            break;
        }

        balance -= bet;

        // Play spin sound and animation
        if (audioEnabled) {
            ma_sound_seek_to_pcm_frame(&spinSound, 0);
            ma_sound_start(&spinSound);
        }

        rollAnimation();

        if (audioEnabled) {
            ma_sound_stop(&spinSound);
        }

        // Generate actual results
        Symbol reel1 = Reelsrng();
        Symbol reel2 = Reelsrng();
        Symbol reel3 = Reelsrng();

        // Display Final Result
        std::cout << BOLD << "FINAL RESULT:" << RESET << "\n";
        std::cout << GREEN << ">> [ " << reel1.name << " ] [ " 
                  << reel2.name << " ] [ " << reel3.name << " ] <<" << RESET << "\n\n";

        Symbol* payoutSymbol = nullptr;
        int matches = 0;

        for (auto& symbol : symbols)
        {
            if (symbol.name == "Wild") continue;

            bool first  = (reel1.name == symbol.name || reel1.name == "Wild");
            bool second = (reel2.name == symbol.name || reel2.name == "Wild");
            bool third  = (reel3.name == symbol.name || reel3.name == "Wild");

            int currentMatches = 0;
            if (first) {
                currentMatches = 1;
                if (second) {
                    currentMatches = 2;
                    if (third) currentMatches = 3;
                }
            }

            if (currentMatches > matches) {
                matches = currentMatches;
                payoutSymbol = &symbol;
            }
        }

        // Win/Loss Payout and Sound Logic
        if (reel1.name == "Wild" && reel2.name == "Wild" && reel3.name == "Wild")
        {
            if (audioEnabled) {
                ma_sound_seek_to_pcm_frame(&winSound, 0);
                ma_sound_start(&winSound);
            }
            int winnings = bet * symbols[WILD].payouts[3];
            std::cout << GREEN << "MEGA JACKPOT! x" << symbols[WILD].payouts[3] << " = $" << winnings << "\n" << RESET;
            balance += winnings;
        }
        else if (payoutSymbol && matches >= 2)
        {
            if (audioEnabled) {
                ma_sound_seek_to_pcm_frame(&winSound, 0);
                ma_sound_start(&winSound);
            }
            int multiplier = payoutSymbol->payouts[matches];
            int winnings = bet * multiplier;
            std::cout << GREEN << "You Win! " << payoutSymbol->name << " x" << multiplier << " = $" << winnings << "\n" << RESET;
            balance += winnings;
        }
        else
        {
            if (audioEnabled) {
                ma_sound_seek_to_pcm_frame(&lossSound, 0);
                ma_sound_start(&lossSound);
            }
            std::cout << RED << "You Lose!\n" << RESET;
        }

        std::cout << GREEN << "Balance: $" << balance << RESET << "\n\n";

        if (balance <= 0) break;

        std::string again;
        while (true)
        {
            std::cout << YELLOW << "Play another round? (y/n): " << RESET;
            std::getline(std::cin, again);
            if (again == "y" || again == "Y") {
                clearScreen();
                Title();
                std::cout << GREEN << "Balance: $" << balance << RESET << "\n\n";
                break;
            }
            if (again == "n" || again == "N") {
                std::cout << CYAN << "Thanks for playing! Final balance: $" << balance << RESET << "\n";
                if (audioEnabled) {
                    ma_sound_uninit(&bgSound);
                    ma_sound_uninit(&spinSound);
                    ma_sound_uninit(&winSound);
                    ma_sound_uninit(&lossSound);
                    ma_engine_uninit(&engine);
                }
                return 0;
            }
        }
    }

    if (audioEnabled) {
        ma_sound_uninit(&bgSound);
        ma_sound_uninit(&spinSound);
        ma_sound_uninit(&winSound);
        ma_sound_uninit(&lossSound);
        ma_engine_uninit(&engine);
    }
    return 0;
}