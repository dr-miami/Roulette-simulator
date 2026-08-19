```markdown
# Terminal Roulette Game (C++)

A lightweight, interactive, cross-platform terminal-based Roulette game built in C++17. Features real-time ANSI colored table navigation, flexible betting queues, and integrated audio effects powered by `miniaudio`.

---

## Features

* **Interactive Table UI:** Navigate numbers, rows, dozens, and outside bets using keyboard controls.
* **Bet Queue System:** Chain multiple bets together before spinning, with full undo capability.
* **Cross-Platform Input:** Smooth, non-blocking controls across Linux and Windows terminals.
* **Dynamic SFX & Music:** Background music, wheel spinning sound, and distinct outcome audio feedback.

---

## Audio Assets

Place the following WAV files in the root folder alongside your executable:

* `sound.wav` — Background music (loops during betting)
* `spin.wav` — Wheel spin animation effect
* `jackpot.wav` — Winning outcome sound effect
* `fart.wav` — Losing outcome / Bankrupt sound effect

---

## Building & Running

### Linux Dependencies
Ensure your sound development libraries are installed before compiling:

```bash
# Ubuntu / Debian / Pop!_OS
sudo apt update && sudo apt install libasound2-dev libpulse-dev

# Arch Linux
sudo pacman -S alsa-lib libpulse

# Fedora
sudo dnf install alsa-lib-devel pulseaudio-libs-devel

```

### Compilation

**Linux (GCC / Clang):**

```bash
g++ -std=c++17 main.cpp -lpthread -ldl -lm -lasound -lpulse -o Roulette
./Roulette

```

**Windows (MSVC / MinGW):**

```bash
g++ -std=c++17 main.cpp -o Roulette.exe
Roulette.exe

```

---

## Controls

| Key | Action |
| --- | --- |
| `W` / `A` / `S` / `D` or `Arrow Keys` | Move cursor around the roulette table |
| `ENTER` | Select current highlighted box to place a bet |
| `BACKSPACE` | Undo the last queued bet |
| `SPACE` | Spin the wheel and evaluate all active bets |
| `X` | Cash out and exit the game |
| `R` | Restart after going bankrupt |

---

## Betting Rules & Limits

* **Maximum Inside Bets:** You can place bets on up to 18 individual numbers per round.
* **Payouts:** Standard roulette odds apply (Straight Up pays 35:1, Column/Dozen pays 2:1, Even Money pays 1:1).

```

```